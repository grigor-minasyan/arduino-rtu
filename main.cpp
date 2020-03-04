#include <Arduino.h>
#include <DS3231_Simple.h>
#include <Wire.h>
#include <SimpleDHT.h>
//pins and max / min definitions
#define MAX_STR 12
#define MAX_CMD_COUNT 3
#define DUAL_LED_PIN1 4
#define DUAL_LED_PIN2 5
#define MIN_DELAY 100
#define CUR_VERSION 2.0
#define BAUD_RATE 9600

//pin and setup for the DHT
#define PINDHT22 2
SimpleDHT22 dht22(PINDHT22);

//when adding a menu item, add in 4 places, enum, help, set_command_flag(), execute_commands()
enum menu_items : byte {M_LED, M_SET, M_STATUS, M_VERSION,
	M_HELP, M_INVALID, M_LEDS, M_GREEN, M_RED, M_DUAL,
M_ON, M_OFF, M_BLINK, M_D13, M_WRITE, M_READ, M_RTC,
M_DHT, M_CURRENT, M_SAVED, M_EXTREME};


//input processing variables
short arr[MAX_CMD_COUNT];
char command[MAX_STR+1];
byte command_size = 0;
byte command_count = 0;

char inByte = 0;
DS3231_Simple Clock;
//timekeeping variables
unsigned int curr_time = 0;
unsigned int prev_time1 = 0;
unsigned int prev_time2 = 0;
unsigned int prev_time_dht_short = 0;
unsigned int prev_time_dht_long = 0;
unsigned short blink_delay = 500;
unsigned short dht_read_short_delay = 5000;
unsigned int dht_read_long_delay = 900000;

//toggles for blinking options
bool blinkD13toggle = false;
bool blinkLEDtoggle = false;
bool dual_blink = false;

//for keeping track fo the current color for blinking
byte current_color = 0;
byte blink_color = M_RED;

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


void read_temp_hum() {
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&cur_temp, &cur_humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err=");
		Serial.println(err);
  }
}

void read_temp_hum_loop() {
	if (curr_time - prev_time_dht_short >= dht_read_short_delay) {
		read_temp_hum();
    prev_time_dht_short = curr_time;
  }
	if (curr_time - prev_time_dht_long >= dht_read_long_delay || curr_time < prev_time_dht_long) {
		//TODO do stuff to save the temp
    prev_time_dht_long = curr_time;
	}
}

//changes the color of the LED, x as the setting for the color
void change_dual_led(byte x) {
	//0 is off
	if (x == 0) {
		current_color = 0;
		digitalWrite(DUAL_LED_PIN1, LOW);
		digitalWrite(DUAL_LED_PIN2, LOW);
	} else if (x == M_GREEN) {
		current_color = blink_color = M_GREEN; // for green
		digitalWrite(DUAL_LED_PIN1, HIGH);
		digitalWrite(DUAL_LED_PIN2, LOW);
	} else {
		current_color = blink_color = M_RED; // for red
		digitalWrite(DUAL_LED_PIN1, LOW);
		digitalWrite(DUAL_LED_PIN2, HIGH);
	}
}

//blink the LED based on the set delay
void blink_LED() {
  if ((curr_time - prev_time2 >= blink_delay)) {
		if (dual_blink) {
	    if (blink_color == M_RED) change_dual_led(M_GREEN);
	    else if (blink_color == M_GREEN) change_dual_led(M_RED);
	    prev_time2 = curr_time;
		} else {
	    if (current_color == blink_color) change_dual_led(0);
	    else if (current_color == 0) change_dual_led(blink_color);
	    prev_time2 = curr_time;
		}
  }
}

//blink the d13 led based on the delay set
void blink_d13() {
  if (curr_time - prev_time1 >= blink_delay) {
    if (digitalRead(13) == LOW) digitalWrite(13, HIGH);
    else if (digitalRead(13) == HIGH) digitalWrite(13, LOW);
    prev_time1 = curr_time;
  }
}

//checks for a non negative number from atring, returns the number or -1 if fails
short is_str_positive_number(char command[]) {

	for (byte i = 0; i < MAX_STR; i++) {
		if (command[i] == '\0') break;
		else if (!isdigit(command[i])) return -1;
	}
	return atoi(command);
	/*
	short tens = 1;
  short retval = 0;
  for (byte i = 0; i < MAX_STR; i++) {
    if (command[i] == '\0') break;
    else if (!isdigit(command[i])) return -1;
    else tens *= 10;
  }
  for (byte i = 0; i < MAX_STR; i++) {
    if (command[i] == '\0') break;
    tens /= 10;
    retval += (tens * (command[i] - '0'));
  }
  return retval;
	*/
}

//takes the array and sets the flags based on the command
void set_command_flag(char command[], short arr[]) {
	if (command_count < MAX_CMD_COUNT) {
		if (!strcmp(command, "D13")) arr[command_count++] = M_D13;
		else if(!strcmp(command, "LED")) arr[command_count++] = M_LED;
		else if(!strcmp(command, "DUAL")) arr[command_count++] = M_DUAL;
		else if(!strcmp(command, "SET")) arr[command_count++] = M_SET;
		else if(!strcmp(command, "STATUS")) arr[command_count++] = M_STATUS;
		else if(!strcmp(command, "VERSION")) arr[command_count++] = M_VERSION;
		else if(!strcmp(command, "HELP")) arr[command_count++] = M_HELP;
		else if(!strcmp(command, "ON")) arr[command_count++] = M_ON;
		else if(!strcmp(command, "OFF")) arr[command_count++] = M_OFF;
		else if(!strcmp(command, "BLINK")) arr[command_count++] = M_BLINK;
		else if(!strcmp(command, "LEDS")) arr[command_count++] = M_LEDS;
		else if(!strcmp(command, "RED")) arr[command_count++] = M_RED;
		else if(!strcmp(command, "GREEN")) arr[command_count++] = M_GREEN;
		else if(!strcmp(command, "WRITE")) arr[command_count++] = M_WRITE;
		else if(!strcmp(command, "READ")) arr[command_count++] = M_READ;
		else if(!strcmp(command, "RTC")) arr[command_count++] = M_RTC;
		else if(!strcmp(command, "DHT")) arr[command_count++] = M_DHT;
		else if(!strcmp(command, "CURRENT")) arr[command_count++] = M_CURRENT;
		else if(!strcmp(command, "SAVED")) arr[command_count++] = M_SAVED;
		else if(!strcmp(command, "EXTREME")) arr[command_count++] = M_EXTREME;
		else if(is_str_positive_number(command) != -1) arr[command_count++] = is_str_positive_number(command);
		else arr[command_count++] = M_INVALID;
	}
}

void execute_commands() {
	Serial.println();
	switch (arr[0]) {
		case M_VERSION:
			Serial.print(F("Version: "));
			Serial.println(CUR_VERSION);
			break;
		case M_HELP:
			Serial.println(F("\r-------------------------\n\rAvailable commands:"));
			Serial.println(F("D13 ON | D13 OFF | D13 BLINK\n\rcontrol the LED on pin 13\n"));
			Serial.println(F("LED GREEN | LED RED | LED OFF | LED BLINK | LED DUAL BLINK\n\rcontrol the dual color LED\n"));
			Serial.print(F("SET BLINK X\n\rset the delay to X ms, minimum "));
			Serial.println(MIN_DELAY);
			Serial.println(F("\n\rSTATUS LEDS\n\rfor led status\n"));
			Serial.println(F("DHT CURRENT | DHT SAVED | DHT EXTREME\n\rshow the current, saved, extreme temp / humidity\n"));
			Serial.println(F("READ RTC | WRITE RTC\n\rupdate / read time\n\n\rVERSION\n\rcurrent version\n\r-------------------------"));
			break;
		case M_SET:
			if (arr[1] == M_BLINK && arr[2] >= MIN_DELAY) {
				blink_delay = arr[2];
				break;
			}
		case M_STATUS:
			if (arr[1] == M_LEDS) {
				if (blinkD13toggle) Serial.println(F("Blinking D13"));
				else {
					if (digitalRead(13) == LOW) Serial.println(F("D13 off"));
					else Serial.println(F("D13 on"));
				}
				if (blinkLEDtoggle || dual_blink) {
					if (blink_color == M_GREEN && !dual_blink) Serial.println(F("Blinking green"));
					else if (blink_color == M_RED && !dual_blink) Serial.println(F("Blinking red"));
					else if (dual_blink) Serial.println(F("Blinking both"));
				} else {
					if (current_color == 0) Serial.println(F("LED off"));
					else if (current_color == M_GREEN) Serial.println(F("LED Green"));
					else Serial.println(F("LED Red"));
				}
			}
			break;
		case M_D13:
			switch (arr[1]) {
				case M_ON:
					blinkD13toggle = false;
					digitalWrite(13, HIGH);
					break;
				case M_OFF:
					blinkD13toggle = false;
					digitalWrite(13, LOW);
					break;
				case M_BLINK:
					blinkD13toggle = true;
					break;
				default:
					Serial.println(F("Invalid command, type HELP"));
			}
			break;
		case M_LED:
			switch (arr[1]) {
				case M_RED:
					blinkLEDtoggle = dual_blink = false;
					change_dual_led(M_RED);
					break;
				case M_GREEN:
					blinkLEDtoggle = dual_blink = false;
					change_dual_led(M_GREEN);
					break;
				case M_DUAL:
					if (arr[2] == M_BLINK) {
						blinkLEDtoggle = false;
						dual_blink = true;
					}
					break;
				case M_BLINK:
					blinkLEDtoggle = true;
					dual_blink = false;
					break;
				case M_OFF:
					blinkLEDtoggle = dual_blink = false;
					change_dual_led(0);
					break;
				default:
					Serial.println(F("Invalid command, type HELP"));
			}
			break;
		case M_RTC:
			switch (arr[1]) {
				case M_WRITE:
					Clock.promptForTimeAndDate(Serial);
					break;
				case M_READ:
					read_temp_hum();
					Serial.print("temp is");
					Serial.println(cur_temp);//TODO fix and move to correct command
					Clock.printDateTo_YMD(Serial);
					Serial.print(' ');
					Clock.printTimeTo_HMS(Serial);
					Serial.println();
					break;
				default:
					Serial.println(F("Invalid command, type HELP"));
			}
			break;
		case M_DHT:
			switch (arr[1]) {
				case M_CURRENT:
					Serial.print("Current temp / humidity is ");
					Serial.print(cur_temp);
					Serial.print("C (");
					Serial.print(cur_temp*1.8+32);
					Serial.print("F), ");
					Serial.print(cur_humidity);
					Serial.println("%");
					break;
				case M_SAVED://TODO
					break;
				case M_EXTREME://TODO
					break;
				default:
					Serial.println(F("Invalid command, type HELP"));
			}
			break;
		default:
			Serial.println(F("Invalid command, type HELP"));
	}
	//reset the commands and counter
	for (int i = 0; i < MAX_CMD_COUNT; i++) arr[i] = 0;
	command_count = 0;
}

void loop() {
	curr_time = millis();

	//blink the LEDs, the functions account for the delay
	if (blinkD13toggle) blink_d13();
	if (blinkLEDtoggle || dual_blink) blink_LED();

	if (Serial.available()) {
		inByte = toupper(Serial.read());
		//when space is entered before anything else ignore
		if (inByte == ' ' && command_size == 0) return;
		//when backspace is hit
		if (inByte == 127) {
			Serial.println();
			Serial.println(F("Re-enter the command"));
			command[0] = '\0';
			command_size = 0;
			for (int i = 0; i < MAX_CMD_COUNT; i++) arr[i] = 0;
			command_count = 0;
			return;
		}

		Serial.print(inByte);
    //when enter or space, add the command to the array
		if (inByte == ' ' || inByte == 13) {
			command[command_size] = '\0';
			set_command_flag(command, arr);
			//resetting for the next command
			command[0] = '\0';
			command_size = 0;

			//if enter key is pressed
			if (inByte == 13) execute_commands();

		} else if (command_size < MAX_STR) {
			command[command_size++] = inByte;
			command[command_size] = '\0';
		}
	}
}
