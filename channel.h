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

#ifndef CHANNEL__H
#define CHANNEL__H

#include <Arduino.h>

// constants
static const uint8_t LEN_CHANNEL_NAME = 20; // max length of the Channel name
static const uint8_t LEN_CHANNEL_COLOR = 7; // max lenght of the Channel color (hex code #FFFF00 e.g.)
static const uint8_t MAX_NUM_OF_ENTRIES = 16; // max number of entries (time-value pair) in the schedule
static const uint8_t MAX_NUM_OF_CHANNELS = 8; // max number of channels
static const uint8_t PWM_GENERATOR_ESP8266 = 0; // Macros either the PWM is generated by a ESP8266 
static const uint8_t PWM_GENERATOR_PCA9685 = 1; // or the I2C PCA9685 module
static const unsigned long MILLIS_BETWEEN_PWM_UPDATES = 5000; // time between PWM updates in ms

// Class defining the channel objects
class Channel {

  public:
    // number of the channel going from 0 to MAX_NUM_OF_CHANNELS-1
    uint8_t channelNumber;
    
    // name of the channel
    char name[LEN_CHANNEL_NAME + 1];
    
    // hex code RGB of the channel color for displaying purpose on the webinterface only
    char color[LEN_CHANNEL_COLOR+1];

    // if true: channel is in manual mode and does not get updated by the according to the schedule (const pwm value)
    // if false: pwm value does get calculated according to the entries in the schedule (t,v arrays)
    bool manual;
    
    // number of entries (time-value pairs) for the automatic mode
    uint8_t numOfEntries;
    
    // times of the (time, value)-tuples stored as seconds that has passed since midnight
    uint32_t t[MAX_NUM_OF_ENTRIES];
    
    // values of the (time, value)-tuples stored as percent
    float v[MAX_NUM_OF_ENTRIES];
    
    // actual PWM value of the channel in %
    float value;

    // if true: channel simulates the moonlight and does not get updated according to the schedule
    // if false: channel is not in moonlight mode, so either in automatic or manual mode
    bool moonlight;
    
    // maximal PWM Value of the moonlight channel in %
    float maxMoonlightValue;
    
    // pin that is generating the PWM signal if the ESP8266 is directly used to generate the PWM signal
    // meaningless if the PWM signal is generated by an external PCA9685
    uint8_t pin;
    
    // powerconsumption of the channel @100% PWM Signal
    float power;

    // constructor
    Channel();
    // destructor
    ~Channel();

    
    // functions

    // prints all information of the channel to the DEBUG_PORT
    void print();
    // updates the pwm signal according to weather manual is true or false (according to the time schedule)
    void updatePWM();
    
};


// global variables
extern uint8_t numOfChannels; // current number of used channels
extern uint32_t PWMFrequency; // current frequency for generating the PWM signal
extern Channel channels[MAX_NUM_OF_CHANNELS]; // arrays with all possible channels
extern uint16_t PWMGenerator; // defines if the PWM signal is generated by the ESP8266 itself or the PCA9685

// prints all channels to DEBUG_PORT
void printAllChannels();

// configures the PWM generation
void configurePWM();

// handle functiom for the PWM generation in main loop
void handlePWM(const bool force);

// sets a new PWM frequency
void setPWMFrequency(const uint32_t f);

#endif
