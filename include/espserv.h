#ifndef ESP_SERV_H
#define ESP_SERV_H
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

                    //requests to esp8266 server to control TDA7439 trough wifi (android app) is:

// esp8266 address       param     ?arg
//
//                  |-----   /standbyoff                    amplifier on/off
//                  |        / standbyon
//  xxx.xxx.xxx.xxx |        /btoff                         CSR8645 on / off
//                  |        / bton
//                  |        /tda?volume=xx                (xx = 0..48). preamp volume
//                  |       /tda?gain=xx                  (xx = 0..48) input gain
//                  |       /tda?input=x                   (x = 1..4)  input selection
//                  |----       // or     192.168.1.2/tda?input=2  for setting input #2

//for example: 192.168.1.2/tda?volume=25
//             192.168.1.2/standbyoff

  void ESP_SERV_SETUP();
  void handleTdaArgs();
  void HandleRoot();
  void HandleNotFound();
  void ESP_SERV_HANDLE_CLIENT() ;

#endif
