/*
 * To start mDNS monitor (OSX) exec:   dns-sd -B _arduino._tcp
 */

#include <JeVe_EasyOTA.h>  
#include <U8g2lib.h>       // https://github.com/olikraus/u8g2/blob/master/cppsrc/U8g2lib.h

// You may need to replace the oled initialisation with another type from
// https://github.com/olikraus/U8g2_Arduino/blob/master/examples/full_buffer/HelloWorld/HelloWorld.ino
#define OLED_SDA  2
#define OLED_SCL 14
#define OLED_RST  4
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA , OLED_RST);

#define WIFI_SSID        "your-wifi-ssid"
#define WIFI_PASSWORD    "your-wifi-password"
#define ARDUINO_HOSTNAME "ota-flash-demo"
EasyOTA OTA(ARDUINO_HOSTNAME);

void displayLine(char *message, int line_nr) {
  const int lineLength = 21;
  static char lines[4][lineLength + 1] = {"EasyOTA demo", "", "", ""};
  snprintf(lines[line_nr + 1], lineLength, "%s", message);
  lines[line_nr - 1][lineLength] = 0x0;
  u8g2.clearBuffer();
	u8g2.drawStr(0, 0, lines[0]);
  u8g2.drawStr(0, 10, lines[1]);
  u8g2.drawStr(0, 21, lines[2]);
  u8g2.drawStr(0, 32, lines[3]);
  u8g2.sendBuffer();
}

void heartBeat(unsigned long now) {
  static long prevMillis = 0;
  static bool dotState = 0;
  static char sign[2] = {0xB7, 0x0};
  if ( now - prevMillis > 500 ) {
    prevMillis = now;
    dotState = !dotState;
    u8g2.drawStr(120, 32, dotState ? (char *) sign : " ");
    u8g2.sendBuffer();
  }
}

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_mf);
  u8g2.setFontMode(0);
  // This callback will be called when EasyOTA has anything to tell you.
  OTA.onMessage([](char *message, int line_nr) {
    Serial.println(message);
    displayLine(message, line_nr);
  });
	// This callback will be called when WiFi connection changes
	OTA.onConnect([](const String& ssid, EasyOTA::STATE state){
		String strstate;
		switch (state)
		{
			case EasyOTA:EOS_AP: strstate = "AP"; break;
			case EasyOTA:EOS_STA: strstate = "STA"; break;
			case EasyOTA:EOS_DISCONNECTED:
			default:
				strstate = "-/-"; break;
		}
		String message = "Conn chngd: " + ssid + " ; " + strstate;
		Serial.println(message);
		displayLine(message, 2);
	});
	OTA.addAP(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
	unsigned long now = millis();
  OTA.loop(now);
  heartBeat(now);
}
