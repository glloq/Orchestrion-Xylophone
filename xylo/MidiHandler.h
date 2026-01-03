/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
---------------------------------------    MIDIHANDLER.H    ---------------------------------------------
_________________________________________________________________________________________________________
classe pour gerer les messages midi recu par usb avec MidUSB.h
Permet de lire les differents messages comme noteOff, noteOn, certains controls change comme la modulation et le volume et un debut de communication sysex

Midihandler est le cerveau du systeme, 
il recoit les messages et decide de l'action a faire effectuer par l'instrument
comence par verifier si on lit tout les channels ou seulement un seul
noteOn : gere le servo pour retirer le silencieux et demande a xylophone l'activation de la note si la note est dans l'interval denotes jouée (et prend en compte le switch extraOctave)
noteOff : gere le servo pour mettre le silencieux avec servoMute quand il n'y a plus de notes jouée
controle change :
  - volume (7) : adapte l'angle du servo pour le volume
  - modulation (1) : fait varier l'angle du servo pour le volume depuis l'angle actuel pour le volume et l'angle 
  
MidiHandler initialise tout les ojets necessaire utilisé, dans ce cas : xylophone et servoVolume 


***********************************************************************************************************/
#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H


#include "Xylophone.h"


class MidiHandler {
public:
  MidiHandler(Xylophone &xylophone);
  void handleMidiEvent();// traite les messages midi recu et appel les differentes fonctions
  void begin (); //initialise tout ce qui doit l'etre
  void test(bool playMelody);
  void update();

private:
  Xylophone& _xylophone;
  bool _extraOctaveEnabled;  //lit si le switch extra octave est actif ou non
//------------------------------------------------------------------
//gestion des messages NoteOn, NoteOff
  void handleNoteOn( byte note, byte velocity);
  void handleNoteOff( byte note);
  bool isNotePlayable(byte note);  // verifie si la note est dans l'intervalle jouable de note
//gestion des Controls change
  void handleControlChange( byte control, byte value);//gestion des CC
  
  ///fonction de gestion des messages System Exclusive  
  void handleSysEx(byte *data, unsigned int length);

  };

#endif
