#include "userinput.h"


volatile int UserInput::rotaryPosition = 0;


UserInput::UserInput() {


}

void UserInput::setup() {
   Serial.println("setting up rotary");
  pinMode(ENCODER_CLK,INPUT_PULLUP);
  pinMode(ENCODER_DT,INPUT_PULLUP);
  pinMode(ENCODER_SW,INPUT_PULLUP);

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

