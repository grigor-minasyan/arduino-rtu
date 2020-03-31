#include "main.h"

#define MJ_UNDER 0b1100
#define MN_UNDER 0b0100
#define MN_OVER 0b0010
#define MJ_OVER 0b0011
#define COMFORTABLE 0

int8_t to_farenheit(int8_t x)  {
  return x * 1.8 + 32;
}

//this keeps track of the data and does the storing in EEPROM
Eeprom_indexes<Data_To_Store> rtc_dht_data_range(28, EEPROM.length()-1);
// Eeprom_indexes rtc_dht_data_range(21, 60); // for testing purposes



int8_t current_threshold = 0;
byte temp_alarm_binary = 0b0000;

int8_t get_threshold(int8_t t) {
  if (t < temp_threshold__arr[0]) return MJ_UNDER;//mj under
  if (t < temp_threshold__arr[1]) return MN_UNDER;//mn under
  if (t > temp_threshold__arr[2]) return MN_OVER;//mn over
  if (t > temp_threshold__arr[3]) return MJ_OVER;//mj over
  return COMFORTABLE;//no alarms
}

//not to be called from outside, only by the looper, limited to 0.5hz maximum
void read_temp_hum() {
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read(&cur_temp, &cur_humidity, NULL)) != SimpleDHTErrSuccess) {
    if constexpr (SERIAL_ENABLE) {
      Serial.print(F("Read DHT22 failed, err="));
    	Serial.println(err);
    }
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

    temp_alarm_binary == get_threshold(cur_temp);

    //check if threshold changed
    if (current_threshold != get_threshold(cur_temp)) {
      if constexpr (SERIAL_ENABLE) {
        Serial.print(F("\n\rSending an alarm packet to "));
        Serial.print(ip_remote);
        Serial.println(F(" for temperature change"));
      }
      current_threshold = get_threshold(cur_temp);
      if constexpr (UDP_OLD_ENABLE) {
        Udp.beginPacket(ip_remote, remotePort);
        Udp.write("ALARM! Temp changed to ");
        if (current_threshold == MJ_UNDER) Udp.write("major under");
        else if (current_threshold == MN_UNDER) Udp.write("minor under");
        else if (current_threshold == COMFORTABLE) Udp.write("comfortable");
        else if (current_threshold == MN_OVER) Udp.write("minor over");
        else Udp.write("major over");

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
      if (current_threshold == MJ_UNDER) leds_all.setPixelColor(0, color_maj_und);
      else if (current_threshold == MN_UNDER) leds_all.setPixelColor(0, color_min_und);
      else if (current_threshold == COMFORTABLE) leds_all.setPixelColor(0, color_comfortable);
      else if (current_threshold == MN_OVER) leds_all.setPixelColor(0, color_min_ovr);
      else leds_all.setPixelColor(1, color_maj_ovr);
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
