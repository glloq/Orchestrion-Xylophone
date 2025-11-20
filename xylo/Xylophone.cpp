/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
---------------------------------------    XYLOPHONE.CPP   ----------------------------------------------
_________________________________________________________________________________________________________

Class to manage xylophone actions

***********************************************************************************************************/

#include "Xylophone.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// Static instance pointer for interrupt callback
static Xylophone* XylophoneInstance;

// ----------------------------------      PUBLIC  --------------------------------------------

/**
 * @brief Constructor - initializes tracking arrays and ticker
 */
Xylophone::Xylophone()
  : _electromagnetTicker(_electromagnetTickerCallback, 5, 0, MILLIS),
    _activeNoteCount(0),
    _electromagnetCount(0),
    _pendingNoteOffCount(0) {

  getMaxMagnetPinBelow16();

  for (byte i = 0; i < INSTRUMENT_RANGE; i++) {
    _noteStartTime[i] = 0;
    _noteActive[i] = false;
    _activeNoteIndices[i] = 0;
  }

  XylophoneInstance = this;
}

//*********************************************************************************************
//******************             INITIALIZE THE OBJECTS AND SETTINGS

/**
 * @brief Initialize I2C bus and MCP23017 expanders
 * @return true if successful, false if I2C devices not found
 */
bool Xylophone::begin() {
  Wire.begin();
  Wire.setClock(I2C_CLOCK_SPEED);

  if (DEBUG_XYLO) {
    Serial.println(F("Start Xylophone init"));
  }

  // Initialize MCP1 (low notes)
  unsigned long startTime = millis();
  bool mcp1Found = false;
  while (millis() - startTime < I2C_TIMEOUT_MS) {
    if (_mcpLowNotes.begin_I2C(MCP1_ADDR)) {
      mcp1Found = true;
      break;
    }
    delay(10);
  }

  if (!mcp1Found) {
    Serial.println(F("ERROR: MCP1 not responding"));
    return false;
  }

  // Initialize MCP2 (high notes)
  startTime = millis();
  bool mcp2Found = false;
  while (millis() - startTime < I2C_TIMEOUT_MS) {
    if (_mcpHighNotes.begin_I2C(MCP2_ADDR)) {
      mcp2Found = true;
      break;
    }
    delay(10);
  }

  if (!mcp2Found) {
    Serial.println(F("ERROR: MCP2 not responding"));
    return false;
  }

  // Configure all pins as outputs and set to LOW
  for (byte i = 0; i < 16; i++) {
    _mcpLowNotes.pinMode(i, OUTPUT);
    _mcpHighNotes.pinMode(i, OUTPUT);

    _mcpLowNotes.digitalWrite(i, LOW);
    _mcpHighNotes.digitalWrite(i, LOW);
  }

  pinMode(PWM_PIN, OUTPUT);  // Configure PWM pin as output

  if (DEBUG_XYLO) {
    Serial.println(F("End Xylophone init"));
  }

  return true;
}

//*********************************************************************************************
//******************         GET THE HIGHEST PIN USED ON MCP1

/**
 * @brief Calculate the highest pin number used on MCP1
 * @note Used for proper array indexing between MCP1 and MCP2
 */
void Xylophone::getMaxMagnetPinBelow16() {
  int maxPin = -1;
  for (byte i = 0; i < INSTRUMENT_RANGE; i++) {
    if (magnetPins[i] < 16 && magnetPins[i] > maxPin) {
      maxPin = magnetPins[i];
    }
  }
  _maxMcp1Pin = maxPin;
}

//*********************************************************************************************
//******************          PLAY THE NOTE ON THE XYLOPHONE

/**
 * @brief Activate electromagnet for the specified note
 * @param note MIDI note number
 * @param velocity MIDI velocity (0-127), mapped to PWM duty cycle
 */
void Xylophone::playNote(byte note, byte velocity) {
  int mcpPin = _noteToMcpPin(note);
  if (mcpPin == -1) {
    return;  // Note out of range
  }

  // Safety check: prevent too many simultaneous electromagnets
  if (_electromagnetCount >= MAX_SIMULTANEOUS_MAGNETS) {
    if (DEBUG_XYLO) {
      Serial.println(F("WARNING: Max simultaneous notes reached"));
    }
    return;
  }

  // Calculate note index for tracking arrays
  int noteIndex = note - INSTRUMENT_START_NOTE;
  if (noteIndex < 0 || noteIndex >= INSTRUMENT_RANGE) {
    return;  // Bounds check
  }

  // Check if note is already active (prevent duplicates)
  if (_noteActive[noteIndex]) {
    if (DEBUG_XYLO) {
      Serial.print(F("WARNING: Note "));
      Serial.print(note);
      Serial.println(F(" already active, ignoring"));
    }
    return;
  }

  // Critical section: disable interrupts while modifying shared data
  noInterrupts();
  {
    // Safety check: ensure active notes array won't overflow
    if (_activeNoteCount >= INSTRUMENT_RANGE) {
      if (DEBUG_XYLO) {
        Serial.println(F("ERROR: Active notes array full"));
      }
      interrupts();
      return;
    }

    // Start ticker if this is the first note
    if (_electromagnetCount == 0) {
      _electromagnetTicker.start();
    }

    // Map velocity to PWM value (higher velocity = more power)
    int pwmValue = map(velocity, 0, 127, MIN_PWM_VALUE, 255);
    analogWrite(PWM_PIN, pwmValue);

    // Activate electromagnet on appropriate MCP
    if (mcpPin < 16) {
      _mcpLowNotes.digitalWrite(mcpPin, HIGH);
      _noteStartTime[noteIndex] = millis();
      _noteActive[noteIndex] = true;
    } else {
      _mcpHighNotes.digitalWrite(mcpPin - 16, HIGH);
      _noteStartTime[noteIndex] = millis();
      _noteActive[noteIndex] = true;
    }

    // Track this note as active
    _activeNoteIndices[_activeNoteCount] = noteIndex;
    _activeNoteCount++;

    _electromagnetCount++;
    _pendingNoteOffCount++;
  }
  interrupts();  // Re-enable interrupts

  if (DEBUG_XYLO) {
    Serial.print(F("playNote: "));
    Serial.print(F("note="));
    Serial.print(note);
    Serial.print(F(", mcpPin="));
    Serial.print(mcpPin);
    Serial.print(F(", activeCount="));
    Serial.println(_electromagnetCount);
  }
}

//*********************************************************************************************
//******************            UPDATE THE TICKER FOR MAGNETS

/**
 * @brief Update ticker state (must be called in main loop)
 */
void Xylophone::update() {
  _electromagnetTicker.update();
}

//*********************************************************************************************
//******************             HANDLE MIDI NOTE OFF MESSAGE

/**
 * @brief Handle MIDI note off message
 * @param note MIDI note number
 * @note Currently just tracks pending notes for future use
 */
void Xylophone::msgNoteOff(byte note) {
  noInterrupts();
  _pendingNoteOffCount--;
  if (_pendingNoteOffCount < 0) {
    _pendingNoteOffCount = 0;
  }
  interrupts();
}

//*********************************************************************************************
//******************            RESET ALL NOTES AND SETTINGS

/**
 * @brief Reset all electromagnets and internal state
 */
void Xylophone::reset() {
  delay(20);       // Wait to ensure no notes are active
  checkNoteOff();  // Force deactivate all electromagnets

  noInterrupts();
  _pendingNoteOffCount = 0;
  _electromagnetCount = 0;      // FIXED: Was duplicated as _midiNotesCount
  _activeNoteCount = 0;
  interrupts();

  if (DEBUG_XYLO) {
    Serial.println(F("Xylophone reset"));
  }
}

//*********************************************************************************************
//******************             CHECK NOTES TO TURN OFF

/**
 * @brief Check active notes and deactivate electromagnets after timeout
 * @note Called by ticker interrupt - optimized to only check active notes
 */
void Xylophone::checkNoteOff() {
  if (_activeNoteCount == 0) {
    return;  // No notes to check
  }

  unsigned long now = millis();

  // OPTIMIZED: Only iterate through active notes, not all 25 notes
  for (byte i = 0; i < _activeNoteCount; i++) {
    byte noteIndex = _activeNoteIndices[i];

    if (_noteActive[noteIndex]) {
      unsigned long elapsed = now - _noteStartTime[noteIndex];

      // Safety timeout: use minimum of TIME_HIT and ELECTROMAGNET_MAX_ON_TIME_MS
      unsigned long timeout = min((unsigned long)TIME_HIT, (unsigned long)ELECTROMAGNET_MAX_ON_TIME_MS);

      if (elapsed >= timeout) {
        if (DEBUG_XYLO && elapsed >= ELECTROMAGNET_MAX_ON_TIME_MS) {
          Serial.print(F("WARNING: Safety timeout for note "));
          Serial.println(noteIndex + INSTRUMENT_START_NOTE);
        }

        stopNote(noteIndex + INSTRUMENT_START_NOTE);

        // Remove from active list by shifting remaining elements
        for (byte j = i; j < _activeNoteCount - 1; j++) {
          _activeNoteIndices[j] = _activeNoteIndices[j + 1];
        }
        _activeNoteCount--;
        i--;  // Re-check this position
      }
    }
  }
}

// ----------------------------------    PRIVATE   --------------------------------------------

//*********************************************************************************************
//******************             GET MCP OUTPUT NUMBER

/**
 * @brief Convert MIDI note to MCP pin number
 * @param note MIDI note number
 * @return MCP pin number, or -1 if invalid
 */
int Xylophone::_noteToMcpPin(byte note) {
  int noteIndex = note - INSTRUMENT_START_NOTE;
  if (noteIndex >= 0 && noteIndex < INSTRUMENT_RANGE) {
    int pin = magnetPins[noteIndex];

    // Validate pin is in expected ranges (0-15 for MCP1, 16-31 for MCP2)
    if ((pin >= 0 && pin < 16) || (pin >= 16 && pin < 32)) {
      return pin;
    }

    // Invalid pin mapping detected
    if (DEBUG_XYLO) {
      Serial.print(F("ERROR: Invalid pin mapping ("));
      Serial.print(pin);
      Serial.print(F(") for note "));
      Serial.println(note);
    }
  }
  return -1;  // Invalid note or pin
}

//*********************************************************************************************
//******************             STOP NOTE (DEACTIVATE ELECTROMAGNET)

/**
 * @brief Deactivate electromagnet for specified note
 * @param midiNote MIDI note number to stop
 */
void Xylophone::stopNote(byte midiNote) {
  int noteIndex = midiNote - INSTRUMENT_START_NOTE;

  // Bounds checking
  if (noteIndex < 0 || noteIndex >= INSTRUMENT_RANGE) {
    if (DEBUG_XYLO) {
      Serial.print(F("ERROR: stopNote index out of bounds: "));
      Serial.println(noteIndex);
    }
    return;
  }

  byte mcpPin = magnetPins[noteIndex];
  if (mcpPin == 255 || mcpPin == -1) {  // Invalid pin
    return;
  }

  // Critical section: disable interrupts while modifying shared data
  noInterrupts();
  {
    // Deactivate electromagnet on appropriate MCP
    if (mcpPin < 16) {
      _mcpLowNotes.digitalWrite(mcpPin, LOW);
    } else {
      _mcpHighNotes.digitalWrite(mcpPin - 16, LOW);
    }

    _noteActive[noteIndex] = false;
    _electromagnetCount--;

    // Ensure count doesn't go negative
    if (_electromagnetCount < 0) {
      _electromagnetCount = 0;
    }

    // Stop ticker if no more notes playing
    if (_electromagnetCount == 0) {
      _electromagnetTicker.pause();
      analogWrite(PWM_PIN, PWM_OFF_VALUE);  // Turn off PWM
    }
  }
  interrupts();  // Re-enable interrupts

  if (DEBUG_XYLO) {
    Serial.print(F("stopNote: "));
    Serial.print(F("note="));
    Serial.print(midiNote);
    Serial.print(F(", mcpPin="));
    Serial.print(mcpPin);
    Serial.print(F(", activeCount="));
    Serial.println(_electromagnetCount);
  }
}

/**
 * @brief Static callback for ticker interrupt
 */
void Xylophone::_electromagnetTickerCallback() {
  if (XylophoneInstance) {
    XylophoneInstance->checkNoteOff();
  }
}
