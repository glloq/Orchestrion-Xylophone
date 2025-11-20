/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Orchestrion Project  : Xylophone/Glockenspiel      ----------------------------
----------------------------------         XYLO.INO         ---------------------------------------------
_________________________________________________________________________________________________________

System parameters and settings are in settings.h

***********************************************************************************************************/

#include <avr/wdt.h>
#include "MidiHandler.h"
#include "Xylophone.h"

// Create instances for Xylophone and MidiHandler objects
Xylophone xylophone;
MidiHandler midiHandler(xylophone);

void setup() {
  // Enable watchdog timer: reset if code doesn't respond within 2 seconds
  wdt_enable(WDTO_2S);

  Serial.begin(SERIAL_BAUD_RATE);

  // Initialize MIDI handler (which initializes xylophone)
  midiHandler.begin();
  Serial.println(F("Orchestrion: Xylophone MIDI Controller"));

  // Test modes: play init melody or play all notes sequentially
  // midiHandler.test(true);  // Play the melody specified in INIT_MELODY
  midiHandler.test(false);   // Play all notes one after another with 200ms between each note
}

bool _serialConnected = true;

void loop() {
  // Pet the watchdog to prevent system reset
  wdt_reset();

  // Process incoming MIDI messages
  midiHandler.handleMidiEvent();
  midiHandler.update(); // Update electromagnet timing

  // Safety feature: disable electromagnets if serial connection is lost
  if (Serial && _serialConnected) {
    // Serial connection is still active
  } else if (!Serial && _serialConnected) {
    // Serial connection was interrupted
    xylophone.reset();
    _serialConnected = false;
  } else if (Serial && !_serialConnected) {
    // Serial connection was restored
    _serialConnected = true;
  }
}
