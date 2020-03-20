#include "main.h"
SimpleDHT22 dht22(PINDHT22);
//for buffer
int arr[MAX_CMD_COUNT];
char command[MAX_STR+1];

Adafruit_NeoPixel leds_all(NUM_LEDS, RGB_DATA_PIN_ALL, NEO_GRB + NEO_KHZ800);

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
int8_t temp_threshold_1, temp_threshold_2, temp_threshold_3, temp_threshold_4;
//Ethernet declarations-------------------------------------------
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip, subnet, gateway;
IPAddress dns(192, 168, 1, 1);
IPAddress ip_remote(192, 168, 1, 111);   // local port to listen on
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
int udp_packets_in_counter = 0, udp_packets_out_counter = 0;
//can hold 12 bytes, for ip sub gateway

//holds bytes for ip(4 bytes), sub(4bytes), gateway(4 bytes) in this order
Eeprom_indexes<byte> ip_sub_gate_config(0, 17);
Eeprom_indexes<int8_t> thresholds_config(18, 27);

//can hold 4 int8_t for thresholds
//end Ethernet declarations-------------------------------------------


LiquidCrystal lcd(6, 7, 5, 4, 3, 2);
byte curr_lcd_menu = LCD_HOME;



void setup() {
  lcd.begin(16, 2);
  show_lcd_menu(LCD_HOME);

	Ethernet.init(10);
  //getting the ip addresses from the eeprom
  ip=IPAddress(ip_sub_gate_config.get_ith(0),ip_sub_gate_config.get_ith(1),ip_sub_gate_config.get_ith(2),ip_sub_gate_config.get_ith(3));
  subnet=IPAddress(ip_sub_gate_config.get_ith(4),ip_sub_gate_config.get_ith(5),ip_sub_gate_config.get_ith(6),ip_sub_gate_config.get_ith(7));
  gateway=IPAddress(ip_sub_gate_config.get_ith(8),ip_sub_gate_config.get_ith(9),ip_sub_gate_config.get_ith(10),ip_sub_gate_config.get_ith(11));

	Ethernet.begin(mac, ip, dns, gateway, subnet);



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

  //getting the temperature thresholds from the eeprom
  temp_threshold_1 = thresholds_config.get_ith(0);
  temp_threshold_2 = thresholds_config.get_ith(1);
  temp_threshold_3 = thresholds_config.get_ith(2);
  temp_threshold_4 = thresholds_config.get_ith(3);
}

void loop() {
	read_temp_hum_loop();

	five_button_read();
  show_lcd_menu(curr_lcd_menu);

	//calls set flags and execute
	take_input();
	take_input_udp();
}
