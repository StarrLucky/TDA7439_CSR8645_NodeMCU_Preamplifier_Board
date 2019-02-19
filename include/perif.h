#ifndef PERIF_H
#define PERIF_H



class PERIF
{
public:
  PERIF();
  void BlinkLed(int ledpin, int times, int delaybetween, int delayafter, int stateafter);
  int ledpin;
};


#endif
//int times, int delaybetween, int delayafter, int stateafter
