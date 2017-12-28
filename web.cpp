#include "web.h"


Web::Web(Display * _display) {
  Serial.println("Init");
  display = _display;
}

InternetMode Web::setup() {
  return configureWifi();
}

void Web::setSaveConfig() {
  shouldSaveConfig = true;
}

void Web::enterForcedConfigMode() {

  File flagFile = SPIFFS.open("/force_config_mode", "w");
  if (!flagFile) {
    Serial.println("failed to create file");
  }
  flagFile.close();
  //reset and try again, or maybe put it to deep sleep
  ESP.restart();

}

void Web::loadConfig() {

  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(mqtt_username, json["mqtt_username"]);
          strcpy(mqtt_password, json["mqtt_password"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

}

InternetMode Web::configureWifi() {

  loadConfig();

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_username("user", "mqtt username", mqtt_username, 32);
  WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 32);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  wiFiManager = new WiFiManager();

  //set config save notify callback
  wiFiManager->setSaveConfigCallback(wifiSaveConfigCallback);

  //set static ip
  //wiFiManager->setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //add all your parameters here
  wiFiManager->addParameter(&custom_mqtt_server);
  wiFiManager->addParameter(&custom_mqtt_port);
  wiFiManager->addParameter(&custom_mqtt_username);
  wiFiManager->addParameter(&custom_mqtt_password);

  wiFiManager->setAPCallback(wifiConfigCallback);


  //reset settings - for testing
  //wiFiManager->resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wiFiManager->setMinimumSignalQuality();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wiFiManager->setTimeout(120);

  //fetches ssid and pass and tries to connect
  bool didConnect;

  if (SPIFFS.exists("/force_config_mode")) {
    SPIFFS.remove("/force_config_mode");
    didConnect = wiFiManager->startConfigPortal(SETUP_SSID, SETUP_PASSWORD);
  } else {
    didConnect = wiFiManager->autoConnect(SETUP_SSID, SETUP_PASSWORD);
  }
  if (!didConnect) {
    Serial.println("failed to connect and hit timeout");
    //delay(3000);
    //reset and try again, or maybe put it to deep sleep
    //ESP.reset();
    //delay(5000);
    return NO_WIFI;
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_username, custom_mqtt_username.getValue());
  strcpy(mqtt_password, custom_mqtt_password.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_username"] = mqtt_username;
    json["mqtt_password"] = mqtt_password;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  wiFiClient = new WiFiClient();
  pubSubClient = new PubSubClient(*wiFiClient);
  pubSubClient->setServer(mqtt_server, 1883);
  pubSubClient->setCallback(pubSubCallbackStatic);
  reconnect();
  return (pubSubClient->connected()) ? CONNECTED : NO_MQTT;

}


void Web::pubSubCallback(char* topicData, byte * payloadData, unsigned int length) {
  String topic = String(topicData);
  String payload = String((char*)payloadData);
  Serial.print("Message arrived on topic '");
  Serial.print(topic);
  Serial.println("'");

  if (topic == "thermostat/boost") {
    Serial.print("Got boost, payload is '");
    Serial.print(payload);
    Serial.println("'");

    boostCounter = 60; // number of minutes to boost
  }

  //Serial.print("payload");
  //for (int i = 0; i < length; i++) {
  //  Serial.print((char)payload[i]);
  //}
  //Serial.println();
}

void Web::reconnect() {
  lastReconnect = millis();
  display->showMQTTConnectView();
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (pubSubClient->connect("SmartThermostat", mqtt_username, mqtt_password)) {
    Serial.println("connected");
    // Once connected, publish an announcement...
    pubSubClient->publish("thermostat/status", "connected");
    // ... and resubscribe
    pubSubClient->subscribe("thermostat");
  } else {
    Serial.print("failed, rc=");
    Serial.print(pubSubClient->state());
  }
}

int Web::getBoostCounter() {
  int tempValue = boostCounter;
  boostCounter = 0;
  return tempValue;
}

void Web::update(float temperature) {
  Serial.println("Update method on web");
  if (!pubSubClient->connected()) {

    long timeSinceLastUpdate = millis() - lastReconnect;

    if (timeSinceLastUpdate > MIN_RECONNECT_INTERVAL || timeSinceLastUpdate < 0 ) {
      Serial.println("calling reconnection");
      reconnect();
    } else {
      Serial.println("skipping reconnection as 5 minutes has not yet elapsed since last reconnect");
      Serial.println(timeSinceLastUpdate);
    }
  }

  if (pubSubClient->connected()) {
    char msg[100];
    //String payload = "{ \"temperature\": " + String(temperature) + " }";
    String payload = String(temperature);
    payload.toCharArray(msg, (payload.length() + 1));
    Serial.print("Publish message: ");
    Serial.println(msg);
    pubSubClient->publish("thermostat/temperature", msg);
  }
}

void Web::loop() {
  if (pubSubClient->connected()) {
    pubSubClient->loop();
  }
}

