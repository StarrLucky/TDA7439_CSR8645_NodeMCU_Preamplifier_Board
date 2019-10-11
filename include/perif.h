#ifndef PERIF_H
#define PERIF_H

#include <defines.h>

class PERIF
{
public:
  PERIF();
  void BlinkLed(int lednumber, int times, int delaybetween, int delayafter, int stateafter);
  int ledpin;
  void EnableLed(int lednumber);
  void DisableLeds();
  void ChangeState(int lednumber);
};


#endif
//int times, int delaybetween, int delayafter, int stateafter
