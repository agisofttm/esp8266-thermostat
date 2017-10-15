#ifndef relay_h
#define relay_h

#include "Arduino.h"

#define RELAY_PIN D5
class Relay {

    public:
        Relay();
        void setup();
        void switchOn();
        void switchOff();
        bool isOn();

    private:
        bool state;

};

#endif

