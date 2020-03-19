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

void Data_To_Store::write_everything(byte shift_to_left, byte size_in_bits, byte num) {
  // flag for makig sure the size is smaller than the size in bits specified
  byte flag = 1 << size_in_bits;
  flag--;
  num = (num & flag);
  unsigned long num_copy = num;
  num_copy = num_copy << shift_to_left;
  //storing the num in the date time
  date_time_temp = date_time_temp | num_copy;
}
byte Data_To_Store::read_everything(byte shift_to_left, byte size_in_bits) {
  //bitwise operations to get the data from the unsigned long
  unsigned long retval = date_time_temp;
  byte flag = (1 << size_in_bits);
  flag--;
  retval = retval >> shift_to_left;
  retval = retval & flag;
  return retval;
}
void Data_To_Store::set_hum(int8_t h) {humidity = h;}
int8_t Data_To_Store::get_hum() {return humidity;}
void Data_To_Store::set_temp(int8_t t) {temp = t;}
int8_t Data_To_Store::get_temp() {return temp;}
byte Data_To_Store::get_year(){return read_everything(26, 6);}
byte Data_To_Store::get_month(){return read_everything(22, 4);}
byte Data_To_Store::get_day(){return read_everything(17, 5);}
byte Data_To_Store::get_hour(){return read_everything(12, 5);}
byte Data_To_Store::get_minute(){return read_everything(6, 6);}
byte Data_To_Store::get_second(){return read_everything(0, 6);}
void Data_To_Store::set_year(byte num){write_everything(26, 6, num);}
void Data_To_Store::set_month(byte num){write_everything(22, 4, num);}
void Data_To_Store::set_day(byte num){write_everything(17, 5, num);}
void Data_To_Store::set_hour(byte num){write_everything(12, 5, num);}
void Data_To_Store::set_minute(byte num){write_everything(6, 6, num);}
void Data_To_Store::set_second(byte num){write_everything(0, 6, num);}




extern LiquidCrystal lcd;
extern void show_lcd_menu(byte x);
extern byte curr_lcd_menu;


template <class T>
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
	Eeprom_indexes<T>(int new_start_i, int new_end_i);
	void store_data(T data_to_store);
	void print_data(int x, int8_t is_udp);
	T get_ith_data(int x);
	void init();
};

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
  short cur, num;
  //get current index and the count of data points
  EEPROM.get(new_start_i, cur);
  EEPROM.get(new_start_i+sizeof(short), num);
  curr_i = cur;
  stored_data_count = num;

  is_underflow = false;
  actual_start_i = (new_start_i + (2*sizeof(short)));
  if (curr_i < actual_start_i) curr_i = actual_start_i;
}

template <class T>
void Eeprom_indexes<T>::store_data(T data_to_store) {
  // Serial.println("debug for loop runs for delayed temperature recording inn eeprom");
  EEPROM.put(curr_i, data_to_store);
  if (!is_underflow) {
    stored_data_count++;
  }
  curr_i += sizeof(T);

  if (curr_i > (end_i - sizeof(T))) {
    curr_i = actual_start_i;
    is_underflow = true;
  }
  EEPROM.put(start_i, curr_i);
  EEPROM.put(start_i+sizeof(int), stored_data_count);
}

template <class T>
void Eeprom_indexes<T>::init() {
  //initialize the class
  curr_i = actual_start_i;
  stored_data_count = 0;
  is_underflow = false;
}


template <class T>
T Eeprom_indexes<T>::get_ith_data(int x) {
  if (stored_data_count == 0) return T();
  if (x >= stored_data_count) x = stored_data_count - 1;
  if (x < 0) x = 0;

  T ret;
  int read_i = curr_i;
  read_i -= ((x+1)*sizeof(T));
  if (read_i < actual_start_i) {
    read_i += sizeof(T)*(int)((end_i - actual_start_i) / sizeof(T));
  }
  EEPROM.get(read_i, ret);
  return ret;
}

template <class T>
void Eeprom_indexes<T>::print_data(int x, int8_t is_udp) {
  int read_i = curr_i;
  if (stored_data_count == 0) {
    Serial.println("No data");
    return;
  }
  Serial.print(stored_data_count);
  Serial.println(" data points.");
  Serial.print("Current index is ");
  Serial.println(curr_i);



  if (x > stored_data_count) x = stored_data_count;
  for (int i = 0; i < x; i++) {
    //getting the numbers from the bitwise
    byte ret_year = get_ith_data(i).get_year();
    byte ret_month = get_ith_data(i).get_month();
    byte ret_day = get_ith_data(i).get_day();
    byte ret_hour = get_ith_data(i).get_hour();
    byte ret_minute = get_ith_data(i).get_minute();
    byte ret_second = get_ith_data(i).get_second();
    int8_t ret_temp = get_ith_data(i).get_temp();
    byte ret_hum = get_ith_data(i).get_hum();
    //


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

    if (is_udp) {
      char buff[8];
      Udp.beginPacket(ip_remote, remotePort);
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

extern Eeprom_indexes<Data_To_Store> rtc_dht_data_range;


#endif
