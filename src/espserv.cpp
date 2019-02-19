
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "TDA7439.h"
#include <espserv.h>
#include <defines.h>
#include <Strings.h>


ESP8266WebServer mserv (80);
TDA7439 preampcontrol  = TDA7439();
String tempstr; int b;


void ESP_SERV_SETUP()
{

	if ( MDNS.begin ( "esp8266" ) ) {
	if (diag) { 	Serial.println ( "MDNS responder started" ); }
	}

	mserv.on ( "/", HandleRoot );



  mserv.on("/ampstandby_on", [] ()
  {
    digitalWrite(STANDBY_PIN, HIGH);
    mserv.send ( 200, "text/plain", "StandBy ON" );
  });

	mserv.on("/amppower_on", [] ()
	{
		digitalWrite(AMPPOWER_PIN, AMPPOWER_ON);
		mserv.send ( 200, "text/plain", "Amplifier Relay Power On" );
	});

	mserv.on("/amppower_off", [] ()
	{
		digitalWrite(AMPPOWER_PIN, AMPPOWER_OFF);
		mserv.send ( 200, "text/plain", "Amplifier Relay Power OFF" );
	});


  mserv.on("/ampstandby_off", [] ()
  {
    digitalWrite(STANDBY_PIN, LOW);
    mserv.send ( 200, "text/plain", "StandBy OFF" );
  });

  mserv.on("/bton", [] ()
  {
    digitalWrite(BT_PIN, HIGH);
    mserv.send ( 200, "text/plain", " CSR8645 module is On" );
  });

  mserv.on("/btoff", [] ()
  {
    digitalWrite(BT_PIN, LOW);
    mserv.send ( 200, "text/plain", " CSR8645 module is OFF" );
  });

	mserv.on("/mute", [] ()
	{
		preampcontrol.mute();

		mserv.send ( 200, "text/plain", " TDA7439 output muted" );
	});

	mserv.on("/unmute", [] ()
	{
		preampcontrol.setVolume(preampcontrol.lastVolume);
		mserv.send ( 200, "text/plain", " TDA7439 output unmuted" );
	});


/*
  mserv.on("/InputVolume", [] ()   // not sure
  {
		//tempstr = mserv.uri().substring(13,15);
 		b = atoi(mserv.uri().substring(12,14).c_str());
		preampcontrol.setVolume(b);
    mserv.send ( 200, "text/plain", "Volume set to " + b );
  });

	mserv.on("/InputGain", [] ()   // not sure
	{
		//tempstr = mserv.uri().substring(13,15);
		b = atoi(mserv.uri().substring(12,14).c_str());
		preampcontrol.setVolume(b);
		mserv.send ( 200, "text/plain", "Gain set to " + b );
	});
*/


	mserv.on("/tda", handleTdaArgs);


	mserv.onNotFound (HandleNotFound);

	mserv.begin();

	if (diag) { 	Serial.println ( "HTTP server started" ); }

}



void ESP_SERV_HANDLE_CLIENT()
{
  mserv.handleClient();
}


void handleTdaArgs()
{

	String message = "";

	if (mserv.arg("volume") != "")
	{
		message = mserv.arg("volume"); b = message.toInt();
		if ( (b>=0) && (b<=48) ) {
    mserv.send ( 200, "text/plain", "volume is at " + message);
		preampcontrol.setVolume(b);
	} else {mserv.send ( 200, "text/plain", "Wrong volume param " );}

	}


	else 	if (mserv.arg("gain") != "")
	{
		message = mserv.arg("gain"); b = message.toInt();
		if ( (b>=1) && (b<=48) ) {
    mserv.send ( 200, "text/plain", "Gain is at " + message);
		preampcontrol.inputGain(b);
	} else {mserv.send ( 200, "text/plain", "Wrong gain param " );}

	}


	else if (mserv.arg("input") !="")
	{
		message = mserv.arg("input"); b = message.toInt();
		if ( (b>=1) && (b<=4) ) {
    mserv.send ( 200, "text/plain", "Input " + message + " enabled" );
		preampcontrol.setInput(b);
	} else {mserv.send ( 200, "text/plain", "Wrong Input number" );}

	}

}



void HandleRoot() {

	char temp[400];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 TDA7439 CSR8645 INTERFACE</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello there!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

		hr, min % 60, sec % 60
	);
	mserv.send ( 200, "text/html", temp );

}

void HandleNotFound() {

	String message = "Invalid Request! (404)\n\n";
	message += "URI: ";
	message += mserv.uri();
	message += "\nMethod: ";
	message += ( mserv.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += mserv.args();
	message += "\n";

	for ( uint8_t i = 0; i < mserv.args(); i++ ) {
		message += " " + mserv.argName ( i ) + ": " + mserv.arg ( i ) + "\n";
	}

	mserv.send ( 404, "text/plain", message );

}
