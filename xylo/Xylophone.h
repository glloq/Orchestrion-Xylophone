/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
---------------------------------------     XYLOPHONE.H    ----------------------------------------------
_________________________________________________________________________________________________________
classe pour gerer les actions sur le xylophone 
le xylophone gere les notes on et off avec le timer1 pour desactiver les electroaiamnt pour ne pas bloquer le code 
il est egalement possile de ne pas prendre en compte les note off en desactivant le servoMute 
( a voir pour utilisation notes étouffé a l'avenir avec un cc ?)

les differents parametres et reglages des notes et pour le servoMute sont dans settings.h
***********************************************************************************************************/

#ifndef XYLOPHONE_H
#define XYLOPHONE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include "Settings.h"
#include "ServoMute.h"
#include "Ticker.h"

class Xylophone {
public:
  Xylophone(); // initialise le xylophone et le servoMute et le timer pour la gestion de l'extinction des electromaimants
  void begin(); // initialise les pins en sorties et les objets comme le servo mute
  void playNote(byte note, byte velocity);// active la note selectionné
  void msgNoteOff(byte note);//gestion des message noteOff pour le servo mute
  void activateServoMute(bool value); //initialise la variable ServoMuteActive
  void reset();//desactive toutes les notes et met le servoMute en position init
  void checkNoteOff();// boucle pour arreter les elecroaimants après le temps indiqué 
  void update();
  
private:
  //partie servoMute
  ServoMute _servoMute; // servomoteur pour gestion du silencieux
  bool _ServoMuteActive=true; // stocke si la gestion du servo pour le silencieux est activé 
  void setServoMute(bool value); // met le servo en position SERVO_ANGLE_NOTE_OFF si true sinon a SERVO_ANGLE_NOTE_ON 
  int _noteToMcpPin(byte note); // renvoi le numero de sortie du mcp en fct de la note
  void stopNote(byte midiNote);
  //boucle du timer interupt
  Ticker _electromagnetTicker;
  static void _electromagnetTickerCallback();
  
  //parties gestions des notes 
  Adafruit_MCP23X17 _mcp1;
  Adafruit_MCP23X17 _mcp2;  
  void getMaxMagnetPinBelow16();//init the hightest number used on mcp1
  int _maxMcp1;

  static const byte _instrumentStartNote= INSTRUMENT_START_NOTE;
  static const byte _instrumentRange = INSTRUMENT_RANGE;
  unsigned long _noteStartTime[INSTRUMENT_RANGE];
  bool _noteActive[INSTRUMENT_RANGE];
  int _playingNotesCount = 0;// nombre de notes/electroaimants actif 
  int _midiNotesCount = 0;// permet d'activer le servo mute aprés la derniere note desactivé
};

#endif // XYLOPHONE_H
