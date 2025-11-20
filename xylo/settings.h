/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
------------------------------------------   SETTINGS.H   -----------------------------------------------
_________________________________________________________________________________________________________

***********************************************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H
#include <Arduino.h>

// ==================== DEBUG FLAGS ====================
#define DEBUG_HANDLER false
#define DEBUG_XYLO false

// ==================== SERIAL COMMUNICATION ====================
#define SERIAL_BAUD_RATE 9600
#define SERIAL_DISCONNECT_SAFETY_ENABLED true

// ==================== PIN DEFINITIONS ====================
// Pin definitions for various inputs/outputs
const byte EXTRA_OCTAVE_SWITCH_PIN = 4;
const int PWM_PIN = 6;  // PWM output pin for electromagnet power supply

// ==================== MIDI CHANNEL SELECTION ====================
#define ALL_CHANNEL true         // Read all MIDI channels
#define CHANNEL_XYLO 6          // If ALL_CHANNEL = false, only read this channel

// ==================== PLAYABLE NOTE RANGE ====================
const byte INSTRUMENT_START_NOTE = 65;  // MIDI note number for first key
const byte INSTRUMENT_RANGE = 25;       // Number of notes (17-32 supported)

// Compile-time validation
static_assert(INSTRUMENT_START_NOTE >= 0 && INSTRUMENT_START_NOTE <= 127,
              "INSTRUMENT_START_NOTE must be valid MIDI note (0-127)");
static_assert(INSTRUMENT_START_NOTE + INSTRUMENT_RANGE <= 128,
              "INSTRUMENT_START_NOTE + INSTRUMENT_RANGE exceeds MIDI range");
static_assert(INSTRUMENT_RANGE >= 17 && INSTRUMENT_RANGE <= 32,
              "INSTRUMENT_RANGE must be between 17 and 32");

// ==================== ELECTROMAGNET TIMING ====================
#define TIME_HIT 20                        // Electromagnet activation time in ms
#define ELECTROMAGNET_MAX_ON_TIME_MS 100   // Safety timeout to prevent coil burn-out

// ==================== PWM CONFIGURATION ====================
const int MIN_PWM_VALUE = 100;    // Minimum PWM to activate electromagnet
const int PWM_OFF_VALUE = 0;      // PWM value to disable electromagnet

// ==================== SAFETY LIMITS ====================
#define MAX_SIMULTANEOUS_MAGNETS 8  // Maximum number of electromagnets active at once

// ==================== MCP23017 PIN DEFINITIONS ====================
// Pin assignments for electromagnets on MCP23017 expanders
// First 13 pins on MCP1 (address 0x20), remaining pins on MCP2 (address 0x21)
const int magnetPins[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,            // MCP1 (pins 0-12)
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27      // MCP2 (pins 16-27, offset by 16)
};

// ==================== MCP23017 I2C ADDRESSES ====================
#define MCP1_ADDR 0x20  // I2C address for first MCP23017
#define MCP2_ADDR 0x21  // I2C address for second MCP23017

// ==================== I2C CONFIGURATION ====================
#define I2C_CLOCK_SPEED 400000  // 400kHz for faster I2C communication
#define I2C_TIMEOUT_MS 1000     // I2C initialization timeout

// ==================== TEST CONFIGURATION ====================
// Melody played by test function at startup if test(true) is called
const byte INIT_MELODY[] = {60, 62, 64, 65, 67, 69, 71, 72};
const byte INIT_MELODY_DELAY[] = {200, 200, 200, 200, 200, 200, 200, 200};
#define TEST_SINGLE_NOTE_DURATION_MS 20
#define TEST_INTER_NOTE_DELAY_MS 200

// ==================== DEBOUNCE CONFIGURATION ====================
#define SWITCH_DEBOUNCE_TIME_MS 50  // Debounce time for switches

// ==================== PIN VALIDATION ====================
// Validate that pins don't conflict (Leonardo has pins 0-13 digital, A0-A5 analog)
static_assert(EXTRA_OCTAVE_SWITCH_PIN != PWM_PIN, "Pin conflict detected: EXTRA_OCTAVE_SWITCH_PIN = PWM_PIN");

#endif // SETTINGS_H
