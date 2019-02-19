#ifndef WLAN_H
#define WLAN_H
#include <Arduino.h>
#include <Strings.h>
#include <ESP8266WiFi.h>


struct wlanstruct {
 char ssid[5];
 char password[5];
 int ssids;
 int lastconnectedN;
 String  LastConnSSID;
 String LastConnPass;
String str;
wl_status_t WiFiStatus;
String LocalIP;
String WiFiAP;
unsigned long int LastCommand;

};

class W_LAN
{

public:
  W_LAN();
  wlanstruct espdata;
  void Connect();
//  void StartServer();
  void WLanControl(W_LAN info);
  void ConnectToAP(String ssid, String password);
  void ConnectToLastAP();
  void SetSSIDs(int num,  String *ssids,  String *pass);


  //wlanstruct espdata;
};
#endif
