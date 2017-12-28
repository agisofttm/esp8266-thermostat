#ifndef Config_h
#define Config_h

// Pinout
#define ENCODER_CLK       D1      // This pin must have a minimum 0.47 uF capacitor
#define ENCODER_DT        D2     // Encoder data pin
#define OLED_SDA          D3
#define OLED_SDC          D4
#define ENCODER_BTN       D5      // rotary encoder button (active LOW)
#define RELAY_PIN         D6
#define TEMPERATURE_PIN   D7


#define TEMPERATURE_MEASURE_INTERVAL 1000
#define WEB_LOOP_INTERVAL 2000
#define WEB_UPDATE_INTERVAL 15000
#define CONTROLLER_UPDATE_INTERVAL 10000
#define DISPLAY_FRAME_PERIOD 50
#define DISPLAY_TIMEOUT 2000
#define BOOST_UPDATE_TASK 60000

#define ENCODER_TEMPERATURE_INCREMENT 0.5
#define MAX_TEMP 35.0
#define MIN_TEMP 5.0

#define ENCODER_BOOST_INCREMENT 15
#define MAX_BOOST 120



// Web
// Wifi network to create during setup mode
#define SETUP_SSID "setup_thermostat"
#define SETUP_PASSWORD "freezing"

#define MIN_RECONNECT_INTERVAL 300000 // how long to wait before reconnecting to MQTT (which freezes
                                      // device for several seconds), currently 5 mins

// Display
#define OLED_ADDR   0x3C

// Temperature
#define NUM_READINGS 5 // number of readings to take a running average of. Bigger = more accuracy but also more lag
#define DALLAS_SENSOR // specify sensor used: DHT_SENSOR or DALLAS_SENSOR

// Rotary
#define DEBOUNCE_TIME 20 // ms debounce period to prevent flickering when pressing or releasing the button
#define HOLD_TIME 5000 // ms hold period: how long to wait for press+hold event


// Don't change below this line
enum InternetMode {
  NO_WIFI,
  NO_MQTT,
  CONNECTED,
};

#endif
