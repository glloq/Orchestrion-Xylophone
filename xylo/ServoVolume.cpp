/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
-----------------------------------   SERVOVOLUME.CPP  --------------------------------------------------
_________________________________________________________________________________________________________

classe pour la gestion du volume et du vibrato avec un servo

les differents parametres et reglages du systeme pour ce servo sont dans settings.h
***********************************************************************************************************/
#include "ServoVolume.h"
#include <Arduino.h>
#include "settings.h"

static  ServoVolume* ServoVolumeInstance;

ServoVolume::ServoVolume(int pin) : _pin(pin), _volume(0), _vibratoEnabled(false), _vibratoFrequency(0), _currentAngle(0), _vibratoTicker(_vibratoTickerCallback, 10, 0, MILLIS) {
  _servo.attach(_pin);

  if(DEBUG_SEROVOLUME){
      Serial.println("ServoVolume initialisé"); 
  }  
  ServoVolumeInstance = this;

}

void ServoVolume::setVolume(int volume) {
  _volume = map(volume, 0, 127, SERVO_VOLUME_MIN_ANGLE, SERVO_VOLUME_MAX_ANGLE);
  _servo.write(_volume);
}

void ServoVolume::setVibrato(bool enabled, float frequency) {
  _vibratoEnabled = enabled;
  if(DEBUG_SEROVOLUME){
      Serial.print("ServoVolume Vibrato ="); 
      Serial.println(_vibratoEnabled); 
  }      
  if (frequency >= 0) {
    float mappedFrequency = map(frequency, 0, 127, SERVO_VOLUME_MIN_FREQUENCY_VIBRATO, SERVO_VOLUME_MAX_FREQUENCY_VIBRATO);
    _vibratoFrequency = constrain(mappedFrequency, SERVO_VOLUME_MIN_FREQUENCY_VIBRATO, SERVO_VOLUME_MAX_FREQUENCY_VIBRATO);
  }

  if (_vibratoEnabled) {
    _vibratoTicker.start();
  } else {
   _vibratoTicker.pause();
  }
}

void ServoVolume::updateVibrato() {
  if (_vibratoEnabled) {
    float vibratoRange = _volume - SERVO_VOLUME_MIN_ANGLE_VIBRATO;
    float vibratoPosition = fmod(millis() * _vibratoFrequency * 2.0 / 1000.0, 2.0);

    float angleVariation;
    float vibratoRangeTimesPosition = vibratoRange * vibratoPosition;
    if (vibratoPosition < 1.0) {
      angleVariation = vibratoRangeTimesPosition;
    } else {
      angleVariation = vibratoRange * (2.0 - vibratoPosition);
    }
    _currentAngle = SERVO_VOLUME_MIN_ANGLE_VIBRATO + angleVariation;
    _servo.write(_currentAngle);
  }
}

void ServoVolume::_vibratoTickerCallback() {
 if (ServoVolumeInstance) {
    ServoVolumeInstance->updateVibrato();
  }
}
void ServoVolume::update() {
  _vibratoTicker.update();
}
