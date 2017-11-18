/*
 * JV_OTA.cpp - library to include to allow Over-The-Air updates of ESP8266
 *
 * Inspired on:
 *    http://simplestuffmatters.com/?p=69
 *    https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS/examples/OTA-mDNS-SPIFFS
 */
 
#include "JV_OTA.h"

WiFiServer TelnetServer(8266);

JV_OTA::JV_OTA(char* wifi_ssid, char* wifi_password, char* wifi_hostname) {
    this->wifi_ssid = wifi_ssid;
    this->wifi_password = wifi_password;
    this->wifi_hostname = wifi_hostname;
    //this->telnetServer = new WifiServer(8266); // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiServer.h
};

void JV_OTA::setup() {
  delay(100);
  String hostname(this->wifi_hostname);
  hostname += String(ESP.getChipId(), HEX);
  delay(500);
  showMessage("Connecting Wifi:", this->wifi_ssid);
  WiFi.hostname(this->wifi_hostname);
  WiFi.begin(this->wifi_ssid, this->wifi_password);
  unsigned long startTime = millis();
  String line2 = "";
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    line2 += ".";
    showMessage("Connecting Wifi:", line2);
  }

  if (WiFi.status() == WL_CONNECTED) {
    showMessage("IP address:", WiFi.localIP().toString());
  } else {
    Serial.println("Can not connect to WiFi station. Go into AP mode.");
    // Go into software AP mode.
    WiFi.mode(WIFI_AP);
    delay(10);
    WiFi.softAP(hostname.c_str());
    String line1 = "AP:" + hostname;
    showMessage(line1, WiFi.softAPIP().toString());
  }

  Serial.print("Configuring OTA device...");
  TelnetServer.begin();   //Necesary to make Arduino Software autodetect OTA device

  ArduinoOTA.onStart([]() {
    //JV_OTA::showMessage("OTA starting...");
  });
  ArduinoOTA.onEnd([]() {
    //JV_OTA::showMessage("OTA finished.", "Rebooting...");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    static unsigned int prevPerc = 100;
    unsigned int perc = (progress / (total / 100));
    unsigned int roundPerc = 5 * (int)(perc / 5);
    if ( roundPerc != prevPerc) {
      prevPerc = roundPerc;
      String line1 = "OTA upload " + String(roundPerc) + "%";
      //JV_OTA::showMessage(line1);
    }
  });
  ArduinoOTA.onError([](ota_error_t error) {
    String line1 = "Error " + String(error) + ": ";
    String line2 = "";
    if (error == OTA_AUTH_ERROR)         line2 = "Auth Failed";
    else if (error == OTA_BEGIN_ERROR)   line2 = "Begin Failed";
    else if (error == OTA_CONNECT_ERROR) line2 = "Connect Failed";
    else if (error == OTA_RECEIVE_ERROR) line2 = "Receive Failed";
    else if (error == OTA_END_ERROR)     line2 = "End Failed";
    //JV_OTA::showMessage(line1, line2);
  });
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.setPassword("flash4me");
  ArduinoOTA.begin();
  Serial.println("Wifi OK");
};

void JV_OTA::loop() {
  ArduinoOTA.handle();
}

void JV_OTA::showMessage(char *line1, char *line2 ) {
  Serial.println(line1);
  if ("" != line2) {
    Serial.println(line2);
  }
}

void JV_OTA::showMessage(char * line1) {
  showMessage( line1, "" );
}
void JV_OTA::showMessage(char * line1, String line2) {
  showMessage( line1, (char *)line2.c_str() );
}
void JV_OTA::showMessage(String line1) {
  showMessage( (char *)line1.c_str() );
}
void JV_OTA::showMessage(String line1, char * line2) {
  showMessage( (char *)line1.c_str(), line2 );
}
void JV_OTA::showMessage(String line1, String line2) {
  showMessage( (char *)line1.c_str(), (char *)line2.c_str() );
}
