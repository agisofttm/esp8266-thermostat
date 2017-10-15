#include "display.h"
#include "serialControl.h"
#include "temperature.h"
#include "relay.h"
#include "userinput.h"
#include "web.h"
#include <TaskScheduler.h>


// TODO
// Set max and min target temperature

Scheduler runner;

Display * display = new Display();
Temperature temperature;
Relay relay;
SerialControl * serial = new SerialControl();
UserInput userInput;
Web web(display);




float targetTemp = 22.0;
float currentTemp = 0.0;
float margin = 0.2;

#define TEMPERATURE_MEASURE_INTERVAL 1000
#define CONTROLLER_UPDATE_INTERVAL 10000
#define DISPLAY_FRAME_PERIOD 50
#define DISPLAY_TIMEOUT 1200

#define ENCODER_INCREMENT 0.5

#define MAX_TEMP 35.0
#define MIN_TEMP 5.0

void measureTemperature();
void updateDisplay();
void updateController();
void checkInputs();
void displayTimeout();
Task measureTask(TEMPERATURE_MEASURE_INTERVAL, TASK_FOREVER, &measureTemperature);
Task controllerTask(CONTROLLER_UPDATE_INTERVAL , TASK_FOREVER, &updateController);
Task displayTask(DISPLAY_FRAME_PERIOD, TASK_FOREVER, &updateDisplay);
Task inputTask(DISPLAY_FRAME_PERIOD, TASK_FOREVER, &checkInputs);
Task displayTimeoutTask(DISPLAY_TIMEOUT, TASK_ONCE, &displayTimeout);

void wifiConfigCallback(WiFiManager* wfm) {
  display->showWifiSetupView(SETUP_SSID, SETUP_PASSWORD);
  display->update();
}

void wifiSaveConfigCallback() {
  web.setSaveConfig();
}

void setupTasks() {
  runner.init();
  //runner.addTask(displayTimeout, TASK_ONCE, &displayTimeout);

  runner.addTask(measureTask);
  measureTask.enable();
  runner.addTask(controllerTask);
  controllerTask.enable();
  runner.addTask(displayTask);
  displayTask.enable();
  runner.addTask(inputTask);
  inputTask.enable();
  runner.addTask(displayTimeoutTask);
}

void setup() {
  // put your setup code here, to run once:

  serial->setup();
  display->setup();
  temperature.setup();
  userInput.setup();
  relay.setup();

  web.setup();

  setupTasks();
}



void updateDisplay() {
  display->update();
}


void checkInputs() {
  int rotary = userInput.getRotary();
  //serial->write("read rotary" + String(rotary));
  if (rotary != 0) {
    targetTemp += ENCODER_INCREMENT * (float) rotary;
    if (targetTemp > MAX_TEMP) targetTemp = MAX_TEMP;
    if (targetTemp < MIN_TEMP) targetTemp = MIN_TEMP;
    updateRelay();
    display->showAdjustView(targetTemp, relay.isOn());
    displayTimeoutTask.restartDelayed(DISPLAY_TIMEOUT);

  }
}

void displayTimeout() {
  serial->write("Display Timeout");
  display->showHomeView(currentTemp, relay.isOn());
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
  if (relay.isOn() && currentTemp > targetTemp + margin) {
    relay.switchOff();
  } else if  (!relay.isOn() && currentTemp < targetTemp - margin) {
    relay.switchOn();
  }
}


void updateController() {

  float newTemp = temperature.get();
  serial->write("Measuring Temperature:" + String(newTemp, 1));

  if (newTemp != currentTemp) {
    serial->write("Changed from previous temperature of " + String(currentTemp, 1));
    currentTemp = newTemp;
    updateRelay();
    display->showHomeView(currentTemp, relay.isOn());
  }


}
