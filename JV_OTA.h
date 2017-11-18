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
    char*  wifi_ssid;
    char*  wifi_password;
    char*  wifi_hostname;
    WiFiServer *telnetServer;

    JV_OTA( char* wifi_ssid, char* wifi_password = "", char* wifi_hostname = "" );
    void setup();
    void loop();
    void showMessage(char * line1, char* line2);
    void showMessage(char * line1);
    void showMessage(char * line1, String line2);
    void showMessage(String line1);
    void showMessage(String line1, char * line2);
    void showMessage(String line1, String line2);
};

#endif