/*
 * EasyOTA.cpp - library to include to allow Over-The-Air updates of ESP8266
 *
 * Inspired on:
 *    http://simplestuffmatters.com/?p=69
 *    https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS/examples/OTA-mDNS-SPIFFS
 *
 *  mDNS monitor (OSX):  dns-sd -B _arduino._tcp .
 *
 */
 
#include "JeVe_EasyOTA.h"  // https://github.com/jeroenvermeulen/JeVe_EasyOTA/blob/master/JeVe_EasyOTA.h

//Necesary to make Arduino Software autodetect OTA device
WiFiServer TelnetServer(8266);

EasyOTA::EasyOTA() {
  // Constructor
}

void EasyOTA::onMessage(THandlerFunction_Message fn) {
  on_message = fn;
}

void EasyOTA::setup(char* wifi_ssid, char* wifi_password, char* hostname) {
  this->wifi_ssid = wifi_ssid;
  this->wifi_password = wifi_password;
  this->hostname = hostname;
  showMessage("", 1); // New line in case of using serial output
  showMessage("Connecting Wifi:", 1);
  showMessage(this->wifi_ssid, 2);
  WiFi.hostname(this->hostname);
  WiFi.begin(this->wifi_ssid, this->wifi_password);
  unsigned long startTime = millis();
  String progressDots = "";
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    progressDots += ".";
    showMessage(progressDots, 2);
  }

  if (WiFi.status() == WL_CONNECTED) {
    showMessage("IP Address:", 1);
    showMessage(WiFi.localIP().toString(), 2);
  } else {
    showMessage("Can't connect WiFi", 1);
    showMessage("Going into AP mode.", 2);
    WiFi.mode(WIFI_AP);
    delay(10);
    WiFi.softAP(this->hostname);
    showMessage("AP: " + String(this->hostname), 1);
    showMessage("IP: " + WiFi.softAPIP().toString(), 2);
  }

  TelnetServer.begin();  // Necesary to make Arduino Software autodetect OTA device

  ArduinoOTA.onStart([this]() {
    showMessage("OTA starting...", 1);
    showMessage("", 2);  // If display: Clean any previous error
  });
  ArduinoOTA.onEnd([this]() {
    showMessage("OTA finished.",1);
    showMessage("Rebooting...",2);
  });
  ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
    static unsigned int prevPerc = 100;
    unsigned int perc = (progress / (total / 100));
    unsigned int roundPerc = 5 * (int)(perc / 5);
    if ( roundPerc != prevPerc) {
      prevPerc = roundPerc;
      showMessage("OTA upload " + String(roundPerc) + "%");
    }
  });
  ArduinoOTA.onError([this](ota_error_t error) {
    showMessage("OTA Error " + String(error) + ":", 1);
    String line2 = "";
    if (error == OTA_AUTH_ERROR)         line2 = "Auth Failed";
    else if (error == OTA_BEGIN_ERROR)   line2 = "Begin Failed";
    else if (error == OTA_CONNECT_ERROR) line2 = "Connect Failed";
    else if (error == OTA_RECEIVE_ERROR) line2 = "Receive Failed";
    else if (error == OTA_END_ERROR)     line2 = "End Failed";
    showMessage(line2, 2);
  });
  ArduinoOTA.setHostname(this->hostname);
  ArduinoOTA.begin();
};

void EasyOTA::loop() {
  ArduinoOTA.handle();
}

void EasyOTA::showMessage(char *message, int line) {
  if (on_message) {
    on_message(message, line);
  }
}
void EasyOTA::showMessage(String message, int line) {
  showMessage((char *)message.c_str(), line);
}

