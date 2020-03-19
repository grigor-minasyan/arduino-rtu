#include "main.h"
int8_t to_farenheit(int8_t x)  {
  return x * 1.8 + 32;
}

//this keeps track of the data and does the storing in EEPROM

Eeprom_indexes<Data_To_Store> rtc_dht_data_range(17, EEPROM.length()-1);
// Eeprom_indexes rtc_dht_data_range(17, 60); // for testing purposes


//x is the number of data to print
void print_EEPROM_data(int x, int8_t is_udp) {
  rtc_dht_data_range.print_data(x, is_udp);
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
    Serial.print(F("Read DHT22 failed, err="));
		Serial.println(err);
  }
  if (cur_temp > 127) cur_temp -= 256;
  if (cur_humidity > 127) cur_humidity -= 256;
}

void read_temp_hum_loop() {
  static unsigned long prev_time_dht_short, prev_time_dht_long;
	if (millis() - prev_time_dht_short >= dht_read_short_delay) {
    prev_time_dht_short = millis();
		read_temp_hum();
        //setting the max and min values
    if (cur_temp <  min_temp) min_temp = cur_temp;
    if (cur_temp > max_temp) max_temp = cur_temp;
    if (cur_humidity <  min_humidity) min_humidity = cur_humidity;
    if (cur_humidity > max_humidity) max_humidity = cur_humidity;

    //check if threshold changed
    if (current_threshold != get_threshold(cur_temp)) {
      Serial.print(F("\n\rSending an alarm packet to "));
      Serial.print(ip_remote);
      Serial.println(F(" for temperature change"));
      current_threshold = get_threshold(cur_temp);
      Udp.beginPacket(ip_remote, remotePort);
      Udp.write("ALARM! Temp changed to ");
      if (current_threshold == 0) {
        leds_all.setPixelColor(0, color_maj_und);
        Udp.write("major under");
      } else if (current_threshold == 1) {
        leds_all.setPixelColor(0, color_min_und);
        Udp.write("minor under");
      } else if (current_threshold == 2) {
        leds_all.setPixelColor(0, color_comfortable);
        Udp.write("comfortable");
      } else if (current_threshold == 3) {
        leds_all.setPixelColor(0, color_min_ovr);
        Udp.write("minor over");
      } else {
        leds_all.setPixelColor(0, color_maj_ovr);
        Udp.write("major over");
      }

      Udp.write(", ");
      char buff[5];
      itoa(cur_temp, buff, 10);
      Udp.write(buff);
      Udp.write("C (");
      itoa(to_farenheit(cur_temp), buff, 10);
      Udp.write(buff);
      Udp.write("F)");

      leds_all.show();
      Udp.endPacket();
  		udp_packets_out_counter++;
    }

  }
	if (((millis() - prev_time_dht_long) >= dht_read_long_delay) || millis() < prev_time_dht_long) {


    Data_To_Store time_and_data;
    time_and_data.set_year((byte)Clock.read().Year);
    time_and_data.set_month((byte)Clock.read().Month);
    time_and_data.set_day((byte)Clock.read().Day);
    time_and_data.set_hour((byte)Clock.read().Hour);
    time_and_data.set_minute((byte)Clock.read().Minute);
    time_and_data.set_second((byte)Clock.read().Second);

    time_and_data.set_hum(cur_humidity);
    time_and_data.set_temp(cur_temp);


    rtc_dht_data_range.store_data(time_and_data);
    prev_time_dht_long = millis();
	}
}
