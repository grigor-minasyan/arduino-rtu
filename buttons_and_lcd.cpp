#include "main.h"
#define FIVE_BUTTON_PIN A3
void sw1func() {
  Serial.println("button 1 pressed");
}
void sw2func() {
  Serial.println("button 2 pressed");
}
void sw3func() {
  Serial.println("button 3 pressed");
}
void sw4func() {
  Serial.println("button 4 pressed");
}
void sw5func() {
  Serial.println("button 5 pressed");
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
