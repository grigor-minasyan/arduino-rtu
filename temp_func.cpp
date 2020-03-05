#include "global_var.h"

char to_farenheit(char x)  {
  return x * 1.8 + 32; //TODO fixme returns character
}

void read_temp_hum() {
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read(&cur_temp, &cur_humidity, NULL)) != SimpleDHTErrSuccess) {
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
		//TODO do stuff to save the temp
    prev_time_dht_long = curr_time;
	}
}
