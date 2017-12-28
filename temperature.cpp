#include "temperature.h"

Temperature::Temperature() {
}

void Temperature::setup() {


#ifdef DHT_SENSOR
  sensor = new DHT_Unified(TEMPERATURE_PIN, DHTTYPE);
#endif

#ifdef DALLAS_SENSOR
  oneWire = new OneWire(TEMPERATURE_PIN);
  sensor = new DallasTemperature(oneWire);
#endif

  sensor->begin();
  float newReading = readFromSensor();

  total = newReading * NUM_READINGS;
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = newReading;
  }

#ifdef DALLAS_SENSOR
  // activate async mode
  sensor->setWaitForConversion(false);
#endif
}

float Temperature::get() {
  return total / float(NUM_READINGS);
}

void Temperature::performReading() {

  float newReading = readFromSensor();

  if (newReading != ERROR_READING) {

    total = total - readings[readingsIndex];
    // read from the sensor:
    readings[readingsIndex] = newReading;
    // add the reading to the total:
    total = total + readings[readingsIndex];
    // advance to the next position in the array:
    readingsIndex ++;

    if (readingsIndex >= NUM_READINGS) {
      readingsIndex = 0;
    }

  }
}

float Temperature::readFromSensor() {


#ifdef DHT_SENSOR
  sensors_event_t event;
  sensor->temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
    return ERROR_READING;
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");

    return event.temperature;
  }

#endif

#ifdef DALLAS_SENSOR

  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.

  float temperature = sensor->getTempCByIndex(0);
  if (temperature < -126) {
    Serial.println("Error reading temperature!");
    return ERROR_READING;
  }
  //Serial.print("Temperature: ");
  //Serial.println(temperature);

    sensor->requestTemperatures(); // Send the command to get temperature for next time
  return temperature;
#endif
}
