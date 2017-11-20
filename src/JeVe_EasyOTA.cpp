/*
 * EasyOTA.cpp - library to include to allow Over-The-Air updates of ESP8266 and ESP32
 *
 * Inspired on:
 *    http://simplestuffmatters.com/?p=69
 *    https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS/examples/OTA-mDNS-SPIFFS
 *    https://github.com/espressif/arduino-esp32/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino
 *
 *  mDNS monitor (OSX):  dns-sd -B _arduino._tcp
 *
 */
 
#include "JeVe_EasyOTA.h"  // https://github.com/jeroenvermeulen/JeVe_EasyOTA/blob/master/src/JeVe_EasyOTA.h

// Constructor
EasyOTA::EasyOTA() {
}

void EasyOTA::onMessage(THandlerFunction_Message fn) {
  on_message = fn;
}

void EasyOTA::setup(char* wifi_ssid, char* wifi_password, char* hostname) {
  showMessage("", 1); // New line in case of using serial output
  String line1 = "Connect WiFi";
  showMessage(line1, 1);
  showMessage("SSID " + String(wifi_ssid), 2);

  WiFi.mode(WIFI_STA);
  #ifdef ESP8266
  WiFi.hostname(hostname);
  #endif
  #ifdef ESP32
  WiFi.setHostname(hostname);
  #endif

  WiFi.begin(wifi_ssid, wifi_password);
  unsigned long startTime = millis();
  String progressDots = "";
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(1000);
    line1 += ".";
    showMessage(line1, 1);
  }
  if (WiFi.status() == WL_CONNECTED) {
    showMessage("IP " + WiFi.localIP().toString(), 1);
  } else {
    showMessage("Can't connect WiFi", 1);
    showMessage("Going into AP mode.", 2);
    WiFi.mode(WIFI_AP);
    delay(500); // Extra delay to show message when using LCD / Oled
    WiFi.softAP(hostname);
    showMessage("AP " + String(hostname), 1);
    showMessage("IP " + WiFi.softAPIP().toString(), 2);
  }

  // ArduinoOTA callback functions
  ArduinoOTA.onStart([this]() {
    showMessage("OTA starting...", 2);
  });
  ArduinoOTA.onEnd([this]() {
    showMessage("OTA done.Reboot...",2);
    //showMessage("Rebooting...",2);
  });
  ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
    static unsigned int prevPerc = 100;
    unsigned int perc = (progress / (total / 100));
    unsigned int roundPerc = 5 * (int)(perc / 5);
    if ( roundPerc != prevPerc) {
      prevPerc = roundPerc;
      showMessage("OTA upload " + String(roundPerc) + "%", 2);
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

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.begin();
};

void EasyOTA::loop() {
  ArduinoOTA.handle();
}

void EasyOTA::showMessage(char *message, int line_nr) {
  if (on_message) {
    on_message(message, line_nr);
  }
}
void EasyOTA::showMessage(String message, int line_nr) {
  showMessage((char *)message.c_str(), line_nr);
}

