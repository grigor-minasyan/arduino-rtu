#include <Arduino.h>
template <class T>

/*
This class is used to help me store the data on the EEPROM, since it is
basically an array and it is hard to store the data in an organized manner.
*/
class Eeprom_indexes {
private:
	// support to start and end the data storage at basically any point in the memory
	// can help have multiple classes store multiple different data in the EEPROM
	int start_i;
	int end_i;
	
	int curr_i; //where the last data is written
	int stored_data_count; //just count, obviously
	int actual_start_i; // the first few bytes are reserved,
	byte is_underflow; // used to rewrite the data on the old data
public:
	int get_start_i();
	int get_end_i();
	int get_curr_i();
	int get_stored_data_count();
	Eeprom_indexes<T>(int new_start_i, int new_end_i); // constructor needs just the start and end index, templated.
	void store_data(T data_to_store);
	void print_data(int x, int8_t is_udp);
	T get_ith_data_from_curr(int x); // used for reading the data from newest to oldest
	T get_ith(int x); // used to read the data from the start of the actual memory reerved for the class.
	void set_ith(int x, T data);

	// to clear the reserved bytes and make it act empty even though it is not.
	// It is not recommended to clear the eeprom data by making everything 0
	void init(); 
};
