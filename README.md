# Arduino based remote telemetry unit (RTU) #

This project was done as my main project for a bootcamp at a company whose focus is to create remote monitoring solutions for companies with custom needs. 

### What is this repository for? ###

This repo is half of the whole project. The code is intended to be compiled for Arduino. It is a simple program that does some useful things that can have real world applications. Here is an overview of some of the functions it can perform.

* Record the temperature and humidity and keep track of max and min temperatures.
* Keep track of the current time using an RTC
* Store the temperature, humidity along with the time it was recorded in the EEPROM of the arduino.
* With RGB lights it can show if there are any alarms based on the temperature and the set thresholds for the alarms.
* It also has an ethernet module, with which it can be polled by a master server via UDP
    * uses DCPx protocol and BCH checksum for data validation
* Can be connected to an LCD screen and can be controlled with a 5-button input. Here are some things you can do via the input and the screen.
    * Show if a request is sent its way with a bad BCH checksum
    * Home screen shows current temperature and humidity
    * Can navigate to a history page where it shows the recorded temperature and humidity data. The old data is overwritten once the memory gets full.
    * Shows the count of sent and received UDP packets
    * Settings page where you can configure the following.
        * IP address, subnet mask, gateway IP.
        * Thresholds for triggering major and minor alarms.
        * Erasing the recorded data.