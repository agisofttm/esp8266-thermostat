#include "temperature.h"

Temperature::Temperature() {
}

void Temperature::setup() {
  dht = new DHT_Unified(DHTPIN, DHTTYPE);
  dht->begin();

  


  sensor_t sensor;
  dht->temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht->humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");

  float newReading = readFromSensor();

  total = newReading * NUM_READINGS;
  for (int i=0; i < NUM_READINGS; i++) {
    readings[i] = newReading;
  }
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

  sensors_event_t event;
  dht->temperature().getEvent(&event);
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
  // Get humidity event and print its value.
  dht->humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }


}
