#include "MidiHandler.h"
#include <MIDIUSB.h>
#include <Arduino.h>
#include "settings.h" 


/*
Midihandler est le cerveau du systeme, 
il recoit les messages et decide de l'action a effectuer
comence par verifier si on lit tout les channels ou seulement un seul
noteOn : gere le servo pour retirer le silencieux et demande a xylophone l'activation de la note si la note est dans l'interval denotes jouée (et prend en compte le switch extraOctave)
noteOff : gere le servo pour mettre le silencieux quand il n'y a plus de notes jouée
controle change :
  - volume (7) : adapte l'angle du servo pour le volume
  - modulation (1) : fait varier l'angle du servo pour le volume depuis l'angle actuel pour le volume et l'angle 
  
MidiHandler initialise tout les ojets necessaire
*/
MidiHandler::MidiHandler(Xylophone &xylophone, ServoVolume &servoVolume)
    : _xylophone(xylophone), _servoVolume(servoVolume) {}

void MidiHandler::begin() {
  _extraOctaveEnabled = digitalRead(EXTRA_OCTAVE_SWITCH_PIN) == LOW;

}

void MidiHandler::handleMidiEvent() {
  //recupere le message midi
  midiEventPacket_t midiPacket = MidiUSB.read();
  //separe les informations du message
  byte messageType = midiPacket.header & 0xF0;
  byte channel = midiPacket.byte1 & 0x0F;
  byte data1 = midiPacket.byte2;
  byte data2 = midiPacket.byte3;
  //reification channel
  if (ALL_CHANNEL==false){
    if (channel!=CHANNEL_XYLO){ 
      return; // on ne fait rien si le channel n'est pas le bon
    }
  }
  //selection de l'action a faire 
  switch (messageType) {        
    case 0x80: // Note Off
      handleNoteOff(data1);
      break;
    case 0x90: // Note On
      handleNoteOn(data1, data2);
      break;
    case 0xB0: // Control Change
      handleControlChange(data1, data2);
      break;
    case 0xF0: // SysEx
     {
        // Lisez le message SysEx
        byte sysExData[USB_EP_SIZE - 4];
        unsigned int sysExLength = 0;
        bool endOfSysEx = false;

        while (!endOfSysEx && MidiUSB.available() > 0) {
          midiPacket = MidiUSB.read();
          if (midiPacket.byte1 != 0xF7) {
            sysExData[sysExLength++] = midiPacket.byte1;
          } else {
            endOfSysEx = true;
          }
          if (midiPacket.byte2 != 0xF7) {
            sysExData[sysExLength++] = midiPacket.byte2;
          } else {
            endOfSysEx = true;
          }
          if (midiPacket.byte3 != 0xF7) {
            sysExData[sysExLength++] = midiPacket.byte3;
          } else {
            endOfSysEx = true;
          }
        }
        handleSysEx(sysExData, sysExLength);
      }
    default:
      // Ignorer les autres types de messages MIDI
      break;
  }
}


 bool MidiHandler:: isNotePlayable(byte note) { 
  return note >= INSTRUMENT_START_NOTE && note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE;
}

void MidiHandler::handleNoteOn( byte note, byte velocity) {
  //permet de jouer une octave en plus de chaque coté (ca devrais sonner correct ... a voir)
if (_extraOctaveEnabled) {
    if (note >= INSTRUMENT_START_NOTE - 12 && note < INSTRUMENT_START_NOTE) {
      note += 12;
    } else if (note >= INSTRUMENT_START_NOTE + INSTRUMENT_RANGE && note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE + 12) {
      note -= 12;
    }
  }
   // Vérifiez si la note est dans la plage jouable du xylophone
  if (isNotePlayable(note)) {
    if(velocity>0) {  
       // Appelle la fonction playNote de la classe Xylophone pour activer la sortie correspondante du MCP
      // avec la vélocité appropriée pour ajuster le PWM
      _xylophone.playNote(note, velocity);
    }      
  }
}

void MidiHandler::handleNoteOff( byte note) {
  if (_extraOctaveEnabled) {
    if (note >= INSTRUMENT_START_NOTE - 12 && note < INSTRUMENT_START_NOTE) {
      note += 12;
    } else if (note >= INSTRUMENT_START_NOTE + INSTRUMENT_RANGE && note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE + 12) {
      note -= 12;
    }
  }
    if (isNotePlayable(note)) {
       _xylophone.msgNoteOff(note);//envoi la note pour un controle plus precis possible dans le futur 
    }
}

void MidiHandler::handleControlChange(byte control, byte value) {
  switch (control) {
    case 7: // Volume
      setServoVolume(value);
      break;
    case 64:
    case 66:
    case 69: // Désactiver le servoMute
      _xylophone.activateServoMute(false);
      break;
    case 1:
    case 91:
    case 92:
    case 94: // Gestion du vibrato
      setServoVolumeVibrato(value);
      break;
    case 121: // Réinitialisation de tous les contrôleurs
      _xylophone.reset();
      setServoVolume(0);
      setServoVolumeVibrato(0);
      break;
    case 123: // Désactiver toutes les notes
      _xylophone.reset();
      break;
  }
}

void MidiHandler::setServoVolume(int volume) {
  _servoVolume.setVolume(volume);
}

void MidiHandler::setServoVolumeVibrato(int modulation) {
  if (modulation == 0) {
    _servoVolume.setVibrato(false);
    
  } else {
    float mappedFrequency = map(modulation, 1, 127, SERVO_VOLUME_MIN_FREQUENCY_VIBRATO, SERVO_VOLUME_MAX_FREQUENCY_VIBRATO);
    _servoVolume.setVibrato(true, mappedFrequency);
  }
}
void MidiHandler::handleSysEx(byte *data, unsigned int length) {
  // Vérifiez si le message reçu est une demande d'identification (0x06, 0x01)
  if (length >= 2 && data[0] == 0x06 && data[1] == 0x01) {
    // Envoyez la réponse d'identification
    byte idResponse[] = {
      0xF0, // Début du message SysEx
      0x7E, // ID non en temps réel
      0x7F, // ID de l'appareil (dispositif de diffusion)
      0x06, // Sous-ID#1 (Réponse d'identification)
      0x02, // Sous-ID#2 (Réponse générale)
      0x00, 0x01, 0x00, // ID du fabricant (par exemple, pour OpenAI: 0x00, 0x01, 0x00)
      0x00, 0x00, 0x00, 0x01, // ID du modèle (par exemple, 0x00000001)
      0x00, 0x00, 0x00, 0x01, // Version du logiciel (par exemple, 0x00000001)
      0xF7  // Fin du message SysEx
    };
    MidiUSB.write(idResponse, sizeof(idResponse));
    MidiUSB.flush();
  }
}


