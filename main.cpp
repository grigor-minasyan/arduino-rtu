#include <Arduino.h>
#include <DS3231_Simple.h>
#include <Wire.h>
#include <SimpleDHT.h>

#include "global_var.h"

SimpleDHT22 dht22(PINDHT22);
//for buffer
short arr[MAX_CMD_COUNT];
char command[MAX_STR+1];
//input processing variables
byte command_size = 0;
byte command_count = 0;
char inByte = 0;

//timekeeping variables
DS3231_Simple Clock;
unsigned int curr_time = 0, prev_time1 = 0, prev_time2 = 0, prev_time_dht_short = 0, prev_time_dht_long = 0;

unsigned short blink_delay = 500, dht_read_short_delay = 5000;
unsigned int dht_read_long_delay = 900000;

//toggles for blinking options
bool blinkD13toggle = false, blinkLEDtoggle = false, dual_blink = false;

//for keeping track fo the current color for blinking
byte current_color = 0, blink_color = M_RED;

//keeping current temp and humidity in global
float cur_temp = 0;
int max_temp = INT8_MIN;
int min_temp = INT8_MAX;
float cur_humidity = 0;



void setup() {
	Serial.begin(BAUD_RATE);
	Clock.begin();
	command[0] = '\0';
	pinMode(13, OUTPUT);
	pinMode(DUAL_LED_PIN1, OUTPUT);
	pinMode(DUAL_LED_PIN2, OUTPUT);
	Serial.println(F("Enter commands or 'HELP'"));
}

void loop() {
	curr_time = millis();
	//blink the LEDs, the functions account for the delay
	if (blinkD13toggle) blink_d13();
	if (blinkLEDtoggle || dual_blink) blink_LED();
	take_input();
}
