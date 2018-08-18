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

#include "server.h"
#include "debug.h"
#include "settings.h"
#include "channel.h"
#include "ntp.h"
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <FS.h>
#include <Arduino.h>
#include <ArduinoJson.h>

// constants for the Websocket interaction 
static const uint8_t ID_REQUEST_MANUAL_FROM_SERVER = 0;
static const uint8_t ID_SEND_MANUAL_TO_CLIENT = 1;
static const uint8_t ID_UPDATE_MANUAL = 2;

static const uint8_t ID_REQUEST_SCHEDULE_FROM_SERVER = 10;
static const uint8_t ID_SEND_SCHEDULE_TO_CLIENT = 11;
static const uint8_t ID_SAVE_SCHEDULE = 12;

static const uint8_t ID_REQUEST_SETTINGS_FROM_SERVER = 20;
static const uint8_t ID_SEND_SETTINGS_TO_CLIENT = 21;
static const uint8_t ID_SAVE_SETTINGS = 22;

static const uint8_t ID_RESTART = 50;
static const uint8_t ID_FACTORY_SETTINGS = 51;


// global variables
ESP8266WebServer server(80); // webserver object
WebSocketsServer webSocket = WebSocketsServer(81); // websocket object


/*
 * called if there is interaction with the websocket.
 * can be eiter
 *  - Disconnected
 *  - Connected
 *  - Text
 *      The function is parsing the incoming String as a JSON object and acts according to
 *      the "id" of the incoming json to return data back to the websocket client or
 *      to save data eg.
 *            
 *      id's are
 *      ID_REQUEST_MANUAL_FROM_SERVER:
 *        The "name", "color", "value", "manual" and "moonlight" of the active channels are send to the client
 *        in a json with id "ID_SEND_MANUAL_TO_CLIENT" to display a table with the current values that can be changed 
 *        manually from the client.
 *        
 *      ID_UPDATE_MANUAL:
 *        "value" and "mode" of the channels are updated according to the incomming JSON 
 *        and a PWM Update is forced
 *        
 *      ID_REQUEST_SCHEDULE_FROM_SERVER:
 *        The "name", "color", "values", "times" and "moonlight" of the active channels and the "time" are send to the client
 *        in a json with id "ID_SEND_SCHEDULE_TO_CLIENT" to display the Schedule in a chart together with the current time
 *        
 *      ID_SAVE_SCHEDULE:
 *        The "times" and "values" of the channels are updated according to the incomming JSON and the new values are stored to the
 *        "SETTINGS_FILE" in the SPIFFS.
 *        A PWM update is forced.
 *        
 *      ID_REQUEST_SETTINGS_FROM_SERVER:
 *        Nearly all data are send to the client in a JSON with id "ID_SEND_SETTINGS_TO_CLIENT". The settings can be changed from the
 *        client
 *        
 *      ID_SAVE_SETTINGS:
 *        The (changed) settings are send back from the client, updated and stored in the "SETTINGS_FILE" in the SPIFFS
 *        A restart of the ESP8266 is might necessary
 *        
 *      ID_RESTART:
 *        The ESP8266 restarts. There might be a problem on the first restart, so the power must be disconnected.
 *        
 *      ID_FACTORY_SETTINGS:
 *        restores the Settings in "SETTINGS_FILE" to the settings after a new flash of the firmware
 */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch (type) {
    case WStype_DISCONNECTED:
      DEBUG_INFO("[%u] Disconnected!", num);
      break;
      
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      DEBUG_INFO("[%u] Connected from %d.%d.%d.%d url: %s", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
      
    case WStype_TEXT:
      // Parsing the incoming JSON
      DynamicJsonBuffer jsonBuffer;
      JsonObject& jsonIn = jsonBuffer.parseObject(payload);
      if (!jsonIn.success()) {
        DEBUG_WARNING("[webSocket_event] parsing of the incoming JSON failed");
        return;
      }

      // check for ID
      if (!jsonIn.containsKey("id")) {
        DEBUG_WARNING("[webSocket_event] no id in the incomming json");
        return;
      }
      uint8_t id = jsonIn["id"];
      DEBUG_INFO("id of the msg: %d", id);
      //jsonIn.prettyPrintTo(DEBUG_PORT);
      DEBUG_INFO("");
           
      switch(id) {

        case ID_REQUEST_MANUAL_FROM_SERVER: {
          DEBUG_INFO("ID_REQUEST_INDEX_FROM_SERVER");

          // create json
          JsonObject& jsonOut = jsonBuffer.createObject();
          // id
          jsonOut["id"] = ID_SEND_MANUAL_TO_CLIENT;
          // channels array
          JsonArray& jsonChannels = jsonOut.createNestedArray(CHAR_CHANNELS);
          for(uint8_t c=0; c<numOfChannels; c++) {
            JsonObject& jsonChannelsChannel = jsonChannels.createNestedObject();
            // channel name
            jsonChannelsChannel[CHAR_CHANNEL_NAME] = jsonBuffer.strdup(channels[c].name);
            // channel color
            jsonChannelsChannel[CHAR_CHANNEL_COLOR] = jsonBuffer.strdup(channels[c].color);
            // channel manual
            jsonChannelsChannel[CHAR_CHANNEL_MANUAL] = channels[c].manual;
            // channel moonlight
            jsonChannelsChannel[CHAR_CHANNEL_MOONLIGHT] = channels[c].moonlight;
            // channel pwm value
            jsonChannelsChannel[CHAR_CHANNEL_VALUE] = channels[c].value;
          }
          
          // send json
          String jsonOutStr;
          jsonOut.printTo(jsonOutStr);
          webSocket.sendTXT(0,jsonOutStr);
          break;
        }

        case ID_UPDATE_MANUAL: {
          DEBUG_INFO("ID_UPDATE_MANUAL");
          for(uint8_t c=0; c<numOfChannels; c++) {
            if(!channels[c].moonlight) {
              channels[c].manual = jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_MANUAL];
              channels[c].value = jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_VALUE];
            }
          }
          // forces PWM update
          handlePWM(true);
          break;             
        }
        
        case ID_REQUEST_SCHEDULE_FROM_SERVER: {
          DEBUG_INFO("ID_REQUEST_SCHEDULE_FROM_SERVER");

          // create json
          JsonObject& jsonOut = jsonBuffer.createObject();
          // id
          jsonOut["id"] = ID_SEND_SCHEDULE_TO_CLIENT;
          // time
          jsonOut[CHAR_TIME] = getLocalSecondsOfTheDay();
          // max num of entries
          jsonOut[CHAR_MAX_NUM_OF_ENTRIES] = MAX_NUM_OF_ENTRIES;
          // channels
          JsonArray& jsonChannels = jsonOut.createNestedArray(CHAR_CHANNELS);
          for(uint8_t c=0; c<numOfChannels; c++) {
            JsonObject& jsonChannelsChannel = jsonChannels.createNestedObject();
            // channel name
            jsonChannelsChannel[CHAR_CHANNEL_NAME] = jsonBuffer.strdup(channels[c].name);
            // channel color
            jsonChannelsChannel[CHAR_CHANNEL_COLOR] = jsonBuffer.strdup(channels[c].color);
            // channel moonlight
            jsonChannelsChannel[CHAR_CHANNEL_MOONLIGHT] = channels[c].moonlight;
            
            // times array
            JsonArray& jsonChannelsChannelT = jsonChannelsChannel.createNestedArray(CHAR_CHANNEL_TIMES);
            // values array
            JsonArray& jsonChannelsChannelV = jsonChannelsChannel.createNestedArray(CHAR_CHANNEL_VALUES);
            for(uint8_t i=0; i<channels[c].numOfEntries; i++) {
              jsonChannelsChannelT.add(channels[c].t[i]);
              jsonChannelsChannelV.add(channels[c].v[i]);
            }
          }

          // send json
          String jsonOutStr;
          jsonOut.printTo(jsonOutStr);
          webSocket.sendTXT(0,jsonOutStr);
          break;
        }

        case ID_SAVE_SCHEDULE: {
          DEBUG_INFO("ID_SAVE_SCHEDULE");
          for(uint8_t c=0; c<numOfChannels; c++) {
            if(!channels[c].moonlight) {
              channels[c].numOfEntries = jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_TIMES].size();
              for(uint8_t i=0; i<channels[c].numOfEntries ;i++) {
                channels[c].v[i] = jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_VALUES][i];
                channels[c].t[i] = jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_TIMES][i];
              }
            }
          }
          // save Settings
          saveSettings();
          // forces PWM Update
          handlePWM(true);
          break;
        }
        
        case ID_REQUEST_SETTINGS_FROM_SERVER: {
          DEBUG_INFO("ID_REQUEST_SETTINGS_FROM_SERVER");

          // create json
          JsonObject& jsonOut = jsonBuffer.createObject();
          // id
          jsonOut["id"] = ID_SEND_SETTINGS_TO_CLIENT;
          // number of channels
          jsonOut[CHAR_NUM_OF_CHANNELS] = numOfChannels;
          // maximum number of channels
          jsonOut[CHAR_MAX_NUM_OF_CHANNELS] = MAX_NUM_OF_CHANNELS;
          // timezone
          jsonOut[CHAR_TIMEZONE] = timezone;
          // time
          jsonOut[CHAR_TIME] = epochTime();
          // PWMFrequency
          jsonOut[CHAR_PWM_FREQUENCY] = PWMFrequency;
          // pwm generator
          jsonOut[CHAR_PWM_GENERATOR] = PWMGenerator;
          // current power
          float p=0;
          for(uint8_t c=0; c<numOfChannels; c++) {
            p += channels[c].value / 100. * channels[c].power;
          }
          jsonOut[CHAR_CURRENT_POWER] = p;
          // channels
          JsonArray& jsonChannels = jsonOut.createNestedArray(CHAR_CHANNELS);
          for(uint8_t c=0; c<MAX_NUM_OF_CHANNELS; c++) {
            JsonObject& jsonChannelsChannel = jsonChannels.createNestedObject();
            // channel name
            jsonChannelsChannel[CHAR_CHANNEL_NAME] = jsonBuffer.strdup(channels[c].name);
            // channel color
            jsonChannelsChannel[CHAR_CHANNEL_COLOR] = jsonBuffer.strdup(channels[c].color);
            // channel moonlight
            jsonChannelsChannel[CHAR_CHANNEL_MOONLIGHT] = channels[c].moonlight;
            // channel max moonlight value
            jsonChannelsChannel[CHAR_CHANNEL_MAX_MOONLIGHT_VALUE] = channels[c].maxMoonlightValue;
            // channel power
            jsonChannelsChannel[CHAR_CHANNEL_POWER] = channels[c].power;
            // channel pin
            jsonChannelsChannel[CHAR_CHANNEL_PIN] = channels[c].pin;
          }
          
          // send json
          String jsonOutStr;
          jsonOut.printTo(jsonOutStr);
          webSocket.sendTXT(0,jsonOutStr);
          break;  
        } 

        case ID_SAVE_SETTINGS: {
          DEBUG_INFO("ID_REQUEST_SAVE_SETTINGS");
          
          // number of channels
          numOfChannels = jsonIn[CHAR_NUM_OF_CHANNELS];
          // timezone
          timezone = jsonIn[CHAR_TIMEZONE];
          // PWMFrequency
          PWMFrequency= jsonIn[CHAR_PWM_FREQUENCY];
          setPWMFrequency(PWMFrequency);
          // pwm generator
          PWMGenerator = jsonIn[CHAR_PWM_GENERATOR];
          
          //channels
          for(uint8_t c=0; c<numOfChannels; c++) {
        
            // channel number
            channels[c].channelNumber = c;
            // channel name
            strcpy(channels[c].name, jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_NAME]); 
            // channel color
            strcpy(channels[c].color, jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_COLOR]); 
            // channel moonlight
            channels[c].moonlight = jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_MOONLIGHT];
            // channel max moonlight value
            channels[c].maxMoonlightValue = jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_MAX_MOONLIGHT_VALUE];
            // channel pin
            channels[c].pin = jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_PIN];
            // channel power
            channels[c].power = jsonIn[CHAR_CHANNELS][c][CHAR_CHANNEL_POWER];
          }
          
          // saves the new settings to EEPROM
          saveSettings();

          break;
       }
     
        case ID_RESTART: {
          DEBUG_INFO("restart in 5s");
          delay(5000);
          ESP.restart();
          break;
        }

        case ID_FACTORY_SETTINGS: {
          DEBUG_INFO("RESTORE_FACTORY_SETTINGS");
          saveDefaultSettings();
          loadSettings();
          break;
        }
     }
  }
}


/*
 * Starts the server and the websocket objects
 */
void startServer() {
  DEBUG_INFO("[startServer]");
  
  // starts the SPIFFS fileystem
  startSPIFFS();
  
  server.onNotFound([] { server.send(404, F("text/plain"), F("Website not found")); });
  server.serveStatic("/", SPIFFS, "/main.html");
  server.serveStatic("/script.js", SPIFFS, "/script.js");
  server.serveStatic("/style.css", SPIFFS, "/style.css");
  server.serveStatic("/settings", SPIFFS, SETTINGS_FILE_NAME);
  server.begin();

  delay(50);

  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
}


/*
 * Listen if there is a event of the webserver or the websocket
 * Function is called in Main loop
 */
void handleServer() {
  server.handleClient();
  webSocket.loop();
}
