#ifndef Display_h
#define Display_h
#include "SSD1306.h"
//#include "SSD1306Ui.h"
#include "fonts.h"
//#include <SSD1306.h>
//#include <SSD1306Fonts.h>
//#include <SSD1306Ui.h>
//#include <Wire.h>


#define OLED_SDA    D3
#define OLED_SDC    D4
#define OLED_ADDR   0x3C

#include "Arduino.h"

class Display {

    public:
        Display();
        void setup();
        void update();
        void draw(String command);
        void showSplashScreen();
        void showWifiSetupView(String ssid, String password);
        void showHomeView(float currentTemperature, bool flame);
        void showAdjustView(float targetTemperature, bool flame);
        void markDirty();
        

    private:
        SSD1306 * d;
        void drawTitle(String title, bool inverted);
        void drawBigTemperature(float temperature);
        void drawFlameIndicator();
        void drawWifiIndicator();
        bool dirty = false;


};

#endif

