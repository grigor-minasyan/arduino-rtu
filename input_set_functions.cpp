#include "global_var.h"

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
			if (inByte == 13) execute_commands();

		} else if (command_size < MAX_STR) {
			command[command_size++] = inByte;
			command[command_size] = '\0';
		}
	}
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
