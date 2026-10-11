#ifndef WLAN_H
#define WLAN_H
#include <Arduino.h>
#include <Strings.h>
#include <ESP8266WiFi.h>


#define WLAN_MAX_APS 5

struct wlanstruct {
 String ssid[WLAN_MAX_APS];
 String password[WLAN_MAX_APS];
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
  void WLanControl();
  void ConnectToAP(String ssid, String password);
  void ConnectToLastAP();
  void SetSSIDs(int num,  String *ssids,  String *pass);


  //wlanstruct espdata;
};
#endif
