/*
 * EasyOTA.h - library to include to allow Over-The-Air updates of ESP8266 and ESP32
 *
 * Refactored from:
 *    https://github.com/jeroenvermeulen/JeVe_EasyOTA
 *
 *  Copyright (C) 2018  Jeroen Vermeulen (info@jeve.eu), foxis (Andrius Mikonis <andrius.mikonis@gmail.com>)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#ifdef OTA_USE_PROGRESS_BAR // Enable this via compiler flag
#ifndef OTA_PROGRESS_BAR_SIZE // Define this via compiler flag to override the default
#define OTA_PROGRESS_BAR_SIZE 20
#endif
#endif

#define EO_GETTER(T, name) T name() { return _##name; }
#define EO_SETTER(T, name) T name(T name) { T pa##name = _##name; _##name = name; return pa##name; }

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
		EO_GETTER(bool, allowOpen)
		EO_SETTER(bool, allowOpen)

		///
		/// Specifies after how many tries to fallback to AP mode if no networks found
		///  maxRetries = 0 - go to AP mode if no networks available
		///  maxRetries = -1 - never go to AP mode
		///  maxRetries > 0 - number of attempts to find suitable network
		EO_GETTER(int, maxRetries)
		EO_SETTER(int, maxRetries)

		///
		/// Sets a timeout for connection to be established
		///
		EO_GETTER(long, timeout)
		EO_SETTER(long, timeout)

		///
		/// Sets an interval for scanning for networks while in AP mode
		///  scanInterval = 0  - always scan for networks while in AP mode
		///  scanInterval = -1 - once in AP mode, stay in AP mode
		///  scanInterval >0 - scan every so much milliseconds
		EO_GETTER(long, scanInterval)
		EO_SETTER(long, scanInterval)

		///
		/// Gets current Wifi STATE
		///
		EO_GETTER(STATE, state)

		///
		/// Gets currently connected SSID or self hostname
		///
		EO_GETTER(String, currentAP)

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
