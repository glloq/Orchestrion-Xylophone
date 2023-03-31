/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
---------------------------------------   SERVOMUTE.H      ----------------------------------------------
_________________________________________________________________________________________________________

classe pour un deplacement rapide et direct d'un servomoteur 
est créer en indiquant un pin pwm       =    ServoMotor(int pin);
est modifié en indiquant l'angle voulu  =    void setServoAngle(int angle);

les differents parametres et reglages du systeme pour ce servo sont dans settings.h
***********************************************************************************************************/
#ifndef ServoMute_H
#define ServoMute_H
class ServoMute {
  public:
    ServoMute(int pin);
    void setServoAngle(int angle);
    
  private:
    int _pin;
    int angleToPulseWidth(int angle);
};
#endif // ServoMute_H
