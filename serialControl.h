#ifndef SerialControl_h
#define SerialControl_h

#include "Arduino.h"
#include "config.h"

class SerialControl {

    public:
        SerialControl();
        void setup();
        void onData();
        boolean received();
        String getCommand();
        void write(String s);

    private:
        String command; 
        boolean receivedCommand; 
};

#endif

