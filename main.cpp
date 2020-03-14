#include "main.h"
SimpleDHT22 dht22(PINDHT22);
//for buffer
int arr[MAX_CMD_COUNT];
char command[MAX_STR+1];

Adafruit_NeoPixel leds(NUM_LEDS, RGB_DATA_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leds_temp(NUM_LEDS, RGB_DATA_PIN_TEMP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leds_link(NUM_LEDS, RGB_DATA_PIN_LINK, NEO_GRB + NEO_KHZ800);

uint32_t color1 = 0x000032;
uint32_t color2 = 0x000000;
uint32_t color_maj_und = 0x800080;//purple
uint32_t color_min_und = 0x000080;//blue
uint32_t color_comfortable = 0x008000;//green
uint32_t color_min_ovr = 0xc87800;//orange
uint32_t color_maj_ovr = 0xc80000;//red
//input processing variables
byte command_size = 0;
byte command_count = 0;
char inByte = 0;

//timekeeping variables
DS3231_Simple Clock;
unsigned int curr_time = 0, prev_time3 = 0, prev_time_udp = 0, prev_time_udp_checker = 0, prev_time_dht_short = 0, prev_time_dht_long = 0;
bool link_status = false;

unsigned int  blink_delay3 = 500;

//keeping current temp and humidity in global
int8_t cur_temp = 0;
int8_t cur_humidity = 0;
int8_t max_temp = INT8_MIN;
int8_t min_temp = INT8_MAX;
int8_t max_humidity = INT8_MIN;
int8_t min_humidity = INT8_MAX;
int8_t current_threshold = 2;

// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
IPAddress ip_remote(192, 168, 1, 111);   // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;


void setup() {
	Ethernet.init(10);
	Ethernet.begin(mac, ip);

	Serial.begin(BAUD_RATE);
	 // wait for serial port to connect. Needed for native USB port only
	while (!Serial) {;}
	// Check for Ethernet hardware present
  Udp.begin(localPort);


	Clock.begin();

	leds.begin();
	leds_temp.begin();
	leds_link.begin();
	leds.setPixelColor(0, color1);
	leds.show();
	leds_temp.setPixelColor(0, color_comfortable);
	leds_temp.show();
	command[0] = '\0';

	pinMode(13, OUTPUT);

	//power on led
	pinMode(DUAL_LED_PIN1, OUTPUT);
	pinMode(DUAL_LED_PIN2, OUTPUT);
	digitalWrite(DUAL_LED_PIN1, HIGH);
	digitalWrite(DUAL_LED_PIN2, LOW);

	Serial.println(F("Enter commands or 'HELP'"));
}

void loop() {
	curr_time = millis();
	read_temp_hum_loop();

	//blink the LEDs, the functions account for the delay
	blink_RGB();

	//calls set flags and execute
	take_input();
	take_input_udp();
}
