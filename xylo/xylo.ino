#include "MidiHandler.h"
#include "Xylophone.h"
#include "ServoVolume.h"

// Créez des instances pour les objets Xylophone, ServoVolume et MidiHandler
Xylophone xylophone;
ServoVolume servoVolume(SERVO_VOLUME_PIN);
MidiHandler midiHandler(xylophone, servoVolume);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Attendez que la connexion série soit établie
  }
  Serial.println("Orchestrion : Xylophone MIDI Controller");  
  midiHandler.begin();
}

void loop() {
  // Traitez les messages MIDI entrants
  midiHandler.handleMidiEvent();
}
