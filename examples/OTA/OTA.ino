/*
 * To start mDNS monitor (OSX) exec:   dns-sd -B _arduino._tcp .
 */

#include <JV_OTA.h>   // https://github.com/jeroenvermeulen/JV_OTA/blob/master/JV_OTA.h

#define WIFI_SSID     "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"
#define WIFI_HOSTNAME "ota-flash-demo"
JV_OTA OTA;

void setup() {
  Serial.begin(9600);
  // This callback will be called when JV_OTA has anything to tell you.
  OTA.onMessage([](char *message, int line) {
    Serial.println(message);
  });
  OTA.setup(WIFI_SSID, WIFI_PASSWORD, WIFI_HOSTNAME);
}

void loop() {
  OTA.loop();
}
