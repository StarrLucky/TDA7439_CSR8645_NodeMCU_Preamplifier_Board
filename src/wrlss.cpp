
#include <time.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <wrlss.h>
#include <perif.h>
#include <defines.h>
#include <Arduino.h>

String ardstr;

PERIF LED = PERIF();

W_LAN::W_LAN(){
  espdata.ssids = 0;
}

void W_LAN::SetSSIDs(int num, String *ssids,  String *pass)
{
  if (num > WLAN_MAX_APS) { num = WLAN_MAX_APS; }   // don't overrun espdata.ssid[]
  if (num < 0) { num = 0; }

  espdata.ssids = num;
  for (int i = 0; i < num; i++)
  {
    espdata.ssid[i] = ssids[i];
    espdata.password[i] = pass[i];
  }
}


void W_LAN::Connect()
{
int iii = 0; int i = 0;
while ((WiFi.status() != WL_CONNECTED))
 {
    if (i >= espdata.ssids) { i = 0; Serial.println("connection failed after 3 attempts"); break; }  // every AP tried

    iii++;
    if (iii<3)   // 3 attempts to connect to the APs
    {

    Serial.println("Attempt "); Serial.print(iii);

    if (diag) { Serial.println("Connecting to "); Serial.print(espdata.ssid[i]); }

    espdata.WiFiStatus =  WiFi.begin(espdata.ssid[i].c_str(), espdata.password[i].c_str());
    delay(10000);  // wait 10 sec for connection

    if  (WiFi.status() == WL_CONNECTED) { espdata.WiFiAP = espdata.LastConnSSID = espdata.ssid[i]; espdata.LastConnPass = espdata.password[i];
    Serial.println("Connected!"); break; }

  }  else { i++; iii=0; }
}
}


  void W_LAN::ConnectToAP(String ssid, String password)
  {
  int iii = 0;
  while ((WiFi.status() != WL_CONNECTED) && (iii<3))    // 3 attempts to connect to the AP
   {
     iii++;
    
      if (diag) { Serial.println("Connecting to"); Serial.println(ssid); }
      espdata.WiFiStatus =  WiFi.begin(ssid.c_str(), password.c_str());
      LED.BlinkLed(2, 3, 100, 10, 0);
      delay(10000);  // wait 10 sec for connection

      if  (WiFi.status() == WL_CONNECTED)   // connection succ
      {
        espdata.WiFiAP = espdata.LastConnSSID = ssid; espdata.LastConnPass = password;
        LED.BlinkLed(2, 5, 50, 10, 1);
        break;
       }
    }
  }


  void W_LAN::ConnectToLastAP()
   {
     int iii = 0;
     while ((WiFi.status() != WL_CONNECTED) && (iii<3))    // 3 attempts to connect to the AP
      {
        iii++;

         if (diag) { Serial.println("Connecting to"); Serial.println(espdata.LastConnSSID); }
         espdata.WiFiStatus =  WiFi.begin(espdata.LastConnSSID.c_str(), espdata.LastConnPass.c_str());
         LED.BlinkLed(2, 3, 100, 10, 0);
         delay(10000);  // wait 10 sec for connection

         if  (WiFi.status() == WL_CONNECTED) { espdata.WiFiAP = espdata.LastConnSSID;
           LED.BlinkLed(2, 5, 50, 10, 1); break;}
       }
   }


   void W_LAN::WLanControl()
   {

     if ((WiFi.status() != WL_CONNECTED))   // if no connection to router
     {
       if  (WiFi.status() == WL_CONNECTION_LOST) // if connection dropped
       {
          // blinking like a slutty tranny in the club
        LED.BlinkLed(2, 3, 200, 200, 0);
         ConnectToLastAP();
       }
       else if (espdata.WiFiStatus != WL_CONNECTED) // if it is wasn't connected at all
       {
         LED.BlinkLed(2, 5, 50, 10, 1);
         Connect();
       }


     }

   }



   // void W_LAN::StartServer()
   //  {
   //
   //      if (diag) { Serial.println("WiFi connected");}
   //      // Start the server
   //      server.begin();
   //      if (diag) { Serial.println("Server started");}
   //      // Print the IP address
   //      if (diag) { Serial.print("IP number assigned by DHCP is"); Serial.println(WiFi.localIP()); }
   //      if (diag) { Serial.println("Board initialized");}
   //
   //
   //  }
