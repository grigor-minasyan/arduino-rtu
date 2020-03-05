#include "global_var.h"
//changes the color of the LED, x as the setting for the color
void change_dual_led(byte x) {
	//0 is off
	if (x == 0) {
		current_color = 0;
		digitalWrite(DUAL_LED_PIN1, LOW);
		digitalWrite(DUAL_LED_PIN2, LOW);
	} else if (x == M_GREEN) {
		current_color = blink_color = M_GREEN; // for green
		digitalWrite(DUAL_LED_PIN1, HIGH);
		digitalWrite(DUAL_LED_PIN2, LOW);
	} else {
		current_color = blink_color = M_RED; // for red
		digitalWrite(DUAL_LED_PIN1, LOW);
		digitalWrite(DUAL_LED_PIN2, HIGH);
	}
}

//blink the LED based on the set delay
void blink_LED() {
  if ((curr_time - prev_time2 >= blink_delay)) {
		if (dual_blink) {
	    if (blink_color == M_RED) change_dual_led(M_GREEN);
	    else if (blink_color == M_GREEN) change_dual_led(M_RED);
	    prev_time2 = curr_time;
		} else {
	    if (current_color == blink_color) change_dual_led(0);
	    else if (current_color == 0) change_dual_led(blink_color);
	    prev_time2 = curr_time;
		}
  }
}

//blink the d13 led based on the delay set
void blink_d13() {
  if (curr_time - prev_time1 >= blink_delay) {
    if (digitalRead(13) == LOW) digitalWrite(13, HIGH);
    else if (digitalRead(13) == HIGH) digitalWrite(13, LOW);
    prev_time1 = curr_time;
  }
}
