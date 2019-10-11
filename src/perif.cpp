#include <Arduino.h>
#include <perif.h>


PERIF::PERIF(){
  EnableLed(1);
}




void PERIF::BlinkLed(int lednumber, int times, int delaybetween, int delayafter, int stateafter)   // need interrupts for this ofc :/
{

  for (int i = 1; i<times; i++)
  {
    switch (lednumber) {
      case 1:
      if (digitalRead(LED_PIN == HIGH)) { pinMode(LED_PIN, INPUT);} else {pinMode(LED_PIN, OUTPUT);  digitalWrite(LED_PIN, HIGH);}
      case 2:
      if (digitalRead(LED_PIN == LOW)) {   pinMode(LED_PIN, INPUT);} else {pinMode(LED_PIN, OUTPUT);     digitalWrite(LED_PIN, LOW);}
    }

     delay(delaybetween);
  }


  switch (lednumber) {
    case 1:
    if (stateafter == 0) { pinMode(LED_PIN, INPUT);} else {pinMode(LED_PIN, OUTPUT);  digitalWrite(LED_PIN, HIGH);}
    case 2:
    if (stateafter == 0) {   pinMode(LED_PIN, INPUT);} else {pinMode(LED_PIN, OUTPUT);     digitalWrite(LED_PIN, LOW);}
  }


  delay(delayafter);

}


void PERIF::EnableLed(int lednumber) {

  pinMode(LED_PIN, OUTPUT);
  switch (lednumber) {
    case 1:
    digitalWrite(LED_PIN, HIGH);
    case 2:
    digitalWrite(LED_PIN, LOW);
  }
}

void PERIF::DisableLeds() {
  pinMode(LED_PIN, INPUT);
}

void ChangeState(int lednumber) {

  pinMode(LED_PIN, OUTPUT);
  switch (lednumber) {
    case 1:
    if (digitalRead(LED_PIN == HIGH)) { pinMode(LED_PIN, INPUT);} else {pinMode(LED_PIN, OUTPUT);  digitalWrite(LED_PIN, HIGH);}
    case 2:
    if (digitalRead(LED_PIN == LOW)) {   pinMode(LED_PIN, INPUT);} else {pinMode(LED_PIN, OUTPUT);     digitalWrite(LED_PIN, LOW);}
  }


}
