#include "main.h"

void execute_commands() {
	Serial.println();

	// Serial.println("debug");
	// for (int i = 0; i < command_count; i++) {
	//
	// 		Serial.println(arr[i]);
	// }

	switch (arr[0]) {
		case M_VERSION:
			Serial.print(F("Version: "));
			Serial.println(CUR_VERSION);
			break;
		case M_HELP:

/*
"├── ADD X Y\n\r├── D13\n\r│   ├── BLINK\n\r│   ├── OFF\n\r│   └── ON\n\r├── DHT\n\r│   ├── CURRENT\n\r│   ├── EXTREME\n\r│   └── SAVED\n\r├── LED\n\r│   ├── BLINK\n\r│   │   ├── DUAL\n\r│   │   ├── GREEN\n\r│   │   └── RED\n\r│   ├── GREEN\n\r│   ├── OFF\n\r│   └── RED\n\r├── RTC\n\r│   ├── READ\n\r│   └── WRITE\n\r├── SET BLINK X\n\r├── STATUS LEDS\n\r└── VERSION"


*/
			Serial.print(F("├── ADD X Y\n\r├── D13\n\r│   ├── BLINK\n\r│   ├── OFF\n\r│   └── ON\n\r├── RGB\n\r│   ├── 1 R G B (RGB color 1 0-255)\n\r│   ├── 2 R G B (RGB color 2 0-255\n\r│   └── SET BLINK X (delay in ms)\n\r├── DHT (temperature and humidity sensors)\n\r│   ├── CURRENT\n\r│   ├── EXTREME\n\r│   ├── SAVED X (X is optional, how many data points to print)\n\r│   └── RESET\n\r├── LED\n\r│   ├── BLINK\n\r│   │   ├── DUAL\n\r│   │   ├── GREEN\n\r│   │   └── RED\n\r│   ├── GREEN\n\r│   ├── OFF\n\r│   └── RED\n\r├── RTC (time clock)\n\r│   ├── READ\n\r│   └── WRITE\n\r├── SET BLINK X (sets for all LEDs, min "));
			Serial.print(MIN_DELAY);
			Serial.println("ms)\n\r├── STATUS LEDS\n\r└── VERSION");
			break;
		case M_SET:
			if (arr[1] == M_BLINK && arr[2] >= MIN_DELAY) blink_delay = blink_delay2 = blink_delay3 = arr[2];
			else Serial.println(F("Invalid command, type HELP"));
			break;
		case M_STATUS:
			if (arr[1] == M_LEDS) {
				if (blinkD13toggle) Serial.println(F("Blinking D13"));
				else {
					if (digitalRead(13) == LOW) Serial.println(F("D13 off"));
					else Serial.println(F("D13 on"));
				}
				if (dual_led_binary == 0b00100010) Serial.println(F("Blinking green"));
				else if (dual_led_binary == 0b00010001) Serial.println(F("Blinking red"));
				else if (dual_led_binary == 0b01100110) Serial.println(F("Blinking both on dual LED"));
				else if (dual_led_binary == 0b00000000) Serial.println(F("LED off"));
				else if (dual_led_binary == 0b10101010) Serial.println(F("LED Green"));
				else Serial.println(F("LED Red"));

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
					dual_led_binary = 0b01010101;
					break;
				case M_GREEN:
					dual_led_binary = 0b10101010;
					break;
				case M_BLINK:
					if (arr[2] == M_GREEN) dual_led_binary = 0b00100010;
					else if (arr[2] == M_DUAL) dual_led_binary = 0b01100110;
					else dual_led_binary = 0b00010001;
					break;
				case M_OFF:
					dual_led_binary = 0b00000000;
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
				case M_RESET:
					reset_EEPROM_data();
					break;
				default:
					Serial.println(F("Invalid command, type HELP"));
			}
			break;
		case M_ADD:
			if (arr[1] != -32768 && arr[2] != -32768) Serial.println(arr[1] + arr[2]); // fixme overflow underflow
			else Serial.println(F("Invalid command, type HELP"));
			break;
		case M_RGB:
			switch(arr[1]) {
				case 1:
					color1.red = arr[2];
					color1.green = arr[3];
					color1.blue = arr[4];
					break;
				case 2:
					color2.red = arr[2];
					color2.green = arr[3];
					color2.blue = arr[4];
					break;
				case M_SET:
					if (arr[2] == M_BLINK && arr[3] >= MIN_DELAY) blink_delay3 = arr[3];
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
