/***********************************************************************************************************
classe pour gerer les action sur le xylophone 



les differents parametres et reglages du systeme pour ce servo est dans settings.h
***********************************************************************************************************/

#ifndef XYLOPHONE_H
#define XYLOPHONE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include "Settings.h"
#include "ServoMute.h"

class Xylophone {
public:
  Xylophone(); // initialise le xylophone et le servoMute et le timer pour la gestion de l'extinction des electromaimants
  void begin(); // initialise les pins en sorties et les objets comme le servo mute
  void playNote(byte note, byte velocity);// active la note selectionné
  void msgNoteOff(byte note);//gestion des message noteOff pour le servo mute
  void activateServoMute(bool value); //initialise la variable ServoMuteActive
  void reset();//desactive toout les note et met le servoMute en position init
  void checkNoteOff();// boucle pour arreter les elecroaimants après le temps indiqué 

private:
  //partie servoMute
  ServoMute _servoMute; // servomoteur pour gestion du silencieux
  bool _ServoMuteActive=true; // stocke si la gestion du servo pour le silencieux est activé 
  void setServoMute(bool value); // met le servo en position SERVO_ANGLE_NOTE_OFF si true sinon a SERVO_ANGLE_NOTE_ON 
  int _noteToMcpPin(byte note); // renvoi le numero de sortie du mcp en fct de la note
  
  //boucle du timer interupt
  void initTimer();//timer 1 pour la desactivation des electroaiamnts
  void stopNote(byte note);//coupe l'electroaimant apellé par checkNoteOff()
  
  //parties gestions des notes 
  Adafruit_MCP23X17 _mcp1;
  Adafruit_MCP23X17 _mcp2;  
  
  
  static const byte _instrumentStartNote= INSTRUMENT_START_NOTE;
  static const byte _instrumentRange = INSTRUMENT_RANGE;
  unsigned long _noteStartTime[INSTRUMENT_RANGE];
  bool _noteActive[INSTRUMENT_RANGE];
  int _playingNotesCount = 0;// nombre de notes/electroaimants actif 
  int _midiNotesCount = 0;// permet d'activer le servo mute aprés la derniere note desactivé
};

#endif // XYLOPHONE_H
