#include "ServoVolume.h"
#include <Arduino.h>
#include "settings.h" 

ServoVolume* servoVolumeInstance;

ServoVolume::ServoVolume(int pin) : _pin(pin), _volume(0), _vibratoEnabled(false), _vibratoFrequency(0), _previousMillis(0), _currentAngle(0) {
  pinMode(_pin, OUTPUT);
  initTimer();
  servoVolumeInstance = this;
}

void ServoVolume::setVolume(int volume) {
 _volume = map(volume, 0, 127, SERVO_VOLUME_MIN_ANGLE, SERVO_VOLUME_MAX_ANGLE);
  writeServo(_volume);
}

void ServoVolume::setVibrato(bool enabled, float frequency) {
  _vibratoEnabled = enabled;
  
  if (frequency >= 0) {
    float mappedFrequency = map(frequency, 0, 127, SERVO_VOLUME_MIN_FREQUENCY_VIBRATO, SERVO_VOLUME_MAX_FREQUENCY_VIBRATO);
    _vibratoFrequency = constrain(mappedFrequency, SERVO_VOLUME_MIN_FREQUENCY_VIBRATO, SERVO_VOLUME_MAX_FREQUENCY_VIBRATO);
  }
}

void ServoVolume::updateVibrato() {
  if (_vibratoEnabled) {
    unsigned long currentMillis = millis();
    float timePassed = (currentMillis - _previousMillis) / 1000.0;
    float vibratoRange = _volume - SERVO_VOLUME_MIN_ANGLE_VIBRATO;
    float vibratoPosition = fmod(timePassed * _vibratoFrequency * 2.0, 2.0);

    float angleVariation;
    if (vibratoPosition < 1.0) {
      angleVariation = vibratoRange * vibratoPosition;
    } else {
      angleVariation = vibratoRange * (2.0 - vibratoPosition);
    }
    _currentAngle = SERVO_VOLUME_MIN_ANGLE_VIBRATO + angleVariation;
    writeServo(_currentAngle);
  }
}



int ServoVolume::angleToPulseWidth(int angle) {
  int pulseWidth = map(angle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
  return pulseWidth;
}

void ServoVolume::writeServo(int angle) {
  int pulseWidth = angleToPulseWidth(angle);
  digitalWrite(_pin, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(_pin, LOW);
}

void ServoVolume::initTimer() {
  noInterrupts(); // Désactive les interruptions
  TCCR3A = 0; // Met à zéro TCCR3A
  TCCR3B = 0; // Met à zéro TCCR3B
  TCNT3  = 0; // Initialise le compteur à zéro

  // Configurez la fréquence de l'interruption TIMER3_COMPA_vect
  uint32_t compareValue = (F_CPU / (SERVO_FREQUENCY * 8)) - 1;

  OCR3A = compareValue; // Configure la valeur de comparaison
  TCCR3B |= (1 << WGM32); // Mode CTC
  TCCR3B |= (1 << CS31); // Pré-diviseur 8
  TIMSK3 |= (1 << OCIE3A); // Active l'interruption TIMER3_COMPA_vect
  interrupts(); // Active les interruptions
}


// Fonction d'interruption pour Timer3
ISR(TIMER3_COMPA_vect) {
  // Gérer le vibrato
  servoVolumeInstance->updateVibrato();}