/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ----------------------------
---------------------------------------    MIDIHANDLER.H    ---------------------------------------------
_________________________________________________________________________________________________________
Classe pour gérer les messages MIDI reçus par Bluetooth (BLE MIDI)
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
#include <BLEMidi.h>

class MidiHandler {
public:
  MidiHandler(Xylophone &xylophone);
  void begin(); // initialise tout ce qui doit l'etre
  void test(bool playMelody);
  void update();

private:
  Xylophone& _xylophone;
  bool _extraOctaveEnabled;  // lit si le switch extra octave est actif ou non
  bool _bleConnected;        // statut de connexion BLE
  bool _bleEnabled;          // BLE activé ou non

  // Gestion bouton et LED d'appairage
  unsigned long _buttonPressTime;      // Temps du début d'appui sur le bouton
  bool _buttonPressed;                 // État du bouton
  unsigned long _lastLedToggle;        // Dernier changement d'état de la LED
  bool _ledState;                      // État actuel de la LED

  void updatePairingButton();          // Vérifie l'état du bouton
  void updateStatusLed();              // Met à jour l'état de la LED
  void enableBLE();                    // Active le BLE
  void disableBLE();                   // Désactive le BLE

  // Callbacks BLE
  static void onConnected();
  static void onDisconnected();
  static void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp);
  static void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp);
  static void onControlChange(uint8_t channel, uint8_t control, uint8_t value, uint16_t timestamp);

  // Instance statique pour les callbacks
  static MidiHandler* _instance;

  // Gestion des messages NoteOn, NoteOff
  void handleNoteOn(byte note, byte velocity);
  void handleNoteOff(byte note);
  bool isNotePlayable(byte note);  // verifie si la note est dans l'intervalle jouable de note

  // Gestion des Controls change
  void handleControlChange(byte control, byte value); // gestion des CC
};

#endif
