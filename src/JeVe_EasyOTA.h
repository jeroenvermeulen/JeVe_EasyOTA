/*
 * JeVe_EasyOTA.h - library to include to allow Over-The-Air updates of ESP8266 and ESP32
 *
 * Inspired on:
 *    http://simplestuffmatters.com/?p=69
 *    https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS/examples/OTA-mDNS-SPIFFS
 *    https://github.com/espressif/arduino-esp32/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino
 *
 *  mDNS monitor (OSX):  dns-sd -B _arduino._tcp
 *
 */

#ifndef EasyOTA_h
#define EasyOTA_h

#ifdef ESP8266
#include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
#include <ESP8266mDNS.h>  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266mDNS/ESP8266mDNS.h
#endif

#ifdef ESP32
#include <WiFi.h>         // https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFi.h
#include <ESPmDNS.h>      // https://github.com/espressif/arduino-esp32/blob/master/libraries/ESPmDNS/src/ESPmDNS.h
#endif

#include <WiFiUdp.h>      // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiUdp.h
#include <ArduinoOTA.h>   // https://github.com/esp8266/Arduino/blob/master/libraries/ArduinoOTA/ArduinoOTA.h

class EasyOTA
{
  public:
    typedef std::function<void(char *message, int line_nr)> THandlerFunction_Message;
    EasyOTA();
    void setup(char* wifi_ssid, char* wifi_password = "", char* hostname = "");
    void loop();
    void onMessage(THandlerFunction_Message fn);

  private:
    THandlerFunction_Message on_message;
    void showMessage(char *message, int line_nr=1);
    void showMessage(String message, int line_nr=1);
};

#endif