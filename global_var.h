#include <Arduino.h>
#include <DS3231_Simple.h>
#include <Wire.h>
#include <SimpleDHT.h>


#ifndef MY_GLOBALS_H



#define MAX_STR 12
#define MAX_CMD_COUNT 3
#define DUAL_LED_PIN1 4
#define DUAL_LED_PIN2 5
#define MIN_DELAY 100
#define CUR_VERSION 2.0
#define BAUD_RATE 9600

//pin and setup for the DHT
#define PINDHT22 2

extern void take_input();
extern void execute_commands();
extern void read_temp_hum();
extern void read_temp_hum_loop();
extern char to_farenheit(char x);
extern void change_dual_led(byte x);
extern void blink_LED();
extern void blink_d13();
extern short is_str_positive_number(char command[]);
extern void set_command_flag(char command[], short arr[]);

//when adding a menu item, add in 4 places, enum, help, set_command_flag(), execute_commands()
typedef enum {M_LED, M_SET, M_STATUS, M_VERSION,
	M_HELP, M_INVALID, M_LEDS, M_GREEN, M_RED, M_DUAL,
M_ON, M_OFF, M_BLINK, M_D13, M_WRITE, M_READ, M_RTC,
M_DHT, M_CURRENT, M_SAVED, M_EXTREME} MENU_ITEMS;
extern MENU_ITEMS menu_items;

extern SimpleDHT22 dht22;
//for buffer
extern short arr[MAX_CMD_COUNT];
extern char command[MAX_STR+1];
//input processing variables
extern byte command_size;
extern byte command_count;
extern char inByte;

//timekeeping variables
extern DS3231_Simple Clock;
extern unsigned int curr_time, prev_time1, prev_time2, prev_time_dht_short, prev_time_dht_long;

extern unsigned short blink_delay, dht_read_short_delay;
extern unsigned int dht_read_long_delay;

//toggles for blinking options
extern bool blinkD13toggle, blinkLEDtoggle, dual_blink;

//for keeping track fo the current color for blinking
extern byte current_color, blink_color;

//keeping current temp and humidity in global
extern byte cur_temp;
extern byte cur_humidity;
extern char max_temp;
extern char min_temp;
extern char max_humidity;
extern char min_humidity;





#endif
