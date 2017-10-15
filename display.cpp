#include "display.h"

Display::Display() {
  d = new SSD1306(OLED_ADDR, OLED_SDA, OLED_SDC);

}

void Display::setup() {


  d->init();

  d->flipScreenVertically();
  d->setContrast(0x20);

  showSplashScreen();
  update();
}

void Display::draw(String command) {

  d->clear();

  // The coordinates define the center of the text
  //display->setTextAlignment(TEXT_ALIGN_CENTER);
  d->setTextAlignment(TEXT_ALIGN_CENTER);
  d->setFont(ArialMT_Plain_16);
  d->drawString(64, 0, command);
  d->display();
  

}


void Display::showSplashScreen() {
  d->clear();

  d->setColor(WHITE);
  d->drawFastImage(56,0,16,16,flame);
  d->setTextAlignment(TEXT_ALIGN_CENTER);
  d->setFont(Noto_Sans_Bold_16);
  
  d->drawString(64, 12, "Smart");
  d->drawString(64, 32, "Thermostat");

  d->setFont(ArialMT_Plain_10);
  d->drawStringMaxWidth(64, 50, 128, "connecting to Wi-Fi...");
  markDirty();

}

void Display::showWifiSetupView(String ssid, String password) {
  d->clear();
  drawTitle("setup", true);
  drawWifiIndicator();

  d->setFont(ArialMT_Plain_10);
  String setupText = "Connect to SSID: " + ssid + ", PASSWORD: " + password + ", http://192.168.4.1";
  d->drawStringMaxWidth(64, 15, 128, setupText);
  markDirty();

}

void Display::showHomeView(float currentTemperature, bool flame) {
  d->clear();
  drawTitle("current", false);
  drawBigTemperature(currentTemperature);

  if (flame) drawFlameIndicator();

  markDirty();

}

void Display::showAdjustView(float targetTemperature, bool flame) {
  d->clear();
  drawTitle("aim for", true);
  drawBigTemperature(targetTemperature);

  if (flame) drawFlameIndicator();
  
  markDirty();
}

void Display::drawTitle(String title, bool inverted) {
  
  d->setColor(WHITE);
  if (inverted) {
    d->fillRect(18, 0, 92, 16);
    d->setColor(BLACK);
    d->setPixel(18,0);
    d->setPixel(109,0);
    d->setPixel(18,15);
    d->setPixel(109,15);
  }
  
  d->setTextAlignment(TEXT_ALIGN_CENTER);
  d->setFont(Noto_Sans_Bold_16);
  d->drawString(64, -4, title);

}


void Display::drawBigTemperature(float temperature) {
  d->setColor(WHITE);
  d->setFont(Noto_Sans_Bold_50);
  d->setTextAlignment(TEXT_ALIGN_RIGHT);
  d->drawString(105, 4, String(temperature, 1));

  d->setTextAlignment(TEXT_ALIGN_LEFT);
  d->setFont(Noto_Sans_Bold_16);
  d->drawString(105, 16, "°C");

}

void Display::drawFlameIndicator() {
  d->setColor(WHITE);
  d->drawFastImage(0,0,16,16,flame);
}

void Display::drawWifiIndicator() {
  d->setColor(WHITE);
  d->drawFastImage(112,0,16,16,wifi);
}

void Display::update() {
  if (dirty) {
    d->display();
    dirty = false;
  }
}

void Display::markDirty() {
  dirty = true;
}

