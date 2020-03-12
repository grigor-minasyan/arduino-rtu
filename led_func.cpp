#include "main.h"
//blink the LED based on the set delay

bool rgb_toggle = false;
void blink_RGB() {
	// Turn the LED on, then pause
	if ((curr_time - prev_time3 > blink_delay3)) {
		if (rgb_toggle) {
			leds[0] = color1;
			FastLED.show();
		}	else {
			leds[0] = color2;
			FastLED.show();
		}
		rgb_toggle = !rgb_toggle;
		prev_time3 = curr_time;
	}
}

void blink_LED(byte &x) {
  if ((curr_time - prev_time2 > blink_delay2)) {//fix the delay
		x = (x << 2) | (x >> 6);
		prev_time2 = curr_time;
		if (x&1) digitalWrite(DUAL_LED_PIN2, HIGH);
		else digitalWrite(DUAL_LED_PIN2, LOW);
		if (x&2) digitalWrite(DUAL_LED_PIN1, HIGH);
		else digitalWrite(DUAL_LED_PIN1, LOW);
  }
}
