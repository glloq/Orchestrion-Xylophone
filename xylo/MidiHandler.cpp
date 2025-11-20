/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
---------------------------------------   MIDIHANDLER.CPP   ---------------------------------------------
_________________________________________________________________________________________________________

Class to handle MIDI messages received via USB using MIDIUSB.h
Reads different messages like noteOff, noteOn, certain control changes, and SysEx communication

***********************************************************************************************************/

#include "MidiHandler.h"
#include <MIDIUSB.h>
#include <Arduino.h>
#include "settings.h"

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
    _extraOctaveEnabled(false) {

  if (DEBUG_HANDLER) {
    Serial.println(F("MidiHandler constructor"));
  }
}

//*********************************************************************************************
//******************          INITIALIZE THE OBJECTS AND SETTINGS

/**
 * @brief Initialize pins and components
 */
void MidiHandler::begin() {
  // Initialize extra octave switch
  _extraOctaveSwitch.update();
  _extraOctaveEnabled = _extraOctaveSwitch.read();

  // Initialize xylophone hardware
  _xylophone.begin();

  if (DEBUG_HANDLER) {
    Serial.print(F("Extra octave mode: "));
    Serial.println(_extraOctaveEnabled ? F("ENABLED") : F("DISABLED"));
  }
}

//*********************************************************************************************
//******************               HANDLE MIDI EVENTS

/**
 * @brief Process incoming MIDI messages
 */
void MidiHandler::handleMidiEvent() {
  // Read MIDI packet
  midiEventPacket_t midiPacket = MidiUSB.read();

  // Extract message information
  byte messageType = midiPacket.header & 0xF0;
  byte channel = midiPacket.byte1 & 0x0F;
  byte data1 = midiPacket.byte2;
  byte data2 = midiPacket.byte3;

  // Channel filtering
  if (ALL_CHANNEL == false) {
    if (channel != CHANNEL_XYLO) {
      return;  // Ignore if not the correct channel
    }
  }

  // Route message to appropriate handler
  switch (messageType) {
    case 0x80:  // Note Off
      handleNoteOff(data1);
      break;

    case 0x90:  // Note On
      handleNoteOn(data1, data2);
      break;

    case 0xB0:  // Control Change
      handleControlChange(data1, data2);
      break;

    case 0xF0:  // SysEx
      {
        // Read SysEx message with bounds checking
        const unsigned int MAX_SYSEX_LENGTH = USB_EP_SIZE - 4;
        byte sysExData[MAX_SYSEX_LENGTH];
        unsigned int sysExLength = 0;
        bool endOfSysEx = false;

        // Helper macro for safe byte insertion
        #define SAFE_ADD_SYSEX(byte_val) \
          do { \
            if (sysExLength < MAX_SYSEX_LENGTH) { \
              if ((byte_val) != 0xF7) { \
                sysExData[sysExLength++] = (byte_val); \
              } else { \
                endOfSysEx = true; \
              } \
            } else { \
              endOfSysEx = true; \
              if (DEBUG_HANDLER) { \
                Serial.println(F("WARNING: SysEx buffer full")); \
              } \
            } \
          } while(0)

        // Read SysEx data with overflow protection
        while (!endOfSysEx && MidiUSB.available() > 0) {
          midiPacket = MidiUSB.read();
          SAFE_ADD_SYSEX(midiPacket.byte1);
          if (!endOfSysEx) SAFE_ADD_SYSEX(midiPacket.byte2);
          if (!endOfSysEx) SAFE_ADD_SYSEX(midiPacket.byte3);
        }

        #undef SAFE_ADD_SYSEX

        // Process SysEx only if we didn't overflow
        if (sysExLength <= MAX_SYSEX_LENGTH) {
          handleSysEx(sysExData, sysExLength);
        } else {
          if (DEBUG_HANDLER) {
            Serial.println(F("ERROR: SysEx buffer overflow prevented"));
          }
        }
      }
      break;

    default:
      // Ignore other MIDI message types
      break;
  }
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
      handleNoteOn(INIT_MELODY[i], 127);  // Play note with velocity 127
      delay(INIT_MELODY_DELAY[i]);        // Wait specified time
      handleNoteOff(INIT_MELODY[i]);      // Send note off
    }
  } else {
    // Play all notes sequentially
    for (byte note = INSTRUMENT_START_NOTE; note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE; note++) {
      handleNoteOn(note, 127);              // Play note with velocity 127
      delay(TEST_SINGLE_NOTE_DURATION_MS);  // Wait for electromagnet to fire
      _xylophone.update();                  // Deactivate electromagnet
      handleNoteOff(note);                  // Send note off
      delay(TEST_INTER_NOTE_DELAY_MS);      // Wait before next note
    }
  }

  if (DEBUG_HANDLER) {
    Serial.println(F("Test melody complete"));
  }
}

/**
 * @brief Update debouncing and xylophone ticker
 */
void MidiHandler::update() {
  _extraOctaveSwitch.update();
  _extraOctaveEnabled = _extraOctaveSwitch.read();
  _xylophone.update();
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
 * @param note MIDI note number
 * @param velocity MIDI velocity (0-127)
 * @note Supports extra octave mode via physical switch
 */
void MidiHandler::handleNoteOn(byte note, byte velocity) {
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
        Serial.print(F("MIDI noteOn: "));
        Serial.println(note);
      }

      // Play note on xylophone with velocity mapping
      _xylophone.playNote(note, velocity);
    }
  }
}

//*********************************************************************************************
//******************              HANDLE NOTE OFF

/**
 * @brief Handle MIDI note off message
 * @param note MIDI note number
 */
void MidiHandler::handleNoteOff(byte note) {
  // Extra octave mode: transpose notes outside normal range
  if (_extraOctaveEnabled) {
    if (note >= INSTRUMENT_START_NOTE - 12 && note < INSTRUMENT_START_NOTE) {
      note += 12;  // Transpose one octave up
    } else if (note >= INSTRUMENT_START_NOTE + INSTRUMENT_RANGE &&
               note < INSTRUMENT_START_NOTE + INSTRUMENT_RANGE + 12) {
      note -= 12;  // Transpose one octave down
    }
  }

  if (isNotePlayable(note)) {
    _xylophone.msgNoteOff(note);

    if (DEBUG_HANDLER) {
      Serial.print(F("MIDI noteOff: "));
      Serial.println(note);
    }
  }
}

//*********************************************************************************************
//******************             HANDLE CONTROL CHANGE

/**
 * @brief Handle MIDI control change messages
 * @param control CC number
 * @param value CC value (0-127)
 */
void MidiHandler::handleControlChange(byte control, byte value) {
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
      // Ignore other control change messages
      if (DEBUG_HANDLER) {
        Serial.print(F("CC"));
        Serial.print(control);
        Serial.print(F(": "));
        Serial.println(value);
      }
      break;
  }
}

//*********************************************************************************************
//******************               HANDLE SYSEX

/**
 * @brief Handle System Exclusive messages
 * @param data Pointer to SysEx data array
 * @param length Length of SysEx data
 * @note Currently implements device identification response
 */
void MidiHandler::handleSysEx(byte* data, unsigned int length) {
  // Check if this is an identity request (0x06, 0x01)
  if (length >= 2 && data[0] == 0x06 && data[1] == 0x01) {
    // Send identity response
    byte idResponse[] = {
      0xF0,  // SysEx start
      0x7E,  // Non-real-time ID
      0x7F,  // Device ID (broadcast)
      0x06,  // Sub-ID#1 (Identity Reply)
      0x02,  // Sub-ID#2 (General Reply)
      0x00, 0x01, 0x00,  // Manufacturer ID (example)
      0x00, 0x00, 0x00, 0x01,  // Device family (example)
      0x00, 0x00, 0x00, 0x01,  // Device model (example)
      0xF7   // SysEx end
    };

    MidiUSB.write(idResponse, sizeof(idResponse));
    MidiUSB.flush();

    if (DEBUG_HANDLER) {
      Serial.println(F("SysEx: Sent identity response"));
    }
  }
}
