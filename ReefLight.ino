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

#include "debug.h"
#include "server.h"
#include "ntp.h"
#include "wifi.h";
#include "settings.h"
#include "channel.h"

void setup() {
  // starts the DEBUG Serial Port defined in debug.h
  DEBUG_BEGIN;
  DEBUG_INFO("[setup] begin");
  // loads the settings from the file "SPIFFS
  if(!loadSettings()) {
    saveDefaultSettings();
    loadSettings();
  }
  // configures the PWM generator
  configurePWM();
  // starts the WiFi
  startWifi();
  // starts the Server
  startServer();
  // starts the NTP Service
  startNTP();
  DEBUG_INFO("[setup] end");
}


void loop() {
  // handles the PWM Update
  handlePWM(false);
  // handles the Server interaction
  handleServer();
  // handles the NTP Service
  handleNTP();
}
