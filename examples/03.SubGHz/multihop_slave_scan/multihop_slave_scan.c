#include "multihop_slave_scan_ide.h"		// Additional Header

/* FILE NAME: multihop_slave_scan.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2020  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define SUBGHZ_CH		SUBGHZ_HOPPING_TS_S
#define SUBGHZ_PANID	( 0xABCD )
#define LIST_SIZE 		( 10 )
#define ORANGE_LED		( 25 )
#define BLUE_LED		( 26 )
#define SCAN_ST_START 	( 0 )
#define SCAN_ST_RUNNING	( 1 )
#define SCAN_ST_DONE 	( 2 )

SUBGHZ_SCAN_LIST scan_list[LIST_SIZE];
uint8_t scan_state,scan_count;
unsigned char send_data[] = {"Welcome to Lazurite SubGHz"};

void print_addr_rssi(SUBGHZ_SCAN_LIST *list) {
	int j;

	for (j=7; j>=0; j--) {
		if (list->addr[j] < 0x10) Serial.print("0");
		Serial.print_long((long)list->addr[j],HEX);
	}
	Serial.print(" : ");
	Serial.print_long((long)list->rssi,DEC); // average
	Serial.print(" (");
	Serial.print_long((long)list->raw_rssi.byte_data[3],DEC); // oldest
	Serial.print(",");
	Serial.print_long((long)list->raw_rssi.byte_data[2],DEC);
	Serial.print(",");
	Serial.print_long((long)list->raw_rssi.byte_data[1],DEC);
	Serial.print(",");
	Serial.print_long((long)list->raw_rssi.byte_data[0],DEC); // latest
	Serial.println(")");
}

void callback(uint8_t num) {
	scan_state = SCAN_ST_DONE;
	scan_count = num;
}

void setup() {
  // put your setup code here, to run once:
	digitalWrite(ORANGE_LED,HIGH);
	digitalWrite(BLUE_LED,HIGH);
  	pinMode(ORANGE_LED,OUTPUT);
  	pinMode(BLUE_LED,OUTPUT);
	Serial.begin(115200);
	SubGHz.init();
	SubGHz.begin(SUBGHZ_CH,SUBGHZ_PANID,SUBGHZ_100KBPS,SUBGHZ_PWR_1MW);
	scan_state = SCAN_ST_START;
}

void loop() {
  // put your main code here, to run repeatedly:
	int i;

	if (scan_state == SCAN_ST_START) {
		scan_state = SCAN_ST_RUNNING;
		SubGHz.scan(SUBGHZ_PANID,scan_list,LIST_SIZE,callback);
		digitalWrite(ORANGE_LED,HIGH);
	} else if (scan_state == SCAN_ST_RUNNING) {
		sleep(500);
		if (digitalRead(ORANGE_LED)) digitalWrite(ORANGE_LED,LOW);
		else digitalWrite(ORANGE_LED,HIGH);
	} else if (scan_state == SCAN_ST_DONE) {
		digitalWrite(ORANGE_LED,HIGH);
		scan_state = SCAN_ST_START;
		if (scan_count == 0) {
			Serial.println("Hosts not found.");
			digitalWrite(ORANGE_LED,LOW);
		} else {
			Serial.println("-- List --\nmac address      : rssi ave (oldest <-> latest)");
			for (i=0; i<scan_count; i++) print_addr_rssi(&scan_list[i]);
			Serial.println("-- End of list --\nSend test data to the host (top of the list)");
			digitalWrite(BLUE_LED,LOW);
			SubGHz.send(SUBGHZ_PANID,scan_list[0].addr[0]|(scan_list[0].addr[1]<<8),send_data,sizeof(send_data),NULL);
			digitalWrite(BLUE_LED,HIGH);
		}
		sleep(5000);
	}
}
