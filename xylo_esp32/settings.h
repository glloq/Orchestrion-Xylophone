/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
----------------------------------   SETTINGS.H (ESP32 VERSION)   ----------------------------------------
_________________________________________________________________________________________________________

Configuration for ESP32 version with BLE/WiFi MIDI support

***********************************************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H
#include <Arduino.h>

// ==================== PLATFORM DETECTION ====================
#define PLATFORM_ESP32
#define ESP32_BOARD "ESP32-DevKitC"  // Change to your board

// ==================== DEBUG FLAGS ====================
#define DEBUG_HANDLER false
#define DEBUG_XYLO false
#define DEBUG_BLE false
#define DEBUG_WIFI false

// ==================== COMMUNICATION ====================
#define SERIAL_BAUD_RATE 115200  // Higher baud rate for ESP32
#define SERIAL_DISCONNECT_SAFETY_ENABLED true

// ==================== MIDI TRANSPORT ====================
// Choose MIDI transport method (only one should be enabled)
#define MIDI_USE_BLE true      // Bluetooth Low Energy MIDI
#define MIDI_USE_WIFI false    // WiFi MIDI (Apple MIDI / RTP-MIDI)
#define MIDI_USE_SERIAL false  // Serial MIDI (via hardware UART)

// BLE MIDI Configuration
#define BLE_DEVICE_NAME "Xylophone-ESP32"
#define BLE_MANUFACTURER "Orchestrion"

// WiFi MIDI Configuration (if MIDI_USE_WIFI = true)
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"
#define WIFI_MIDI_SESSION_NAME "Xylophone-ESP32"
#define WIFI_MIDI_PORT 5004

// ==================== ESP32 PIN DEFINITIONS ====================
// I2C pins for MCP23017
#define I2C_SDA_PIN 21  // ESP32 default SDA
#define I2C_SCL_PIN 22  // ESP32 default SCL

// Control pins
const byte EXTRA_OCTAVE_SWITCH_PIN = 4;
const int PWM_PIN = 25;  // PWM output pin for electromagnet power (any PWM-capable pin)

// PWM Configuration for ESP32
#define PWM_CHANNEL 0      // PWM channel (ESP32 has 16 channels)
#define PWM_FREQUENCY 5000 // PWM frequency in Hz
#define PWM_RESOLUTION 8   // 8-bit resolution (0-255)

// ==================== MIDI CHANNEL SELECTION ====================
#define ALL_CHANNEL true   // Read all MIDI channels
#define CHANNEL_XYLO 6     // If ALL_CHANNEL = false, only read this channel

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

// ==================== ESP32 SPECIFIC SETTINGS ====================
#define WATCHDOG_TIMEOUT_SECONDS 3  // Watchdog timeout for ESP32
#define CORE_DEBUG_LEVEL 0          // 0=None, 1=Error, 2=Warn, 3=Info, 4=Debug, 5=Verbose

// Task priorities for FreeRTOS
#define MIDI_TASK_PRIORITY 2
#define XYLOPHONE_TASK_PRIORITY 2
#define TASK_STACK_SIZE 4096

// ==================== PIN VALIDATION ====================
static_assert(EXTRA_OCTAVE_SWITCH_PIN != PWM_PIN,
              "Pin conflict: EXTRA_OCTAVE_SWITCH_PIN = PWM_PIN");
static_assert(EXTRA_OCTAVE_SWITCH_PIN != I2C_SDA_PIN && EXTRA_OCTAVE_SWITCH_PIN != I2C_SCL_PIN,
              "Pin conflict: EXTRA_OCTAVE_SWITCH uses I2C pins");
static_assert(PWM_PIN != I2C_SDA_PIN && PWM_PIN != I2C_SCL_PIN,
              "Pin conflict: PWM_PIN uses I2C pins");

// ==================== TRANSPORT VALIDATION ====================
#if MIDI_USE_BLE && MIDI_USE_WIFI
  #error "Cannot enable both BLE and WiFi MIDI simultaneously. Choose one."
#endif

#if !MIDI_USE_BLE && !MIDI_USE_WIFI && !MIDI_USE_SERIAL
  #error "At least one MIDI transport must be enabled (BLE, WiFi, or Serial)"
#endif

#endif // SETTINGS_H
