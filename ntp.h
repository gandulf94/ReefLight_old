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

#ifndef NTP__H
#define NTP__H

#include <Arduino.h>

// constants
static uint32_t NTP_UPDATE_INTERVAL = 60000; // time between updates from the NTP Server in ms

// global variables
extern int8_t timezone; // timezone in full hours from the GMT time
extern char NTPServer[40]; // name of the NTP Server

// starts the NTP updating
void startNTP();
// handling function in the main loop
void handleNTP();
// returns seconds of the day considering the "timezone"
uint32_t getLocalSecondsOfTheDay();
// returns the epoch time
unsigned long epochTime();

#endif
