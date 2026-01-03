/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ----------------------------
---------------------------------------   MIDIHANDLER.CPP   ---------------------------------------------
_________________________________________________________________________________________________________
Classe pour gérer les messages MIDI reçus par WiFi (AppleMIDI/RTP-MIDI)
Version ESP32

***********************************************************************************************************/

#include "MidiHandler.h"
#include <Arduino.h>
#include "settings.h"

// Instance statique pour les callbacks
MidiHandler* MidiHandler::_instance = nullptr;

// Instance AppleMIDI
APPLEMIDI_CREATE_DEFAULTSESSION_INSTANCE();

// ----------------------------------      PUBLIC  --------------------------------------------

MidiHandler::MidiHandler(Xylophone &xylophone) : _xylophone(xylophone), _wifiConnected(false), _midiConnected(false) {
  _extraOctaveEnabled = digitalRead(EXTRA_OCTAVE_SWITCH_PIN) == LOW;
  _instance = this;

  if(DEBUG_HANDLER){
    Serial.println(F("constructor handler (ESP32 WiFi)"));
  }
}

//*********************************************************************************************
//******************          INITIALISE THE OBJECTS AND SETINGS

void MidiHandler::begin() {
  pinMode(EXTRA_OCTAVE_SWITCH_PIN, INPUT_PULLUP);
  _extraOctaveEnabled = digitalRead(EXTRA_OCTAVE_SWITCH_PIN) == LOW;

  // Connexion WiFi
  connectWiFi();

  // Initialisation AppleMIDI
  Serial.println("Initialisation AppleMIDI...");
  AppleMIDI.begin(APPLEMIDI_SESSION_NAME);

  // Configuration des callbacks
  AppleMIDI.setHandleConnected(onConnected);
  AppleMIDI.setHandleDisconnected(onDisconnected);
  AppleMIDI.setHandleNoteOn(onNoteOn);
  AppleMIDI.setHandleNoteOff(onNoteOff);
  AppleMIDI.setHandleControlChange(onControlChange);

  _xylophone.begin();

  Serial.println("AppleMIDI initialisé - En attente de connexion...");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
}

//*********************************************************************************************
//******************          CONNECT TO WIFI

void MidiHandler::connectWiFi() {
  Serial.print("Connexion au WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    _wifiConnected = true;
    Serial.println();
    Serial.println("WiFi connecté!");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Échec de connexion WiFi!");
  }
}

//*********************************************************************************************
//******************          CALLBACKS APPLEMIDI

void MidiHandler::onConnected(const ssrc_t & ssrc, const char* name) {
  if(_instance) {
    _instance->_midiConnected = true;
    Serial.print("AppleMIDI Connecté à session: ");
    Serial.println(name);
  }
}

void MidiHandler::onDisconnected(const ssrc_t & ssrc) {
  if(_instance) {
    _instance->_midiConnected = false;
    _instance->_xylophone.reset();
    Serial.println("AppleMIDI Déconnecté!");
  }
}

void MidiHandler::onNoteOn(byte channel, byte note, byte velocity) {
  if(_instance) {
    // Vérification du canal
    if (!ALL_CHANNEL && channel != CHANNEL_XYLO) {
      return;
    }
    _instance->handleNoteOn(note, velocity);
  }
}

void MidiHandler::onNoteOff(byte channel, byte note, byte velocity) {
  if(_instance) {
    // Vérification du canal
    if (!ALL_CHANNEL && channel != CHANNEL_XYLO) {
      return;
    }
    _instance->handleNoteOff(note);
  }
}

void MidiHandler::onControlChange(byte channel, byte control, byte value) {
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
  // Lecture des messages MIDI entrants
  AppleMIDI.run();

  // Mise à jour du xylophone
  _xylophone.update();

  // Vérification de la connexion WiFi
  if (WiFi.status() != WL_CONNECTED && _wifiConnected) {
    _wifiConnected = false;
    Serial.println("WiFi déconnecté! Tentative de reconnexion...");
    connectWiFi();
  }
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
