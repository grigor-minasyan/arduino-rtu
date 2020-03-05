#include "global_var.h"

float to_farenheit(float x)  {
  return x * 1.8 + 32;
}

Eeprom_indexes rtc_dht_data_range;
data_to_store time_and_data;


void print_EEPROM_data(int x) {
  int read_i = rtc_dht_data_range.curr_i;
  if (rtc_dht_data_range.stored_data_count == 0) {
    Serial.println("No data");
    return;
  }
  if (x > rtc_dht_data_range.stored_data_count) x = rtc_dht_data_range.stored_data_count;
  for (int i = 0; i < x; i++) {
    data_to_store ret;
    read_i -= sizeof(ret);
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

//not to be called from outside, only by the looper
void read_temp_hum() {
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&cur_temp, &cur_humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err=");
		Serial.println(err);
  }
  if (cur_temp > 127) cur_temp -= 256;
  if (cur_humidity > 127) cur_humidity -= 256;
}

void read_temp_hum_loop() {
	if (curr_time - prev_time_dht_short >= dht_read_short_delay) {
		read_temp_hum();
    if (cur_temp <  min_temp) min_temp = cur_temp;
    if (cur_temp > max_temp) max_temp = cur_temp;
    if (cur_humidity <  min_humidity) min_humidity = cur_humidity;
    if (cur_humidity > max_humidity) max_humidity = cur_humidity;
    prev_time_dht_short = curr_time;
  }
	if (curr_time - prev_time_dht_long >= dht_read_long_delay || curr_time < prev_time_dht_long) {

    // Serial.println("debug for loop runs for delayed temperature recording inn eeprom");
    time_and_data.date_time = Clock.read();
    time_and_data.temp = cur_temp;
    time_and_data.humidity = cur_humidity;
    EEPROM.put(rtc_dht_data_range.curr_i, time_and_data);
    rtc_dht_data_range.stored_data_count++;
    rtc_dht_data_range.curr_i += sizeof(time_and_data);
    if (rtc_dht_data_range.curr_i > (rtc_dht_data_range.end_i - sizeof(time_and_data))) {
      rtc_dht_data_range.stored_data_count = 0; // resets back to 0, FIXME
      rtc_dht_data_range.curr_i = 0;
    }

    prev_time_dht_long = curr_time;
	}
}
