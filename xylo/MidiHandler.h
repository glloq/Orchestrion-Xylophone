/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
---------------------------------------    MIDIHANDLER.H    ---------------------------------------------
_________________________________________________________________________________________________________

Class to handle MIDI messages received via USB using MIDIUSB.h
Reads different messages like noteOff, noteOn, certain control changes, and SysEx communication

MidiHandler is the brain of the system:
- Receives MIDI messages and decides what action the instrument should perform
- Verifies if reading all channels or only one specific channel
- noteOn: activates the note on xylophone if the note is in playable range (considering extra octave switch)
- noteOff: tracks note off messages for future use
- Control change: handles various CC messages for system control

MidiHandler initializes all necessary objects used in the system

***********************************************************************************************************/

#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H

#include "Xylophone.h"

/**
 * @class DebouncedSwitch
 * @brief Simple debouncing for digital input switches
 */
class DebouncedSwitch {
private:
  int _pin;
  bool _lastState;
  unsigned long _lastChangeTime;
  bool _debouncedState;

public:
  /**
   * @brief Constructor
   * @param pin Pin number for the switch (configured as INPUT_PULLUP)
   */
  DebouncedSwitch(int pin);

  /**
   * @brief Read debounced state of switch
   * @return true if switch is pressed (LOW), false otherwise
   */
  bool read();

  /**
   * @brief Update debounce state (call regularly)
   */
  void update();
};

/**
 * @class MidiHandler
 * @brief Handles MIDI message processing and routing
 */
class MidiHandler {
public:
  /**
   * @brief Constructor
   * @param xylophone Reference to Xylophone instance
   */
  MidiHandler(Xylophone& xylophone);

  /**
   * @brief Handle incoming MIDI events
   * @note Processes MIDI packets and routes to appropriate handlers
   */
  void handleMidiEvent();

  /**
   * @brief Initialize all components
   * @note Configures pins and initializes xylophone
   */
  void begin();

  /**
   * @brief Test function for startup
   * @param playMelody If true, plays INIT_MELODY; if false, plays all notes sequentially
   */
  void test(bool playMelody);

  /**
   * @brief Update components (xylophone ticker, debouncing)
   * @note Must be called in main loop
   */
  void update();

private:
  Xylophone& _xylophone;  ///< Reference to xylophone instance
  DebouncedSwitch _extraOctaveSwitch;  ///< Debounced switch for extra octave mode
  bool _extraOctaveEnabled;  ///< Whether extra octave mode is active

  // ==================== NOTE HANDLERS ====================
  /**
   * @brief Handle MIDI note on message
   * @param note MIDI note number
   * @param velocity MIDI velocity (0-127)
   */
  void handleNoteOn(byte note, byte velocity);

  /**
   * @brief Handle MIDI note off message
   * @param note MIDI note number
   */
  void handleNoteOff(byte note);

  /**
   * @brief Check if note is playable on xylophone
   * @param note MIDI note number
   * @return true if note is in playable range
   */
  bool isNotePlayable(byte note);

  // ==================== CONTROL CHANGE HANDLERS ====================
  /**
   * @brief Handle MIDI control change messages
   * @param control CC number
   * @param value CC value (0-127)
   */
  void handleControlChange(byte control, byte value);

  // ==================== SYSEX HANDLER ====================
  /**
   * @brief Handle System Exclusive messages
   * @param data Pointer to SysEx data array
   * @param length Length of SysEx data
   */
  void handleSysEx(byte* data, unsigned int length);
};

#endif  // MIDI_HANDLER_H
