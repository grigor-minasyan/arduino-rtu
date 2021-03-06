#include "main.h"

/*
Those functions teach arduino to create DCP commands as well as
reading them, interpreting them and respoing accordingly.
*/

// right now only one command is defined.
#define DCP_FUDRop 3

// needs an ID to communicate thru DCPx
byte RTU_DEVICE_ID = 2;

typedef struct _DCP_OP_NTRY {
  byte text[5];
  byte code;
  byte lne;
} DCP_OP_NTRY;
typedef struct _DCP_OP_NTRY* PDCP_OP_NTRY;

DCP_OP_NTRY DCP_op_table[] = {
  {"FUDR", DCP_FUDRop, 2},
  {0, 0, 0}
};

PDCP_OP_NTRY DCP_op_lookup(byte op) {
  byte i = 0;
  for( ; DCP_op_table[i].code != 0 && DCP_op_table[i].code != op; i++);
  return (&DCP_op_table[i]);
}

// generate a BCH checksum for a DCP command.
byte DCP_genCmndBCH(byte buff[], int count) {
  byte bch = 0, nBCHpoly = 0xb8, fBCHpoly = 0xff;
  for (byte i = 0; i < count; i++) {
    bch ^= buff[i];
    for (byte j = 0; j < 8; j++) {
        if ((bch & 1) == 1) bch = (bch >> 1) ^ nBCHpoly;
        else bch >>= 1;
    }
  }
  bch ^= fBCHpoly;
  return bch;
}

//todo make lcd show if not valid BCH
// commented since I am using this inline
// bool DCP_validate_BCH(byte buff[], int count) {
//   return buff[count-1] == DCP_genCmndBCH(buff, count-1);
// }

// those 3 functions are here because I am using c style arrays and not vectors, so I have to do the removing, adding and copying manually.
int remove_element(byte array[], int index, int *array_length){
  int retval = array[index];
  for(int i = index; i < *array_length - 1; i++) array[i] = array[i + 1];
  (*array_length)--;
  return retval;
}
void insert_element(byte array[], int index, int *array_length, int value) {
  for (int i = *array_length - 1; i >= index; i--) array[i+1] = array[i];
  array[index] = value;
  (*array_length)++;
}
void copy_array(byte source[], byte dest[], int array_length) {
  for (int i = 0; i < array_length; i++) dest[i] = source[i];
}

// in dcpX protocol, there might be duplicate AA bytes next to each other, but it will be compressed
// into 0xAA 0xXX, where XX is the count of how many AA bytes there are
int DCP_compress_AA_byte(byte buffer[], byte buffer_big[], int count){
  //its copied into bueffer big because sometimes this compression can make the array larger.
  copy_array(buffer, buffer_big, count);
  for (int i = 2; i < count - 1; i++) { // skipping the first 2 opening bytes
    int aa_counter = 1;
    // # if the next one is also AA
    if (buffer_big[i] == 0xaa && buffer_big[i+1] == 0xaa){
      remove_element(buffer_big, i+1, &count); // remove the next element
      aa_counter += 1; // increase the counter
      int j = i + 1; // keep checking if there is another AA byte
      // # check how many AA bytes, and insert when done
      while (j < count){
        if (buffer_big[j] == 0xaa){
          remove_element(buffer_big, j, &count);
          aa_counter+=1;
        }
        else{
          insert_element(buffer_big, j, &count, aa_counter);
          break;
        }
      }
    } else if (buffer_big[i] == 0xaa) {
      insert_element(buffer_big, i+1, &count, 1);// # if AA byte but not repeating add a counter of how many
    }
  }
  // # take care of the last AA if exists
  if (buffer_big[count-1] == 0xaa) insert_element(buffer_big, count, &count, 1);
  return count;
}

// the reverse process of compressing
int DCP_expand_AA_byte(byte buffer[], byte buffer_big[], int count){
  //its copied into bueffer big because this expansion can make the array larger.
  copy_array(buffer, buffer_big, count);
  // # take care of the last AA if exists
  if (buffer_big[count-2] == 0xaa && buffer_big[count-1] == 1) remove_element(buffer_big, count-1, &count);
  for (int i = 2; i < count - 1; i++) {
    // # if AA is found
    if (buffer_big[i] == 0xaa && buffer_big[i-1] != 0xaa && buffer_big[i+1] != 0xaa){
      int aa_counter = remove_element(buffer_big, i+1, &count);
      aa_counter -= 1; //takes care is only 1 AA no need to insert
      // # reinsert required bytes
      while (aa_counter){
        insert_element(buffer_big, i+1, &count, 0xaa);
        aa_counter -= 1;
      }
    }
  }
  return count;
}

// do actions based on a DCPX command received.
void DCP_respond(byte command, IPAddress remote_ip, unsigned int remote_port) {
  PDCP_OP_NTRY command_entry = DCP_op_lookup(command);

  switch (command_entry->code) {
    // right now only this command is supported, FULL UPDATE
    case DCP_FUDRop:
      /*response packet to this is going to look like this
      [aa][fa][addr][opcode][00][val1][val2][val3][val4][alarms][bch] - means sending the threshold in C
      [aa][fa][addr][opcode][01][dt1][dt2][dt3][dt4][dt5][dt6][temp][hum][bch] - current temp
      [aa][fa][addr][opcode][02][dt1][dt2][dt3][dt4][dt5][dt6][temp][hum][bch] - year month sent in bitwise with temp and humidity
      */
      //sending information about thresholds
      int size_dcp_thresholds = 11;
      // construct the array
      byte dcp_thresholds[size_dcp_thresholds] = {0xaa, 0xfa, RTU_DEVICE_ID, DCP_FUDRop, 0x00, temp_threshold__arr[0], temp_threshold__arr[1], temp_threshold__arr[2], temp_threshold__arr[3], temp_alarm_binary};
      dcp_thresholds[size_dcp_thresholds-1] = DCP_genCmndBCH(dcp_thresholds, size_dcp_thresholds-1);
      byte dcp_thresholds_big[2*size_dcp_thresholds];

      //compress the array and send it over
      size_dcp_thresholds = DCP_compress_AA_byte(dcp_thresholds, dcp_thresholds_big, size_dcp_thresholds);
			Udp.beginPacket(remote_ip, remote_port);
			Udp.write(dcp_thresholds_big, size_dcp_thresholds);
			Udp.endPacket();

      //sending information about current temp
      int size_dcp_cur = 14;
      // construct the array
      byte dcp_cur[size_dcp_cur] = {0xaa, 0xfa, RTU_DEVICE_ID, DCP_FUDRop, 0x01,
                (byte)Clock.read().Year, (byte)Clock.read().Month, (byte)Clock.read().Day,
                (byte)Clock.read().Hour, (byte)Clock.read().Minute, (byte)Clock.read().Second,
                cur_temp, cur_humidity};
      dcp_cur[size_dcp_cur-1] = DCP_genCmndBCH(dcp_cur, size_dcp_cur-1);
      byte dcp_cur_big[2*size_dcp_cur];

      //compress the array and send it over
      size_dcp_cur = DCP_compress_AA_byte(dcp_cur, dcp_cur_big, size_dcp_cur);
			Udp.beginPacket(remote_ip, remote_port);
			Udp.write(dcp_cur_big, size_dcp_cur);
			Udp.endPacket();


      /*
      //sending the stored data packets, used to send the whole contents of EEPROM over UDP
      for (int i = 0; i < rtc_dht_data_range.get_stored_data_count(); i++) {
        //getting the numbers from the bitwise
        byte ret_year = rtc_dht_data_range.get_ith_data_from_curr(i).get_year();
        byte ret_month = rtc_dht_data_range.get_ith_data_from_curr(i).get_month();
        byte ret_day = rtc_dht_data_range.get_ith_data_from_curr(i).get_day();
        byte ret_hour = rtc_dht_data_range.get_ith_data_from_curr(i).get_hour();
        byte ret_minute = rtc_dht_data_range.get_ith_data_from_curr(i).get_minute();
        byte ret_second = rtc_dht_data_range.get_ith_data_from_curr(i).get_second();
        int8_t ret_temp = rtc_dht_data_range.get_ith_data_from_curr(i).get_temp();
        byte ret_hum = rtc_dht_data_range.get_ith_data_from_curr(i).get_hum();
        //

        //sending information about hisr temp
        int size_dcp_hist = 14;
        byte dcp_hist[size_dcp_hist] = {0xaa, 0xfa, RTU_DEVICE_ID, DCP_FUDRop, 0x02,
                  ret_year, ret_month, ret_day, ret_hour, ret_minute, ret_second, ret_temp, ret_hum};
        dcp_hist[size_dcp_hist-1] = DCP_genCmndBCH(dcp_hist, size_dcp_hist-1);
        byte dcp_hist_big[2*size_dcp_hist];

        //compress the array and send it over
        size_dcp_hist = DCP_compress_AA_byte(dcp_hist, dcp_hist_big, size_dcp_hist);
  			Udp.beginPacket(remote_ip, remote_port);
  			Udp.write(dcp_hist_big, size_dcp_hist);
  			Udp.endPacket();
      }
      udp_packets_out_counter++;
      */


      /*
      int size = 15;
      byte test1[size] = {0xaa, 0xfc, 0xaa, 0x12, 0xaa, 0xaa, 0x64, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xab, 0xaa};
      byte test_big[2*size];
      byte test_big2[2*size];

      Udp.beginPacket(ip_remote, REMOTEPORT_SERVER);
      Udp.write(test1, size);
      udp_packets_out_counter++;Udp.endPacket();
      size = DCP_compress_AA_byte(test1, test_big, size);
			Udp.beginPacket(ip_remote, REMOTEPORT_SERVER);
			Udp.write(test_big, size);
			udp_packets_out_counter++;Udp.endPacket();
      size = DCP_expand_AA_byte(test_big, test_big2, size);
			Udp.beginPacket(ip_remote, REMOTEPORT_SERVER);
			Udp.write(test_big2, size);
			udp_packets_out_counter++;Udp.endPacket();
      */
      break;
    default:
    ;
  }
}


//listen to comands from UDP
void take_input_udp_dcpx() {
	static unsigned long prev_time_udp, prev_time_udp_checker;

  // added delay to not check udp constantly
	if ((millis() - prev_time_udp > UDP_LISTEN_DELAY)) {

    //checking if still online, if not received any commands for too long will show red on network light.
		if (millis() - prev_time_udp_checker > UDP_CHECKER_DELAY) {
			leds_all.setPixelColor(1, 0x1e0000);
			leds_all.show();
			prev_time_udp_checker = millis();
		}

		prev_time_udp = millis();
		int packetSize = Udp.parsePacket();
		if (packetSize) {
      if constexpr (SERIAL_DEBUG_ENABLE) {
        Serial.print(F("received a packet"));
      }
			// read the packet into packetBuffer
			Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

      //check if 'y' is sent thru connection checker, fix the light reset the timer and return
			if(packetSize == 2 && packetBuffer[0] == 'y') {
				leds_all.setPixelColor(1, 0x001e00);
				leds_all.show();
				prev_time_udp_checker = millis();
				return;
			}

      // used to show sent received packets on the LCD
			udp_packets_in_counter++;

      //if BCH is valid and device id mathces
      if (packetBuffer[packetSize-1] == (char)DCP_genCmndBCH(packetBuffer, packetSize-1)) {
        if (packetBuffer[2] == RTU_DEVICE_ID) {
          DCP_respond(packetBuffer[3], Udp.remoteIP(), Udp.remotePort());//packetbuffer[3] is the command to respond with
        }
      } else {
        // show on the LCD that a command with wrong checksum is received.
        show_wrong_bch_lcd(packetBuffer[packetSize-1], DCP_genCmndBCH(packetBuffer, packetSize-1));
      }

		}
	}
}
