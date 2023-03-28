/***********************************************************************************************************
classe pour gerer les messages midi recu par usb ave midUSB.h



les differents parametres et reglages du systeme pour ce servo est dans settings.h
***********************************************************************************************************/
#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H


#include "Xylophone.h"
#include "ServoVolume.h"


class MidiHandler {
public:
  MidiHandler(Xylophone &xylophone, ServoVolume &_servoVolume);
  void handleMidiEvent();// traite les messages midi recu et appel les differentes fonctions 
  void begin (); //initialise tout ce qui doit l'etre
  
private:
  Xylophone& _xylophone;
  ServoVolume _servoVolume;
  bool _extraOctaveEnabled = digitalRead(EXTRA_OCTAVE_SWITCH_PIN) == LOW; //lit si le switch extra octave est actif ou non 
//------------------------------------------------------------------ 
//gestion des messages NoteOn, NoteOff
  void handleNoteOn( byte note, byte velocity); 
  void handleNoteOff( byte note);
  bool isNotePlayable(byte note);  // verifie si la note est dans l'intervalle jouable de note 
//gestion des Controls change
  void handleControlChange( byte control, byte value);//il me reste du taf sur cette partie ***************************
  void setServoVolume (int volume); //recoit la valeur du volume par la cc 7 et ajuste l'angle en fonction
  void setServoVolumeVibrato (int modulation); //recoit la valeur du vibrato par la cc 1 et ajuste la frequence de vibration en fonction de l'interval indiqué dans settings
  
  ///fonction de gestion des messages System Exclusive  
  void handleSysEx(byte *data, unsigned int length);

  };

#endif
