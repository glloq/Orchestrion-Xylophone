/***********************************************************************************************************
classe pour gerer tout les reglages des actionneurs pour le projet Orchestion  => version Xylophone / gloskenspiel / vibraphone etc ...



les differents parametres et reglages du systeme pour ce servo est dans settings.h
***********************************************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H
#include <Arduino.h>

const byte EXTRA_OCTAVE_SWITCH_PIN = 4;
const byte SERVO_MUTE_PIN = 5;// pin pwm pour le servo du silencieux servoMute
const byte SERVO_VOLUME_PIN = 6; // pin pwm pour le servo du volume servoVolume
const int PWM_PIN = 9; //pin de sortie pour le PWM puissance alim electroaiamants

 
// selection channel midi 
#define ALL_CHANNEL true //lit tout les cannaux midi
#define CHANNEL_XYLO 6 //si ALL_CHANNEL = false on lit seulement ce channel

//reglages des notes jouables 
#define INSTRUMENT_START_NOTE 65
#define INSTRUMENT_RANGE 25

// temps d'activation electroaimant en ms
#define TIME_HIT 10
// valeur minimale pour le PWM
const int MIN_PWM_VALUE = 100; //pwm minimum pour activer l'electroaimant 
const int PWM_OFF_VALUE = 0; // valeur pour désactiver le PWM

//**** Définition des broches des PCM utilisé pour les electroaiamants 
const int magnetPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,            // 1er mcp
                         16, 17, 18, 19, 20, 21, 22, 23, 24, 25 ,26 ,27 };     // 2nd mcp

// reglage servo silencieux
const int SERVO_ANGLE_NOTE_ON = 100;
const int SERVO_ANGLE_NOTE_OFF = 40;

// reglage servo volume et vibrato/modulation
const int SERVO_VOLUME_MIN_ANGLE  = 100;
const int SERVO_VOLUME_MAX_ANGLE  = 20;
const int SERVO_VOLUME_MIN_ANGLE_VIBRATO = 40;
const float SERVO_VOLUME_MIN_FREQUENCY_VIBRATO = 0.2;//frequence minimum vibrato pour une valeur a 1
const float SERVO_VOLUME_MAX_FREQUENCY_VIBRATO = 4.0;;//frequence maximum vibrato pour une valeur a 127

//parametres des servo utilisé
const int SERVO_MIN_PULSE_WIDTH = 544;
const int SERVO_MAX_PULSE_WIDTH = 2400;
const int SERVO_FREQUENCY = 50; //frequence interuption pour la mise a jour du servo volume en vibrato en hz


// ... autres variables a vernir ...

#endif // SETTINGS_H
