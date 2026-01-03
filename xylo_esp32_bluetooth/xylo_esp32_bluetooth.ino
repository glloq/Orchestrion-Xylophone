/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ----------------------------
------------------------------    XYLO_ESP32_BLUETOOTH.INO    -------------------------------------------
_________________________________________________________________________________________________________
Version ESP32 avec réception MIDI via Bluetooth (BLE)

Les différents paramètres et réglages du système sont dans settings.h

Bibliothèques requises:
- Adafruit_MCP23X17
- Ticker (ESP32)
- ESP32-BLE-MIDI (https://github.com/lathoub/Arduino-BLE-MIDI)

***********************************************************************************************************/

#include "MidiHandler.h"
#include "Xylophone.h"

// Les instances pour les objets Xylophone et MidiHandler
Xylophone xylophone;
MidiHandler midiHandler(xylophone);

void setup() {
  Serial.begin(115200);

  // Attendre que la connexion série soit établie (optionnel)
  delay(1000);

  Serial.println("===========================================");
  Serial.println("Orchestrion : Xylophone MIDI Controller");
  Serial.println("Version ESP32 avec Bluetooth BLE");
  Serial.println("===========================================");

  midiHandler.begin(); // Définition de tous les pins, I2C, BLE, etc

  // Test optionnel - décommenter pour tester au démarrage
  // midiHandler.test(true);  // Joue la mélodie spécifiée dans INIT_MELODY
  // midiHandler.test(false); // Joue toutes les notes l'une après l'autre

  Serial.println("Système prêt - En attente de connexion BLE MIDI...");
}

void loop() {
  // Mise à jour pour la gestion des électroaimants
  midiHandler.update();

  // Petite pause pour éviter de surcharger le CPU
  delay(1);
}
