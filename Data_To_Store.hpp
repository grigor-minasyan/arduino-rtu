#include <Arduino.h>

/*
this is a class that can help store the data we have nicely in the EEPROM of the arduino.
The date and time is stored as a single unsigned long to help save storage and store more data.
it has 2 int8_t to store the temperature and humidity. 
*/
class Data_To_Store {
private:
	unsigned long date_time_temp;
	int8_t humidity;
	int8_t temp;

	// private functions to be used to store date and time with bitshifting.
	void write_everything(byte shift_to_left, byte size_in_bits, byte num);
	byte read_everything(byte shift_to_left, byte size_in_bits);
public:
	//getters and setter
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
