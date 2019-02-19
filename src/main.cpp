
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

bool diag = true;

// Router APs
String ssid [] = {"StarrLucky","Lan_Indoor"};
String password [] = {"luckys322", "KurwaPidarGandonJevlarSuccMaPusseyDQ"};

int ssidnum = 2;
unsigned long time1 = 0, lastUIupdate = 9999, lastcheck = 9999;
int AmpPower_state = 0;

PERIF LED1 = PERIF();
TDA7439 preamp  = TDA7439();                // using TDA7439 2-wire library
W_LAN RouterAP = W_LAN();

volatile unsigned long int rise_time;

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
  preamp.setInput(1);                      //  (1-4)
  preamp.setVolume(5);                     // (0-48), 0 Mute


  if (diag) { Serial.begin(9600); Serial.println("Serial Ready. Setup routine");}
  pinMode(MCUON_PIN, OUTPUT);               //    MCU_ON state Light Diode
  digitalWrite(MCUON_PIN, HIGH);
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

   if ((millis() - 400) >  rise_time)   // if interrupt was made less than XXXms
                                        // then it's switching chatter and must be filtered.
   {
     digitalWrite(AMPPOWER_PIN, !AmpPower_state);
     AmpPower_state = !AmpPower_state;
     rise_time = millis();
   }
 }