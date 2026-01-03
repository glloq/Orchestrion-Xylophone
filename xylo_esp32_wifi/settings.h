/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ----------------------------
------------------------------------------   SETTINGS.H   -----------------------------------------------
_________________________________________________________________________________________________________
Configuration pour ESP32 avec MIDI WiFi (AppleMIDI/RTP-MIDI)

***********************************************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H
#include <Arduino.h>
#define DEBUG_HANDLER false
#define DEBUG_XYLO 1

// Configuration WiFi
#define WIFI_SSID "VotreSSID"           // À modifier : nom de votre réseau WiFi
#define WIFI_PASSWORD "VotreMotDePasse" // À modifier : mot de passe WiFi
#define APPLEMIDI_SESSION_NAME "Xylophone-WiFi"

//definition des pins utilisé pour les differentes entrées/sorties (ESP32)
const byte EXTRA_OCTAVE_SWITCH_PIN = 4;
const int PWM_PIN = 25; // Pin PWM pour le contrôle de puissance des électroaimants

// Pins I2C pour ESP32 (par défaut SDA=21, SCL=22, mais on peut les redéfinir)
const int I2C_SDA = 21;
const int I2C_SCL = 22;

// selection channel midi
#define ALL_CHANNEL true //lit tout les cannaux midi
#define CHANNEL_XYLO 1 //si ALL_CHANNEL = false on lit seulement ce channel

//reglages des notes jouables
const byte INSTRUMENT_START_NOTE = 65;
const byte INSTRUMENT_RANGE = 25;

// temps d'activation electroaimant en ms
#define TIME_HIT 20

// valeur minimale pour le PWM (ESP32 utilise 0-255 pour analogWrite avec ledc)
const int MIN_PWM_VALUE = 100; //pwm minimum pour activer l'electroaimant
const int PWM_OFF_VALUE = 0; // valeur pour désactiver le PWM

// Configuration PWM pour ESP32
const int PWM_CHANNEL = 0;  // Canal PWM (0-15)
const int PWM_FREQ = 5000;  // Fréquence PWM en Hz
const int PWM_RESOLUTION = 8; // Résolution 8 bits (0-255)

//**** Définition des broches des MCP utilisé pour les electroaiamants
const int magnetPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,            // 1er mcp
                         16, 17, 18, 19, 20, 21, 22, 23, 24, 25 ,26 ,27 };     // 2nd mcp

//adresses des mcp
#define MCP1_ADDR  0x20
#define MCP2_ADDR  0x21

// meloldie joué par la fonction test au demmarage si on utilise test(true) au setup
const byte INIT_MELODY[] = {60, 62, 64, 65, 67, 69, 71, 72};
const byte INIT_MELODY_DELAY[] = {200, 200, 200, 200, 200, 200, 200, 200};

// ... autres variables a venir ...
#endif // SETTINGS_H
