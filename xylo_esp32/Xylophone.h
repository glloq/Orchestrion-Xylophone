/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
---------------------------------------     XYLOPHONE.H    ----------------------------------------------
_________________________________________________________________________________________________________

Class to manage xylophone actions.
The xylophone manages note on/off events with a ticker timer to deactivate electromagnets
without blocking the code execution.

System parameters and settings are in settings.h
***********************************************************************************************************/

#ifndef XYLOPHONE_H
#define XYLOPHONE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include "settings.h"
#include <Ticker.h>  // ESP32 native Ticker library

/**
 * @class Xylophone
 * @brief Controls electromagnets for xylophone note activation
 *
 * Manages 25 solenoid electromagnets via two MCP23017 I2C expanders.
 * Uses ticker-based interrupts for automatic electromagnet deactivation.
 */
class Xylophone {
public:
  /**
   * @brief Constructor - initializes xylophone and ticker timer
   */
  Xylophone();

  /**
   * @brief Initialize hardware (I2C, MCP expanders, pins)
   * @return true if initialization successful, false otherwise
   * @note Blocks indefinitely if I2C devices not found (will be improved)
   */
  bool begin();

  /**
   * @brief Play a note on the xylophone
   * @param note MIDI note number (0-127), must be in playable range
   * @param velocity Note velocity (0-127): 0 = silent, 127 = maximum force
   * @note Velocity is mapped to PWM value for electromagnet power control
   */
  void playNote(byte note, byte velocity);

  /**
   * @brief Handle MIDI note off message
   * @param note MIDI note number that ended
   * @note Currently not used as electromagnets auto-deactivate after TIME_HIT
   */
  void msgNoteOff(byte note);

  /**
   * @brief Reset all electromagnets and counters
   * @note Deactivates all notes and resets internal state
   */
  void reset();

  /**
   * @brief Check and stop electromagnets that have exceeded TIME_HIT
   * @note Called by ticker interrupt callback
   */
  void checkNoteOff();

  /**
   * @brief Update ticker state (must be called in main loop)
   */
  void update();

private:
  // ==================== ELECTROMAGNET MANAGEMENT ====================
  /**
   * @brief Convert MIDI note to MCP pin number
   * @param note MIDI note number
   * @return MCP pin number, or -1 if note out of range
   */
  int _noteToMcpPin(byte note);

  /**
   * @brief Stop a specific note (deactivate electromagnet)
   * @param midiNote MIDI note number to stop
   */
  void stopNote(byte midiNote);

  /**
   * @brief Calculate the highest MCP1 pin number used
   * @note Used for proper array indexing between MCP1 and MCP2
   */
  void getMaxMagnetPinBelow16();

  // ==================== MCP23017 I2C EXPANDERS ====================
  Adafruit_MCP23X17 _mcpLowNotes;   ///< MCP for notes 1-16 (pins 0-15)
  Adafruit_MCP23X17 _mcpHighNotes;  ///< MCP for notes 17-32 (pins 16-31)
  int _maxMcp1Pin;                   ///< Highest pin number used on MCP1

  // ==================== TICKER INTERRUPT ====================
  Ticker _electromagnetTicker;               ///< Timer for electromagnet auto-shutoff
  static void _electromagnetTickerCallback(); ///< Static callback for ticker

  // ==================== NOTE TRACKING ====================
  static const byte _instrumentStartNote = INSTRUMENT_START_NOTE;
  static const byte _instrumentRange = INSTRUMENT_RANGE;

  unsigned long _noteStartTime[INSTRUMENT_RANGE]; ///< Timestamp when each note was activated
  bool _noteActive[INSTRUMENT_RANGE];             ///< Whether each note is currently active
  byte _activeNoteIndices[INSTRUMENT_RANGE];      ///< Indices of currently active notes
  byte _activeNoteCount;                          ///< Number of notes currently active

  int _electromagnetCount;  ///< Number of electromagnets currently ON
  int _pendingNoteOffCount; ///< MIDI notes received but not yet released
};

#endif // XYLOPHONE_H
