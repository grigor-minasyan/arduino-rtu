#include "main.h"

void execute_commands() {
	Serial.println();
	switch (arr[0]) {
		case M_VERSION:
			Serial.print(F("Version: "));
			Serial.println(CUR_VERSION);
			break;
		case M_HELP:
			Serial.println(F("\r-------------------------\n\rAvailable commands:"));
			Serial.println(F("D13 ON | D13 OFF | D13 BLINK | (control pin 13 LED)\n"));
			Serial.println(F("LED GREEN | LED RED | LED OFF | LED BLINK | LED DUAL BLINK | (control the dual color LED)\n"));
			Serial.print(F("SET BLINK X | (set the delay to X ms, minimum "));
			Serial.print(MIN_DELAY);
			Serial.println(F(")\n\n\rSTATUS LEDS | (show led status)\n"));
			Serial.println(F("ADD X Y | (add X and Y, integers only)\n"));
			Serial.println(F("DHT CURRENT | DHT SAVED | DHT EXTREME | (show the current, saved, extreme temp / humidity)\n"));
			Serial.println(F("RTC READ | RTC WRITE | (update / read time)\n\n\rVERSION | (current version)\n\r-------------------------"));
			break;
		case M_SET:
			if (arr[1] == M_BLINK && arr[2] >= MIN_DELAY) blink_delay = arr[2];
			else Serial.println(F("Invalid command, type HELP"));
			break;
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
					else if (dual_blink) Serial.println(F("Blinking both on dual LED"));
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
					Serial.print(to_farenheit(cur_temp));
					Serial.print("F), ");
					Serial.print(cur_humidity);
					Serial.println("%");
					break;
				case M_SAVED:
					if (arr[2] > 0) print_EEPROM_data(arr[2]);
					else print_EEPROM_data(10);//default printing lines
					break;
				case M_EXTREME:
					Serial.print("Maximum recorded temp / humidity is ");
					Serial.print(max_temp);
					Serial.print("C (");
					Serial.print(to_farenheit(max_temp));
					Serial.print("F), ");
					Serial.print(max_humidity);
					Serial.print("%\n\rMimimum recorded temp / humidity is ");
					Serial.print(min_temp);
					Serial.print("C (");
					Serial.print(to_farenheit(min_temp));
					Serial.print("F), ");
					Serial.print(min_humidity);
					Serial.println("%");
					break;
				default:
					Serial.println(F("Invalid command, type HELP"));
			}
			break;
		case M_ADD:
			if (arr[1] != -32768 && arr[2] != -32768) Serial.println(arr[1] + arr[2]); // fixme overflow underflow
			else Serial.println(F("Invalid command, type HELP"));
			break;
		default:
			Serial.println(F("Invalid command, type HELP"));
	}
	//reset the commands and counter
	for (int i = 0; i < MAX_CMD_COUNT; i++) arr[i] = 0;
	command_count = 0;
}
