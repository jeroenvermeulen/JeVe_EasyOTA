/*
 * JV_OTA.h - library to include to allow Over-The-Air updates of ESP8266
 *
 * Inspired on:
 *    http://simplestuffmatters.com/?p=69
 *    https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS/examples/OTA-mDNS-SPIFFS
 *
 *  mDNS monitor (OSX):  dns-sd -B _arduino._tcp .
 *
 */

#ifndef JV_OTA_h
#define JV_OTA_h

#include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
#include <ESP8266mDNS.h>  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266mDNS/ESP8266mDNS.h
#include <WiFiUdp.h>      // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiUdp.h
#include <ArduinoOTA.h>   // https://github.com/esp8266/Arduino/blob/master/libraries/ArduinoOTA/ArduinoOTA.h

class JV_OTA
{
  public:
    typedef std::function<void(char *message, int line)> THandlerFunction_Message;

    JV_OTA(void);
    void setup(char* wifi_ssid, char* wifi_password = "", char* wifi_hostname = "");
    void loop();
    void onMessage(THandlerFunction_Message fn);

  private:
    char*  wifi_ssid;
    char*  wifi_password;
    char*  wifi_hostname;
    THandlerFunction_Message on_message;
    void showMessage(char *message, int line=1);
    void showMessage(String message, int line=1);
};

#endif