/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
---------------------------------   MIDIHANDLER.CPP (ESP32 VERSION)   ------------------------------------
_________________________________________________________________________________________________________

Class to handle MIDI messages received via BLE/WiFi using ESP32

***********************************************************************************************************/

#include "MidiHandler.h"
#include <Arduino.h>
#include "settings.h"

// Static instance for callbacks
MidiHandler* MidiHandler::_instance = nullptr;

// ----------------------------------   DEBOUNCED SWITCH   --------------------------------------------

/**
 * @brief Constructor for debounced switch
 * @param pin Pin number (will be configured as INPUT_PULLUP)
 */
DebouncedSwitch::DebouncedSwitch(int pin)
  : _pin(pin),
    _lastState(false),
    _lastChangeTime(0),
    _debouncedState(false) {
  pinMode(_pin, INPUT_PULLUP);
}

/**
 * @brief Read current debounced state
 * @return true if switch is pressed (LOW), false otherwise
 */
bool DebouncedSwitch::read() {
  return _debouncedState;
}

/**
 * @brief Update debounce logic (call regularly)
 */
void DebouncedSwitch::update() {
  bool currentState = (digitalRead(_pin) == LOW);

  if (currentState != _lastState) {
    _lastChangeTime = millis();
    _lastState = currentState;
  } else if (millis() - _lastChangeTime >= SWITCH_DEBOUNCE_TIME_MS) {
    _debouncedState = _lastState;
  }
}

// ----------------------------------   MIDI HANDLER   --------------------------------------------

/**
 * @brief Constructor for MIDI handler
 * @param xylophone Reference to Xylophone instance
 */
MidiHandler::MidiHandler(Xylophone& xylophone)
  : _xylophone(xylophone),
    _extraOctaveSwitch(EXTRA_OCTAVE_SWITCH_PIN),
    _extraOctaveEnabled(false),
    _connected(false) {

  _instance = this;  // Set static instance for callbacks

#if MIDI_USE_WIFI
  _lastWiFiCheck = 0;
  _wifiConnected = false;
#endif

  if (DEBUG_HANDLER) {
    Serial.println(F("MidiHandler constructor (ESP32)"));
  }
}

//*********************************************************************************************
//******************          INITIALIZE BLE MIDI

#if MIDI_USE_BLE
bool MidiHandler::initBLE() {
  Serial.println(F("Initializing BLE MIDI..."));

  // Initialize BLE MIDI
  BLEMidiServer.begin(BLE_DEVICE_NAME);

  // Set callbacks
  BLEMidiServer.setOnConnectCallback(onBLEConnected);
  BLEMidiServer.setOnDisconnectCallback(onBLEDisconnected);

  // Set MIDI note handlers
  BLEMidiServer.setNoteOnCallback([](uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp) {
    if (_instance) {
      _instance->handleNoteOn(channel, note, velocity);
    }
  });

  BLEMidiServer.setNoteOffCallback([](uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp) {
    if (_instance) {
      _instance->handleNoteOff(channel, note, velocity);
    }
  });

  BLEMidiServer.setControlChangeCallback([](uint8_t channel, uint8_t control, uint8_t value, uint16_t timestamp) {
    if (_instance) {
      _instance->handleControlChange(channel, control, value);
    }
  });

  Serial.print(F("BLE MIDI Device Name: "));
  Serial.println(BLE_DEVICE_NAME);
  Serial.println(F("Waiting for BLE MIDI connection..."));

  return true;
}

void MidiHandler::onBLEConnected() {
  Serial.println(F("BLE MIDI Connected!"));
  if (_instance) {
    _instance->_connected = true;
  }
}

void MidiHandler::onBLEDisconnected() {
  Serial.println(F("BLE MIDI Disconnected"));
  if (_instance) {
    _instance->_connected = false;
    _instance->_xylophone.reset();  // Reset xylophone on disconnect
  }
}
#endif

//*********************************************************************************************
//******************          INITIALIZE WIFI MIDI

#if MIDI_USE_WIFI
bool MidiHandler::initWiFi() {
  Serial.println(F("Initializing WiFi MIDI..."));

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Connecting to WiFi"));

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("\nERROR: WiFi connection failed"));
    return false;
  }

  _wifiConnected = true;
  Serial.println(F("\nWiFi connected!"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());

  // Initialize AppleMIDI
  AppleMIDI.begin(WIFI_MIDI_SESSION_NAME);

  // Set callbacks
  AppleMIDI.setHandleConnected(onWiFiConnected);
  AppleMIDI.setHandleDisconnected(onWiFiDisconnected);

  // Set MIDI handlers
  AppleMIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    if (_instance) {
      _instance->handleNoteOn(channel, note, velocity);
    }
  });

  AppleMIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    if (_instance) {
      _instance->handleNoteOff(channel, note, velocity);
    }
  });

  AppleMIDI.setHandleControlChange([](byte channel, byte control, byte value) {
    if (_instance) {
      _instance->handleControlChange(channel, control, value);
    }
  });

  Serial.println(F("WiFi MIDI initialized. Waiting for connections..."));
  return true;
}

void MidiHandler::onWiFiConnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc, const char* name) {
  Serial.print(F("WiFi MIDI Connected to session: "));
  Serial.println(name);
  if (_instance) {
    _instance->_connected = true;
  }
}

void MidiHandler::onWiFiDisconnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc) {
  Serial.println(F("WiFi MIDI Disconnected"));
  if (_instance) {
    _instance->_connected = false;
    _instance->_xylophone.reset();
  }
}
#endif

//*********************************************************************************************
//******************          INITIALIZE THE OBJECTS AND SETTINGS

/**
 * @brief Initialize pins and components
 * @return true if initialization successful, false otherwise
 */
bool MidiHandler::begin() {
  // Initialize extra octave switch
  _extraOctaveSwitch.update();
  _extraOctaveEnabled = _extraOctaveSwitch.read();

  // Initialize xylophone hardware
  bool xyloInitialized = _xylophone.begin();

  if (DEBUG_HANDLER) {
    Serial.print(F("Extra octave mode: "));
    Serial.println(_extraOctaveEnabled ? F("ENABLED") : F("DISABLED"));

    if (xyloInitialized) {
      Serial.println(F("Xylophone initialized successfully"));
    } else {
      Serial.println(F("ERROR: Xylophone initialization failed"));
      return false;
    }
  }

  // Initialize MIDI transport
  bool midiInitialized = false;

#if MIDI_USE_BLE
  midiInitialized = initBLE();
#elif MIDI_USE_WIFI
  midiInitialized = initWiFi();
#elif MIDI_USE_SERIAL
  Serial.println(F("Serial MIDI not yet implemented"));
  midiInitialized = false;
#endif

  if (!midiInitialized) {
    Serial.println(F("ERROR: MIDI transport initialization failed"));
    return false;
  }

  return xyloInitialized && midiInitialized;
}

//*********************************************************************************************
//******************               HANDLE MIDI EVENTS

/**
 * @brief Process incoming MIDI messages
 * @note For ESP32, MIDI is handled via callbacks, so this is a placeholder
 */
void MidiHandler::handleMidiEvent() {
  // For BLE/WiFi MIDI, events are handled via callbacks
  // This function is kept for compatibility but doesn't need to do anything
#if MIDI_USE_WIFI
  // AppleMIDI needs to be called in the loop
  AppleMIDI.run();
#endif
}

//*********************************************************************************************
//******************          TEST FUNCTION FOR STARTUP

/**
 * @brief Test function - plays melody or all notes
 * @param playMelody If true, plays INIT_MELODY; if false, plays all notes sequentially
 */
void MidiHandler::test(bool playMelody) {
  if (playMelody) {
    // Play predefined melody
    for (size_t i = 0; i < sizeof(INIT_MELODY) / sizeof(INIT_MELODY[0]); i++) {
      handleNoteOn(0, INIT_MELODY[i], 127);  // Channel 0, velocity 127
      delay(INIT_MELODY_DELAY[i]);
      handleNoteOff(0, INIT_MELODY[i], 0);
    }
  } else {
    // Play all notes sequentially
    for (byte note = INSTRUMENT_START_NOTE; note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE; note++) {
      handleNoteOn(0, note, 127);  // Channel 0, velocity 127
      delay(TEST_SINGLE_NOTE_DURATION_MS);
      _xylophone.update();
      handleNoteOff(0, note, 0);
      delay(TEST_INTER_NOTE_DELAY_MS);
    }
  }

  if (DEBUG_HANDLER) {
    Serial.println(F("Test complete"));
  }
}

/**
 * @brief Update debouncing and xylophone ticker
 */
void MidiHandler::update() {
  _extraOctaveSwitch.update();
  _extraOctaveEnabled = _extraOctaveSwitch.read();
  _xylophone.update();

#if MIDI_USE_WIFI
  // Check WiFi status periodically
  if (millis() - _lastWiFiCheck > 5000) {
    _lastWiFiCheck = millis();
    if (WiFi.status() != WL_CONNECTED && _wifiConnected) {
      Serial.println(F("WiFi connection lost!"));
      _wifiConnected = false;
      _connected = false;
      _xylophone.reset();
    }
  }
#endif
}

/**
 * @brief Check if MIDI is connected
 */
bool MidiHandler::isConnected() {
  return _connected;
}

// ----------------------------------      PRIVATE  --------------------------------------------

//*********************************************************************************************
//******************          CHECK IF THE NOTE IS PLAYABLE

/**
 * @brief Check if note is in playable range
 * @param note MIDI note number
 * @return true if note is playable
 */
bool MidiHandler::isNotePlayable(byte note) {
  return note >= INSTRUMENT_START_NOTE && note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE;
}

//*********************************************************************************************
//******************               HANDLE NOTE ON

/**
 * @brief Handle MIDI note on message
 * @param channel MIDI channel (0-15)
 * @param note MIDI note number
 * @param velocity MIDI velocity (0-127)
 */
void MidiHandler::handleNoteOn(byte channel, byte note, byte velocity) {
  // Channel filtering
  if (ALL_CHANNEL == false) {
    if (channel != CHANNEL_XYLO) {
      return;
    }
  }

  // Extra octave mode: transpose notes outside normal range
  if (_extraOctaveEnabled) {
    if (note >= INSTRUMENT_START_NOTE - 12 && note < INSTRUMENT_START_NOTE) {
      note += 12;  // Transpose one octave up
    } else if (note >= INSTRUMENT_START_NOTE + INSTRUMENT_RANGE &&
               note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE + 12) {
      note -= 12;  // Transpose one octave down
    }
  }

  // Check if note is in playable range
  if (isNotePlayable(note)) {
    if (velocity > 0) {
      if (DEBUG_HANDLER) {
        Serial.print(F("MIDI noteOn: ch="));
        Serial.print(channel);
        Serial.print(F(" note="));
        Serial.print(note);
        Serial.print(F(" vel="));
        Serial.println(velocity);
      }

      _xylophone.playNote(note, velocity);
    }
  }
}

//*********************************************************************************************
//******************              HANDLE NOTE OFF

/**
 * @brief Handle MIDI note off message
 * @param channel MIDI channel (0-15)
 * @param note MIDI note number
 * @param velocity Release velocity
 */
void MidiHandler::handleNoteOff(byte channel, byte note, byte velocity) {
  // Channel filtering
  if (ALL_CHANNEL == false) {
    if (channel != CHANNEL_XYLO) {
      return;
    }
  }

  // Extra octave mode: transpose notes outside normal range
  if (_extraOctaveEnabled) {
    if (note >= INSTRUMENT_START_NOTE - 12 && note < INSTRUMENT_START_NOTE) {
      note += 12;
    } else if (note >= INSTRUMENT_START_NOTE + INSTRUMENT_RANGE &&
               note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE + 12) {
      note -= 12;
    }
  }

  if (isNotePlayable(note)) {
    _xylophone.msgNoteOff(note);

    if (DEBUG_HANDLER) {
      Serial.print(F("MIDI noteOff: ch="));
      Serial.print(channel);
      Serial.print(F(" note="));
      Serial.println(note);
    }
  }
}

//*********************************************************************************************
//******************             HANDLE CONTROL CHANGE

/**
 * @brief Handle MIDI control change messages
 * @param channel MIDI channel (0-15)
 * @param control CC number
 * @param value CC value (0-127)
 */
void MidiHandler::handleControlChange(byte channel, byte control, byte value) {
  // Channel filtering
  if (ALL_CHANNEL == false) {
    if (channel != CHANNEL_XYLO) {
      return;
    }
  }

  switch (control) {
    case 121:  // Reset all controllers
      _xylophone.reset();
      if (DEBUG_HANDLER) {
        Serial.println(F("CC121: Reset all controllers"));
      }
      break;

    case 123:  // All notes off
      _xylophone.reset();
      if (DEBUG_HANDLER) {
        Serial.println(F("CC123: All notes off"));
      }
      break;

    default:
      if (DEBUG_HANDLER) {
        Serial.print(F("CC"));
        Serial.print(control);
        Serial.print(F(": "));
        Serial.println(value);
      }
      break;
  }
}
