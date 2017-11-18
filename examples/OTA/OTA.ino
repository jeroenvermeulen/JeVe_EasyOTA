
#include <JV_OTA.h>  
#include <Arduino.h>      // https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/Arduino.h

#define wifi_ssid "your-wifi-ssid"
#define wifi_password "your-wifi-password"
#define wifi_hostname "ESP8266-"

JV_OTA OTA( wifi_ssid, wifi_password, wifi_hostname );

void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Startup");
  OTA.setup();
}

void loop() {
  OTA.loop();
  heartBeat();
  delay(100);
}