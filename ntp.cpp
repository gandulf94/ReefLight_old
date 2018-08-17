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

#include "ntp.h"
#include "debug.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

/*
 * Global variables
 */
WiFiUDP ntpUDP; // UDP Obeject for polling the NTP data from "NTPServer"
NTPClient timeClient(ntpUDP); // Object that automatically updates the time from the "NTPServer"
int8_t timezone; // timezone in full hours from the GMT time
char NTPServer[40]; // name of the NTP Server


/*
 * starts the NTP Server
 * Using "NTPServer"
 * no timeOffset -> timezones are handled externally, not from the timeClient Object!
 * sets the time between updates to "NTP_UPDATE_INTERVAL"
 */
void startNTP() {
  DEBUG_INFO("[startNTP]");
  timeClient.setPoolServerName(NTPServer);
  timeClient.setTimeOffset(0);
  timeClient.setUpdateInterval(NTP_UPDATE_INTERVAL);
  timeClient.begin();
}

/*
 * handles the timeClient object in the Main Loop
 * if the time since the last update is greater than "NTP_UPDATE_INTERVAL"
 * a new update is made
 */
void handleNTP() {
  timeClient.update();
}


/*
 * returns the seconds that has passed in the current day
 * it also consideres the "timezone"
 */
uint32_t getLocalSecondsOfTheDay() {
  return uint32_t(timeClient.getSeconds()) + uint32_t(60*timeClient.getMinutes()) + uint32_t(3600*timeClient.getHours()) + 60*60*timezone;
}

/*
 * returns the EPOCH time
 */
unsigned long epochTime() {
  return timeClient.getEpochTime();
}

