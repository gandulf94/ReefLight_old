/*
 * Copyright (c) 2018 Michael Dahsler
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
 * is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 */

#include "wifi.h"
#include "debug.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> 
#include <WiFiManager.h>

/* 
 *  Starts the Wifi using the WiFiManager library.
 *  If the last connected WiFi is not found or the device has not been
 *  connected to a WiFi yet, the device start its own WiFi with SSID "ReefLight".
 *  The SSID and PASSWORD of your home WiFi can than be entered
 *  
 *  Creates a MDNS responder, so if supported by your computer
 *  or smartphone the website can be loaded via "mdns_name".local,
 *  so no IP is necessary
 */
bool startWifi() {
  DEBUG_INFO("[startWifi] begin");

  
  WiFiManager wifiManager;
  wifiManager.autoConnect("ReefLight");
   
  WiFi.hostname(mdns_name);
  if (!MDNS.begin(mdns_name)) { DEBUG_WARNING("[startWifi] error setting up MDNS responder!"); }
  else { DEBUG_INFO("[WiFi] mDNS name: %s.local", mdns_name); }

  DEBUG_INFO("[startWifi] end");
  return true;
}

