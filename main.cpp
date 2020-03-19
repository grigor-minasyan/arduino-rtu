#include "main.h"
SimpleDHT22 dht22(PINDHT22);
//for buffer
int arr[MAX_CMD_COUNT];
char command[MAX_STR+1];

Adafruit_NeoPixel leds_all(NUM_LEDS, RGB_DATA_PIN_LINK, NEO_GRB + NEO_KHZ800);

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
int8_t temp_threshold_1 = 16, temp_threshold_2 = 21, temp_threshold_3 = 27, temp_threshold_4 = 32;
//Ethernet declarations-------------------------------------------
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
IPAddress ip_remote(192, 168, 1, 111);   // local port to listen on
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
int udp_packets_in_counter = 0, udp_packets_out_counter = 0;
//can hold 12 bytes, for ip sub gateway
Eeprom_indexes<byte> ip_sub_gate(0, 18);

//can hold 4 int8_t for thresholds
Eeprom_indexes<int8_t> temp_thresholds(20, 30);
//end Ethernet declarations-------------------------------------------


LiquidCrystal lcd(9, 7, 5, 4, 3, 2);
byte curr_lcd_menu = LCD_HOME;



void setup() {
  lcd.begin(16, 2);
  show_lcd_menu(LCD_HOME);

	Ethernet.init(10);
	Ethernet.begin(mac, ip);



	Serial.begin(BAUD_RATE);
	 // wait for serial port to connect. Needed for native USB port only
	while (!Serial) {;}
	// Check for Ethernet hardware present
  Udp.begin(localPort);


	Clock.begin();

  leds_all.begin();
	leds_all.setPixelColor(0, color_comfortable);
	leds_all.show();
	command[0] = '\0';

	pinMode(13, OUTPUT);

  Serial.println(F("Enter commands or 'HELP'"));

  for (int i = 0; i < 12; i++) ip_sub_gate.set_ith_data(i, 2);
  for (int i = 0; i < 4; i++) temp_thresholds.set_ith_data(i, 2);
  Serial.println(F("-------------"));
  for (int i = 0; i < 12; i++) {
    Serial.print(ip_sub_gate.get_ith_data_from_back(i));
    Serial.print(" ");
  }
  Serial.println(F("\n\r-------------"));

  for (int i = 0; i < 4; i++) {
    Serial.print(temp_thresholds.get_ith_data_from_back(i));
    Serial.print(" ");
  }
  Serial.println(F("\n\r-------------"));

}

void loop() {
	read_temp_hum_loop();
	five_button_read();
  show_lcd_menu(curr_lcd_menu);

	//calls set flags and execute
	take_input();
	take_input_udp();
}
