#include "main.h"

//function to send an invalid command update to UDP and serial
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


	if (is_udp) {
		Serial.println(F("Executing command from UDP."));
	}

	switch (arr[0]) {
		case M_VERSION:
			Serial.print(F("Version: "));
			Serial.println(CUR_VERSION);
			break;
		case M_HELP:
			if (is_udp) {
				Udp.beginPacket(ip_remote, remotePort);
				Udp.write("DHT CURRENT | DHT EXTREME | DHT SAVED | RTC READ");
				Udp.endPacket();
			}
			Serial.print(F("├── ADD X Y\n\r├── RGB\n\r│   ├── 1 R G B (RGB color 1 0-255)\n\r│   ├── 2 R G B (RGB color 2 0-255\n\r│   └── SET BLINK X (delay in ms)\n\r├── DHT (temperature and humidity sensors)\n\r│   ├── CURRENT\n\r│   ├── EXTREME\n\r│   ├── SAVED X (X is optional, how many data points to print)\n\r│   └── RESET\n\r├── RTC (time clock)\n\r│   ├── READ\n\r│   └── WRITE\n\r└── VERSION\n\r"));
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
					Serial.print(F("Current temp / humidity is "));
					Serial.print(cur_temp);
					Serial.print(F("C ("));
					Serial.print(to_farenheit(cur_temp));
					Serial.print(F("F), "));
					Serial.print(cur_humidity);
					Serial.println(F("%"));
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
			if (arr[1] != M_INVALID && arr[2] != M_INVALID) Serial.println(arr[1] + arr[2]);
			else print_invalid_command(is_udp);
			break;
		case M_RGB:
			switch(arr[1]) {
				case 1: // fixme does not show the red one
					color1 = (((uint32_t)arr[2]) << 16 | ((uint32_t)arr[3]) << 8 | ((uint32_t)arr[4]));
					break;
				case 2:
					color2 = (((uint32_t)arr[2]) << 16 | ((uint32_t)arr[3]) << 8 | ((uint32_t)arr[4]));
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
