#include "main.h"

int8_t temporaryint8_t1, temporaryint8_t2, temporaryint8_t3, temporaryint8_t4;
byte temporarybyte1, temporarybyte2, temporarybyte3, temporarybyte4;
byte cursor_loc = 1;

byte temp_history_ith_element;
void show_lcd_menu(byte x) {
  //fixme does not update the lcd
  static unsigned long prev_time;
  if (millis() - prev_time >= 500 || curr_lcd_menu != x) {
    prev_time = millis();
    curr_lcd_menu = x;
    lcd.clear();
    lcd.setCursor(0, 0);
    if (x == LCD_HOME) {
      lcd.print(F("Temp:"));
      lcd.print(cur_temp);
      lcd.print(F("C("));
      lcd.print(to_farenheit(cur_temp));
      lcd.print(F("F)"));
      lcd.setCursor(0, 1);
      lcd.print(F("Humidity:"));
      lcd.print(cur_humidity);
      lcd.print(F("%"));
    } else if (x == LCD_HISTORY_OUT) {
      lcd.print(F("Temp humidity"));
      lcd.setCursor(0, 1);
      lcd.print(F("history ("));
      lcd.print(rtc_dht_data_range.get_stored_data_count());
      lcd.print(F(")"));
    } else if (x == LCD_HISTORY_IN) {
      lcd.print(rtc_dht_data_range.get_ith_data_from_curr(temp_history_ith_element).get_month()); lcd.print("/");
      lcd.print(rtc_dht_data_range.get_ith_data_from_curr(temp_history_ith_element).get_day()); lcd.print(" ");
      lcd.print(rtc_dht_data_range.get_ith_data_from_curr(temp_history_ith_element).get_hour()); lcd.print(":");
      lcd.print(rtc_dht_data_range.get_ith_data_from_curr(temp_history_ith_element).get_minute()); lcd.print(":");
      lcd.print(rtc_dht_data_range.get_ith_data_from_curr(temp_history_ith_element).get_second());
      lcd.setCursor(0, 1);
      lcd.print(rtc_dht_data_range.get_ith_data_from_curr(temp_history_ith_element).get_temp()); lcd.print("C (");
      lcd.print(to_farenheit(rtc_dht_data_range.get_ith_data_from_curr(temp_history_ith_element).get_temp())); lcd.print("F) ");
      lcd.print(rtc_dht_data_range.get_ith_data_from_curr(temp_history_ith_element).get_hum()); lcd.print("%");
    } else if (x == LCD_PACKETS) {
      lcd.print(F("Rx: "));
      lcd.print(udp_packets_in_counter);
      lcd.setCursor(0, 1);
      lcd.print(F("Tx: "));
      lcd.print(udp_packets_out_counter);
    } else if (x == LCD_SETTINGS_OUT) lcd.print(F("Settings"));
    else if (x == LCD_SETTINGS_IP_OUT) lcd.print(F("Change IP"));
    else if (x == LCD_SETTINGS_SUB_OUT) lcd.print(F("Change subnet"));
    else if (x == LCD_SETTINGS_GATE_OUT) lcd.print(F("Change gateway"));
    else if (x == LCD_SETTINGS_THRESHOLD_OUT) lcd.print(F("Thresholds"));
    else if (x == LCD_SETTINGS_ERASE_OUT) lcd.print(F("Erase temp/hum"));
    else if (x == LCD_SETTINGS_ERASE_IN) {
      lcd.print(F("Erase? back(no)"));
      lcd.setCursor(0, 1);
      lcd.print(F("enter(yes)"));
    } else if (x == LCD_SETTINGS_THRESHOLD_IN) {
      lcd.print(F("Enter to save"));
      lcd.setCursor(0, 1);
      if (cursor_loc == 1) {
        lcd.print(">");
        lcd.print(temporaryint8_t1);
        lcd.print(" ");
        lcd.print(temporaryint8_t2);
        lcd.print(" ");
        lcd.print(temporaryint8_t3);
        lcd.print(" ");
        lcd.print(temporaryint8_t4);
      } else if (cursor_loc == 2) {
        lcd.print(temporaryint8_t1);
        lcd.print(" ");
        lcd.print(">");
        lcd.print(temporaryint8_t2);
        lcd.print(" ");
        lcd.print(temporaryint8_t3);
        lcd.print(" ");
        lcd.print(temporaryint8_t4);
      } else if (cursor_loc == 3) {
        lcd.print(temporaryint8_t1);
        lcd.print(" ");
        lcd.print(temporaryint8_t2);
        lcd.print(" ");
        lcd.print(">");
        lcd.print(temporaryint8_t3);
        lcd.print(" ");
        lcd.print(temporaryint8_t4);
      } else if (cursor_loc == 4) {
        lcd.print(temporaryint8_t1);
        lcd.print(" ");
        lcd.print(temporaryint8_t2);
        lcd.print(" ");
        lcd.print(temporaryint8_t3);
        lcd.print(" ");
        lcd.print(">");
        lcd.print(temporaryint8_t4);
      }
    }
  }
}
