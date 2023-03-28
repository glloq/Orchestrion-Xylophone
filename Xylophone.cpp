
#include "Xylophone.h"

//*********************************************************************************************************************
//   Public 
//*********************************************************************************************************************
Xylophone* XylophoneInstance;

Xylophone::Xylophone(): _servoMute(SERVO_MUTE_PIN)  {
  for (byte i = 0; i < INSTRUMENT_RANGE; i++) {
    _noteStartTime[i] = 0;
    _noteActive[i] = false;
    XylophoneInstance = this;

  }
}

void Xylophone::begin() {
  _mcp1.begin_I2C(0);
  _mcp2.begin_I2C(1);

  for (byte i = 0; i < 16; i++) {
    _mcp1.pinMode(i, OUTPUT);
    _mcp2.pinMode(i, OUTPUT);

    _mcp1.digitalWrite(i, LOW);
    _mcp2.digitalWrite(i, LOW);
  }
  initTimer();  //initialise le timer pour l'extinction des electroaimants
  activateServoMute(true);//active la gestion du servomute 
  setServoMute(true);  //init le servo en position mute
}



void Xylophone::playNote(byte note, byte velocity) {
  int mcpPin = _noteToMcpPin(note);
  if (mcpPin != -1) {
    //si la gestion des notes off n'est pas utilisé
    if(_ServoMuteActive==true){
      //si il n'y a pas d'autre notes en cours alors on active le servo mute avant de jouer la note     
      if(_playingNotesCount==0)  {
        // met le servo en umute
        setServoMute(0);
      }       
    } else {setServoMute(0);}
   
    // Mettre à jour le PWM en fonction de la vélocité
      int pwmValue = map(velocity, 0, 127, MIN_PWM_VALUE, 255);
      analogWrite(PWM_PIN, pwmValue);
    // active l'electroaimant
    if (mcpPin < 16) {
      _mcp1.digitalWrite(mcpPin, HIGH);
    } else {
      _mcp2.digitalWrite(mcpPin - 16, HIGH);
    }
    //met a jour le tableau pour couper l'electroaiamant avec l'interuption après le temps indiqué
    _noteStartTime[mcpPin] = millis();
    _noteActive[mcpPin] = true;
    _playingNotesCount++;
    _midiNotesCount++;
  }    
}
//gestion des messages noteOff pour le servoMute
void Xylophone:: msgNoteOff(byte note){
  _midiNotesCount--;
  if(_ServoMuteActive==true){
    if (_midiNotesCount==0) {
      setServoMute(1);
    }   
  }  
}
void Xylophone::activateServoMute(bool actif) {
  if(actif){
    _ServoMuteActive=true;
  }else{
    _ServoMuteActive=false;
  }
}

void Xylophone:: reset (){
  delay(20);// attend pour etre sur qu'il n'y a plus de notes active
  checkNoteOff();
  setServoMute(true);
  _midiNotesCount=0;
  _midiNotesCount=0;
  _ServoMuteActive=true;
}
//********************************************************************************************************************
//   Private 
//*********************************************************************************************************************
void Xylophone::setServoMute(bool actif) {
  if(actif){
    _servoMute.setServoAngle(SERVO_ANGLE_NOTE_ON);
  }else{
    _servoMute.setServoAngle(SERVO_ANGLE_NOTE_OFF);
  }
}

//renvoi le numero de la sortie sur les mcp 
int Xylophone::_noteToMcpPin(byte note) {
  int noteIndex = note - INSTRUMENT_START_NOTE;
  if (noteIndex >= 0 && noteIndex < INSTRUMENT_RANGE) {
    return magnetPins[noteIndex];
  }
  return -1; // Invalid note
}

/********************************************************************************/
//fonctions appellé par l'interuption materiel  pour eteindre les electroaimants 
/******************************************************************************/
void Xylophone::stopNote(byte note) {
  byte mcpPin = _noteToMcpPin(note);
  if (mcpPin != -1) {
    if (mcpPin < 16) {
      _mcp1.digitalWrite(mcpPin, LOW);
    } else {
      _mcp2.digitalWrite(mcpPin - 16, LOW);
    }
      _noteActive[mcpPin] = false;
      _playingNotesCount--;
        
  }      
}
void Xylophone::checkNoteOff() {
  if(_playingNotesCount>0){
    for (byte note = _instrumentStartNote; note < _instrumentStartNote + _instrumentRange; note++) {
      byte noteIndex = note - _instrumentStartNote;
      // si la note est activevet si le temps et passé on coupe l'alim de la note
      if (_noteActive[noteIndex] && millis() - _noteStartTime[noteIndex] >= TIME_HIT) {
        stopNote(note);
      }
    }
  }
}

void Xylophone::initTimer() {
  // Configurer Timer1 en mode CTC (Clear Timer on Compare Match)
  TCCR1A = 0;
  TCCR1B = (1 << WGM12);

  // Définir la fréquence d'interruption (par exemple, 1000 Hz)
  OCR1A = F_CPU / (2 * 1000) - 1;

  // Activer l'interruption sur le comparateur A
  TIMSK1 = (1 << OCIE1A);

  // Démarrer le timer avec un diviseur de fréquence de 8
  TCCR1B |= (1 << CS11);
}
// Fonction d'interruption pour Timer1
ISR(TIMER1_COMPA_vect) {
    XylophoneInstance->checkNoteOff();
}


