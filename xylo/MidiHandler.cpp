/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
---------------------------------------   MIDIHANDLER.CPP   ---------------------------------------------
_________________________________________________________________________________________________________
classe pour gerer les messages midi recu par usb avec MidUSB.h
Permet de lire les differents messages comme noteOff, noteOn, certains controls change comme la modulation et le volume et un debut de communication sysex

***********************************************************************************************************/

#include "MidiHandler.h"
#include <MIDIUSB.h>
#include <Arduino.h>
#include "settings.h" 

// ----------------------------------      PUBLIC  --------------------------------------------
MidiHandler::MidiHandler(Xylophone &xylophone) : _xylophone(xylophone) {
  _extraOctaveEnabled = digitalRead(EXTRA_OCTAVE_SWITCH_PIN) == LOW;
    if(DEBUG_HANDLER){
    Serial.println(F("constructor handler"));
  }
}


//*********************************************************************************************
//******************          INITIALISE THE OBJECTS AND SETINGS

void MidiHandler::begin() {
  pinMode(EXTRA_OCTAVE_SWITCH_PIN, INPUT_PULLUP);// Définition de la broche extra octave  
  _extraOctaveEnabled = digitalRead(EXTRA_OCTAVE_SWITCH_PIN) == LOW;
  _xylophone.begin();
}

//*********************************************************************************************
//******************               HANDLE MIDI EVENTS

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
    case 0xF0: // SysEx => ne devrais surement pas rester pour ce projet pour l'instant
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
//*********************************************************************************************
//******************          FUNCTION FOR TEST 

void MidiHandler::test(bool playMelody) {
  if (playMelody) {
    for (size_t i = 0; i < sizeof(INIT_MELODY) / sizeof(INIT_MELODY[0]); i++) {
      handleNoteOn(INIT_MELODY[i], 127);  // Jouer la note avec une vélocité de 127
      delay(INIT_MELODY_DELAY[i]);    // Attendre le temps indiqué dans INIT_MELODY_DELAY
      handleNoteOff(INIT_MELODY[i]);     // Envoyer un message de note off
    }
  } else {
     //joue tout les notes l'une après l'autre
    for (byte note = INSTRUMENT_START_NOTE; note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE; note++) {
      handleNoteOn(note, 127);  // Jouer la note avec une vélocité de 127
      delay(20);           // Attendre 200 ms
      _xylophone.update(); // coupe l'electroaiman      
      handleNoteOff(note);     // Envoyer un message de note off
      delay(200);           // Attendre 200 ms
       }
      
     // handleNoteOn(INSTRUMENT_START_NOTE, 127);  // Jouer la note avec une vélocité de 127
      
   
  }
  if(DEBUG_HANDLER){
    Serial.println(F("fin du test melodie init"));  
  }
}

void MidiHandler::update() {
  _xylophone.update();
}
  
// ----------------------------------      PUBLIC  --------------------------------------------

//*********************************************************************************************
//******************          CHECK IF THE NOTE IS PLAYABLE

 bool MidiHandler:: isNotePlayable(byte note) { 
return note >= INSTRUMENT_START_NOTE && note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE;
}


//*********************************************************************************************
//******************               HANDLE NOTES ON

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

        if(DEBUG_HANDLER){
        Serial.print(F("MIDIHandler noteOn = ")); 
        Serial.println(note); 
      }
       // Appelle la fonction playNote de la classe Xylophone pour activer la sortie correspondante du MCP
      // avec la vélocité appropriée pour ajuster le PWM
      _xylophone.playNote(note, velocity);
      
    
    }      
  }
}

//*********************************************************************************************
//******************              HANDLES NOTES OFF 

void MidiHandler::handleNoteOff( byte note) {
  if (_extraOctaveEnabled) {
    if (note >= INSTRUMENT_START_NOTE - 12 && note < INSTRUMENT_START_NOTE) {
      note += 12;
    } else if (note >= INSTRUMENT_START_NOTE + INSTRUMENT_RANGE && note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE + 12) {
      note -= 12;
    }
  }
    if (isNotePlayable(note)) {
       _xylophone.msgNoteOff(note);
      if(DEBUG_HANDLER){
        Serial.print(F("MIDIHandler noteOff = ")); 
        Serial.println(note); 
      }
    }
}

//*********************************************************************************************
//******************             HANDLE CONTROLS CHANGE 

void MidiHandler::handleControlChange(byte control, byte value) {
  switch (control) {
    case 121: // Réinitialisation de tous les contrôleurs
      _xylophone.reset();
      break;
    case 123: // Désactiver toutes les notes
      _xylophone.reset();
      break;
  }
}

//*********************************************************************************************
//******************               HANDLE SYSTEMS EX

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


