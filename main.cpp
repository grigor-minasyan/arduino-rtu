#include "main.h"

//humidity temperature sensor instance declaration.
SimpleDHT22 dht22(PINDHT22);

//for command buffer
int arr[MAX_CMD_COUNT];
char command[MAX_STR+1];

//rgb leds stip declaration
Adafruit_NeoPixel leds_all(NUM_LEDS, RGB_DATA_PIN_ALL, NEO_GRB + NEO_KHZ800);

//colors for temperature alarms.
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
int8_t temp_threshold__arr[4];

//Ethernet declarations-------------------------------------------
// The IP address will be dependent on your local network:
byte mac[] = {0xBB, 0xAA, 0xAA, 0xEF, 0xFE, 0xED}; // a mac address will likely be unique lol, better to have a random one.
IPAddress ip, subnet, gateway;
IPAddress dns(192, 168, 1, 1);
IPAddress ip_remote(192, 168, 1, 101);   // local port to listen on
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
long udp_packets_in_counter = 0, udp_packets_out_counter = 0;
//can hold 12 bytes, for ip sub gateway

//holds bytes for ip(4 bytes), sub(4bytes), gateway(4 bytes) in this order
Eeprom_indexes<byte> ip_sub_gate_config(0, 17);

//can hold 4 int8_t for thresholds
Eeprom_indexes<int8_t> thresholds_config(18, 28);

//stores the history of the recorded data.
Eeprom_indexes<Data_To_Store> rtc_dht_data_range(29, EEPROM.length()-1);
// Eeprom_indexes rtc_dht_data_range(21, 60); // for testing purposes to see how overfow is handled.

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
byte curr_lcd_menu = LCD_HOME;

void setup() {
  if constexpr (SERIAL_ENABLE || SERIAL_DEBUG_ENABLE) {
  	Serial.begin(BAUD_RATE);
  	while (!Serial) {;}// wait for serial port to connect. Needed for native USB port only
  }

  /*
  //setting up default addresses, thresholds and initializing the history, not needed after the LCD.
  ip_sub_gate_config.init();
  thresholds_config.init();
  rtc_dht_data_range.init(); // do this to reset for the first time
  ip_sub_gate_config.set_ith(0, 192);
  ip_sub_gate_config.set_ith(1, 168);
  ip_sub_gate_config.set_ith(2, 1);
  ip_sub_gate_config.set_ith(3, 102);
  ip_sub_gate_config.set_ith(4, 255);
  ip_sub_gate_config.set_ith(5, 255);
  ip_sub_gate_config.set_ith(6, 255);
  ip_sub_gate_config.set_ith(7, 0);
  ip_sub_gate_config.set_ith(8, 192);
  ip_sub_gate_config.set_ith(9, 168);
  ip_sub_gate_config.set_ith(10, 1);
  ip_sub_gate_config.set_ith(11, 1);
  for (byte i = 0; i < 4; i++) thresholds_config.set_ith(i, 10+i); // setting the thresholds
  thresholds_config.set_ith(4, RTU_DEVICE_ID); // use the last byte to store the device id
  */



  //getting the ip addresses from the eeprom
  ip=IPAddress(ip_sub_gate_config.get_ith(0),ip_sub_gate_config.get_ith(1),ip_sub_gate_config.get_ith(2),ip_sub_gate_config.get_ith(3));
  subnet=IPAddress(ip_sub_gate_config.get_ith(4),ip_sub_gate_config.get_ith(5),ip_sub_gate_config.get_ith(6),ip_sub_gate_config.get_ith(7));
  gateway=IPAddress(ip_sub_gate_config.get_ith(8),ip_sub_gate_config.get_ith(9),ip_sub_gate_config.get_ith(10),ip_sub_gate_config.get_ith(11));

  //getting the temperature thresholds from the eeprom
  for (byte i = 0; i < 4; i++) temp_threshold__arr[i] = thresholds_config.get_ith(i);
  RTU_DEVICE_ID = thresholds_config.get_ith(4);


  lcd.begin(16, 2);
  show_lcd_menu(LCD_HOME);
	Ethernet.init(10);
	Ethernet.begin(mac, ip, dns, gateway, subnet);
  Udp.begin(localPort);
	Clock.begin();
  leds_all.begin();
	leds_all.setPixelColor(0, color_comfortable);
	leds_all.show();
	command[0] = '\0';



  if constexpr (SERIAL_ENABLE || SERIAL_DEBUG_ENABLE) {
    Serial.println(F("Current addresses and thresholds"));
    Serial.print(F("IP:\t\t"));
    for (byte i = 0; i < 4; i++) {
      Serial.print(ip_sub_gate_config.get_ith(i));
      Serial.print((i < 3 ? "." : "\n\r"));
    }
    Serial.print(F("Subnet:\t\t"));
    for (byte i = 0; i < 4; i++) {
      Serial.print(ip_sub_gate_config.get_ith(i+4));
      Serial.print((i < 3 ? "." : "\n\r"));
    }
    Serial.print(F("Gateway:\t"));
    for (byte i = 0; i < 4; i++) {
      Serial.print(ip_sub_gate_config.get_ith(i+8));
      Serial.print((i < 3 ? "." : "\n\r"));
    }
    Serial.print(F("Thresholds:\t"));
    for (byte i = 0; i < 4; i++) {
      Serial.print(thresholds_config.get_ith(i));
      Serial.print((i < 3 ? " " : "\n\r"));
    }
    Serial.println(F("\n\rEnter commands or 'HELP'"));
  }
}


//main loop where it runs and calls everything.
void loop() {
	read_temp_hum_loop(); // keeps reading the data from the sensor, takes care of sotring the data too.
	five_button_read(); // reads the input from the buttons and acts accordingly
  show_lcd_menu(curr_lcd_menu); // updates menus on LCD.
	if constexpr (SERIAL_ENABLE) take_input(); // listens on serial
  if constexpr (UDP_OLD_ENABLE) take_input_udp(); // listens on UDP, the old ways
  take_input_udp_dcpx(); // listens on UPD thru DCPx protocol
}
