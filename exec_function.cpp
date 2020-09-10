#include "main.h"

//function to send an invalid command update to UDP and serial
void print_invalid_command(byte is_udp) {
	if constexpr (SERIAL_ENABLE) {
		Serial.println(F("Invalid command, type HELP"));
	}

	if (is_udp && UDP_OLD_ENABLE) {
		Udp.beginPacket(ip_remote, REMOTEPORT);
		Udp.write("Invalid command");
		Udp.endPacket();
		udp_packets_out_counter++;
	}
}


//execute commands, from arr array, i know not ideal
//it executed based on if serial is enable or not, or old udp responder enabled or not.
void execute_commands(byte is_udp) {
	if constexpr (SERIAL_ENABLE) {
		Serial.println();
	}

	if (is_udp) {
		if constexpr (SERIAL_ENABLE) {
			Serial.println(F("Executing command from UDP."));
		}
	}

	switch (arr[0]) {
		case M_VERSION:
			if (is_udp&& UDP_OLD_ENABLE) {
				Udp.beginPacket(ip_remote, REMOTEPORT);
				Udp.write(CUR_VERSION);
				Udp.endPacket();
				udp_packets_out_counter++;
			}
			if constexpr (SERIAL_ENABLE) {
				Serial.print(F("Version: "));
				Serial.println(CUR_VERSION);
			}
			break;
		case M_HELP:
			if (is_udp&& UDP_OLD_ENABLE) {
				Udp.beginPacket(ip_remote, REMOTEPORT);
				Udp.write("DHT CURRENT | DHT EXTREME | DHT SAVED | RTC READ");
				Udp.endPacket();
	      udp_packets_out_counter++;
			}
			if constexpr (SERIAL_ENABLE) {
				Serial.print(F("├── DHT (temperature and humidity sensors)\n\r│   ├── CURRENT\n\r│   ├── EXTREME\n\r│   ├── SAVED X (X is optional, how many data points to print)\n\r│   └── RESET\n\r├── RTC (time clock)\n\r│   ├── READ\n\r│   └── WRITE\n\r└── VERSION\n\r"));
			}
			break;
		case M_RTC:
			switch (arr[1]) {
				case M_WRITE:
					if constexpr (SERIAL_ENABLE) {
						if (!is_udp) Clock.promptForTimeAndDate(Serial);
					}
					else print_invalid_command(is_udp);
					break;
				case M_READ:
					if (is_udp&& UDP_OLD_ENABLE) {
						char buff[5];
						Udp.beginPacket(ip_remote, REMOTEPORT);
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
						udp_packets_out_counter++;
					}
					if constexpr (SERIAL_ENABLE) {
						Clock.printDateTo_YMD(Serial);
						Serial.print(' ');
						Clock.printTimeTo_HMS(Serial);
						Serial.println();
					}
					break;
				default:
					print_invalid_command(is_udp);
			}
			break;
		case M_DHT:
			switch (arr[1]) {
				case M_CURRENT:
					if (is_udp&& UDP_OLD_ENABLE) {
						char buff[5];
						Udp.beginPacket(ip_remote, REMOTEPORT);
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
			      udp_packets_out_counter++;
					}
					if constexpr (SERIAL_ENABLE) {
						Serial.print(F("Current temp / humidity is "));
						Serial.print(cur_temp);
						Serial.print(F("C ("));
						Serial.print(to_farenheit(cur_temp));
						Serial.print(F("F), "));
						Serial.print(cur_humidity);
						Serial.println(F("%"));
					}
					break;
				case M_SAVED:
					if (arr[2] > 0) rtc_dht_data_range.print_data(arr[2], is_udp);
					else rtc_dht_data_range.print_data(10, is_udp);//default printing lines
					break;
				case M_EXTREME:
					if (is_udp&& UDP_OLD_ENABLE) {
						char buff[5];
						Udp.beginPacket(ip_remote, REMOTEPORT);
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
			      udp_packets_out_counter++;
					}
					if constexpr (SERIAL_ENABLE) {
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
					}
					break;
				case M_RESET:
					if (is_udp&& UDP_OLD_ENABLE) {
						Udp.beginPacket(ip_remote, REMOTEPORT);
						Udp.write("DHT reset");
						Udp.endPacket();
						udp_packets_out_counter++;
					}
					if constexpr (SERIAL_ENABLE) {
						Serial.println(F("DHT reset"));
					}
					rtc_dht_data_range.init();
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
