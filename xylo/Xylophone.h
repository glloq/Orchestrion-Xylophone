/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
---------------------------------------     XYLOPHONE.H    ----------------------------------------------
_________________________________________________________________________________________________________
Classe pour gérer les actions sur le xylophone
Le xylophone gère les notes on et off avec un timer pour désactiver les électroaimants
après un temps défini sans bloquer le code.

Les différents paramètres et réglages des notes sont dans settings.h
***********************************************************************************************************/

#ifndef XYLOPHONE_H
#define XYLOPHONE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include "settings.h"
#include "Ticker.h"

class Xylophone {
public:
  Xylophone(); // initialise le xylophone et le timer pour la gestion de l'extinction des electromaimants
  void begin(); // initialise les pins en sorties
  void playNote(byte note, byte velocity);// active la note selectionné
  void reset();//desactive toutes les notes
  void checkNoteOff();// boucle pour arreter les elecroaimants après le temps indiqué
  void update();

private:
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
};

#endif // XYLOPHONE_H
