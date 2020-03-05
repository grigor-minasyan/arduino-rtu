#include "global_var.h"
void read_temp_hum() {
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&cur_temp, &cur_humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err=");
		Serial.println(err);
  }
}

void read_temp_hum_loop() {
	if (curr_time - prev_time_dht_short >= dht_read_short_delay) {
		read_temp_hum();
    prev_time_dht_short = curr_time;
  }
	if (curr_time - prev_time_dht_long >= dht_read_long_delay || curr_time < prev_time_dht_long) {
		//TODO do stuff to save the temp
    prev_time_dht_long = curr_time;
	}
}
