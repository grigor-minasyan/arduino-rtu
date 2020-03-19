#include "Data_To_Store.hpp"

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
