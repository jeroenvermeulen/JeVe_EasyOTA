/*
 * EasyOTA.h - library to include to allow Over-The-Air updates of ESP8266 and ESP32
 *
 * Refactored from:
 *    https://github.com/jeroenvermeulen/JeVe_EasyOTA
 *
 *	 MIT License
 *
 *	 Copyright (c) 2018 foxis (Andrius Mikonis <andrius.mikonis@gmail.com>)
 *
 *	 Permission is hereby granted, free of charge, to any person obtaining a copy
 *	 of this software and associated documentation files (the "Software"), to deal
 *	 in the Software without restriction, including without limitation the rights
 *	 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	 copies of the Software, and to permit persons to whom the Software is
 *	 furnished to do so, subject to the following conditions:

 *	 The above copyright notice and this permission notice shall be included in all
 *	 copies or substantial portions of the Software.

 *	 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	 SOFTWARE.
 *
 *  mDNS monitor (OSX):  dns-sd -B _arduino._tcp
 *
 */

#ifndef EasyOTA_h
#define EasyOTA_h

#ifdef ESP8266
#include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
#include <ESP8266mDNS.h>  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266mDNS/ESP8266mDNS.h
#endif

#ifdef ESP32
#include <WiFi.h>         // https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFi.h
#include <ESPmDNS.h>      // https://github.com/espressif/arduino-esp32/blob/master/libraries/ESPmDNS/src/ESPmDNS.h
#endif

#include <WiFiUdp.h>      // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiUdp.h
#include <ArduinoOTA.h>   // https://github.com/esp8266/Arduino/blob/master/libraries/ArduinoOTA/ArduinoOTA.h

#include <map>

#define DEFAULT_OTA_PORT 8266
#define DEFAULT_OTA_HOSTNAME "EasyOta"

#define GETTER(T, name) T name() { return _##name; }
#define SETTER(T, name) T name(T name) { T pa##name = _##name; _##name = name; return pa##name; }

class EasyOTA
{
  public:
		typedef enum {
			EOS_UNKNOWN,
			EOS_SETUP_WIFI,
			EOS_SETUP_OTA,
			EOS_STA,
			EOS_AP,
			EOS_DISCONNECTED,
		} STATE;

		typedef std::function<void(const String& message, int line_nr)> THandlerFunction_Message;
		typedef std::function<void(const String& ssid, STATE state)> THandlerFunction_Connect;
		typedef std::function<void(const String& ssid, uint8_t sec, int32_t rssi, const uint8_t * BSSID, int32_t chan, bool hidden)> THandlerFunction_Scan;
    typedef std::function<void(const String& ssid, const String& passw, void * param)> THandlerFunction_APList;

		EasyOTA();
		EasyOTA(const String& hostname);
    EasyOTA(const String& hostname, int port);

		void addAP(const String& wifi_ssid, const String& wifi_password);
		void removeAP(const String& wifi_ssid);
		void eachAP(THandlerFunction_APList fn, void * param);
		void setPassword(const String& password);

		///
		/// one of these must be called inside main loop
		///
		void loop(unsigned long now);
    void loop();

		///
		/// Callback setters
		///
		void onMessage(THandlerFunction_Message fn);
		void onConnect(THandlerFunction_Connect fn);
    void onScan(THandlerFunction_Scan fn);

		///
		/// Allows open wifi networks
		/// NOTE: will scan for networks
		///
		GETTER(bool, allowOpen)
		SETTER(bool, allowOpen)

		///
		/// Specifies after how many tries to fallback to AP mode if no networks found
		///  maxRetries = 0 - go to AP mode if no networks available
		///  maxRetries = -1 - never go to AP mode
		///  maxRetries > 0 - number of attempts to find suitable network
		GETTER(int, maxRetries)
		SETTER(int, maxRetries)

		///
		/// Sets a timeout for connection to be established
		///
		GETTER(long, timeout)
		SETTER(long, timeout)

		///
		/// Sets an interval for scanning for networks while in AP mode
		///  scanInterval = 0  - always scan for networks while in AP mode
		///  scanInterval = -1 - once in AP mode, stay in AP mode
		///  scanInterval >0 - scan every so much milliseconds
		GETTER(long, scanInterval)
		SETTER(long, scanInterval)

		///
		/// Gets current Wifi STATE
		///
		GETTER(STATE, state)

		///
		/// Gets currently connected SSID or self hostname
		///
		GETTER(String, currentAP)

  private:
		STATE _state;
		bool _allowOpen;
		int _maxRetries;
		int _retries_current;
		bool _ap;
		long _timeout;
		long _scanInterval;

		std::map<String, String> _access_points;
		String _hostname;
		String _password;
		String _currentAP;
		int _port;

		THandlerFunction_Message on_message;
		THandlerFunction_Connect on_connect;
    THandlerFunction_Scan on_scan;

		void showMessage(const String& message, int line_nr=1);
    void callConnect(EasyOTA::STATE state);
		void onScan(const String& ssid, uint8_t sec, int32_t rssi, const uint8_t * BSSID, int32_t chan, bool hidden);

		int setupWifi(unsigned long now);
		int connectWifi(unsigned long now, const String& wifi_ssid, const String& wifi_password, const uint8_t * BSSID, int32_t chan);
		int scanWifi(unsigned long now);
		int setupOTA(unsigned long now);
};

#endif
