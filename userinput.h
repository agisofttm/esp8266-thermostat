#ifndef userinput_h
#define userinput_h

#include "Arduino.h"
#include "config.h"


class UserInput {

    public:
        UserInput();
        void setup();
        void increaseRotary();
        void decreaseRotary();
        void pollButton( void (*pressCallback)(), void (*holdCallback)());
        int getRotary();

    private:
    static void rotaryInterrupt();
    static volatile int rotaryPosition;
    int buttonVal = 0;
    int lastButtonVal = 0;
    long btnDnTime; // time the button was pressed down
    long btnUpTime; // time the button was released
    bool emittedHold = false;
        
};

#endif

