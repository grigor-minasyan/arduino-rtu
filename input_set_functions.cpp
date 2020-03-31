#include "main.h"

//checks for a non negative number from atring, returns the number or -1 if fails
bool is_str_number(char command[], int &ret) {

	for (byte i = 0; i < MAX_STR; i++) {
		if (command[i] == '\0') break;
		else if (command[0] == '-' && i == 0) continue;
		else if (!isdigit(command[i])) return false;
	}
	ret = atoi(command);
	return true;
}

void take_input() {
	if constexpr (SERIAL_ENABLE) {
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
				if (inByte == 13) execute_commands(0);

			} else if (command_size < MAX_STR) {
				command[command_size++] = inByte;
				command[command_size] = '\0';
			}
		}
	}
}

//this function is called only if constexpr UDP_OLD_ENABLE
void take_input_udp() {
	static unsigned long prev_time_udp, prev_time_udp_checker;
	if ((millis() - prev_time_udp > UDP_LISTEN_DELAY)) {
		if (millis() - prev_time_udp_checker > UDP_CHECKER_DELAY) {
			leds_all.setPixelColor(1, 0x1e0000);
			leds_all.show();
			prev_time_udp_checker = millis();
		}
		prev_time_udp = millis();
		int packetSize = Udp.parsePacket();
		if (packetSize) {
			// read the packet into packetBufffer
			Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
			if(packetSize == 2 && packetBuffer[0] == 'y') {
				leds_all.setPixelColor(1, 0x001e00);
				leds_all.show();
				prev_time_udp_checker = millis();
				return;
			}
			udp_packets_in_counter++;
			for (byte i = 0; i < packetSize; i++) {
				packetBuffer[i] = toupper(packetBuffer[i]);
				//when space is entered before anything else ignore
				if (packetBuffer[i] == ' ' && command_size == 0) continue;
		    //when enter or space, add the command to the array
				if (packetBuffer[i] == ' ') {
					command[command_size] = '\0';
					set_command_flag(command, arr);
					//resetting for the next command
					command[0] = '\0';
					command_size = 0;
				} else if (command_size < MAX_STR) {
					command[command_size++] = packetBuffer[i];
					command[command_size] = '\0';
				}
			}
			command[command_size] = '\0';
			set_command_flag(command, arr);
			//resetting for the next command
			command[0] = '\0';
			command_size = 0;
			execute_commands(1);


			// // send a reply to the IP address and port that sent us the packet we received
			// Udp.beginPacket(ip_remote, remotePort);
			// Udp.write(ReplyBuffer);
			// Udp.endPacket();
			// udp_packets_out_counter++;
		}
	}
}

//takes the array and sets the flags based on the command
void set_command_flag(char command[], int arr[]) {
	int num;
	if (command_count < MAX_CMD_COUNT) {
		if(!strcmp(command, "VERSION")) arr[command_count++] = M_VERSION;
		else if(!strcmp(command, "HELP")) arr[command_count++] = M_HELP;
		else if(!strcmp(command, "OFF")) arr[command_count++] = M_OFF;
		else if(!strcmp(command, "WRITE")) arr[command_count++] = M_WRITE;
		else if(!strcmp(command, "READ")) arr[command_count++] = M_READ;
		else if(!strcmp(command, "RTC")) arr[command_count++] = M_RTC;
		else if(!strcmp(command, "DHT")) arr[command_count++] = M_DHT;
		else if(!strcmp(command, "CURRENT")) arr[command_count++] = M_CURRENT;
		else if(!strcmp(command, "SAVED")) arr[command_count++] = M_SAVED;
		else if(!strcmp(command, "EXTREME")) arr[command_count++] = M_EXTREME;
		else if(!strcmp(command, "RESET")) arr[command_count++] = M_RESET;
		else if(arr[0] == M_DHT && arr[1] == M_SAVED) {
			if (is_str_number(command, num)) arr[command_count++] = num;
			else arr[command_count++] = M_INVALID;
		}  else arr[command_count++] = M_INVALID;
	}
}
