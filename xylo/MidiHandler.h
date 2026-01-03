/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
---------------------------------------    MIDIHANDLER.H    ---------------------------------------------
_________________________________________________________________________________________________________
Classe pour gérer les messages MIDI reçus par USB avec MidiUSB.h
Permet de lire les différents messages comme noteOff, noteOn, certains control change
et un début de communication sysex.

MidiHandler est le cerveau du système,
il reçoit les messages et décide de l'action à faire effectuer par l'instrument.
Commence par vérifier si on lit tous les channels ou seulement un seul.

noteOn : Demande à xylophone l'activation de la note si la note est dans l'intervalle
         de notes jouées (et prend en compte le switch extraOctave)
noteOff : Enregistre le noteOff pour gérer les compteurs de notes actives
controle change :
  - CC 121 : Réinitialisation de tous les contrôleurs
  - CC 123 : Désactiver toutes les notes

MidiHandler initialise tous les objets nécessaires utilisés, dans ce cas : xylophone 


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
