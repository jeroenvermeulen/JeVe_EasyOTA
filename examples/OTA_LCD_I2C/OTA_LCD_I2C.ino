/*
 * Check for your board where the hardware SCA and SCL pins are.
 * Connect the display to GND, 5V + the SCA and SCL pins.
 *
 * To start mDNS monitor (OSX) exec:   dns-sd -B _arduino._tcp
 */

#include <JeVe_EasyOTA.h>           // https://github.com/jeroenvermeulen/JeVe_EasyOTA/blob/master/src/JeVe_EasyOTA.h
#include <LiquidCrystal_PCF8574.h>  // https://github.com/mathertel/LiquidCrystal_PCF8574/blob/master/src/LiquidCrystal_PCF8574.h

#define WIFI_SSID        "your-wifi-ssid"
#define WIFI_PASSWORD    "your-wifi-password"
#define ARDUINO_HOSTNAME "ota-flash-demo"
EasyOTA OTA;

#define LCD_I2C_ADDRESS  0x3F  // You can also try 0x27 or use I2cScanner
#define LCD_WIDTH        16
#define LCD_HEIGHT       2
LiquidCrystal_PCF8574 lcd(LCD_I2C_ADDRESS);

void displayLine(char *message, int line_nr=1) {
  static char line[LCD_WIDTH+1] = "";
  strncpy(line, message, LCD_WIDTH);
  for (int i=strlen(message); i <= LCD_WIDTH; i++) {
    line[i] = 32; // space
  }
  line[LCD_WIDTH] = 0; // string terminator
  lcd.setCursor(0, line_nr - 1);
  lcd.print(line);
}

void setup() {
  Serial.begin(9600);

  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  lcd.clear();
  lcd.setBacklight(255);
  // This callback will be called when JeVe_EasyOTA has anything to tell you.
  OTA.onMessage([](char *message, int line_nr) {
    Serial.println(message);
    displayLine(message, line_nr);
  });
  OTA.setup(WIFI_SSID, WIFI_PASSWORD, ARDUINO_HOSTNAME);
}

void loop() {
  OTA.loop();
}
