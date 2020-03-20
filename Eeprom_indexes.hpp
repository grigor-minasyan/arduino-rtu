#include <Arduino.h>
template <class T>
class Eeprom_indexes {
private:
	int start_i;
	int end_i;
	int curr_i;
	int stored_data_count;
	int actual_start_i;
	byte is_underflow;
public:
	int get_start_i();
	int get_end_i();
	int get_curr_i();
	int get_stored_data_count();
	Eeprom_indexes<T>(int new_start_i, int new_end_i);
	void store_data(T data_to_store);
	void print_data(int x, int8_t is_udp);
	T get_ith_data_from_curr(int x);
	T get_ith(int x);
	void set_ith(int x, T data);
	void init();
};
