BOARD_TAG = uno
ARDUINO_LIBS = Adafruit_NeoPixel EEPROM DS3231_Simple Wire SimpleDHT Ethernet SPI LiquidCrystal
MONITOR_PORT  = /dev/ttyUSB0
include $(ARDMK_DIR)/Arduino.mk
