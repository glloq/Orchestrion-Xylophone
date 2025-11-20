/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
----------------------------------    MIDIHANDLER.H (ESP32 VERSION)    ----------------------------------
_________________________________________________________________________________________________________

Class to handle MIDI messages received via BLE/WiFi using ESP32
Reads different messages like noteOff, noteOn, certain control changes, and SysEx communication

MidiHandler is the brain of the system:
- Receives MIDI messages via BLE/WiFi and decides what action the instrument should perform
- Verifies if reading all channels or only one specific channel
- noteOn: activates the note on xylophone if the note is in playable range (considering extra octave switch)
- noteOff: tracks note off messages for future use
- Control change: handles various CC messages for system control

***********************************************************************************************************/

#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H

#include "Xylophone.h"

// MIDI transport includes based on configuration
#if MIDI_USE_BLE
  #include <BLEMidi.h>  // ESP32-BLE-MIDI library
#endif

#if MIDI_USE_WIFI
  #include <WiFi.h>
  #include <AppleMIDI.h>  // AppleMIDI library for WiFi MIDI
#endif

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
 * @brief Handles MIDI message processing and routing for ESP32
 */
class MidiHandler {
public:
  /**
   * @brief Constructor
   * @param xylophone Reference to Xylophone instance
   */
  MidiHandler(Xylophone& xylophone);

  /**
   * @brief Initialize all components (BLE/WiFi, pins, xylophone)
   * @return true if initialization successful, false otherwise
   */
  bool begin();

  /**
   * @brief Handle incoming MIDI events
   * @note Processes MIDI packets and routes to appropriate handlers
   */
  void handleMidiEvent();

  /**
   * @brief Test function for startup
   * @param playMelody If true, plays INIT_MELODY; if false, plays all notes sequentially
   */
  void test(bool playMelody);

  /**
   * @brief Update components (xylophone ticker, debouncing, connection status)
   * @note Must be called in main loop
   */
  void update();

  /**
   * @brief Check if MIDI connection is active
   * @return true if connected (BLE or WiFi)
   */
  bool isConnected();

private:
  Xylophone& _xylophone;  ///< Reference to xylophone instance
  DebouncedSwitch _extraOctaveSwitch;  ///< Debounced switch for extra octave mode
  bool _extraOctaveEnabled;  ///< Whether extra octave mode is active
  bool _connected;  ///< Connection status

#if MIDI_USE_WIFI
  unsigned long _lastWiFiCheck;  ///< Last time WiFi status was checked
  bool _wifiConnected;  ///< WiFi connection status
#endif

  // ==================== MIDI CALLBACKS ====================
#if MIDI_USE_BLE
  /**
   * @brief BLE MIDI connected callback
   */
  static void onBLEConnected();

  /**
   * @brief BLE MIDI disconnected callback
   */
  static void onBLEDisconnected();
#endif

#if MIDI_USE_WIFI
  /**
   * @brief WiFi MIDI connected callback
   */
  static void onWiFiConnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc, const char* name);

  /**
   * @brief WiFi MIDI disconnected callback
   */
  static void onWiFiDisconnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc);
#endif

  // ==================== NOTE HANDLERS ====================
  /**
   * @brief Handle MIDI note on message
   * @param channel MIDI channel (0-15)
   * @param note MIDI note number
   * @param velocity MIDI velocity (0-127)
   */
  void handleNoteOn(byte channel, byte note, byte velocity);

  /**
   * @brief Handle MIDI note off message
   * @param channel MIDI channel (0-15)
   * @param note MIDI note number
   * @param velocity Release velocity
   */
  void handleNoteOff(byte channel, byte note, byte velocity);

  /**
   * @brief Check if note is playable on xylophone
   * @param note MIDI note number
   * @return true if note is in playable range
   */
  bool isNotePlayable(byte note);

  // ==================== CONTROL CHANGE HANDLERS ====================
  /**
   * @brief Handle MIDI control change messages
   * @param channel MIDI channel (0-15)
   * @param control CC number
   * @param value CC value (0-127)
   */
  void handleControlChange(byte channel, byte control, byte value);

  /**
   * @brief Initialize BLE MIDI
   * @return true if successful
   */
  bool initBLE();

  /**
   * @brief Initialize WiFi MIDI
   * @return true if successful
   */
  bool initWiFi();

  // Static instance pointer for callbacks
  static MidiHandler* _instance;
};

#endif  // MIDI_HANDLER_H
