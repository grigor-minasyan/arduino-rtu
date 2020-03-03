#include <Arduino.h>

//pins and max / min definitions
#define MAX_STR 12
#define MAX_CMD_COUNT 3
#define DUAL_LED_PIN1 4
#define DUAL_LED_PIN2 5
#define MIN_DELAY 100
#define CUR_VERSION 1.0

//byte [0 1 2] menu
#define D13 50
#define LED 51
#define SET 52
#define STATUS 53
#define VERSION 54
#define HELP 55
#define INVALID 56
#define ON 57
#define OFF 58
#define BLINK 59
#define LEDS 60
#define GREEN 61
#define RED 62
#define DUAL 63

//input processing variables
short arr[MAX_CMD_COUNT];
char command[MAX_STR+1];
byte command_size = 0;
byte command_count = 0;

char inByte = 0;

//timekeeping variables
unsigned int curr_time = 0;
unsigned int prev_time1 = 0;
unsigned int prev_time2 = 0;
unsigned short blink_delay = 500;

//toggles for blinking options
bool blinkD13toggle = false;
bool blinkLEDtoggle = false;
bool dual_blink = false;

//for keeping track fo the current color for blinking
byte current_color = 0;
byte blink_color = RED;

void setup() {
	Serial.begin(9600);
	command[0] = '\0';
	pinMode(13, OUTPUT);
	pinMode(DUAL_LED_PIN1, OUTPUT);
	pinMode(DUAL_LED_PIN2, OUTPUT);
	Serial.println(F("Enter commands or 'HELP'"));
}

//changes the color of the LED, x as the setting for the color
void change_dual_led(byte x) {
	//0 is off
	if (x == 0) {
		current_color = 0;
		digitalWrite(DUAL_LED_PIN1, LOW);
		digitalWrite(DUAL_LED_PIN2, LOW);
	} else if (x == GREEN) {
		current_color = blink_color = GREEN; // for green
		digitalWrite(DUAL_LED_PIN1, HIGH);
		digitalWrite(DUAL_LED_PIN2, LOW);
	} else {
		current_color = blink_color = RED; // for red
		digitalWrite(DUAL_LED_PIN1, LOW);
		digitalWrite(DUAL_LED_PIN2, HIGH);
	}
}

//blink the LED based on the set delay
void blink_LED() {
  if ((curr_time - prev_time2 >= blink_delay)) {
		if (dual_blink) {
	    if (blink_color == RED) change_dual_led(GREEN);
	    else if (blink_color == GREEN) change_dual_led(RED);
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
}

//takes the array and sets the flags based on the command
void set_command_flag(char command[], short arr[]) {
	if (command_count < MAX_CMD_COUNT) {
		if (strcmp(command, "D13") == 0) arr[command_count++] = D13;
		else if(strcmp(command, "LED") == 0) arr[command_count++] = LED;
		else if(strcmp(command, "DUAL") == 0) arr[command_count++] = DUAL;
		else if(strcmp(command, "SET") == 0) arr[command_count++] = SET;
		else if(strcmp(command, "STATUS") == 0) arr[command_count++] = STATUS;
		else if(strcmp(command, "VERSION") == 0) arr[command_count++] = VERSION;
		else if(strcmp(command, "HELP") == 0) arr[command_count++] = HELP;
		else if(strcmp(command, "ON") == 0) arr[command_count++] = ON;
		else if(strcmp(command, "OFF") == 0) arr[command_count++] = OFF;
		else if(strcmp(command, "BLINK") == 0) arr[command_count++] = BLINK;
		else if(strcmp(command, "LEDS") == 0) arr[command_count++] = LEDS;
		else if(strcmp(command, "RED") == 0) arr[command_count++] = RED;
		else if(strcmp(command, "GREEN") == 0) arr[command_count++] = GREEN;
		else if(is_str_positive_number(command) != -1) arr[command_count++] = is_str_positive_number(command);
		else arr[command_count++] = INVALID;
	}
}

void execute_commands() {
	Serial.println();
	if (arr[0] == VERSION) {
		Serial.print(F("Version: "));
		Serial.println(CUR_VERSION);
	}
	else if (arr[0] == HELP) {
    Serial.print(F("\rAvailable commands:\n\r'D13 ON' 'D13 OFF' 'D13 BLINK' control the LED on pin 13,\n\r'LED GREEN' 'LED RED' 'LED OFF' 'LED BLINK' 'LED DUAL BLINK' control the dual color LED,\n\r'SET BLINK X' set the delay to X ms, minimum "));
    Serial.print(MIN_DELAY);
    Serial.print(F(",\n\r'STATUS LEDS'\n\r'VERSION'\n\r"));
  }
  else if (arr[0] == SET && arr[1] == BLINK && arr[2] >= MIN_DELAY) blink_delay = arr[2];
  else if (arr[0] == STATUS && arr[1] == LEDS) {
    if (blinkD13toggle) Serial.println(F("Blinking D13"));
    else {
      if (digitalRead(13) == LOW) Serial.println(F("D13 off"));
      else Serial.println(F("D13 on"));
    }
    if (blinkLEDtoggle || dual_blink) {
      if (blink_color == GREEN && !dual_blink) Serial.println(F("Blinking green"));
      else if (blink_color == RED && !dual_blink) Serial.println(F("Blinking red"));
      else if (dual_blink) Serial.println(F("Blinking both"));
    } else {
      if (current_color == 0) Serial.println(F("LED off"));
      else if (current_color == GREEN) Serial.println(F("LED Green"));
      else Serial.println(F("LED Red"));
    }
  } else if (arr[0] == D13) {
		if (arr[1] == ON) {
			blinkD13toggle = false;
			digitalWrite(13, HIGH);
		}
		else if (arr[1] == OFF) {
			blinkD13toggle = false;
			digitalWrite(13, LOW);
		}
		else if (arr[1] == BLINK) blinkD13toggle = true;
	} else if (arr[0] == LED) {
		if (arr[1] == RED) {
			blinkLEDtoggle = dual_blink = false;
			change_dual_led(RED);
		}	else if (arr[1] == GREEN) {
			blinkLEDtoggle = dual_blink = false;
			change_dual_led(GREEN);
		}
		else if (arr[1] == DUAL && arr[2] == BLINK) {
			blinkLEDtoggle = false;
			dual_blink = true;
		}
		else if (arr[1] == BLINK) {
			blinkLEDtoggle = true;
			dual_blink = false;
		}
		else if (arr[1] == OFF) {
			blinkLEDtoggle = dual_blink = false;
			change_dual_led(0);
		}
	} else {
		Serial.println(F("Invalid command"));
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
