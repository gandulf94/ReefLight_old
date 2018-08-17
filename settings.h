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

#ifndef SETTINGS__H
#define SETTINGS__H

#include <Arduino.h>


// constants

// for the settings file
static const char SETTINGS_FILE_NAME[] = "/configFile.json";
static const uint16_t MAX_JSON_SIZE = 10000;

// name definitions for the JSON Format
static const char CHAR_NUM_OF_CHANNELS[] = "numOfChannels";
static const char CHAR_MAX_NUM_OF_CHANNELS[] = "maxNumOfChannels";
static const char CHAR_MAX_NUM_OF_ENTRIES[] = "maxNumOfEntries";
static const char CHAR_PWM_FREQUENCY[] = "PWMFrequency";
static const char CHAR_PWM_GENERATOR[] = "PWMGenerator";
static const char CHAR_NTP_SERVER[] = "NTPServer";
static const char CHAR_TIMEZONE[] = "timezone";
static const char CHAR_TIME[] = "time";
static const char CHAR_CURRENT_POWER[] = "currentPower";
static const char CHAR_CHANNELS[] = "channels";
static const char CHAR_CHANNEL_NAME[] = "name";
static const char CHAR_CHANNEL_COLOR[] = "color";
static const char CHAR_CHANNEL_MANUAL[] = "manual";
static const char CHAR_CHANNEL_MOONLIGHT[] = "moonlight";
static const char CHAR_CHANNEL_MAX_MOONLIGHT_VALUE[] = "MaxMoonlightValue";
static const char CHAR_CHANNEL_PIN[] = "pin";
static const char CHAR_CHANNEL_POWER[] = "power";
static const char CHAR_CHANNEL_VALUE[] = "value";
static const char CHAR_CHANNEL_TIMES[] = "times";
static const char CHAR_CHANNEL_VALUES[] = "values";

// global variables
extern bool SPIFFS_started; // true if the SPIFFS has started yet, false otherwise


/* 
 * loads the settings from the file "SETTINGS_FILE_NAME" in the SPIFFS
 * returns true loading was successfull, false otherwise
 */
bool loadSettings();

/*
 * saves the settings to the file "CONFIG_FILE_NAME" in the SPIFFS 
 * returns true loading was successfull, false otherwise
 */
bool saveSettings();

/*
 * creates default settings and saves them to the file "CONFIG_FILE_NAME" in the SPIFFS 
 * returns true loading was successfull, false otherwise
 */
bool saveDefaultSettings();

// starts the SPIFFS filesystem if it hasn't started yet
void startSPIFFS();
#endif
