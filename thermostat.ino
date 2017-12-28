#include "display.h"
#include "serialControl.h"
#include "temperature.h"
#include "relay.h"
#include "userinput.h"
#include "web.h"
#include "config.h"
#include <TaskScheduler.h>

enum OverrideMode {
  FORCE_ON,
  FORCE_OFF,
  NO_OVERRIDE,
};

enum DisplayMode {
  HOME,
  SET_TEMPERATURE,
  SET_BOOST,
};

Scheduler runner;

Display * display = new Display();
Temperature temperature;
Relay relay;
SerialControl * serial = new SerialControl();
UserInput userInput;
Web web(display);

int boostMinsRemaining = -1;

InternetMode internetMode = NO_WIFI;
DisplayMode displayMode = HOME;

float targetTemp = 22.0;
float currentTemp = 0.0;
float margin = 0.2;

OverrideMode overrideMode = NO_OVERRIDE;

void measureTemperature();
void updateDisplay();
void updateWeb();
void loopWeb();
void updateRelay();
void updateController();
void checkInputs();
void displayTimeout();
void boostUpdate();

Task measureTask(TEMPERATURE_MEASURE_INTERVAL, TASK_FOREVER, &measureTemperature);
Task controllerTask(CONTROLLER_UPDATE_INTERVAL , TASK_FOREVER, &updateController);
Task webLoopTask(WEB_LOOP_INTERVAL , TASK_FOREVER, &loopWeb);
Task webUpdateTask(WEB_UPDATE_INTERVAL , TASK_FOREVER, &updateWeb);
Task displayTask(DISPLAY_FRAME_PERIOD, TASK_FOREVER, &updateDisplay);
Task inputTask(DISPLAY_FRAME_PERIOD, TASK_FOREVER, &checkInputs);
Task displayTimeoutTask(DISPLAY_TIMEOUT, TASK_ONCE, &displayTimeout);
Task boostUpdateTask(BOOST_UPDATE_TASK, TASK_FOREVER, &boostUpdate);

void wifiConfigCallback(WiFiManager* wfm) {
  display->showWifiSetupView(SETUP_SSID, SETUP_PASSWORD);
  display->update();
}

void pubSubCallbackStatic(char* topic, byte* payload, unsigned int length) {
  serial->write("Got a callback from mqtt");
  web.pubSubCallback(topic, payload, length);
}

void wifiSaveConfigCallback() {
  web.setSaveConfig();
}

void setupTasks() {
  runner.init();

  runner.addTask(measureTask);
  measureTask.enable();
  runner.addTask(controllerTask);
  controllerTask.enable();
  if (internetMode != NO_WIFI) {
    runner.addTask(webLoopTask);
    webLoopTask.enable();
    runner.addTask(webUpdateTask);
    webUpdateTask.enable();
  }
  runner.addTask(displayTask);
  displayTask.enable();
  runner.addTask(inputTask);
  inputTask.enable();
  runner.addTask(displayTimeoutTask);
  runner.addTask(boostUpdateTask);
  boostUpdateTask.enable();

}

void setup() {
  // put your setup code here, to run once:

  serial->setup();
  display->setup();
  temperature.setup();
  userInput.setup();
  relay.setup();

  internetMode = web.setup();

  setupTasks();

  serial->write("Setup Complete");
}


void updateDisplay() {
  display->update();

}

void clickCallback() {
  displayMode = SET_BOOST;
  display->showAdjustBoostView(boostMinsRemaining);
  displayTimeoutTask.restartDelayed(DISPLAY_TIMEOUT);

}

void holdCallback() {
  web.enterForcedConfigMode();
}

void checkInputs() {

  int rotary = userInput.getRotary();
  //serial->write("read rotary" + String(rotary));
  if (rotary != 0) {

    if (displayMode == SET_BOOST) {
      boostMinsRemaining += ENCODER_BOOST_INCREMENT * (float) rotary;
      // round
      boostMinsRemaining = ( (boostMinsRemaining + ENCODER_BOOST_INCREMENT / 2) / ENCODER_BOOST_INCREMENT) * ENCODER_BOOST_INCREMENT;

      if (boostMinsRemaining > MAX_BOOST) boostMinsRemaining = MAX_BOOST;
      if (boostMinsRemaining < 0) boostMinsRemaining = 0;

      display->showAdjustBoostView(boostMinsRemaining);
      displayTimeoutTask.restartDelayed(DISPLAY_TIMEOUT);
    } else {
      targetTemp += ENCODER_TEMPERATURE_INCREMENT * (float) rotary;
      if (targetTemp > MAX_TEMP) targetTemp = MAX_TEMP;
      if (targetTemp < MIN_TEMP) targetTemp = MIN_TEMP;
      updateRelay();
      display->showAdjustView(targetTemp, relay.isOn(), internetMode);
      displayTimeoutTask.restartDelayed(DISPLAY_TIMEOUT);

    }
  }

  userInput.pollButton(clickCallback, holdCallback);

}

void displayTimeout() {
  displayMode = HOME;
  serial->write("Display Timeout");
  display->showHomeView(currentTemp, relay.isOn(), internetMode, boostMinsRemaining);
}

void loop() {

  runner.execute();
  debugSerial();

}

void debugSerial() {
  if (Serial.available()) {
    serial->onData();
  }

  if (serial->received()) {
    //serial->write("received serial");
    String command = serial->getCommand();
    serial->write("Input: " + command);
    //serial->write(command.substring(0, 1));
    if (command.substring(0, 1) == "r") {
      serial->write("Increasing Rotary");

      userInput.increaseRotary();
    }
    else if (command.substring(0, 1) == "l") {
      serial->write("Decreasing Rotary");
      userInput.decreaseRotary();
    }
  }
}

void measureTemperature() {
  temperature.performReading();
}

void updateRelay() {

  if (overrideMode == FORCE_ON) {
    relay.switchOn();
  } else if (overrideMode == FORCE_OFF) {
    relay.switchOff();
  } else {

    if (relay.isOn() && currentTemp > targetTemp + margin) {
      relay.switchOff();
    } else if  (!relay.isOn() && currentTemp < targetTemp - margin) {
      relay.switchOn();
    }

  }
}


void updateController() {

  float newTemp = temperature.get();
  serial->write("Measuring Temperature:" + String(newTemp, 1));

  if (newTemp != currentTemp) {
    serial->write("Changed from previous temperature of " + String(currentTemp, 1));
    currentTemp = newTemp;
    updateRelay();
    display->showHomeView(currentTemp, relay.isOn(), internetMode, boostMinsRemaining);
  }

}

void loopWeb() {
  web.loop();
}

void updateWeb() {
  web.update(currentTemp);

  int newBoost = web.getBoostCounter();
  if (newBoost > 0) {
    overrideMode = FORCE_ON;
    updateRelay();
    boostMinsRemaining = newBoost;
  }

}

void boostUpdate() {
  if (boostMinsRemaining == 0) {
    overrideMode = NO_OVERRIDE;
    updateRelay();
    boostMinsRemaining = -1; // disable boost

  } else {
    boostMinsRemaining -= 1;
  }



}
