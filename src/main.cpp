
#include <defines.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <Strings.h>
#include <perif.h>
#include "TDA7439.h"
#include <wrlss.h>
#include <espserv.h>


//   TO DO:
// + access points connection and mdnsserver
// + server handlers for controlling TDA preamp (gain, volume, input settings)
// + Power Button handlers
// + Amplifier Power On Relay control
// - Sensor Readings
// - Saving settings to EEPROM
// - CSR8645 Control
//
// Starr Lucky, jan 2019

bool diag = false;

// Router APs
String ssid [] = {"StarrLucky","Lan_Indoor"};
String password [] = {"luckys322", "luckys322"};

int ssidnum = 2;
unsigned long time1 = 0, lastUIupdate = 9999, lastcheck = 9999;
bool AmpPower_state = false;                // true while the amplifier relay is powered

PERIF LED1 = PERIF();
TDA7439 preamp  = TDA7439();                // using TDA7439 2-wire library
W_LAN RouterAP = W_LAN();

// Button handling is split in two. The ISR below only timestamps edges; the
// actual work happens in loop(). Driving I2C (changeInput) or Serial from
// interrupt context is what trips the hardware watchdog - see issue #1.
volatile bool interruptenable = false;
volatile bool buttonDown = false;           // debounced button state
volatile unsigned long lastEdge = 0;        // millis() of the last accepted edge
volatile unsigned long pressStarted = 0;    // millis() when the button went down
volatile unsigned long pendingPress = 0;    // length of a finished press, 0 = nothing to do

void ICACHE_RAM_ATTR PowerButton();
void HandleButtonPress(unsigned long duration);



void setup()
{

  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), (PowerButton), CHANGE);   // both edges: press and release

  pinMode(AMPPOWER_PIN, OUTPUT);
  digitalWrite(AMPPOWER_PIN, AMPPOWER_OFF);


  preamp.setSnd(3, 1);                     // Bass (-7 - 7)
  preamp.setSnd(3, 2);                     // Midle (-7 - 7)
  preamp.setSnd(5, 3);                     // High (-7 - 7)
  preamp.setInput(2);                      //  (1-4)
  preamp.inputGain(2);
  preamp.setVolume(33);                     // (0-48), 0 Mute
  preamp.spkAtt(0);


  if (diag) { Serial.begin(9600); Serial.println("Serial Ready. Setup routine");}



  pinMode(STANDBY_PIN, OUTPUT);            //    AMPLIFIER STANDBY control pin
  digitalWrite(STANDBY_PIN, LOW);         //     Not in standby mode

  W_LAN();
  RouterAP.SetSSIDs(ssidnum, ssid, password);
  RouterAP.Connect();


  ESP_SERV_SETUP();

  digitalWrite(AMPPOWER_PIN, AMPPOWER_ON);
  AmpPower_state = true;

  if (diag) { Serial.println("Setup success");}
  Serial.println("Local IP:");
  Serial.println(WiFi.localIP());
  interruptenable = true;

}



void loop()
{
  time1= millis();

  if (time1 > (lastcheck + 10000))
   {
     RouterAP.WLanControl();        // checking if wifi connection is ok for every 10 sec
     lastcheck = millis();
   }

   unsigned long press;                    // a press the ISR has finished timing
   noInterrupts();
   press = pendingPress;
   pendingPress = 0;
   interrupts();

   if (press > 0) { HandleButtonPress(press); }

   ESP_SERV_HANDLE_CLIENT();               // waiting for mDNS request

 }



 void ICACHE_RAM_ATTR PowerButton()      // Button interrupt: timestamps only, no I2C, no Serial
{
  if (!interruptenable) { return; }

  unsigned long now = millis();
  if (now - lastEdge < BUTTON_DEBOUNCE_MS) { return; }   // contact bounce
  lastEdge = now;

  bool down = (digitalRead(BUTTON_PIN) == BUTTON_PRESSED_LEVEL);
  if (down == buttonDown) { return; }                    // bounced back to the same level
  buttonDown = down;

  if (down) { pressStarted = now; }
  else if (pressStarted != 0) { pendingPress = now - pressStarted; }   // loop() classifies it
}


void HandleButtonPress(unsigned long duration)   // runs from loop(), safe to touch I2C
{
  if (duration >= BUTTON_LONGPRESS_MS)
  {
    if (diag) { Serial.println("LONG TAP"); }

    AmpPower_state = !AmpPower_state;
    digitalWrite(AMPPOWER_PIN, AmpPower_state ? AMPPOWER_ON : AMPPOWER_OFF);
  }
  else
  {
    if (diag) { Serial.println("SINGLE TAP"); }

    preamp.changeInput();
  }
}
