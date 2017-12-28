#ifndef Display_h
#define Display_h
#include "SSD1306.h"
#include "config.h"
#include "fonts.h"

#include "Arduino.h"

class Display {

    public:
        Display();
        void setup();
        void update();
        void draw(String command);
        void showSplashScreen();
        void showWifiSetupView(String ssid, String password);
        void showHomeView(float currentTemperature, bool flame, InternetMode wifi, int boostMins);
        void showAdjustView(float targetTemperature, bool flame, InternetMode wifi);
        void showAdjustBoostView(int boostMins);
        void showMQTTConnectView();
        void markDirty();
        

    private:
        SSD1306 * d;
        void drawTitle(String title, bool inverted);
        void drawBigTemperature(float temperature);
        void drawBigBoost(int boostMins);
        void drawFlameIndicator();
        void drawBoostIndicator();
        void drawBoostMins(int mins);
        void drawInternetIndicator(InternetMode internet);

        bool dirty = false;


};

#endif

