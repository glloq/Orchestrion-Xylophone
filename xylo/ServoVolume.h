/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
-----------------------------------   SERVOVOLUME.H    --------------------------------------------------
_________________________________________________________________________________________________________

classe pour la gestion du volume et du vibrato avec un servo
utilise des messages midi comme controles
permet :
un deplacement rapide et direct d'un servomoteur entre 2 angles 
ainsi que regulierement entre 2 angles a une frequence demandé

Créer en indiquant un pin pwm  =  ServoVolume(int pin);
modifie l'angle en fct du volume  =   void setVolume(int volume);
Gere le fonctionnement du vibrato =   void setVibrato(bool enabled, float frequency);


les differents parametres et reglages du systeme pour ce servo sont dans settings.h
***********************************************************************************************************/
#ifndef ServoVolume_H
#define ServoVolume_H
#include "settings.h"
#include <Servo.h>
#include "Ticker.h"

class ServoVolume {
  public:
    ServoVolume(int pin);
    void setVolume(int volume);
    void setVibrato(bool enabled, float frequency=0);
    static void _vibratoTickerCallback();
    void update();
    
  private:
    void updateVibrato();
    int _pin;
    int _volume;
    bool _vibratoEnabled;
    float _vibratoFrequency;
    int _currentAngle;
    Servo _servo;
    Ticker _vibratoTicker;
};

#endif // ServoVolume_H


