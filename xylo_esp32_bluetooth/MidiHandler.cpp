/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ----------------------------
---------------------------------------   MIDIHANDLER.CPP   ---------------------------------------------
_________________________________________________________________________________________________________
Classe pour gérer les messages MIDI reçus par Bluetooth (BLE MIDI)
Version ESP32

***********************************************************************************************************/

#include "MidiHandler.h"
#include <Arduino.h>
#include "settings.h"

// Instance statique pour les callbacks
MidiHandler* MidiHandler::_instance = nullptr;

// ----------------------------------      PUBLIC  --------------------------------------------

MidiHandler::MidiHandler(Xylophone &xylophone) : _xylophone(xylophone), _bleConnected(false), _bleEnabled(BLE_ENABLED_BY_DEFAULT) {
  _extraOctaveEnabled = digitalRead(EXTRA_OCTAVE_SWITCH_PIN) == LOW;
  _buttonPressTime = 0;
  _buttonPressed = false;
  _lastLedToggle = 0;
  _ledState = false;
  _instance = this;

  if(DEBUG_HANDLER){
    Serial.println(F("constructor handler (ESP32 BLE)"));
  }
}

//*********************************************************************************************
//******************          INITIALISE THE OBJECTS AND SETINGS

void MidiHandler::begin() {
  pinMode(EXTRA_OCTAVE_SWITCH_PIN, INPUT_PULLUP);
  _extraOctaveEnabled = digitalRead(EXTRA_OCTAVE_SWITCH_PIN) == LOW;

  // Initialisation du bouton d'appairage et de la LED
  pinMode(BLE_PAIRING_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BLE_STATUS_LED_PIN, OUTPUT);
  digitalWrite(BLE_STATUS_LED_PIN, LOW);

  _xylophone.begin();

  // Initialisation BLE selon la configuration
  if (_bleEnabled) {
    enableBLE();
  } else {
    Serial.println("BLE désactivé par défaut - Appuyez sur le bouton d'appairage pour activer");
    digitalWrite(BLE_STATUS_LED_PIN, LOW); // LED éteinte
  }
}

//*********************************************************************************************
//******************          CALLBACKS BLE

void MidiHandler::onConnected() {
  if(_instance) {
    _instance->_bleConnected = true;
    Serial.println("BLE MIDI Connecté!");
  }
}

void MidiHandler::onDisconnected() {
  if(_instance) {
    _instance->_bleConnected = false;
    _instance->_xylophone.reset();
    Serial.println("BLE MIDI Déconnecté!");
  }
}

void MidiHandler::onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp) {
  if(_instance) {
    // Vérification du canal
    if (!ALL_CHANNEL && channel != CHANNEL_XYLO) {
      return;
    }
    _instance->handleNoteOn(note, velocity);
  }
}

void MidiHandler::onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp) {
  if(_instance) {
    // Vérification du canal
    if (!ALL_CHANNEL && channel != CHANNEL_XYLO) {
      return;
    }
    _instance->handleNoteOff(note);
  }
}

void MidiHandler::onControlChange(uint8_t channel, uint8_t control, uint8_t value, uint16_t timestamp) {
  if(_instance) {
    // Vérification du canal
    if (!ALL_CHANNEL && channel != CHANNEL_XYLO) {
      return;
    }
    _instance->handleControlChange(control, value);
  }
}

//*********************************************************************************************
//******************          FUNCTION FOR TEST

void MidiHandler::test(bool playMelody) {
  if (playMelody) {
    for (size_t i = 0; i < sizeof(INIT_MELODY) / sizeof(INIT_MELODY[0]); i++) {
      handleNoteOn(INIT_MELODY[i], 127);
      delay(INIT_MELODY_DELAY[i]);
      handleNoteOff(INIT_MELODY[i]);
    }
  } else {
    // Joue toutes les notes l'une après l'autre
    for (byte note = INSTRUMENT_START_NOTE; note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE; note++) {
      handleNoteOn(note, 127);
      delay(20);
      _xylophone.update();
      handleNoteOff(note);
      delay(200);
    }
  }

  if(DEBUG_HANDLER){
    Serial.println(F("fin du test melodie init"));
  }
}

void MidiHandler::update() {
  updatePairingButton();  // Gestion du bouton d'appairage
  updateStatusLed();      // Gestion de la LED de statut
  _xylophone.update();
}

// ----------------------------------      PRIVATE  --------------------------------------------

//*********************************************************************************************
//******************          CHECK IF THE NOTE IS PLAYABLE

bool MidiHandler::isNotePlayable(byte note) {
  return note >= INSTRUMENT_START_NOTE && note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE;
}

//*********************************************************************************************
//******************               HANDLE NOTES ON

void MidiHandler::handleNoteOn(byte note, byte velocity) {
  // Permet de jouer une octave en plus de chaque côté
  if (_extraOctaveEnabled) {
    if (note >= INSTRUMENT_START_NOTE - 12 && note < INSTRUMENT_START_NOTE) {
      note += 12;
    } else if (note >= INSTRUMENT_START_NOTE + INSTRUMENT_RANGE && note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE + 12) {
      note -= 12;
    }
  }

  // Vérifiez si la note est dans la plage jouable du xylophone
  if (isNotePlayable(note)) {
    if(velocity > 0) {
      if(DEBUG_HANDLER){
        Serial.print(F("MIDIHandler noteOn = "));
        Serial.println(note);
      }
      _xylophone.playNote(note, velocity);
    }
  }
}

//*********************************************************************************************
//******************              HANDLES NOTES OFF

void MidiHandler::handleNoteOff(byte note) {
  if (_extraOctaveEnabled) {
    if (note >= INSTRUMENT_START_NOTE - 12 && note < INSTRUMENT_START_NOTE) {
      note += 12;
    } else if (note >= INSTRUMENT_START_NOTE + INSTRUMENT_RANGE && note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE + 12) {
      note -= 12;
    }
  }

  if (isNotePlayable(note)) {
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
//******************             ENABLE BLE

void MidiHandler::enableBLE() {
  if (!_bleEnabled) {
    Serial.println("Activation du BLE MIDI...");

    // Initialisation BLE MIDI
    BLEMidiServer.begin(BLE_DEVICE_NAME);

    // Configuration des callbacks
    BLEMidiServer.setOnConnectCallback(onConnected);
    BLEMidiServer.setOnDisconnectCallback(onDisconnected);
    BLEMidiServer.setNoteOnCallback(onNoteOn);
    BLEMidiServer.setNoteOffCallback(onNoteOff);
    BLEMidiServer.setControlChangeCallback(onControlChange);

    _bleEnabled = true;
    Serial.println("BLE MIDI activé - En attente de connexion...");
  }
}

//*********************************************************************************************
//******************             DISABLE BLE

void MidiHandler::disableBLE() {
  if (_bleEnabled) {
    Serial.println("Désactivation du BLE MIDI...");
    // Note: BLEMidi ne fournit pas de méthode end(), donc on marque juste comme désactivé
    _bleEnabled = false;
    _bleConnected = false;
    digitalWrite(BLE_STATUS_LED_PIN, LOW);
    Serial.println("BLE MIDI désactivé");
  }
}

//*********************************************************************************************
//******************             UPDATE PAIRING BUTTON

void MidiHandler::updatePairingButton() {
  bool currentButtonState = digitalRead(BLE_PAIRING_BUTTON_PIN) == LOW; // LOW = pressé (INPUT_PULLUP)

  // Détection du front montant (bouton appuyé)
  if (currentButtonState && !_buttonPressed) {
    _buttonPressed = true;
    _buttonPressTime = millis();
  }

  // Détection du front descendant (bouton relâché)
  if (!currentButtonState && _buttonPressed) {
    unsigned long pressDuration = millis() - _buttonPressTime;

    if (pressDuration >= LONG_PRESS_TIME) {
      // Appui long : désactiver le BLE
      disableBLE();
    } else {
      // Appui court : activer le BLE
      if (!_bleEnabled) {
        enableBLE();
      }
    }

    _buttonPressed = false;
  }
}

//*********************************************************************************************
//******************             UPDATE STATUS LED

void MidiHandler::updateStatusLed() {
  if (!_bleEnabled) {
    // BLE désactivé : LED éteinte
    digitalWrite(BLE_STATUS_LED_PIN, LOW);
    _ledState = false;
  } else if (_bleConnected) {
    // BLE connecté : LED allumée fixe
    digitalWrite(BLE_STATUS_LED_PIN, HIGH);
    _ledState = true;
  } else {
    // BLE activé mais non connecté : LED clignotante
    unsigned long currentTime = millis();
    if (currentTime - _lastLedToggle >= LED_BLINK_INTERVAL) {
      _ledState = !_ledState;
      digitalWrite(BLE_STATUS_LED_PIN, _ledState ? HIGH : LOW);
      _lastLedToggle = currentTime;
    }
  }
}
