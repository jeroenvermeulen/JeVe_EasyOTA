/*
 * To start mDNS monitor (OSX) exec:   dns-sd -B _arduino._tcp
 */

#include <JeVe_EasyOTA.h>           // https://github.com/jeroenvermeulen/JeVe_EasyOTA/blob/master/JeVe_EasyOTA.h
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define WIFI_SSID        "your-wifi-ssid"
#define WIFI_PASSWORD    "your-wifi-password"
#define ARDUINO_HOSTNAME "ota-flash-demo"
EasyOTA OTA;

LiquidCrystal_PCF8574 lcd(0x3F);  // LCD address, you can also try 0x3F or use I2cScanner

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2); // initialize the lcd
  lcd.home();
  lcd.clear();
  // This callback will be called when JeVe_EasyOTA has anything to tell you.
  OTA.onMessage([](char *message, int line) {
    //Serial.println(message);
    lcd.print(message);
  });
  OTA.setup(WIFI_SSID, WIFI_PASSWORD, ARDUINO_HOSTNAME);
}

void loop() {
  OTA.loop();
}
