#include "main.h"


void Data_To_Store::write_everything(byte shift_to_left, byte size_in_bits, unsigned long num) {

  //flag for makig sure the size is smaller than the size in bits specified
  int8_t flag = 1 << size_in_bits;
  flag--;
  num = (num & flag);
  num = num << shift_to_left;
  date_time_temp = date_time_temp | num;
}

unsigned long Data_To_Store::read_everything(byte shift_to_left, byte size_in_bits) {
  //bitwise operations to get the data from the unsigned long
  unsigned long retval = date_time_temp;
  unsigned long flag = 1 << size_in_bits;
  flag--;
  flag = flag << shift_to_left;
  retval = retval & flag;
  retval = retval >> shift_to_left;
  return retval;
}
void Data_To_Store::set_hum(int8_t h) {humidity = h;}
int8_t Data_To_Store::get_hum() {return humidity;}



int Eeprom_indexes::get_start_i(){return start_i;}
int Eeprom_indexes::get_end_i(){return end_i;}
int Eeprom_indexes::get_curr_i(){return curr_i;}
int Eeprom_indexes::get_stored_data_count(){return stored_data_count;}

Eeprom_indexes::Eeprom_indexes(int new_start_i, int new_end_i, int new_curr_i, int new_stored_data_count) :
start_i(new_start_i),	end_i(new_end_i), curr_i(new_curr_i), stored_data_count(new_stored_data_count) {
  short cur, num;
  EEPROM.get(0, cur);
  EEPROM.get(2, num);
  curr_i = cur;
  stored_data_count = num;

  is_underflow = false;
  actual_start_i = (start_i + (3*sizeof(short)));
  if (curr_i < actual_start_i) curr_i = actual_start_i;
}

void Eeprom_indexes::store_data(Data_To_Store data_to_store) {
  // Serial.println("debug for loop runs for delayed temperature recording inn eeprom");
  EEPROM.put(curr_i, data_to_store);
  if (!is_underflow) {
    stored_data_count++;
  }
  curr_i += sizeof(Data_To_Store);

  if (curr_i > (end_i - sizeof(Data_To_Store))) {
    this->init();//fixme todo fix the underflow
  }
  EEPROM.put(0, curr_i);
  EEPROM.put(2, stored_data_count);
}

void Eeprom_indexes::init() {
  curr_i = actual_start_i;
  stored_data_count = 0;
  is_underflow = false;
}

void Eeprom_indexes::print_data(int x, int8_t is_udp) {
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
    Data_To_Store ret;
    read_i -= sizeof(Data_To_Store);
    //FIXME undeflow overflow
    if (read_i < actual_start_i) {
      read_i += (int)((end_i - actual_start_i) / sizeof(Data_To_Store));
    }

    EEPROM.get(read_i, ret);


    //getting the numbers from the bitwise
    byte ret_year = CUR_YEAR + ret.read_everything(31, 1);
    byte ret_month = ret.read_everything(27, 4);
    byte ret_day = ret.read_everything(22, 5);
    byte ret_hour = ret.read_everything(17, 5);
    byte ret_minute = ret.read_everything(11, 6);
    byte ret_second = 15 * ret.read_everything(9, 2);
    int8_t ret_temp = ret.read_everything(1, 8);
    byte ret_hum = ret.get_hum();
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
      Udp.endPacket();
    }

  }

}
