/*
 * EasyOTA.cpp - library to include to allow Over-The-Air updates of ESP8266 and ESP32
 *
 * Inspired on:
 *    http://simplestuffmatters.com/?p=69
 *    https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS/examples/OTA-mDNS-SPIFFS
 *    https://github.com/espressif/arduino-esp32/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino
 *
 *  Copyright (C) 2018  foxis (Andrius Mikonis <andrius.mikonis@gmail.com>)
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

#include "EasyOTA.h"  // https://github.com/foxis/EasyOTA/blob/master/src/EasyOTA.h
#include <set>

// Constructors
EasyOTA::EasyOTA() : EasyOTA(DEFAULT_OTA_HOSTNAME, DEFAULT_OTA_PORT)
{
}

EasyOTA::EasyOTA(const String& hostname) : EasyOTA(hostname, DEFAULT_OTA_PORT)
{
}

EasyOTA::EasyOTA(const String& hostname, int port) {
	_hostname = hostname;
	_port = port;
	_state = EOS_UNKNOWN;
	on_message = NULL;
	on_connect = NULL;
	on_scan = NULL;
	_timeout = 10000;
	_ap = false;
	_maxRetries = 0;
	_retries_current = 0;
	_scanInterval = -1;
}

void EasyOTA::onMessage(THandlerFunction_Message fn) {
  on_message = fn;
}
void EasyOTA::onConnect(THandlerFunction_Connect fn) {
  on_connect = fn;
}
void EasyOTA::onScan(THandlerFunction_Scan fn) {
  on_scan = fn;
}
void EasyOTA::setPassword(const String& password)
{
	_password = password;
}
void EasyOTA::addAP(const String& wifi_ssid, const String& wifi_password)
{
	_access_points.insert(std::pair<String, String>(wifi_ssid, wifi_password));
}
void EasyOTA::removeAP(const String& wifi_ssid)
{
	_access_points.erase(wifi_ssid);
}
void EasyOTA::eachAP(THandlerFunction_APList fn, void * param)
{
	if (fn == NULL)
		return;
	std::map<String, String>::iterator I = _access_points.begin();
	while (I != _access_points.end()) {
		fn(I->first, I->second, param);
		I++;
	}
}

int EasyOTA::setupOTA(unsigned long now)
{
	// ArduinoOTA callback functions
  ArduinoOTA.onStart([this]() {
    showMessage("OTA starting...", 1);
  });
  ArduinoOTA.onEnd([this]() {
    showMessage("OTA done.Reboot...",1);
  });
  ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
    static unsigned int prevPerc = 100;
    unsigned int perc = (progress / (total / 100));
    unsigned int roundPerc = 5 * (int)(perc / 5);
    if ( roundPerc != prevPerc) {
      prevPerc = roundPerc;
      showMessage("OTA upload " + String(roundPerc) + "%", 1);
    }
  });
  ArduinoOTA.onError([this](ota_error_t error) {
    showMessage("OTA Error " + String(error) + ":", 0);
    String line2 = "";
    if (error == OTA_AUTH_ERROR)         line2 = "Auth Failed";
    else if (error == OTA_BEGIN_ERROR)   line2 = "Begin Failed";
    else if (error == OTA_CONNECT_ERROR) line2 = "Connect Failed";
    else if (error == OTA_RECEIVE_ERROR) line2 = "Receive Failed";
    else if (error == OTA_END_ERROR)     line2 = "End Failed";
    showMessage(line2, 1);
  });

  ArduinoOTA.setPort(_port);
	if (_password != "")
		ArduinoOTA.setPassword(_password.c_str());
  ArduinoOTA.setHostname(_hostname.c_str());
  ArduinoOTA.begin();

	return 0;
}

int EasyOTA::connectWifi(unsigned long startTime, const String& wifi_ssid, const String& wifi_password, const uint8_t * BSSID, int32_t chan)
{
	wl_status_t status;

	showMessage("Trying " + wifi_ssid, 0);

	if (BSSID)
		WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str(), chan, BSSID);
	else
		WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
	do {
		status = WiFi.status();

		switch (status){
			case WL_CONNECTED:
				showMessage("IP " + WiFi.localIP().toString(), 0);
				showMessage("SSID " + wifi_ssid, 1);
				_ap = false;
				_currentAP = wifi_ssid;
				return 0;
			case WL_NO_SSID_AVAIL:
			case WL_CONNECT_FAILED:
				break;
		}
		delay(10);
	} while (millis() - startTime < _timeout);

	return 1;
}

int EasyOTA::setupWifi(unsigned long now) {
	static unsigned long retry_ms;

	if (_access_points.size() == 1 && !_allowOpen) {
		// Skip scanning phase for single ap
		std::map<String, String>::iterator I = _access_points.begin();
		if (!connectWifi(now, I->first, I->second, NULL, 0))
			return 0;
	} else if (_access_points.size() > 1 || _allowOpen) {
		if (WiFi.status() == WL_CONNECTED)
		{
			WiFi.mode(WIFI_STA);
			WiFi.disconnect();
		}

		int result = scanWifi(now);
		switch (result) {
			case 0: return 0; // connected
			case 1:
				WiFi.scanDelete();
				return 1; // scanning/connecting
			default:
				WiFi.scanDelete();
				break; // no wifi available, fallback to AP mode
		}
	}

	if (_retries_current == 0)
		retry_ms = now;

	if (now - retry_ms <= 10 * _timeout)
		return 1;

	if (_maxRetries < 0)
	{
		retry_ms = now;
		return 1;
	}

	if (_retries_current < _maxRetries) {
		_retries_current++;
		retry_ms = now;
		return 1;
	}

	// fallback to AP mode
	showMessage("No AP available", 0);
	showMessage("Starting AP", 1);
	WiFi.mode(WIFI_AP);
	WiFi.softAP(_hostname.c_str());
	showMessage("AP " + _hostname, 0);
	showMessage("IP " + WiFi.softAPIP().toString(), 1);
	_ap = true;
	_currentAP = _hostname;
	return 0;
};

int EasyOTA::scanWifi(unsigned long now)
{
	wl_status_t status = WiFi.status();

	if (status != WL_CONNECTED)
	{
		int8_t scanResult = WiFi.scanComplete();
		if (scanResult == WIFI_SCAN_RUNNING) {
			return 1;
		} else {
			if (scanResult <= 0) {
				showMessage("Wifi scan", 0);
				WiFi.scanNetworks(true);
				return 1;
			} else {
				String bestSSID;
				int32_t bestRSSI = -1000;
				uint8 bestBSSID[6];
				int32_t bestChannel;
				std::set<String> black_list;
				bool call_scan = true;

				while (true) {
					for(int8_t i = 0; i < scanResult; ++i) {
						String ssid_scan;
						int32_t rssi_scan;
						uint8_t sec_scan;
						uint8_t* BSSID_scan;
						int32_t chan_scan;
						bool hidden_scan;

						WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);
						if (call_scan)
							onScan(ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);

						bool bl = black_list.count(ssid_scan);
						bool cap = _access_points.count(ssid_scan);
						bool exhausted = true;
						// prioritize configured ap
						std::map<String, String>::iterator I = _access_points.begin();
						while (I != _access_points.end()) {
							exhausted = black_list.count(I->first);
							if (!exhausted)
								break;
							I++;
						}

						// skip black-listed networks (those that failed to connect to)
						if (rssi_scan > bestRSSI && !bl) {
							if ((sec_scan == ENC_TYPE_NONE && _allowOpen && exhausted || cap)) {
								bestSSID = ssid_scan;
								bestRSSI = rssi_scan;
								bestChannel = chan_scan;
								memcpy(bestBSSID, BSSID_scan, sizeof(bestBSSID));
							}
						}
					}
					call_scan = false;

					// some network found
					if (bestRSSI != -1000) {
						std::map<String, String>::iterator f = _access_points.find(bestSSID);
						int result = connectWifi(now, bestSSID, f != _access_points.end() ? _access_points[bestSSID] : "", bestBSSID, bestChannel);
						if (result)
						{
							black_list.insert(bestSSID);
							bestRSSI = -1000;
						}
						else
						{
							return 0;
						}
					}
					else
						return 2; // no network found. fallback to AP mode
				}
			}
		}
	}
	// showMessage("should not go here", 1);
	return 0;
}

void EasyOTA::loop() {
	loop(millis());
}

void EasyOTA::loop(unsigned long now) {
	unsigned long rescan_ms = 0;

	switch (_state)
	{
		case EOS_UNKNOWN:
			WiFi.mode(WIFI_STA);
#ifdef ESP8266
		  WiFi.hostname(_hostname);
#elif ESP32
		  WiFi.setHostname(_hostname);
#endif
			_retries_current = 0;
			_ap = false;
			_currentAP = "";
			_state = EOS_SETUP_WIFI;
			break;
		case EOS_SETUP_WIFI:
			if (!setupWifi(now))
				_state = EOS_SETUP_OTA;
			break;
		case EOS_SETUP_OTA:
			if (!setupOTA(now))
				callConnect(_ap ? EOS_AP : EOS_STA);
			rescan_ms = now;
			break;
		case EOS_DISCONNECTED:
			// TODO Prepare for reconnect
			_state = EOS_UNKNOWN;
			break;
		default:
			if (WiFi.status() == WL_CONNECTED || _ap)
				ArduinoOTA.handle();
			else
			{
				callConnect(EOS_DISCONNECTED);
			}

			if (_ap && now - rescan_ms > _scanInterval && _scanInterval > 0)
			{
				// TODO: scan for available wifi and connect to it
				rescan_ms = now;
			}
	}
}

void EasyOTA::showMessage(const String& message, int line_nr) {
  if (on_message) {
    on_message(message, line_nr);
  }
}

void EasyOTA::callConnect(STATE state) {
	STATE last_state = _state;
	_state = state;

	if (on_connect && state != last_state)
	{
		on_connect(_currentAP, state);
	}
}
void EasyOTA::onScan(const String& ssid, uint8_t sec, int32_t rssi, const uint8_t * BSSID, int32_t chan, bool hidden)
{
	if (on_scan)
	{
		on_scan(ssid, sec, rssi, BSSID, chan, hidden);
	}
}
