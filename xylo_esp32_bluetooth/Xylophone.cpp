/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ----------------------------
---------------------------------------    XYLOPHONE.CPP   ----------------------------------------------
_________________________________________________________________________________________________________
Classe pour gérer les actions sur le xylophone - Version ESP32

***********************************************************************************************************/

#include "Xylophone.h"

// ----------------------------------      PUBLIC  --------------------------------------------

static Xylophone* XylophoneInstance;

Xylophone::Xylophone(): _electromagnetTicker(_electromagnetTickerCallback, 5, 0, MILLIS)   {
  getMaxMagnetPinBelow16();
  for (byte i = 0; i < INSTRUMENT_RANGE; i++) {
    _noteStartTime[i] = 0;
    _noteActive[i] = false;
  }
  XylophoneInstance = this;
}

//*********************************************************************************************
//******************             INITIALISE THE OBJECTS AND SETINGS

void Xylophone::begin() {
  // Initialisation I2C avec les pins spécifiques pour ESP32
  Wire.begin(I2C_SDA, I2C_SCL);

  if(DEBUG_XYLO){
    Serial.println("start Xylophone init (ESP32)");
  }

  if (!_mcp1.begin_I2C(MCP1_ADDR)) {
    Serial.println("Error mcp1");
    while (1);
  }

  if (!_mcp2.begin_I2C(MCP2_ADDR)) {
    Serial.println("Error mcp2");
    while (1);
  }

  for (byte i = 0; i < 16; i++) {
    _mcp1.pinMode(i, OUTPUT);
    _mcp2.pinMode(i, OUTPUT);
    _mcp1.digitalWrite(i, LOW);
    _mcp2.digitalWrite(i, LOW);
  }

  // Configuration PWM pour ESP32 avec LEDC
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PWM_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0); // Initialisation à 0

  if(DEBUG_XYLO){
    Serial.println("end Xylophone init");
  }
}

//*********************************************************************************************
//******************         GET THE LAST PIN USED ON MCP1

void Xylophone::getMaxMagnetPinBelow16() {
  int maxPin = -1;
  for (byte i = 0; i < INSTRUMENT_RANGE; i++) {
    if (magnetPins[i] < 16 && magnetPins[i] > maxPin) {
      maxPin = magnetPins[i];
    }
  }
  _maxMcp1 = maxPin;
}

//*********************************************************************************************
//******************          PLAY THE NOTE ON THE XYLOPHONE

void Xylophone::playNote(byte note, byte velocity) {
  int mcpPin = _noteToMcpPin(note);
  if (mcpPin != -1) {
    if(_playingNotesCount == 0)  {
      _electromagnetTicker.start();
    }

    // Mettre à jour le PWM en fonction de la vélocité (ESP32 utilise ledc)
    int pwmValue = map(velocity, 0, 127, MIN_PWM_VALUE, 255);
    ledcWrite(PWM_CHANNEL, pwmValue);

    // Active l'électroaimant
    if (mcpPin < 16) {
      _mcp1.digitalWrite(mcpPin, HIGH);
      _noteStartTime[mcpPin] = millis();
      _noteActive[mcpPin] = true;
    } else {
      _mcp2.digitalWrite(mcpPin - 16, HIGH);
      _noteStartTime[mcpPin - 16 + _maxMcp1 + 1] = millis();
      _noteActive[mcpPin - 16 + _maxMcp1 + 1] = true;
    }

    _playingNotesCount++;

    if(DEBUG_XYLO){
      Serial.print("playNote: ");
      Serial.print("note: ");
      Serial.print(note);
      Serial.print(", mcpPin: ");
      Serial.println(mcpPin);
      Serial.print("_playingNotesCount: ");
      Serial.println(_playingNotesCount);
    }
  }
}

//*********************************************************************************************
//******************            UPDATE THE TICKER FOR MAGNETS

void Xylophone::update() {
  _electromagnetTicker.update();
}

//*********************************************************************************************
//******************            RESET THE SETTINGS

void Xylophone::reset(){
  delay(20);
  checkNoteOff();
}

//*********************************************************************************************
//******************             CHECK NOTE TO TURN OFF

void Xylophone::checkNoteOff() {
  unsigned long time = 0;
  if (_playingNotesCount > 0){
    for (byte i = 0; i < _instrumentRange; i++) {
      if(_noteActive[i]){
        time = millis() - _noteStartTime[i];

        if(DEBUG_XYLO){
          Serial.print("time Note = ");
          Serial.println(time);
        }

        if (time >= TIME_HIT) {
          if(DEBUG_XYLO){
            Serial.print("checkNoteOff: Appel stopNote : ");
            Serial.println(i);
          }
          stopNote(i + INSTRUMENT_START_NOTE);
        }
      }
    }
  }
}

// ----------------------------------    PRIVATE   --------------------------------------------

//*********************************************************************************************
//******************             GET MCP OUTPUT NUMBER

int Xylophone::_noteToMcpPin(byte note) {
  int noteIndex = note - INSTRUMENT_START_NOTE;
  if (noteIndex >= 0 && noteIndex < INSTRUMENT_RANGE) {
    return magnetPins[noteIndex];
  }
  return -1;
}

//*********************************************************************************************
//******************             STOP NOTE

void Xylophone::stopNote(byte midiNote) {
  int noteIndex = midiNote - INSTRUMENT_START_NOTE;
  if (noteIndex >= 0 && noteIndex < INSTRUMENT_RANGE) {
    byte mcpPin = magnetPins[noteIndex];

    if (mcpPin != -1) {
      if (mcpPin < 16) {
        _mcp1.digitalWrite(mcpPin, LOW);
      } else {
        _mcp2.digitalWrite(mcpPin - 16, LOW);
      }
      _noteActive[noteIndex] = false;
      _playingNotesCount--;

      if(_playingNotesCount == 0)  {
        _electromagnetTicker.pause();
      }

      if(DEBUG_XYLO){
        Serial.print("stopNote: ");
        Serial.print("midiNote: ");
        Serial.print(midiNote);
        Serial.print(", mcpPin: ");
        Serial.println(mcpPin);
        Serial.print("_playingNotesCount: ");
        Serial.println(_playingNotesCount);
      }
    }
  }
}

void Xylophone::_electromagnetTickerCallback() {
  XylophoneInstance->checkNoteOff();
}
