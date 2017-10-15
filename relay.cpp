#include "relay.h"

Relay::Relay() {
    state = false;
     
}

void Relay::setup() {
   digitalWrite(RELAY_PIN,HIGH);
   pinMode(RELAY_PIN,OUTPUT);
   
}


void Relay::switchOn() {
    Serial.println("relay switched ON");
    digitalWrite(RELAY_PIN,LOW);
    state = true;
}


void Relay::switchOff() {
    Serial.println("relay switched OFF");
    digitalWrite(RELAY_PIN,HIGH);
    state = false;
}

bool Relay::isOn() {
    return state;
}
