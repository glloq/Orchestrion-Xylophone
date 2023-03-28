#include "Settings.h"
#include "ServoMute.h"

ServoMute::ServoMute(int pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
}

void ServoMute::setServoAngle(int angle) {
  int pulseWidth = angleToPulseWidth(angle);
  analogWrite(_pin, pulseWidth);
}

int ServoMute::angleToPulseWidth(int angle) {
  int constrainedAngle = constrain(angle, 0, 180);
  int pulseWidth = map(constrainedAngle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
  return pulseWidth;
}

