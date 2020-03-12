#include "main.h"
int8_t to_farenheit(int8_t x)  {
  return x * 1.8 + 32;
}

//this keeps track of the data and does the storing in EEPROM
Eeprom_indexes rtc_dht_data_range(0, EEPROM.length()-1, 0, 0);
Data_To_Store time_and_data;

//x is the number of data to print
void print_EEPROM_data(int x) {
  rtc_dht_data_range.print_data(x);
}


//reset data
void reset_EEPROM_data() {
  rtc_dht_data_range.init();
}

int8_t get_threshold(int8_t t) {
  if (t < temp_threshold_1) return 0;
  if (t < temp_threshold_2) return 1;
  if (t < temp_threshold_3) return 2;
  if (t < temp_threshold_4) return 3;
  return 4;
}

//not to be called from outside, only by the looper, limited to 0.5hz maximum
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
    prev_time_dht_short = curr_time;
		read_temp_hum();
        //setting the max and min values
    if (cur_temp <  min_temp) min_temp = cur_temp;
    if (cur_temp > max_temp) max_temp = cur_temp;
    if (cur_humidity <  min_humidity) min_humidity = cur_humidity;
    if (cur_humidity > max_humidity) max_humidity = cur_humidity;

    //check if threshold changed
    if (current_threshold != get_threshold(cur_temp)) {
      Serial.print(F("Sending an alarm packet to "));
      Serial.print(ip_remote);
      Serial.println(F(" for temperature change"));
      current_threshold = get_threshold(cur_temp);
      Udp.beginPacket(ip_remote, remotePort);
      Udp.write("ALARM! Temp threshold changed to ");
      if (current_threshold == 0) Udp.write("major under");
      else if (current_threshold == 1) Udp.write("minor under");
      else if (current_threshold == 2) Udp.write("comfortable");
      else if (current_threshold == 3) Udp.write("minor over");
      else  Udp.write("major over");
      Udp.endPacket();
    }

  }
	if (curr_time - prev_time_dht_long >= dht_read_long_delay || curr_time < prev_time_dht_long) {
    // // Serial.println("debug for loop runs for delayed temperature recording inn eeprom");
    time_and_data.date_time = Clock.read();
    time_and_data.temp = cur_temp;
    time_and_data.humidity = cur_humidity;
    rtc_dht_data_range.store_data(time_and_data);
    prev_time_dht_long = curr_time;
	}
}
