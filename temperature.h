#ifndef temperature_h
#define temperature_h

#include "Arduino.h"
#include "config.h"

#define ERROR_READING -99

#ifdef DHT_SENSOR
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTTYPE  DHT11
#endif

#ifdef DALLAS_SENSOR
#include <OneWire.h>
#include <DallasTemperature.h>
#endif


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
#ifdef DHT_SENSOR
        DHT_Unified* sensor;
#endif
#ifdef DALLAS_SENSOR
        OneWire* oneWire;
        DallasTemperature* sensor;
#endif
};

#endif

