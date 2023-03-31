/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
---------------------------------------   SERVOMUTE.CPP    ----------------------------------------------
_________________________________________________________________________________________________________

classe pour un deplacement rapide et direct d'un servomoteur 
est créer en indiquant un pin pwm       =    ServoMotor(int pin);
est modifié en indiquant l'angle voulu  =    void setServoAngle(int angle);

les differents parametres et reglages du systeme pour ce servo sont dans settings.h
***********************************************************************************************************/

#include "Settings.h"
#include "ServoMute.h"
// ----------------------------------      PUBLIC  --------------------------------------------

ServoMute::ServoMute(int pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
}

//*********************************************************************************************
//******************            SET SERVO ANGLE

void ServoMute::setServoAngle(int angle) {
  int pulseWidth = angleToPulseWidth(angle);
  analogWrite(_pin, pulseWidth);
}
// ----------------------------------      PRIVATE   ------------------------------------------

//*********************************************************************************************
//******************         RETURN THE PUSLE WITH FOR THE SERVO 

int ServoMute::angleToPulseWidth(int angle) {
  int constrainedAngle = constrain(angle, 0, 180);
  int pulseWidth = map(constrainedAngle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
  return pulseWidth;
}

