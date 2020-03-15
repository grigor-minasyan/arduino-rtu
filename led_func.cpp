#include "main.h"
//blink the LED based on the set delay

bool rgb_toggle = false;
void blink_RGB() {
	static unsigned long prev_time3;
	// Turn the LED on, then pause
	if ((millis() - prev_time3 > blink_delay3)) {
		if (rgb_toggle) {
			leds.setPixelColor(0, color1);
			leds.show();
		}	else {
			leds.setPixelColor(0, color2);
			leds.show();
		}
		rgb_toggle = !rgb_toggle;
		prev_time3 = millis();
	}
}
