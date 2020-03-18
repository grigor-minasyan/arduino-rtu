#include "main.h"
#define FIVE_BUTTON_PIN A3

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
      lcd.print(F("history"));
    } else if (x == LCD_HISTORY_IN) {
      lcd.print(rtc_dht_data_range.get_ith_data(temp_history_ith_element).get_month()); lcd.print("/");
      lcd.print(rtc_dht_data_range.get_ith_data(temp_history_ith_element).get_day()); lcd.print(" ");
      lcd.print(rtc_dht_data_range.get_ith_data(temp_history_ith_element).get_hour()); lcd.print(":");
      lcd.print(rtc_dht_data_range.get_ith_data(temp_history_ith_element).get_minute()); lcd.print(":");
      lcd.print(rtc_dht_data_range.get_ith_data(temp_history_ith_element).get_second());
      lcd.setCursor(0, 1);
      lcd.print(rtc_dht_data_range.get_ith_data(temp_history_ith_element).get_temp()); lcd.print("C (");
      lcd.print(to_farenheit(rtc_dht_data_range.get_ith_data(temp_history_ith_element).get_temp())); lcd.print("F) ");
      lcd.print(rtc_dht_data_range.get_ith_data(temp_history_ith_element).get_hum()); lcd.print("%");
    } else if (x == LCD_PACKETS) {
      lcd.print(F("Rx: "));
      lcd.print(udp_packets_in_counter);
      lcd.setCursor(0, 1);
      lcd.print(F("Tx: "));
      lcd.print(udp_packets_out_counter);
    } else if (x == LCD_SETTINGS_OUT) lcd.print(F("Settings"));
    else if (x == LCD_HISTORY_IN) {
      lcd.print(F("03/03/20 15:15"));
      lcd.setCursor(0, 1);
      lcd.print(F("03/03/20 15:15"));
    } else if (x == LCD_SETTINGS_IP_OUT) lcd.print(F("Change IP"));
    else if (x == LCD_SETTINGS_SUB_OUT) lcd.print(F("Change subnet"));
    else if (x == LCD_SETTINGS_GATE_OUT) lcd.print(F("Change gateway"));
    else if (x == LCD_SETTINGS_THRESHOLD_OUT) lcd.print(F("Thresholds"));
    else if (x == LCD_SETTINGS_ERASE_OUT) lcd.print(F("Erase temp/hum"));

  }
}

void sw1func() {//left
  if (curr_lcd_menu == LCD_HISTORY_IN) show_lcd_menu(LCD_HISTORY_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_IP_OUT ||
    curr_lcd_menu ==  LCD_SETTINGS_SUB_OUT ||
    curr_lcd_menu ==  LCD_SETTINGS_GATE_OUT ||
    curr_lcd_menu == LCD_SETTINGS_THRESHOLD_OUT ||
    curr_lcd_menu == LCD_SETTINGS_ERASE_OUT) show_lcd_menu(LCD_SETTINGS_OUT);
}
void sw2func() {//up
  //------------------------------------------------------
  //main level 1 menu
  if (curr_lcd_menu == LCD_SETTINGS_OUT) show_lcd_menu(LCD_PACKETS);
  else if (curr_lcd_menu == LCD_PACKETS) show_lcd_menu(LCD_HISTORY_OUT);
  else if (curr_lcd_menu == LCD_HISTORY_OUT) show_lcd_menu(LCD_HOME);
  //------------------------------------------------------
  //level 2 settings
  else if (curr_lcd_menu == LCD_SETTINGS_SUB_OUT) show_lcd_menu(LCD_SETTINGS_IP_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_GATE_OUT) show_lcd_menu(LCD_SETTINGS_SUB_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_OUT) show_lcd_menu(LCD_SETTINGS_GATE_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_ERASE_OUT) show_lcd_menu(LCD_SETTINGS_THRESHOLD_OUT);
  //------------------------------------------------------
  //level 3 temp history data
  else if (curr_lcd_menu == LCD_HISTORY_IN) {
    temp_history_ith_element++;
    if (temp_history_ith_element < 0) temp_history_ith_element = 0;
    if (temp_history_ith_element > rtc_dht_data_range.get_stored_data_count()) temp_history_ith_element = rtc_dht_data_range.get_stored_data_count();
    show_lcd_menu(LCD_HISTORY_IN);
  }
  //------------------------------------------------------
}
void sw3func() { // down
  //------------------------------------------------------
  //main level 1 menu
  if (curr_lcd_menu == LCD_HOME) show_lcd_menu(LCD_HISTORY_OUT);
  else if (curr_lcd_menu == LCD_HISTORY_OUT) show_lcd_menu(LCD_PACKETS);
  else if (curr_lcd_menu == LCD_PACKETS) show_lcd_menu(LCD_SETTINGS_OUT);
  //------------------------------------------------------
  //level 2 setting menu
  else if (curr_lcd_menu == LCD_SETTINGS_IP_OUT) show_lcd_menu(LCD_SETTINGS_SUB_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_SUB_OUT) show_lcd_menu(LCD_SETTINGS_GATE_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_GATE_OUT) show_lcd_menu(LCD_SETTINGS_THRESHOLD_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_OUT) show_lcd_menu(LCD_SETTINGS_ERASE_OUT);
  //------------------------------------------------------
  //level 3 temp history data
  else if (curr_lcd_menu == LCD_HISTORY_IN) {
    temp_history_ith_element--;
    if (temp_history_ith_element < 0) temp_history_ith_element = 0;
    show_lcd_menu(LCD_HISTORY_IN);
  }
  //------------------------------------------------------
}
void sw4func() {//right
  if (curr_lcd_menu == LCD_HISTORY_OUT) show_lcd_menu(LCD_HISTORY_IN);
  else if (curr_lcd_menu == LCD_SETTINGS_OUT) show_lcd_menu(LCD_SETTINGS_IP_OUT);
}
void sw5func() {//enter
  if (curr_lcd_menu == LCD_HISTORY_OUT) show_lcd_menu(LCD_HISTORY_IN);
  else if (curr_lcd_menu == LCD_SETTINGS_OUT) show_lcd_menu(LCD_SETTINGS_IP_OUT);
}

void five_button_read() {
  static unsigned long prev_time = 0;
  static int val;
  static bool is_released;
  if (millis() - prev_time > 10) {
    prev_time = millis();
    val = analogRead(FIVE_BUTTON_PIN);
    if (val > 1000) {
      is_released = true;
    } else if (val > 700 && is_released) {
      is_released = false;
      sw5func();
    } else if (val > 450 && is_released) {
      is_released = false;
      sw4func();
    } else if (val > 300 && is_released) {
      is_released = false;
      sw3func();
    } else if (val > 100 && is_released) {
      is_released = false;
      sw2func();
    } else if (is_released) {
      is_released = false;
      sw1func();
    }
  }
}
