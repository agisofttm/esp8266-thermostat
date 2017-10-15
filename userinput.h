#ifndef userinput_h
#define userinput_h

#include "Arduino.h"

#define ENCODER_CLK        D1      // This pin must have a minimum 0.47 uF capacitor
                                    // The current library only allows pins 2 or 3
#define ENCODER_DT         D2     // data pin
#define ENCODER_SW         D6      // switch pin (active LOW)
#define MAX_ROTARIES        1      // this example define two rotaries for this encoder

class UserInput {

    public:
        UserInput();
        void setup();
        void increaseRotary();
        void decreaseRotary();
        int getRotary();

    private:
    static void rotaryInterrupt();
    static volatile int rotaryPosition;

       
        
};

#endif

