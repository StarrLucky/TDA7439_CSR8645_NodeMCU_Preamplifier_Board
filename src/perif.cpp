#include <Arduino.h>
#include <perif.h>


PERIF::PERIF(){}




void PERIF::BlinkLed(int ledpin, int times, int delaybetween, int delayafter, int stateafter)   // need interrupts for this ofc :/
{

  for (int i = 1; i<times; i++)
  {
     digitalWrite(ledpin, !digitalRead(ledpin));
     delay(delaybetween);

  }
  digitalWrite(ledpin, stateafter);
  delay(delayafter);

}
