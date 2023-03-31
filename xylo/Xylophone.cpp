/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
---------------------------------------    XYLOPHONE.CPP   ----------------------------------------------
_________________________________________________________________________________________________________
classe pour gerer les actions sur le xylophone 

***********************************************************************************************************/


#include "Xylophone.h"
#include <avr/interrupt.h>
#include <avr/io.h>
// ----------------------------------      PUBLIC  --------------------------------------------

static Xylophone* XylophoneInstance;

Xylophone::Xylophone(): _servoMute(SERVO_MUTE_PIN) , _electromagnetTicker(_electromagnetTickerCallback, 5, 0, MILLIS)   {
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
  Wire.begin();
 if(DEBUG_XYLO){
    Serial.println("start Xyophone init"); 
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
  pinMode(PWM_PIN, OUTPUT);// Définition de la broche PWM en tant que SORTIE   

  activateServoMute(true);//active la gestion du servomute 
  setServoMute(true);  //init le servo en position mute
  
  if(DEBUG_XYLO){
    Serial.println("end Xyophone init"); 
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
  _maxMcp1=maxPin;
}

//*********************************************************************************************
//******************          PLAY THE NOTE ON THE XYLOPHONE

void Xylophone::playNote(byte note, byte velocity) {
  int mcpPin = _noteToMcpPin(note);
  if (mcpPin != -1) {
    if(_playingNotesCount==0)  {        //si il n'y a pas d'autre notes en cours      
      _electromagnetTicker.start();     // active la gestion de la coupure des electroaimants 
      if(_ServoMuteActive==true){       // si la gestion du servoMute est activé 
        setServoMute(false);            //  on active le servo mute avant de jouer la note
      }       
    } else {setServoMute(false);}
   
    // Mettre à jour le PWM en fonction de la vélocité
      int pwmValue = map(velocity, 0, 127, MIN_PWM_VALUE, 255);
      analogWrite(PWM_PIN, pwmValue);
    // active l'electroaimant
    if (mcpPin < 16) {
      _mcp1.digitalWrite(mcpPin, HIGH);
      //met a jour le tableau pour couper l'electroaiamant avec l'interuption après le temps indiqué
      _noteStartTime[mcpPin] = millis();
      _noteActive[mcpPin] = true;      
    } else {
      _mcp2.digitalWrite(mcpPin - 16, HIGH);
      //met a jour le tableau pour couper l'electroaiamant avec l'interuption après le temps indiqué
      _noteStartTime[mcpPin - 16+_maxMcp1+1] = millis();
      _noteActive[mcpPin - 16+_maxMcp1+1] = true;
    }

    _playingNotesCount++;
    _midiNotesCount++;

    if(DEBUG_XYLO){
      Serial.print("playNote: ");
      Serial.print("note: ");
      Serial.print(note);
      Serial.print(", mcpPin: ");
      Serial.println(mcpPin);
      Serial.print("_noteActive[");
      Serial.print(mcpPin);
      Serial.print("]: ");
      Serial.println(_noteActive[mcpPin]);
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
//******************             GET MSG NOTEOFF FOR SERVO MUTE

//gestion des messages noteOff pour le servoMute
void Xylophone:: msgNoteOff(byte note){
  _midiNotesCount--;
  if(_ServoMuteActive==true){
    if (_midiNotesCount==0) {
      setServoMute(true);
    }   
  
  }  
}

//*********************************************************************************************
//******************            ACTIVATE SERVO MUTE 

void Xylophone::activateServoMute(bool actif) {
  if(actif){
    _ServoMuteActive=true;
  }else{
    _ServoMuteActive=false;
  }
}

//*********************************************************************************************
//******************            RESET THE SETTINGS

void Xylophone:: reset (){
  delay(20);// attend pour etre sur qu'il n'y a plus de notes active
  checkNoteOff(); // coupe tout les electroaiamnts 
  setServoMute(true);
  _midiNotesCount=0;
  _midiNotesCount=0;
  _ServoMuteActive=true;
}

//*********************************************************************************************
//******************             CHECK NOTE TO TURN OFF

void Xylophone::checkNoteOff() {
  unsigned long time=0;
  if (_playingNotesCount>0){                    // si il y a une note en cours 
    
    for (byte i = 0; i < _instrumentRange; i++) {  // Parcourir toutes les _noteActive pour vérifier si a true

      if(_noteActive[i]){    // Si la note est active 
      
        time =  millis()  - _noteStartTime[i]; // calcul le temps depuis que l'electroaiamants est actif

        if(DEBUG_XYLO){
          Serial.print("time Note = "); 
          Serial.println(time);     
        }     
           
        if  (time >= TIME_HIT) {// si le temps est passé, on coupe l'alim de la note
         
          if(DEBUG_XYLO){
            Serial.print("checkNoteOff: Appel stopNote : ");
            Serial.println(i);
          }        
          stopNote( i+INSTRUMENT_START_NOTE );
        }
         
      }
    }
  }  
}


// ----------------------------------    PRIVATE   --------------------------------------------

//*********************************************************************************************
//******************             SET SERVO MUTE OR UNMUTE

void Xylophone::setServoMute(bool actif) {
  if(actif){
    _servoMute.setServoAngle(SERVO_ANGLE_NOTE_ON);
  }else{
    _servoMute.setServoAngle(SERVO_ANGLE_NOTE_OFF);
  }
}

//*********************************************************************************************
//******************             GET MCP OUTPUT NUMBER

int Xylophone::_noteToMcpPin(byte note) {
  int noteIndex = note - INSTRUMENT_START_NOTE;
  if (noteIndex >= 0 && noteIndex < INSTRUMENT_RANGE) {
    return magnetPins[noteIndex];
  }
  return -1; // Invalid note
}

//*********************************************************************************************
//******************             STOP NOTE

void Xylophone::stopNote(byte midiNote) {
      Serial.print("stopNote"); 
      Serial.println(midiNote); 
int noteIndex = midiNote - INSTRUMENT_START_NOTE;
  if (noteIndex >= 0 && noteIndex < INSTRUMENT_RANGE) {
    byte mcpPin = magnetPins[noteIndex];
   /* if(DEBUG_XYLO){
      Serial.print("Xyophone timer NoteOff = "); 
      Serial.print(midiNote); 
      Serial.print("Xyophone mcpPin = "); 
      Serial.println(mcpPin);       
      }*/

    if (mcpPin != -1) {
      if (mcpPin < 16) {
        _mcp1.digitalWrite(mcpPin, LOW);
      } else {
        _mcp2.digitalWrite(mcpPin - 16, LOW);
      }
      _noteActive[noteIndex] = false;
      _playingNotesCount--;
      
      if(_playingNotesCount==0)  {
        _electromagnetTicker.pause();         
      } 
      
      if(DEBUG_XYLO){
        Serial.print("stopNote: ");
        Serial.print("midiNote: ");
        Serial.print(midiNote);
        Serial.print(", mcpPin: ");
        Serial.println(mcpPin);
        Serial.print("_noteActive[");
        Serial.print(mcpPin);
        Serial.print("]: ");
        Serial.println(_noteActive[mcpPin]);
        Serial.print("_playingNotesCount: ");
        Serial.println(_playingNotesCount);          
      }
    } 
  }
}

void Xylophone::_electromagnetTickerCallback() {
  XylophoneInstance->checkNoteOff();
}
