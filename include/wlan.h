#ifndef WLAN_H
#define WLAN_H
#include <Arduino.h>
#include <Strings.h>

#define numOfClients 4   // num of max clients which http requests ESP8266 will handle

struct wlanstruct {
String str;
String WiFiStatus;
String NTPStatus;
String LocalIP;
String WiFiAP;
unsigned long int LastCommand;
};


class W_LAN
{
public:
  W_LAN();
  	void Connect(); // 1 to 4
  int SSIDn;
  char ssid[];
  char password[];
  void mute();
struct wlanstruct espdata;
};
#endif
