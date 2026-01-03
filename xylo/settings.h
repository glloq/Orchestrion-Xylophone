/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
------------------------------------------   SETTINGS.H   -----------------------------------------------
_________________________________________________________________________________________________________


***********************************************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H
#include <Arduino.h>
#define DEBUG_HANDLER false
#define DEBUG_XYLO 1
#define DEBUG_SEROVOLUME false


//definition des pins utilisé pour les differentes entrées/sorties
const byte EXTRA_OCTAVE_SWITCH_PIN = 4;
const int PWM_PIN = 6; //pin de sortie pour le PWM puissance alim electroaiamants

 
// selection channel midi 
#define ALL_CHANNEL true //lit tout les cannaux midi
#define CHANNEL_XYLO 6 //si ALL_CHANNEL = false on lit seulement ce channel

//reglages des notes jouables 
const byte INSTRUMENT_START_NOTE= 65;
const byte INSTRUMENT_RANGE= 25;

// temps d'activation electroaimant en ms
#define TIME_HIT 20

// valeur minimale pour le PWM
const int MIN_PWM_VALUE = 100; //pwm minimum pour activer l'electroaimant 
const int PWM_OFF_VALUE = 0; // valeur pour désactiver le PWM

//**** Définition des broches des MCP utilisé pour les electroaiamants 
const int magnetPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,            // 1er mcp
                         16, 17, 18, 19, 20, 21, 22, 23, 24, 25 ,26 ,27 };     // 2nd mcp

//adresses des mcp
#define MCP1_ADDR  0x20 
#define MCP2_ADDR  0x21 


// meloldie joué par la fonction test au demmarage si on utilise test(true) au setup
const byte INIT_MELODY[] = {60, 62, 64, 65, 67, 69, 71, 72};
const byte INIT_MELODY_DELAY[] = {200, 200, 200, 200, 200, 200, 200, 200};

/*// strip led
#define LED_PIN 6 // La broche utilisée pour contrôler le bandeau LED
#define LED_COUNT 30 // Le nombre de LEDs sur le bandeau
*/
// ... autres variables a venir ...
// ex potar pour gestion servo vulume et servo mute
#endif // SETTINGS_H
