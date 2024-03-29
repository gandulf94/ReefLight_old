# ReefLight
Programmable Aquarium Light Controller based on the cheap ESP8266 Micro controller

## Description
The ESP8266 produces up to 8 PWM Signals at the same time, so for example 8 different LED colors can be controlled at the same time. The Device is fully configurable via WiFi. The number of PWM Signals (channels), timezone and other settings can be configured together with the daily lightshedule via the Browser, so only one initial flashing of the ESP8266 is necessary.

## Features
- Aquarium Light Controller based on the cheap ESP8266 Micro controller
- Up to 8 PWM Signals (channels) at the same time can be generated either by the ESP8266 or the I2C device PCA9685 (not tested yet)
- Full configurable via WiFi
- Time update from a NTP Server over WiFi
- The device supports different modes for each channel
  - **Automatic Mode**
    The ESP8266 get the actual time from NTP Server via Wifi and sets the PWM duty cycle of the channel
    according to the daily shedule of the Channel that can be configured via WiFi
  - **Manual Mode**
    The duty cycle of the PWM Signal of the channel is set manual via WiFi, so the perfect light compositions of all channels together can be easily found
  - **Moonlight Mode (not implemented yet)**
    The channel in Moonlight mode generates a PWM duty cycle reflecting the actual status of the moon 
    
## Preview
In this section a preview of the webinterface is shown. In my own home built Aquarium lamp I use 3 different LED channels for my small reef aquarium 

### Settings page
![alt text](https://github.com/mich4el-git/ReefLight/blob/master/pictures/settings.png)
In this page the device can be fully configured. The settings that can be changed are
- **number of channels**
Number of the used channels
- **PWM Generator**
It can be choosen if the PWM signal of the single channels is generated by the pins of the ESP8266 itself or by the PCA9685 PWM generator module that can be connected to the ESP8266 via I2C (not tested yet).
- **PWM Frequency**
The frequency of the PWM duty cycle can be changed, so annoying summing depending of the LED driver you are using can be avoided.
- **timezone**
timezone in which you live

Additionally the settings for each channel can be configured such as
- **name** 
Name of the channel for better overview in the schedule chart and on the index page
- **color**
Color of the channel for better seperation of the different channels in the schedule chart.
- **moonlight**
Weither the channel is a "normal" channel being either in *manual* or *automatic* mode or in *moonlight* mode, so the PWM duty cycle of the channel is calculated according to the current brightness of the moon.
- **max moonlight value**
Just in *moonlight* mode available. Sets the maximum brightness of the channel in *moonlight* mode.
- **power**
Power of the channel at 100% duty cycle. Needed, so the current Power consumption can be calculated.
- **PWM pin of the ESP8266**
If the signal is generated by the EPS8266, the pin on which the PWM signal is generated can be choosen here. It is noteable that the Arduino definition of the pin must be used. I use for example the WEMOS D1 mini module (see the following picture), so the pins 12,13,14 correspond to the physical pins D6,D7,D5 for example.

The settings must be saved with the **Save** button. The **Reload** button discards changes made and reloads the old settings.
The **Restart** button restarts the ESP8266. (After first boot a manual restart might be necessary before this feature works).
The **Restore Factory Settings** restores the settings that are stored on first start after flashing the device.
![alt text](https://github.com/mich4el-git/ReefLight/blob/master/pictures/wemosD1mini.png)
### Index page
![alt text](https://github.com/mich4el-git/ReefLight/blob/master/pictures/index.png)
In this page the value of the different channels can be set manually or put back to automatic mode

### Schedule page
![alt text](https://github.com/mich4el-git/ReefLight/blob/master/pictures/schedule.png)
In this page the daily schedule of the single channels can be configured. In the chart points for each channel are shown.
The points can be moved via drag and drop. 
If you click on a point it gets selected and it can be deleted with the remove **Delete Point** Button. The **Add Point** adds a new Point to the channel in which a Point is selected.

The red verticle line shows the actual time.

The shedule must be saved with the **Save** button.
The **Reload** button discards changes and reloads the old schedule.

## Getting started
To bring the firmware on the ESP8266 a few easy steps are necessary.

- Download the Arduino IDE Software (Version 1.8.5 used, newer might work)
- Install the following libraries in the Arduino IDE (Sketch->Include Library->Manage Library)
  - **Adafruit PWM Server Driver Library** by Adafruit (Version 1.0.2 used, newer might work)
  - **NTPClient** by Fabrice Weinberg (Version 3.1.0 used, newer might work)
  - **ArduinoJson** by Benoit Blanchon (Version 5.13.1 used, newer might work)
  - **WiFiManager** by tzapu (Version 0.12.0 used, newer might work)
  - **WebSockets** by Markus Sattler (Version 2.1.0 used, newer might work)
- Install the ESP8266 board library
Instructions under https://github.com/esp8266/Arduino
- Install the SPIFFS download tool
Instructions under https://github.com/esp8266/arduino-esp8266fs-plugin

- Compile the sketch and flash the ESP8266
  - Connect the ESP8266 chip that you are using with a USB cable to your PC
  - Select the Eexact ESP8266 board that you are using under Tools->Board
  - Select the Port that the ESP8266 is connected to your Computer under Tools->Port (usually automatically detected)
  - Compile the sketch under Sketch->Verify/Compile
  - Flash the ESP8266 under Sketch->Upload
- upload the files from the **data** folder via the SPIFFS download tool under Tools->ESP8266 Sketch Data Upload

- Now open the Serial Monitor under Tools->Serial Monitor. Select Baudrate 9600
You shoul now see some information comming from the ESP8266

- Connect the ESP8266 to your local WiFi
  - Log yourself into the WiFi that is created by the ESP8266 called **ReefLight** with your computer/smartphone
  - Select your SSID and enter your PASSWORD
  - The ESP8266 should now connect to your WiFi
  
- Connect your computer/smartphone back to your own WiFi and connect to the Website generated by the ESP8266
  - open the Website **reeflight.local** (mDNS must be supported for that)
  - otherwise get the IP adress from the **Serial Monitor** or your **router** and type in the IP adress in your browser
  
- Configure the device as you wish! :)
  - you might have to restart the ESP8266 if you changes some settings such as PWM Generator or the PWM Pins
  
- Enjoy your new Aquarium Lamp Controller
