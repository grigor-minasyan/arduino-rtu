#include <Arduino.h>
#include <EEPROM.h>
#include <DS3231_Simple.h>
#include <Wire.h>
#include <SimpleDHT.h>
#include <Adafruit_NeoPixel.h>

#include <Ethernet.h>
#include <EthernetUdp.h>

#include <LiquidCrystal.h>

#ifndef MY_GLOBALS_H


#define MAX_STR 12
#define MAX_CMD_COUNT 5
#define MIN_DELAY 50
#define CUR_VERSION 2.3
#define BAUD_RATE 9600
#define UDP_LISTEN_DELAY 50
#define UDP_CHECKER_DELAY 2111

//pin and setup for the DHT
#define PINDHT22 8
#define NUM_LEDS 2

#define RGB_DATA_PIN_TEMP 7
#define RGB_DATA_PIN_LINK 6
#define RGB_DATA_PIN_ALL 6

#define dht_read_short_delay 3000
#define dht_read_long_delay 60000//600000


#define remotePort 54211
#define localPort 8888


extern Adafruit_NeoPixel leds_all;

extern uint32_t color_maj_und, color_min_und, color_comfortable, color_min_ovr, color_maj_ovr;


extern int8_t temp_threshold_1, temp_threshold_2, temp_threshold_3, temp_threshold_4;

extern void five_button_read();
extern void take_input();
extern void take_input_udp();
extern void execute_commands(byte is_udp);
extern void read_temp_hum_loop();
extern void print_EEPROM_data(int x, int8_t is_udp);
extern void reset_EEPROM_data();
extern int8_t to_farenheit(int8_t x);
extern void blink_RGB();
extern bool is_str_number(char command[], int &ret);
extern void set_command_flag(char command[], int arr[]);

//when adding a menu item, add in 4 places, enum, help, set_command_flag(), execute_commands()
typedef enum {M_SET = -128, M_VERSION,
	M_HELP, M_OFF, M_BLINK, M_WRITE, M_READ, M_RTC,
M_DHT, M_CURRENT, M_SAVED, M_EXTREME, M_RESET, M_INVALID} MENU_ITEMS;
extern MENU_ITEMS menu_items;


typedef enum  {LCD_HOME = 0, LCD_HISTORY_OUT, LCD_HISTORY_IN, LCD_PACKETS, LCD_SETTINGS_OUT,
  LCD_SETTINGS_IP_OUT, LCD_SETTINGS_IP_IN,
  LCD_SETTINGS_GATE_OUT, LCD_SETTINGS_GATE_IN,
  LCD_SETTINGS_SUB_OUT, LCD_SETTINGS_SUB_IN,
  LCD_SETTINGS_THRESHOLD_OUT, LCD_SETTINGS_THRESHOLD_IN,
  LCD_SETTINGS_ERASE_OUT, LCD_SETTINGS_ERASE_IN,
  } LCD_SCREEN_MENU;
extern LCD_SCREEN_MENU lcd_screen_menu;


extern SimpleDHT22 dht22;
//for buffer
extern int arr[MAX_CMD_COUNT];
extern char command[MAX_STR+1];
//input processing variables
extern byte command_size;
extern byte command_count;
extern char inByte;

//timekeeping variables
extern DS3231_Simple Clock;
extern bool link_status;

extern unsigned int blink_delay3;


//keeping current temp and humidity in global
extern int8_t cur_temp;
extern int8_t cur_humidity;
extern int8_t max_temp;
extern int8_t min_temp;
extern int8_t max_humidity;
extern int8_t min_humidity;

extern int8_t current_threshold;


//Ethernet declarations-------------------------------------------
// The IP address will be dependent on your local network:
extern byte mac[];
extern IPAddress ip;
extern IPAddress ip_remote;
// local port to listen on
// buffers for receiving and sending data
extern char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,
extern char ReplyBuffer[];// a string to send back
// An EthernetUDP instance to let us send and receive packets over UDP
extern EthernetUDP Udp;
extern int udp_packets_in_counter, udp_packets_out_counter;
//end Ethernet declarations-------------------------------------------


class Data_To_Store {
private:
	unsigned long date_time_temp;
	int8_t humidity;
	int8_t temp;
	void write_everything(byte shift_to_left, byte size_in_bits, byte num);
	byte read_everything(byte shift_to_left, byte size_in_bits);
public:
	byte get_year();
	byte get_month();
	byte get_day();
	byte get_hour();
	byte get_minute();
	byte get_second();
	void set_year(byte num);
	void set_month(byte num);
	void set_day(byte num);
	void set_hour(byte num);
	void set_minute(byte num);
	void set_second(byte num);
	void set_hum(int8_t h);
	int8_t get_hum();
	void set_temp(int8_t t);
	int8_t get_temp();
};

class Eeprom_indexes {
private:
	int start_i;
	int end_i;
	int curr_i;
	int stored_data_count;
	int actual_start_i;
	bool is_underflow;
public:
	int get_start_i();
	int get_end_i();
	int get_curr_i();
	int get_stored_data_count();
	Eeprom_indexes(int new_start_i, int new_end_i);
	void store_data(Data_To_Store data_to_store);
	void print_data(int x, int8_t is_udp);
	Data_To_Store get_ith_data(int x);
	void init();
};
extern Eeprom_indexes rtc_dht_data_range;

extern LiquidCrystal lcd;
extern void show_lcd_menu(byte x);
extern byte curr_lcd_menu;

#endif
