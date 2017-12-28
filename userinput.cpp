#include "userinput.h"

volatile int UserInput::rotaryPosition = 0;

UserInput::UserInput() {
}

void UserInput::setup() {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_BTN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK),
                  UserInput::rotaryInterrupt, FALLING);

  UserInput::rotaryPosition = 0;

}

void UserInput::rotaryInterrupt() {
  if ( digitalRead(ENCODER_DT) == HIGH ) {
    Serial.println("increasing rot");
    UserInput::rotaryPosition ++;
  } else {
    Serial.println("decreasing rot");
    UserInput::rotaryPosition --;
  }

}

void UserInput::pollButton( void (*pressCallback)(), void (*holdCallback)()) {
  buttonVal = digitalRead(ENCODER_BTN);

  // press
  if (buttonVal == LOW && lastButtonVal == HIGH && (millis() - btnUpTime) > long(DEBOUNCE_TIME)) {
    btnDnTime = millis();
    emittedHold = false;
  }
  // release

  if (buttonVal == HIGH && lastButtonVal == LOW &&
      (millis() - btnDnTime) > long(DEBOUNCE_TIME)
      &&  (millis() - btnDnTime) < long(HOLD_TIME)) {
    pressCallback();
  }

  if (buttonVal == LOW && (millis() - btnDnTime) > long(HOLD_TIME) && ! emittedHold) {
    emittedHold = true;
    holdCallback();
  }

  lastButtonVal = buttonVal;
}

void UserInput::increaseRotary() {
  UserInput::rotaryPosition ++;
}

void UserInput::decreaseRotary() {
  UserInput::rotaryPosition --;
}

int UserInput::getRotary() {
  //Serial.println("checking rotary");

  int pos = UserInput::rotaryPosition;
  UserInput::rotaryPosition = 0;
  return - pos;
}

