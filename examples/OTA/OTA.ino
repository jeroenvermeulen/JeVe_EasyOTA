/*
 * To start mDNS monitor (OSX) exec:   dns-sd -B _arduino._tcp
 */

#include <JeVe_EasyOTA.h> 

#define WIFI_SSID        "your-wifi-ssid"
#define WIFI_PASSWORD    "your-wifi-password"
#define ARDUINO_HOSTNAME "ota-flash-demo"
EasyOTA OTA(ARDUINO_HOSTNAME);

void setup() {
  Serial.begin(9600);
  // This callback will be called when EasyOTA has anything to tell you.
  OTA.onMessage([](const String& message, int line) {
    Serial.println(message);
  });
  OTA.addAP(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  OTA.loop();
}
