/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ----------------------------
--------------------------------    XYLO_ESP32_WIFI.INO    ----------------------------------------------
_________________________________________________________________________________________________________
Version ESP32 avec réception MIDI via WiFi (AppleMIDI/RTP-MIDI)

Les différents paramètres et réglages du système sont dans settings.h
IMPORTANT: Modifiez WIFI_SSID et WIFI_PASSWORD dans settings.h avant téléversement

Bibliothèques requises:
- Adafruit_MCP23X17
- Ticker (ESP32)
- AppleMIDI (https://github.com/lathoub/Arduino-AppleMIDI-Library)
- WiFi (inclus avec ESP32)

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
  Serial.println("Version ESP32 avec WiFi (AppleMIDI)");
  Serial.println("===========================================");

  midiHandler.begin(); // Définition de tous les pins, I2C, WiFi, AppleMIDI, etc

  // Test optionnel - décommenter pour tester au démarrage
  // midiHandler.test(true);  // Joue la mélodie spécifiée dans INIT_MELODY
  // midiHandler.test(false); // Joue toutes les notes l'une après l'autre

  Serial.println("Système prêt - En attente de connexion AppleMIDI...");
  Serial.println("Utilisez une application compatible AppleMIDI pour vous connecter");
}

void loop() {
  // Mise à jour pour la gestion des électroaimants et réception MIDI
  midiHandler.update();

  // Petite pause pour éviter de surcharger le CPU
  delay(1);
}
