#include "main.h"
#define FIVE_BUTTON_PIN A3

extern byte temp_history_ith_element;
extern int8_t temporaryint8_t1, temporaryint8_t2, temporaryint8_t3, temporaryint8_t4;
extern byte temporarybyte1, temporarybyte2, temporarybyte3, temporarybyte4;
extern byte cursor_loc;


void sw1func() {//left
  if (curr_lcd_menu == LCD_HISTORY_IN) show_lcd_menu(LCD_HISTORY_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_IP_OUT ||
            curr_lcd_menu ==  LCD_SETTINGS_SUB_OUT ||
            curr_lcd_menu ==  LCD_SETTINGS_GATE_OUT ||
            curr_lcd_menu == LCD_SETTINGS_THRESHOLD_OUT ||
            curr_lcd_menu == LCD_SETTINGS_ERASE_OUT) show_lcd_menu(LCD_SETTINGS_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_ERASE_IN) show_lcd_menu(LCD_SETTINGS_ERASE_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN) {
    if (cursor_loc > 1) cursor_loc--;
  };
}

void sw4func() {//right
  if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN) {
    if (cursor_loc < 4) cursor_loc++;
  };
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
    if (temp_history_ith_element < rtc_dht_data_range.get_stored_data_count()-1) temp_history_ith_element++;
    show_lcd_menu(LCD_HISTORY_IN);
  }
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN) {
    if (cursor_loc == 1 && temporaryint8_t1 < temporaryint8_t2-1) temporaryint8_t1++;
    else if (cursor_loc == 2 && temporaryint8_t2 < temporaryint8_t3-1) temporaryint8_t2++;
    else if (cursor_loc == 3 && temporaryint8_t3 < temporaryint8_t4-1) temporaryint8_t3++;
    else if (cursor_loc == 4 && temporaryint8_t4 < 127) temporaryint8_t4++;
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
    if (temp_history_ith_element > 0) temp_history_ith_element--;
    show_lcd_menu(LCD_HISTORY_IN);
  }
  else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN) {
    if (cursor_loc == 1 && temporaryint8_t1 > -128) temporaryint8_t1--;
    else if (cursor_loc == 2 && temporaryint8_t2 > temporaryint8_t1+1) temporaryint8_t2--;
    else if (cursor_loc == 3 && temporaryint8_t3 > temporaryint8_t2+1) temporaryint8_t3--;
    else if (cursor_loc == 4 && temporaryint8_t4 > temporaryint8_t3+1) temporaryint8_t4--;
  }
  //------------------------------------------------------
}
void sw5func() {//enter
  if (curr_lcd_menu == LCD_HISTORY_OUT) show_lcd_menu(LCD_HISTORY_IN);
  else if (curr_lcd_menu == LCD_SETTINGS_OUT) show_lcd_menu(LCD_SETTINGS_IP_OUT);
  else if (curr_lcd_menu == LCD_SETTINGS_ERASE_OUT) show_lcd_menu(LCD_SETTINGS_ERASE_IN);
  else if (curr_lcd_menu == LCD_SETTINGS_ERASE_IN) {
    rtc_dht_data_range.init();
    show_lcd_menu(LCD_SETTINGS_ERASE_OUT);
  } else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_OUT) {
    temporaryint8_t1 = temp_threshold_1;
    temporaryint8_t2 = temp_threshold_2;
    temporaryint8_t3 = temp_threshold_3;
    temporaryint8_t4 = temp_threshold_4;
    cursor_loc = 1;
    show_lcd_menu(LCD_SETTINGS_THRESHOLD_IN);
  } else if (curr_lcd_menu == LCD_SETTINGS_THRESHOLD_IN) {
    temp_threshold_1 = temporaryint8_t1;
    temp_threshold_2 = temporaryint8_t2;
    temp_threshold_3 = temporaryint8_t3;
    temp_threshold_4 = temporaryint8_t4;
    thresholds_config.set_ith(0, temp_threshold_1);
    thresholds_config.set_ith(1, temp_threshold_2);
    thresholds_config.set_ith(2, temp_threshold_3);
    thresholds_config.set_ith(3, temp_threshold_4);
    cursor_loc = 1;
    show_lcd_menu(LCD_SETTINGS_THRESHOLD_OUT);
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
