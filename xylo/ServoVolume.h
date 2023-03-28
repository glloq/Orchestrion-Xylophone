/***********************************************************************************************************
classe pour la gestion du volume et du vibrato avec un servo
utilise des messages midi comme controles
permet :
un deplacement rapide et direct d'un servomoteur entre 2 angles 
ainsi que regulierement entre 2 angles a une frequence demandé

Créer en indiquant un pin pwm  =  ServoVolume(int pin);
modifie l'angle en fct du volume  =   void setVolume(int volume);
Gere le fonctionnement du vibrato =   void setVibrato(bool enabled, float frequency);



les differents parametres et reglages du systeme pour ce servo est dans settings.h
***********************************************************************************************************/
#ifndef ServoVolume_H
#define ServoVolume_H
#include "settings.h" 

class ServoVolume {
  public:
    ServoVolume(int pin);
    void setVolume(int volume);
    void setVibrato(bool enabled, float frequency=0); // gere l'ajout de la gestion du vibrato avec un servo 
    void updateVibrato(); // fonction qui gerer la deplacement du servo dans le temps

  private:
    int _pin;
    int _volume;// volume actuel c'est a dire l'angle du servo actuel
    bool _vibratoEnabled;
    float _vibratoFrequency;
    unsigned long _previousMillis;
    int _currentAngle;
    int angleToPulseWidth(int angle);
    void writeServo(int angle);
    void initTimer(); // timer pour l'interuption avec Timer3
};
#endif // ServoVolume_H
