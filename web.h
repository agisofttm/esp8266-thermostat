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

#include <PubSubClient.h>
#include "config.h"



void wifiConfigCallback(WiFiManager * wfm);
void wifiSaveConfigCallback();
void pubSubCallbackStatic(char* topic, byte* payload, unsigned int length);

class Web {

  public:
    Web(Display * _display);
    InternetMode setup();
    void update(float temperature);
    void loop();
    void setSaveConfig();
    void pubSubCallback(char* topic, byte* payload, unsigned int length);
    int getBoostCounter();
    void enterForcedConfigMode();


  private:
    InternetMode configureWifi();
    void loadConfig();
    void reconnect(); 
    
    WiFiManager * wiFiManager;
    WiFiClient * wiFiClient;
    PubSubClient* pubSubClient;

    //define your default values here, if there are different values in config.json, they are overwritten.
    char mqtt_server[40];
    char mqtt_port[6];// = "8080";
    char mqtt_username[32];
    char mqtt_password[32];
    bool shouldSaveConfig = false;
    int boostCounter = 0;

    unsigned long lastReconnect = 1000000; // make it huge!

    Display * display;

};

#endif

