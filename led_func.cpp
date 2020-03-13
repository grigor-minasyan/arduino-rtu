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
