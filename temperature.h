#ifndef temperature_h
#define temperature_h


#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "Arduino.h"

#define DHTPIN   D7
#define DHTTYPE  DHT11

#define NUM_READINGS 20
#define ERROR_READING -1234


class Temperature {

    public:
        Temperature();
        void setup();
        float get();
        void writeDummy(float value);
        void performReading();

    private:
        float readFromSensor();
        
        float readings[NUM_READINGS];
        int readingsIndex = 0;
        float total;
        DHT_Unified* dht;
        

};

#endif

