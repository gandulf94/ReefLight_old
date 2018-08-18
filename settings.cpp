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

#include "settings.h"
#include "channel.h"
#include "server.h"
#include "debug.h"
#include "ntp.h"
#include <ArduinoJson.h>
#include <FS.h>

bool SPIFFS_started = false;


bool saveDefaultSettings() {
  DEBUG_INFO("[saveDefaultSettings]");

  // starts the SPIFFS fileystem
  startSPIFFS();
  
  // main json object
  DynamicJsonBuffer jsonBuffer(MAX_JSON_SIZE);
  JsonObject& json = jsonBuffer.createObject();

  // number of channels
  json[CHAR_NUM_OF_CHANNELS] = 1;
  // pwm PWMFrequency
  json[CHAR_PWM_FREQUENCY] = 1000;
  // timezone
  json[CHAR_TIMEZONE] = 0;
  // ntp server
  json[CHAR_NTP_SERVER] = "pool.ntp.org";
  // pwm generator
  json[CHAR_PWM_GENERATOR] = PWM_GENERATOR_ESP8266;
  
  // channels
  JsonArray& jsonChannels = json.createNestedArray(CHAR_CHANNELS);
  for(uint8_t c=0; c<MAX_NUM_OF_CHANNELS; c++) {
    // channel json object
    JsonObject& jsonChannelsChannel = jsonChannels.createNestedObject();
    
    // channel name
    jsonChannelsChannel[CHAR_CHANNEL_NAME] = "channel "+String(c+1);
    // channel color
    jsonChannelsChannel[CHAR_CHANNEL_COLOR] = "#000000";
    // channel manual
    jsonChannelsChannel[CHAR_CHANNEL_MANUAL] = false;
    // channel moonlight
    jsonChannelsChannel[CHAR_CHANNEL_MOONLIGHT] = false;
    // channel max moonlight value
    jsonChannelsChannel[CHAR_CHANNEL_MAX_MOONLIGHT_VALUE] = 100;
    // channel pin
    jsonChannelsChannel[CHAR_CHANNEL_PIN] = 12;
    // channel power
    jsonChannelsChannel[CHAR_CHANNEL_POWER] = 10;
    // times array
    JsonArray& jsonChannelsChannelT = jsonChannelsChannel.createNestedArray(CHAR_CHANNEL_TIMES);
    // values array
    JsonArray& jsonChannelsChannelV = jsonChannelsChannel.createNestedArray(CHAR_CHANNEL_VALUES);
    // some default entries
    jsonChannelsChannelT.add(9*60*60+5*60*c);
    jsonChannelsChannelV.add(0);
    jsonChannelsChannelT.add(10*60*60+10*60*c);
    jsonChannelsChannelV.add(50*(100.-float(5*c))/100.);
    jsonChannelsChannelT.add(11*60*60+10*60*c);
    jsonChannelsChannelV.add(70*(100.-float(5*c))/100.);
    jsonChannelsChannelT.add(19*60*60-10*60*c);
    jsonChannelsChannelV.add(70*(100.-float(5*c))/100.);
    jsonChannelsChannelT.add(20*60*60-10*60*c);
    jsonChannelsChannelV.add(50*(100.-float(5*c))/100.);
    jsonChannelsChannelT.add(21*60*60-10*60*c);
    jsonChannelsChannelV.add(0);
  }

  if(json.measureLength()+1 > MAX_JSON_SIZE) {
    DEBUG_WARNING("[saveDefaultSettings] json size too large");
    return false;
  }

  File settings_file = SPIFFS.open(SETTINGS_FILE_NAME, "w");
  json.printTo(settings_file);
  settings_file.close(); 

  return true;
}

bool saveSettings() {
  DEBUG_INFO("[saveSettings]");

  // starts the SPIFFS fileystem
  startSPIFFS();
  
  // main json object
  DynamicJsonBuffer jsonBuffer(MAX_JSON_SIZE);;
  JsonObject& json = jsonBuffer.createObject();

  // number of channels
  json[CHAR_NUM_OF_CHANNELS] = numOfChannels;
  // pwm PWMFrequency
  json[CHAR_PWM_FREQUENCY] = PWMFrequency;
  // NTP server
  json[CHAR_NTP_SERVER] = jsonBuffer.strdup(NTPServer);
  // timezone
  json[CHAR_TIMEZONE] = timezone;
  
  // channels
  JsonArray& jsonChannels = json.createNestedArray(CHAR_CHANNELS);
  for(uint8_t c=0; c<MAX_NUM_OF_CHANNELS; c++) {
    // channel json object
    JsonObject& jsonChannelsChannel = jsonChannels.createNestedObject();
    
    // channel name
    jsonChannelsChannel[CHAR_CHANNEL_NAME] = jsonBuffer.strdup(channels[c].name);
    // channel color
    jsonChannelsChannel[CHAR_CHANNEL_COLOR] = jsonBuffer.strdup(channels[c].color);
    // channel manual
    jsonChannelsChannel[CHAR_CHANNEL_MANUAL] = channels[c].manual;
    // channel moonlight
    jsonChannelsChannel[CHAR_CHANNEL_MOONLIGHT] = channels[c].moonlight;
    // max moonlight value
    jsonChannelsChannel[CHAR_CHANNEL_MAX_MOONLIGHT_VALUE] = channels[c].maxMoonlightValue;
    // channel pin
    jsonChannelsChannel[CHAR_CHANNEL_PIN] = channels[c].pin;
    // channel power
    jsonChannelsChannel[CHAR_CHANNEL_POWER] = channels[c].power;
    // times array
    JsonArray& jsonChannelsChannelT = jsonChannelsChannel.createNestedArray(CHAR_CHANNEL_TIMES);
    // values array
    JsonArray& jsonChannelsChannelV = jsonChannelsChannel.createNestedArray(CHAR_CHANNEL_VALUES);
    for(uint8_t i=0; i<channels[c].numOfEntries; i++) {
      jsonChannelsChannelT.add(channels[c].t[i]);
      jsonChannelsChannelV.add(channels[c].v[i]);
    }
  }
  
  if(json.measureLength()+1 > MAX_JSON_SIZE) {
    DEBUG_WARNING("[saveSettings] json size too large");
    return false;
  }

  File settings_file = SPIFFS.open(SETTINGS_FILE_NAME, "w");
  json.printTo(settings_file);
  settings_file.close(); 
  return true;
}

bool loadSettings() {
  DEBUG_INFO("[loadSettings]");

  // starts the SPIFFS fileystem
  startSPIFFS();

  // try to open file
  File settings_file = SPIFFS.open(SETTINGS_FILE_NAME, "r");
  if (!settings_file) {
    DEBUG_WARNING("[loadSettings] no config file found, create default file");
    return false;
  }

  // check filesize
  size_t size = settings_file.size();
  if (size > MAX_JSON_SIZE) {
    DEBUG_WARNING("[loadSettings] config file size is too large");
    return false;
  }

  // read file
  std::unique_ptr<char[]> buf(new char[size]);
  settings_file.readBytes(buf.get(), size);

  DynamicJsonBuffer jsonBuffer(MAX_JSON_SIZE);
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  // check json parsing
  if (!json.success()) {
    DEBUG_WARNING("[loadSettings] json parsing failed");
    return false;
  }
  
  // load the settings from the json file

  // number of channels
  numOfChannels = json[CHAR_NUM_OF_CHANNELS];
  // pwm PWMFrequency
  PWMFrequency = json[CHAR_PWM_FREQUENCY];
  // name of the ntp server
  strcpy(NTPServer, json[CHAR_NTP_SERVER]);
  // timezone
  timezone = json[CHAR_TIMEZONE];

  //channels
  for(uint8_t c=0; c<MAX_NUM_OF_CHANNELS; c++) {

    // channel number
    channels[c].channelNumber = c;
    // channel name
    strcpy(channels[c].name, json[CHAR_CHANNELS][c][CHAR_CHANNEL_NAME]); 
    // channel color
    strcpy(channels[c].color, json[CHAR_CHANNELS][c][CHAR_CHANNEL_COLOR]); 
    // channel manual
    channels[c].manual = json[CHAR_CHANNELS][c][CHAR_CHANNEL_MANUAL];
    // channel moonlight
    channels[c].moonlight = json[CHAR_CHANNELS][c][CHAR_CHANNEL_MOONLIGHT];
    // max moonlight value
    channels[c].maxMoonlightValue = json[CHAR_CHANNELS][c][CHAR_CHANNEL_MAX_MOONLIGHT_VALUE];
    // channel pin
    channels[c].pin = json[CHAR_CHANNELS][c][CHAR_CHANNEL_PIN];
    // channel power
    channels[c].power = json[CHAR_CHANNELS][c][CHAR_CHANNEL_POWER];
    // number of entries
    channels[c].numOfEntries = json[CHAR_CHANNELS][c][CHAR_CHANNEL_TIMES].size();
    // times and values
    for(uint8_t i=0; i<channels[c].numOfEntries; i++) {
      channels[c].t[i] = json[CHAR_CHANNELS][c][CHAR_CHANNEL_TIMES][i]; 
      channels[c].v[i] = json[CHAR_CHANNELS][c][CHAR_CHANNEL_VALUES][i]; 
    }
    
  }
  return true;
}

void startSPIFFS() {
  if(!SPIFFS_started) {
    DEBUG_INFO("start SPIFFS");
    SPIFFS.begin();
    SPIFFS_started = true;
  }
}

