JeVe_EasyOTA library for Arduino
================================

# Purpose

Library which makes it easy to add support for OTA updates to your project. Works with ESP8266 and ESP32.

# Installation

## Install as ZIP

* [Click here to download the ZIP file](https://github.com/jeroenvermeulen/JeVe_EasyOTA/archive/master.zip)
* Start the Arduino IDE
* Go to _Sketch > Include Library > Add .ZIP Library..._
* Select the ZIP file you just downloaded

## Install using Git (OSX / Linux)

* Open a terminal window
* Go to your Arduino libraries dir: `cd ~/Documents/Arduino/libraries`
* Execute: `git clone https://github.com/jeroenvermeulen/JeVe_EasyOTA.git`
* (Re)start your Arduino IDE

# Usage

The included examples are the easiest way to learn how to use this library.

After you put the code in your Arduino, after a few minutes you should see the OTA port in the Arduino IDE under the menu _Tools > Port_ 
![Arduino IDE Menu > Port](docs/menu_ota_port.png)

## To include it in your own project:

Put on top of your Arduino file:
```
#include <JeVe_EasyOTA.h> 
EasyOTA OTA;
```

Put this in your `setup()` function. This example logs debugging output to Serial:
```
Serial.begin(9600);
// This callback will be called when JeVe_EasyOTA has anything to tell you.
OTA.onMessage([](char *message, int line) {
  Serial.println(message);
});
OTA.setup("wifi_ssid", "wifi_password", "arduino_hostname");
```

Put this in you `loop` function:
```
OTA.loop();
```

That's all folks.

# Problem Solving

## Network port is not showing in IDE

* It sometimes takes a few minutes for the port to show in the Arduino IDE
* Check if the port is found using console (OSX): `dns-sd -B _arduino._tcp`
* Try to restart the Arduino, wait 5 minutes, check if the _Network port_ shows up
* Try to restart your Arduino IDE, wait 5 minutes, check if the _Network port_ shows up
* On Windows 8.1 or older, mDNS does may not work, or only when you install Bonjour.
* Your may need op open 'mDNS' on your firewall: UDP port 5353