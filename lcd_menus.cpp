#include "main.h"
#define LCD_TEMP_SHOW_DELAY 2000

byte temp_bytes[4];
int8_t temp_int8_t[4];
byte cursor_loc = 0;
byte temp_device_id;

void show_wrong_bch_lcd(byte wrong, byte correct) {
  unsigned long prev_time = millis();
  byte prev_menu = curr_lcd_menu;
  curr_lcd_menu = -1;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Bad checksum"));
  lcd.setCursor(0, 1);
  lcd.print(F("EXP:"));
  lcd.print(correct, HEX);
  lcd.print(F(" GOT:"));
  lcd.print(wrong, HEX);
  while (millis() - prev_time < LCD_TEMP_SHOW_DELAY) {
    continue;
  }
  show_lcd_menu(prev_menu);
}

void show_saved_lcd(byte is_reboot) {
  unsigned long prev_time = millis();
  byte prev_menu = curr_lcd_menu;
  curr_lcd_menu = -1;
  lcd.clear();
  lcd.setCursor(0, 0);
  if (is_reboot) lcd.print(F("Reboot to save"));
  else lcd.print(F("Values saved"));
  while (millis() - prev_time < LCD_TEMP_SHOW_DELAY) {;}
  show_lcd_menu(prev_menu);
}

int temp_history_ith_element;
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
    else if (x == LCD_SETTINGS_THRESHOLD_OUT) lcd.print(F("Thresholds(in C)"));
    else if (x == LCD_SETTINGS_ERASE_OUT) {
      lcd.print(F("Erase temp/hum"));
      lcd.setCursor(0, 1);
      lcd.print(F("history"));
    }
    else if (x == LCD_SETTINGS_ERASE_IN) {
      lcd.print(F("Erase? back(no)"));
      lcd.setCursor(0, 1);
      lcd.print(F("enter(yes)"));
    } else if (x == LCD_SETTINGS_THRESHOLD_IN || x == LCD_SETTINGS_IP_IN || x == LCD_SETTINGS_SUB_IN || x == LCD_SETTINGS_GATE_IN) {
      lcd.print(F("Enter to save"));
      lcd.setCursor(0, 1);
      for (byte i = 0; i < 4; i++) {
        lcd.print((cursor_loc == i ? ">" : " "));
        lcd.print((x == LCD_SETTINGS_THRESHOLD_IN ? temp_int8_t[i] : temp_bytes[i]));
      }
    } else if (x == LCD_SETTINGS_ID_OUT) lcd.print(F("Change DCP ID"));
    else if (x == LCD_SETTINGS_ID_IN) {
      lcd.print(F("Use up/down"));
      lcd.setCursor(0, 1);
      lcd.print(F("to change: "));
      lcd.print(temp_device_id);
    }
  }
}
