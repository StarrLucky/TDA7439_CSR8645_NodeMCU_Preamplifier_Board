
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
int AmpPower_state = 0;

PERIF LED1 = PERIF();
TDA7439 preamp  = TDA7439();                // using TDA7439 2-wire library
W_LAN RouterAP = W_LAN();

volatile unsigned long int rise_time = 1000;
volatile unsigned long int lastinterr, thisinterr, lastpush = 0;
volatile bool interrhandled = true; volatile bool interruptenable = false;
unsigned int buttonpushed = 0;
void PowerButton();



void setup()
{

  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), (PowerButton), RISING);   // attaching interrupt for pushing power button

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
     RouterAP.WLanControl(RouterAP);        // checking if wifi connection is ok for every 10 sec
     lastcheck = millis();
   }

   ESP_SERV_HANDLE_CLIENT();               // waiting for mDNS request

 }



 void PowerButton() {                    // Button interrupt function
if (interruptenable) {

thisinterr =  millis();

if ((thisinterr > lastinterr + 200) && (thisinterr<lastinterr+2000) && (thisinterr> lastpush+2000) )
{
    if (diag) {        Serial.println("SINGLE TAP");}
      lastpush = millis();
      preamp.changeInput();
    }

else if ( (thisinterr>lastinterr+3000) && (thisinterr<lastinterr+6000) && (thisinterr> lastpush+2000))   // trying to identify if button is being pressed
{

    if (diag) { Serial.println("LONG TAP");}
    lastpush = millis();

    digitalWrite(AMPPOWER_PIN, !AmpPower_state);
    AmpPower_state = !AmpPower_state;
}

lastinterr = millis();
}
}
