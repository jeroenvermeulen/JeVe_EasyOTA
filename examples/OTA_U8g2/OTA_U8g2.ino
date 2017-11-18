/*
 * To start mDNS monitor (OSX) exec:   dns-sd -B _arduino._tcp .
 */

#include <JV_OTA.h>   // https://github.com/jeroenvermeulen/JV_OTA/blob/master/JV_OTA.h
#include <U8g2lib.h>  // https://github.com/olikraus/u8g2/blob/master/cppsrc/U8g2lib.h

#define OLED_SDA  2
#define OLED_SCL 14
#define OLED_RST  4
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA , OLED_RST);

#define WIFI_SSID     "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"
#define WIFI_HOSTNAME "ota-flash-demo"
JV_OTA OTA;

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_mf);
  u8g2.setFontMode(0);
  u8g2.setCursor(0, 15);
  u8g2.drawStr(0, 10, "Startup");
  // This callback will be called when JV_OTA has anything to tell you.
  OTA.onMessage([](char *message, int line) {
    Serial.println(message);
    displayLine(message, line);
  });
  OTA.setup(WIFI_SSID, WIFI_PASSWORD, WIFI_HOSTNAME);
}

void loop() {
  OTA.loop();
  heartBeat();
}

void displayLine(char *message, int line) {
  static char lines[3][23] = {"", "", ""};
  strncpy(lines[line-1], message, sizeof(lines[line-1]));
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, lines[0]);
  u8g2.drawStr(0, 21, lines[1]);
  u8g2.drawStr(0, 32, lines[2]);
  u8g2.sendBuffer();
}

void heartBeat() {
  static int prevMillis = 0;
  static int dotState = 0;
  static char sign[2] = {0xB7, 0x0};
  int curMillis = millis();
  if ( curMillis - prevMillis > 500 ) {
    prevMillis = curMillis;
    dotState = !dotState;
    u8g2.drawStr(118, 32, dotState ? (char *) sign : " ");
    u8g2.sendBuffer();
  }
}
