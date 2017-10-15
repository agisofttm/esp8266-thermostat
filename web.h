#ifndef web_h
#define web_h

#include "Arduino.h"

#include "display.h"

#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#define SETUP_SSID "setup_thermostat"
#define SETUP_PASSWORD "iamcold"

void wifiConfigCallback(WiFiManager * wfm);
void wifiSaveConfigCallback();

class Web {

  public:
    Web(Display * _display);
    void setup();
    void setSaveConfig();


  private:
    void configureWifi();
    
    WiFiManager * wiFiManager;

    //define your default values here, if there are different values in config.json, they are overwritten.
    char mqtt_server[40];
    char mqtt_port[6];// = "8080";
    char blynk_token[34];// = "YOUR_BLYNK_TOKEN";
    bool shouldSaveConfig = false;

    Display * display;

};

#endif

