#include <Arduino.h>
#include <DS3231_Simple.h>
#include <Wire.h>
#include <SimpleDHT.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <LiquidCrystal.h>
#include "Data_To_Store.hpp"
#include "Eeprom_indexes.hpp"


#ifndef MY_GLOBALS_H


#define MAX_STR 12
#define MAX_CMD_COUNT 5
#define MIN_DELAY 50
#define CUR_VERSION 2.4
#define SERIAL_ENABLE false
#define UDP_OLD_ENABLE false
#define BAUD_RATE 9600
#define UDP_LISTEN_DELAY 50
#define UDP_CHECKER_DELAY 2111

//pin and setup for the DHT
#define PINDHT22 8
#define NUM_LEDS 2

#define RGB_DATA_PIN_ALL 9

#define dht_read_short_delay 3000
#define dht_read_long_delay 60000//600000


#define REMOTEPORT 54211
#define REMOTEPORT_SERVER 10000
#define localPort 8888

//-------------------------------
//lcd pins
#define RS 6
#define EN 7
#define D4 5
#define D5 4
#define D6 3
#define D7 2
//-------------------------------

extern Adafruit_NeoPixel leds_all;

extern uint32_t color_maj_und, color_min_und, color_comfortable, color_min_ovr, color_maj_ovr;

extern int8_t temp_threshold__arr[4];

extern void five_button_read();
extern void take_input();
extern void take_input_udp();
extern void take_input_udp_dcpx();
extern void execute_commands(byte is_udp);
extern void read_temp_hum_loop();
extern int8_t to_farenheit(int8_t x);
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

//temperaure and humidity related variables------------------------
extern int8_t cur_temp;
extern int8_t cur_humidity;
extern int8_t max_temp;
extern int8_t min_temp;
extern int8_t max_humidity;
extern int8_t min_humidity;
extern int8_t current_threshold;
extern byte temp_alarm_binary; // 0b0000 alarms clear, mj under, mn under, mn over mj over in order if bit is set
//end temperaure and humidity related variables------------------------


//Ethernet declarations-------------------------------------------
extern byte mac[];
extern IPAddress ip;
extern IPAddress subnet;
extern IPAddress gateway;
extern IPAddress ip_remote;

// buffers for receiving and sending data
extern char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,
extern char ReplyBuffer[];// a string to send back
// An EthernetUDP instance to let us send and receive packets over UDP
extern EthernetUDP Udp;
extern long udp_packets_in_counter, udp_packets_out_counter;
//end Ethernet declarations-------------------------------------------


//lcd related functions and declarations-------------------------------------------
extern LiquidCrystal lcd;
extern void show_lcd_menu(byte x);
extern byte curr_lcd_menu;
//end lcd related functions and declarations-------------------------------------------

//start Eeprom_indexes class definitions----------------------------------------
template <class T>
int Eeprom_indexes<T>::get_start_i(){return start_i;}
template <class T>
int Eeprom_indexes<T>::get_end_i(){return end_i;}
template <class T>
int Eeprom_indexes<T>::get_curr_i(){return curr_i;}
template <class T>
int Eeprom_indexes<T>::get_stored_data_count(){return stored_data_count;}


template <class T>
Eeprom_indexes<T>::Eeprom_indexes(int new_start_i, int new_end_i) {
  start_i = new_start_i;
  end_i = new_end_i;
  //get current index, the count of data points and the is_underflow value from the eeprom
  EEPROM.get(new_start_i, curr_i);
  EEPROM.get(new_start_i+sizeof(int), stored_data_count);
  EEPROM.get(new_start_i+2*sizeof(int), is_underflow);

	//assign the actual start value so it can keep the critical start and count values in the eeprom
  actual_start_i = (new_start_i + (2*sizeof(int) + sizeof(byte)));

	//should never happen, to not owerwrite the config start count and is_underflow data
  if (curr_i < actual_start_i) curr_i = actual_start_i;
}

//store data appends the data at the end and if overflows, starts overwriting the old data
template <class T>
void Eeprom_indexes<T>::store_data(T data_to_store) {
  EEPROM.put(curr_i, data_to_store);
  if (!is_underflow) stored_data_count++;
  curr_i += sizeof(T);

  if (curr_i > (end_i - sizeof(T))) {
    curr_i = actual_start_i;
    is_underflow = 1;
  }
  EEPROM.put(start_i, curr_i);
  EEPROM.put(start_i+sizeof(int), stored_data_count);
  EEPROM.put(start_i+2*sizeof(int), is_underflow);
}

template <class T>
void Eeprom_indexes<T>::init() {
  //initialize the class
  curr_i = actual_start_i;
  stored_data_count = 0;
  is_underflow = 0;
  EEPROM.put(start_i, curr_i);
  EEPROM.put(start_i+sizeof(int), stored_data_count);
  EEPROM.put(start_i+2*sizeof(int), is_underflow);
}

template <class T>
T Eeprom_indexes<T>::get_ith_data_from_curr(int x) {
  if (stored_data_count == 0) return T();
  if (x >= stored_data_count) x = stored_data_count - 1;
  if (x < 0) x = 0;

  T ret;
  int read_i = curr_i;
  read_i -= (x+1)*sizeof(T);
  if (read_i < actual_start_i) {
    read_i += sizeof(T)*(int)((end_i - actual_start_i) / sizeof(T));
  }
  EEPROM.get(read_i, ret);
  return ret;
}

template <class T>
T Eeprom_indexes<T>::get_ith(int x) {
	T ret;
	if (x < 0 || x > (int)((end_i-actual_start_i)/sizeof(T))) return T();
	EEPROM.get(actual_start_i+(sizeof(T)*x), ret);
	return ret;
}

template <class T>
void Eeprom_indexes<T>::set_ith(int x, T data) {
	if (x < 0 || x > (int)((end_i-actual_start_i)/sizeof(T))) return;
	EEPROM.put(actual_start_i+(sizeof(T)*x), data);
}

template <class T>
void Eeprom_indexes<T>::print_data(int x, int8_t is_udp) {
  if (stored_data_count == 0) {
		if constexpr (SERIAL_ENABLE) {
	    Serial.println("No data");
		}
		return;
  }
	if constexpr (SERIAL_ENABLE) {
		Serial.print(stored_data_count);
		Serial.println(" data points.");
		Serial.print("Current index is ");
		Serial.println(curr_i);
	}
  if (x > stored_data_count) x = stored_data_count;
  for (int i = 0; i < x; i++) {
    //getting the numbers from the bitwise
    byte ret_year = get_ith_data_from_curr(i).get_year();
    byte ret_month = get_ith_data_from_curr(i).get_month();
    byte ret_day = get_ith_data_from_curr(i).get_day();
    byte ret_hour = get_ith_data_from_curr(i).get_hour();
    byte ret_minute = get_ith_data_from_curr(i).get_minute();
    byte ret_second = get_ith_data_from_curr(i).get_second();
    int8_t ret_temp = get_ith_data_from_curr(i).get_temp();
    byte ret_hum = get_ith_data_from_curr(i).get_hum();
    //

		if constexpr (SERIAL_ENABLE) {
	    Serial.print(i+1);
	    Serial.print("\t");
	    Serial.print(ret_year);
	    Serial.print("/");
	    Serial.print(ret_month);
	    Serial.print("/");
	    Serial.print(ret_day);
	    Serial.print(" ");
	    Serial.print(ret_hour);
	    Serial.print(":");
	    Serial.print(ret_minute);
	    Serial.print(":");
	    Serial.print(ret_second);
	    Serial.print(" ");
	    Serial.print(ret_temp);
	    Serial.print("C (");
	    Serial.print(to_farenheit(ret_temp));
	    Serial.print("F), ");
	    Serial.print(ret_hum);
	    Serial.println("%");
		}
		if constexpr (UDP_OLD_ENABLE) {
	    if (is_udp) {
	      char buff[8];
	      Udp.beginPacket(ip_remote, REMOTEPORT);
	      itoa(ret_year, buff, 10);
	      Udp.write(buff);
	      Udp.write("/");
	      itoa(ret_month, buff, 10);
	      Udp.write(buff);
	      Udp.write("/");
	      itoa(ret_day, buff, 10);
	      Udp.write(buff);
	      Udp.write(" ");
	      itoa(ret_hour, buff, 10);
	      Udp.write(buff);
	      Udp.write(":");
	      itoa(ret_minute, buff, 10);
	      Udp.write(buff);
	      Udp.write(":");
	      itoa(ret_second, buff, 10);
	      Udp.write(buff);
	      Udp.write(" ");
	      itoa(ret_temp, buff, 10);
	      Udp.write(buff);
	      Udp.write("C (");
	      itoa(to_farenheit(ret_temp), buff, 10);
	      Udp.write(buff);
	      Udp.write("F), ");
	      itoa(ret_hum, buff, 10);
	      Udp.write(buff);
	      Udp.write("%");
	      udp_packets_out_counter++;
	      Udp.endPacket();
	    }
		}
  }
}
//end Eeprom_indexes class definitions----------------------------------------

//objct containers for temperature log and config files
extern Eeprom_indexes<Data_To_Store> rtc_dht_data_range;
extern Eeprom_indexes<byte> ip_sub_gate_config;
extern Eeprom_indexes<int8_t> thresholds_config;


#endif
