#include "main.h"
#define FIVE_BUTTON_PIN A1

extern int temp_history_ith_element;
extern int8_t temp_int8_t[4];
extern byte temp_bytes[4];
extern byte cursor_loc;
extern byte temp_device_id;

void sw1func();
void sw2func();
void sw3func();
void sw4func();
void sw5func();

void sw4func() {//left
  if (curr_lcd_menu == LCD_HISTORY_IN) show_lcd_menu(LCD_HISTORY_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_IP_OUT || curr_lcd_menu ==  LCD_SETTINGS_SUB_OUT ||
            curr_lcd_menu ==  LCD_SETTINGS_GATE_OUT || curr_lcd_menu == LCD_SETTINGS_THRESHOLD_OUT ||
            curr_lcd_menu == LCD_SETTINGS_ERASE_OUT || curr_lcd_menu == LCD_SETTINGS_ID_OUT) show_lcd_menu(LCD_SETTINGS_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_ERASE_IN) show_lcd_menu(LCD_SETTINGS_ERASE_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN || curr_lcd_menu == LCD_SETTINGS_IP_IN || curr_lcd_menu == LCD_SETTINGS_SUB_IN || curr_lcd_menu == LCD_SETTINGS_GATE_IN) {
    if (cursor_loc > 0) cursor_loc--;
  }
}

void sw1func() {//right
  if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN || curr_lcd_menu == LCD_SETTINGS_IP_IN || curr_lcd_menu == LCD_SETTINGS_SUB_IN || curr_lcd_menu == LCD_SETTINGS_GATE_IN) {
    if (cursor_loc < 3) cursor_loc++;
  } else sw5func(); // act like the enter key
}

void sw3func() {//up
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
  else if (curr_lcd_menu == LCD_SETTINGS_ID_OUT) show_lcd_menu(LCD_SETTINGS_ERASE_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_ID_IN) temp_device_id++;
  //------------------------------------------------------
  //level 3 temp history data
  else if (curr_lcd_menu == LCD_HISTORY_IN) {
    if (temp_history_ith_element > 0) temp_history_ith_element--;
    show_lcd_menu(LCD_HISTORY_IN);
  }
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN && cursor_loc >= 0 && cursor_loc <= 3) {
    if (cursor_loc == 3 && temp_int8_t[3] < 127) temp_int8_t[3]++;
    else if (temp_int8_t[cursor_loc] < temp_int8_t[cursor_loc+1]-1) temp_int8_t[cursor_loc]++;
  } else if ((curr_lcd_menu == LCD_SETTINGS_IP_IN || curr_lcd_menu == LCD_SETTINGS_SUB_IN || curr_lcd_menu == LCD_SETTINGS_GATE_IN) && cursor_loc >= 0 && cursor_loc <= 3) {
    if (temp_bytes[cursor_loc] < 255) temp_bytes[cursor_loc]++;
  }
  //------------------------------------------------------
}
void sw2func() { // down
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
  else if (curr_lcd_menu == LCD_SETTINGS_ERASE_OUT) show_lcd_menu(LCD_SETTINGS_ID_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_ID_IN) temp_device_id--;
  //------------------------------------------------------
  //level 3 temp history data
  else if (curr_lcd_menu == LCD_HISTORY_IN) {
    if (temp_history_ith_element < rtc_dht_data_range.get_stored_data_count()-1) temp_history_ith_element++;
    show_lcd_menu(LCD_HISTORY_IN);
  }
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN && cursor_loc >= 0 && cursor_loc <= 3) {
    if (cursor_loc == 0 && temp_int8_t[0] > -128) temp_int8_t[0]--;
    else if (temp_int8_t[cursor_loc] > temp_int8_t[cursor_loc-1]+1) temp_int8_t[cursor_loc]--;
  } else if ((curr_lcd_menu == LCD_SETTINGS_IP_IN || curr_lcd_menu == LCD_SETTINGS_SUB_IN || curr_lcd_menu == LCD_SETTINGS_GATE_IN) && cursor_loc >= 0 && cursor_loc <= 3) {
    if (temp_bytes[cursor_loc] > 0) temp_bytes[cursor_loc]--;
  }
  //------------------------------------------------------
}
void sw5func() {//enter
  if (curr_lcd_menu == LCD_HISTORY_OUT) show_lcd_menu(LCD_HISTORY_IN);
  else if (curr_lcd_menu == LCD_SETTINGS_OUT) show_lcd_menu(LCD_SETTINGS_IP_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_ERASE_OUT) show_lcd_menu(LCD_SETTINGS_ERASE_IN);
  else if (curr_lcd_menu == LCD_SETTINGS_ERASE_IN) {
    rtc_dht_data_range.init();
    show_saved_lcd(0);
    show_lcd_menu(LCD_SETTINGS_ERASE_OUT);
  } //enter into threshold settings
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_OUT) {
    for (byte i = 0; i < 4; i++) temp_int8_t[i] = temp_threshold__arr[i];
    cursor_loc = 0;
    show_lcd_menu(LCD_SETTINGS_THRESHOLD_IN);
  } //leaving threshold settings
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN) {
    for (byte i = 0; i < 4; i++) {
      temp_threshold__arr[i] = temp_int8_t[i];
      thresholds_config.set_ith(i, temp_threshold__arr[i]);
    }
    cursor_loc = 0;
    show_saved_lcd(0);
    show_lcd_menu(LCD_SETTINGS_THRESHOLD_OUT);
  } // entering into ID Settings
  else if (curr_lcd_menu == LCD_SETTINGS_ID_OUT) {
    temp_device_id = RTU_DEVICE_ID;
    show_lcd_menu(LCD_SETTINGS_ID_IN);
  }//leading the id settings menu
  else if (curr_lcd_menu == LCD_SETTINGS_ID_IN) {
    RTU_DEVICE_ID = temp_device_id;
    thresholds_config.set_ith(4, RTU_DEVICE_ID);
    show_saved_lcd(0);
    show_lcd_menu(LCD_SETTINGS_ID_OUT);
  }//entering ip subnet or gateway
  else if (curr_lcd_menu == LCD_SETTINGS_IP_OUT) {
    for (byte i = 0; i < 4; i++) temp_bytes[i] = ip[i];
    cursor_loc = 0;
    show_lcd_menu(LCD_SETTINGS_IP_IN);
  } else if (curr_lcd_menu == LCD_SETTINGS_SUB_OUT) {
    for (byte i = 0; i < 4; i++) temp_bytes[i] = subnet[i];
    cursor_loc = 0;
    show_lcd_menu(LCD_SETTINGS_SUB_IN);
  } else if (curr_lcd_menu == LCD_SETTINGS_GATE_OUT) {
    for (byte i = 0; i < 4; i++) temp_bytes[i] = gateway[i];
    cursor_loc = 0;
    show_lcd_menu(LCD_SETTINGS_GATE_IN);
  }
  //saving the ip subnet or gateway
   else if (curr_lcd_menu == LCD_SETTINGS_IP_IN) {
    for (byte i = 0; i < 4; i++) ip_sub_gate_config.set_ith(i, temp_bytes[i]);
    ip = IPAddress(temp_bytes[0], temp_bytes[1], temp_bytes[2], temp_bytes[3]);
    cursor_loc = 0;
    show_saved_lcd(1);
    show_lcd_menu(LCD_SETTINGS_IP_OUT);
  } else if (curr_lcd_menu == LCD_SETTINGS_SUB_IN) {
    for (byte i = 0; i < 4; i++) ip_sub_gate_config.set_ith(i+4, temp_bytes[i]);
    subnet = IPAddress(temp_bytes[0], temp_bytes[1], temp_bytes[2], temp_bytes[3]);
    cursor_loc = 0;
    show_saved_lcd(1);
    show_lcd_menu(LCD_SETTINGS_SUB_OUT);
  } else if (curr_lcd_menu == LCD_SETTINGS_GATE_IN) {
    for (byte i = 0; i < 4; i++) ip_sub_gate_config.set_ith(i+8, temp_bytes[i]);
    gateway = IPAddress(temp_bytes[0], temp_bytes[1], temp_bytes[2], temp_bytes[3]);
    cursor_loc = 0;
    show_saved_lcd(1);
    show_lcd_menu(LCD_SETTINGS_GATE_OUT);
  }
}

void five_button_read() {
  static unsigned long prev_time = 0;
  static unsigned long prev_time_2 = 0;
  const int hold_timeout_1 = 100;
  const int hold_timeout_2 = 400;
  static int val;
  static bool is_released;
  if ((millis() - prev_time > 20 && is_released) || millis() - prev_time > hold_timeout_1) {
    prev_time = millis();
    val = analogRead(FIVE_BUTTON_PIN);
    if (val > 1000) {
      is_released = true;
      prev_time_2 = millis();
    } else if(is_released || millis() - prev_time_2 > hold_timeout_2) {
      if (val > 700 && val < 760) {
        is_released = false;
        sw5func();
      } else if (val > 470 && val < 530) {
        is_released = false;
        sw4func();
      } else if (val > 310 && val < 350) {
        is_released = false;
        sw3func();
      } else if (val > 130 && val < 160) {
        is_released = false;
        sw2func();
      } else if (val < 30) {
        is_released = false;
        sw1func();
      }
    }
  }
}
