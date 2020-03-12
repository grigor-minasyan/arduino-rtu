#include "main.h"

void print_invalid_command(byte is_udp) {
	Serial.println(F("Invalid command, type HELP"));
	if (is_udp) {
		Udp.beginPacket(ip_remote, remotePort);
		Udp.write("Invalid command");
		Udp.endPacket();
	}
}

void execute_commands( byte is_udp) {
	Serial.println();

	// Serial.println("debug");
	// for (int i = 0; i < command_count; i++) {
	//
	// 		Serial.println(arr[i]);
	// }

	if (is_udp) {
		Serial.println(F("Executing command from UDP."));
	}

	switch (arr[0]) {
		case M_VERSION:
			Serial.print(F("Version: "));
			Serial.println(CUR_VERSION);

			break;
		case M_HELP:

/*
"├── ADD X Y\n\r├── D13\n\r│   ├── BLINK\n\r│   ├── OFF\n\r│   └── ON\n\r├── DHT\n\r│   ├── CURRENT\n\r│   ├── EXTREME\n\r│   └── SAVED\n\r├── LED\n\r│   ├── BLINK\n\r│   │   ├── DUAL\n\r│   │   ├── GREEN\n\r│   │   └── RED\n\r│   ├── GREEN\n\r│   ├── OFF\n\r│   └── RED\n\r├── RTC\n\r│   ├── READ\n\r│   └── WRITE\n\r├── SET BLINK X\n\r├── STATUS LEDS\n\r└── VERSION"


*/
			if (is_udp) {
				Udp.beginPacket(ip_remote, remotePort);
				Udp.write("DHT CURRENT | DHT EXTREME | DHT SAVED | RTC READ");
				Udp.endPacket();
			}

			Serial.print(F("├── ADD X Y\n\r├── RGB\n\r│   ├── 1 R G B (RGB color 1 0-255)\n\r│   ├── 2 R G B (RGB color 2 0-255\n\r│   └── SET BLINK X (delay in ms)\n\r├── DHT (temperature and humidity sensors)\n\r│   ├── CURRENT\n\r│   ├── EXTREME\n\r│   ├── SAVED X (X is optional, how many data points to print)\n\r│   └── RESET\n\r├── LED\n\r│   ├── BLINK\n\r│   │   ├── DUAL\n\r│   │   ├── GREEN\n\r│   │   └── RED\n\r│   ├── GREEN\n\r│   ├── OFF\n\r│   └── RED\n\r├── RTC (time clock)\n\r│   ├── READ\n\r│   └── WRITE\n\r├── SET BLINK X (sets for all LEDs, min "));
			Serial.print(MIN_DELAY);
			Serial.println(F("ms)\n\r├── STATUS LEDS\n\r└── VERSION"));

			break;
		case M_SET:
			if (arr[1] == M_BLINK && arr[2] >= MIN_DELAY) blink_delay2 = blink_delay3 = arr[2];
			else print_invalid_command(is_udp);
			break;
		case M_STATUS:
			if (arr[1] == M_LEDS) {
				if (dual_led_binary == 0b00100010 || dual_led_binary == 0b10001000) Serial.println(F("Blinking green"));
				else if (dual_led_binary == 0b00010001 || dual_led_binary == 0b01000100) Serial.println(F("Blinking red"));
				else if (dual_led_binary == 0b01100110 || dual_led_binary == 0b10011001) Serial.println(F("Blinking both on dual LED"));
				else if (dual_led_binary == 0b00000000) Serial.println(F("LED off"));
				else if (dual_led_binary == 0b10101010) Serial.println(F("LED Green"));
				else Serial.println(F("LED Red"));

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
					print_invalid_command(is_udp);
			}
			break;
		case M_RTC:
			switch (arr[1]) {
				case M_WRITE:
					Clock.promptForTimeAndDate(Serial);
					break;
				case M_READ:
					if (is_udp) {
						char buff[5];
						Udp.beginPacket(ip_remote, remotePort);
						itoa(Clock.read().Year, buff, 10);
						Udp.write(buff);
						Udp.write("/");
						itoa(Clock.read().Month, buff, 10);
						Udp.write(buff);
						Udp.write("/");
						itoa(Clock.read().Day, buff, 10);
						Udp.write(buff);
						Udp.write(" ");
						itoa(Clock.read().Hour, buff, 10);
						Udp.write(buff);
						Udp.write(":");
						itoa(Clock.read().Minute, buff, 10);
						Udp.write(buff);
						Udp.write(":");
						itoa(Clock.read().Second, buff, 10);
						Udp.write(buff);
						Udp.endPacket();
					}
					Clock.printDateTo_YMD(Serial);
					Serial.print(' ');
					Clock.printTimeTo_HMS(Serial);
					Serial.println();
					break;
				default:
					print_invalid_command(is_udp);
			}
			break;
		case M_DHT:
			switch (arr[1]) {
				case M_CURRENT:
					if (is_udp) {
						char buff[5];
						Udp.beginPacket(ip_remote, remotePort);
						itoa(cur_temp, buff, 10);
						Udp.write(buff);
						Udp.write("C (");
						itoa(to_farenheit(cur_temp), buff, 10);
						Udp.write(buff);
						Udp.write("F), ");
						itoa(cur_humidity, buff, 10);
						Udp.write(buff);
						Udp.write("%");
						Udp.endPacket();
					}
					Serial.print(("Current temp / humidity is "));
					Serial.print(cur_temp);
					Serial.print(("C ("));
					Serial.print(to_farenheit(cur_temp));
					Serial.print(("F), "));
					Serial.print(cur_humidity);
					Serial.println(("%"));
					break;
				case M_SAVED:
					if (arr[2] > 0) print_EEPROM_data(arr[2], is_udp);
					else print_EEPROM_data(10, is_udp);//default printing lines
					break;
				case M_EXTREME:
					if (is_udp) {
						char buff[5];
						Udp.beginPacket(ip_remote, remotePort);
						Udp.write("Max temp / humidity ");
						itoa(max_temp, buff, 10);
						Udp.write(buff);
						Udp.write("C (");
						itoa(to_farenheit(max_temp), buff, 10);
						Udp.write(buff);
						Udp.write("F), ");
						itoa(max_humidity, buff, 10);
						Udp.write(buff);
						Udp.write("%, min temp / humidity ");
						itoa(min_temp, buff, 10);
						Udp.write(buff);
						Udp.write("C (");
						itoa(to_farenheit(min_temp), buff, 10);
						Udp.write(buff);
						Udp.write("F), ");
						itoa(min_humidity, buff, 10);
						Udp.write(buff);
						Udp.write("%");
						Udp.endPacket();
					}
					Serial.print(F("Maximum recorded temp / humidity is "));
					Serial.print(max_temp);
					Serial.print(F("C ("));
					Serial.print(to_farenheit(max_temp));
					Serial.print(F("F), "));
					Serial.print(max_humidity);
					Serial.print(F("%\n\rMinimum recorded temp / humidity is "));
					Serial.print(min_temp);
					Serial.print(F("C ("));
					Serial.print(to_farenheit(min_temp));
					Serial.print(F("F), "));
					Serial.print(min_humidity);
					Serial.println(F("%"));
					break;
				case M_RESET:
					reset_EEPROM_data();
					break;
				default:
					print_invalid_command(is_udp);
			}
			break;
		case M_ADD:
			if (arr[1] != -32768 && arr[2] != -32768) Serial.println(arr[1] + arr[2]); // fixme overflow underflow
			else print_invalid_command(is_udp);
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
					print_invalid_command(is_udp);
			}
			break;
		default:
			print_invalid_command(is_udp);
	}
	//reset the commands and counter
	for (int i = 0; i < MAX_CMD_COUNT; i++) arr[i] = 0;
	command_count = 0;
}
