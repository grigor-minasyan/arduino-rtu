#include "main.h"


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
    this->init();
  }
  EEPROM.put(0, curr_i);
  EEPROM.put(2, stored_data_count);
}

void Eeprom_indexes::init() {
  curr_i = actual_start_i;
  stored_data_count = 0;
  is_underflow = false;
}

void Eeprom_indexes::print_data(int x) {
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
    Serial.print(i+1);
    Serial.print("\t");
    Serial.print(ret.date_time.Year);
    Serial.print("/");
    Serial.print(ret.date_time.Month);
    Serial.print("/");
    Serial.print(ret.date_time.Day);
    Serial.print(" ");
    Serial.print(ret.date_time.Hour);
    Serial.print(":");
    Serial.print(ret.date_time.Minute);
    Serial.print(":");
    Serial.print(ret.date_time.Second);
    Serial.print(" ");
    Serial.print(ret.temp);
    Serial.print("C (");
    Serial.print(to_farenheit(ret.temp));
    Serial.print("F), ");
    Serial.print(ret.humidity);
    Serial.println("%");
  }
}
