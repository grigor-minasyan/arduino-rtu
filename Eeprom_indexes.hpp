#include <Arduino.h>
template <class T>
class Eeprom_indexes {
private:
	uint16_t start_i;
	uint16_t end_i;
	uint16_t curr_i;
	uint16_t stored_data_count;
	uint16_t actual_start_i;
	bool is_underflow;
public:
	uint16_t get_start_i();
	uint16_t get_end_i();
	uint16_t get_curr_i();
	uint16_t get_stored_data_count();
	Eeprom_indexes<T>(uint16_t new_start_i, uint16_t new_end_i);
	void store_data(T data_to_store);
	void print_data(uint8_t x, int8_t is_udp);
	T get_ith_data_from_curr(uint8_t x);
	T get_ith(uint8_t x);
	void set_ith(uint8_t x, T data);
	void init();
};
