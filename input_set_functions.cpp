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


void take_input_udp() {
	if ((curr_time - prev_time_udp > UDP_LISTEN_DELAY)) {
		prev_time_udp = curr_time;
		int packetSize = Udp.parsePacket();
		if (packetSize) {

			/*
			Serial.print("Received packet of size ");
			Serial.println(packetSize);
			Serial.print("From ");
			IPAddress remote = Udp.remoteIP();
			for (int i=0; i < 4; i++) {
				Serial.print(remote[i], DEC);
				if (i < 3) {
					Serial.print(".");
				}
			}
			Serial.print(", port ");
			Serial.println(Udp.remotePort());
			*/

			// read the packet into packetBufffer
			Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
			Serial.println(F("Contents:"));
			Serial.println(packetBuffer);
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


			// send a reply to the IP address and port that sent us the packet we received
			Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
			Udp.write(ReplyBuffer);
			Udp.endPacket();
		}
	}
}

//takes the array and sets the flags based on the command
void set_command_flag(char command[], int arr[]) {
	int num;
	if (command_count < MAX_CMD_COUNT) {
		if(!strcmp(command, "LED")) arr[command_count++] = M_LED;
		else if(!strcmp(command, "DUAL")) arr[command_count++] = M_DUAL;
		else if(!strcmp(command, "SET")) arr[command_count++] = M_SET;
		else if(!strcmp(command, "STATUS")) arr[command_count++] = M_STATUS;
		else if(!strcmp(command, "VERSION")) arr[command_count++] = M_VERSION;
		else if(!strcmp(command, "HELP")) arr[command_count++] = M_HELP;
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
		else if(!strcmp(command, "ADD")) arr[command_count++] = M_ADD;
		else if(!strcmp(command, "RGB")) arr[command_count++] = M_RGB;
		else if(!strcmp(command, "RESET")) arr[command_count++] = M_RESET;
		else if(arr[0] == M_ADD || arr[0] == M_RGB || (arr[0] == M_SET && arr[1] == M_BLINK) || (arr[0] == M_DHT && arr[1] == M_SAVED)) {
			if (is_str_number(command, num)) arr[command_count++] = num;
		}  else arr[command_count++] = M_INVALID;
	}
}
