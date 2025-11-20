/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
------------------------------         XYLO_ESP32.INO         -------------------------------------------
_________________________________________________________________________________________________________

ESP32 version with BLE/WiFi MIDI support
System parameters and settings are in settings.h

***********************************************************************************************************/

#include <esp_task_wdt.h>
#include "MidiHandler.h"
#include "Xylophone.h"

// Create instances for Xylophone and MidiHandler objects
Xylophone xylophone;
MidiHandler midiHandler(xylophone);

// Connection monitoring
bool _midiWasConnected = false;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);  // Give time for serial to initialize

  Serial.println(F("\n\n========================================="));
  Serial.println(F("  Orchestrion: Xylophone MIDI Controller"));
  Serial.println(F("           ESP32 Version"));
  Serial.println(F("=========================================\n"));

  // Configure watchdog timer for ESP32 (3 seconds timeout)
  esp_task_wdt_init(WATCHDOG_TIMEOUT_SECONDS, true);
  esp_task_wdt_add(NULL);  // Add current task to watchdog

  Serial.print(F("Free heap: "));
  Serial.print(ESP.getFreeHeap());
  Serial.println(F(" bytes"));

  // Initialize I2C with custom pins
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Serial.print(F("I2C initialized: SDA="));
  Serial.print(I2C_SDA_PIN);
  Serial.print(F(", SCL="));
  Serial.println(I2C_SCL_PIN);

  // Initialize MIDI handler (which initializes xylophone and MIDI transport)
  Serial.println(F("\nInitializing system..."));
  if (!midiHandler.begin()) {
    Serial.println(F("\n╔════════════════════════════════════════╗"));
    Serial.println(F("║  CRITICAL ERROR: Init Failed           ║"));
    Serial.println(F("╚════════════════════════════════════════╝"));
    Serial.println(F("\nPlease check:"));
    Serial.println(F("  1. I2C connections (SDA/SCL)"));
    Serial.println(F("  2. MCP23017 addresses (0x20, 0x21)"));
    Serial.println(F("  3. Power supply to MCP23017"));
#if MIDI_USE_WIFI
    Serial.println(F("  4. WiFi credentials in settings.h"));
#endif
    Serial.println(F("\nSystem halted."));

    while (1) {
      esp_task_wdt_reset();  // Keep resetting watchdog
      delay(1000);
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  // Blink LED
    }
  }

  Serial.println(F("\n✓ System initialized successfully!\n"));

  // Print MIDI transport info
#if MIDI_USE_BLE
  Serial.println(F("MIDI Transport: Bluetooth Low Energy"));
  Serial.println(F("Connect via BLE MIDI from your device"));
  Serial.print(F("Device name: "));
  Serial.println(BLE_DEVICE_NAME);
#elif MIDI_USE_WIFI
  Serial.println(F("MIDI Transport: WiFi (Apple MIDI)"));
  Serial.print(F("Session name: "));
  Serial.println(WIFI_MIDI_SESSION_NAME);
  Serial.print(F("IP Address: "));
  Serial.println(WiFi.localIP());
#elif MIDI_USE_SERIAL
  Serial.println(F("MIDI Transport: Serial"));
#endif

  Serial.println();

  // Play test sequence
  // Uncomment one of the following:
  // midiHandler.test(true);   // Play INIT_MELODY
  midiHandler.test(false);  // Play all notes sequentially

  Serial.println(F("\n✓ Ready to receive MIDI!"));
  Serial.println(F("==========================================\n"));

  // Configure built-in LED for connection status
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // Reset watchdog timer
  esp_task_wdt_reset();

  // Process MIDI events
  midiHandler.handleMidiEvent();

  // Update components (xylophone ticker, debouncing, etc.)
  midiHandler.update();

  // Connection status monitoring with LED
  bool connected = midiHandler.isConnected();

  if (connected && !_midiWasConnected) {
    Serial.println(F("\n✓ MIDI device connected!"));
    _midiWasConnected = true;
    digitalWrite(LED_BUILTIN, HIGH);  // LED on when connected
  } else if (!connected && _midiWasConnected) {
    Serial.println(F("\n✗ MIDI device disconnected"));
    xylophone.reset();  // Safety: turn off all electromagnets
    _midiWasConnected = false;
    digitalWrite(LED_BUILTIN, LOW);  // LED off when disconnected
  }

  // Blink LED slowly when waiting for connection
  if (!connected) {
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 1000) {
      lastBlink = millis();
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }

  // Small delay to prevent task watchdog timeout
  delay(1);
}
