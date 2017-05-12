#include "SubGHz_rxtx2_ide.h"		// Additional Header

/* FILE NAME: SubGHz_tx64rx.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015  Lapis Semiconductor Co.,Ltd.
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


#define ORANGE_LED 25						// pin number of Blue LED
#define BLUE_LED 26						// pin number of Blue LED

#define SUBGHZ_CH		36			// channel number (frequency)
#define SUBGHZ_PANID	0xabcd		// panid
#define HOST_ADDRESS	0xffff       // distination address

//uint8_t host[] = HOST_ADDRESS;

unsigned char send_data[] = {"group cast!!!!\r\n"};
unsigned char rx_data[256];
SUBGHZ_STATUS rx;							// structure for getting rx status

void print_hex_func(uint8_t data)
{
	if(data == 0) Serial.print("00");
	else if(data < 16) {
		Serial.print("0");
		Serial.print_long(data,HEX);
	} else {
		Serial.print_long(data,HEX);
	}
	

}

void setup(void)
{
	
	SubGHz.init();					// initializing Sub-GHz
	Serial.begin(115200);
	pinMode(ORANGE_LED,OUTPUT);			// setting of LED
	pinMode(BLUE_LED,OUTPUT);			// setting of LED
	digitalWrite(ORANGE_LED,HIGH);			// setting of LED
	digitalWrite(BLUE_LED,HIGH);			// setting of LED
	SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);		// start Sub-GHz
	SubGHz.rxEnable(NULL);
}

void loop(void)
{
	volatile unsigned long current_time;
	static volatile unsigned long last_send_time = 0x80000000;
	int rx_len;
	SUBGHZ_MAC_PARAM mac;
	SUBGHZ_MSG msg;
	
	// Initializing
	current_time = millis();
	
	// preparing data
	if((current_time - last_send_time) > 500) {
		digitalWrite(BLUE_LED,LOW);														// LED ON
		msg=SubGHz.send(SUBGHZ_PANID,HOST_ADDRESS, &send_data, sizeof(send_data),NULL);// send data
		digitalWrite(BLUE_LED,HIGH);														// LED off
		SubGHz.msgOut(msg);
		last_send_time = current_time;
	}

	rx_len = SubGHz.readData(rx_data,sizeof(rx_data));
	rx_data[rx_len]=0;
	if(rx_len>0)
	{
		digitalWrite(ORANGE_LED, LOW);
		SubGHz.getStatus(NULL,&rx);										// get status of rx
		SubGHz.decMac(&mac,rx_data,rx_len);

		Serial.print_long(millis(),DEC);
		Serial.print("\t");

		Serial.print_long(mac.mac_header.fc16,HEX);
		Serial.print("\t");

		Serial.print_long(mac.seq_num,HEX);
		Serial.print("\t");

		Serial.print_long(mac.dst_panid,HEX);
		Serial.print("\t");

		print_hex_func(mac.dst_addr[7]);
		print_hex_func(mac.dst_addr[6]);
		print_hex_func(mac.dst_addr[5]);
		print_hex_func(mac.dst_addr[4]);
		print_hex_func(mac.dst_addr[3]);
		print_hex_func(mac.dst_addr[2]);
		print_hex_func(mac.dst_addr[1]);
		print_hex_func(mac.dst_addr[0]);
		Serial.print("\t");

		print_hex_func(mac.src_addr[7]);
		print_hex_func(mac.src_addr[6]);
		print_hex_func(mac.src_addr[5]);
		print_hex_func(mac.src_addr[4]);
		print_hex_func(mac.src_addr[3]);
		print_hex_func(mac.src_addr[2]);
		print_hex_func(mac.src_addr[1]);
		print_hex_func(mac.src_addr[0]);
		Serial.print("\t");

		Serial.print_long(rx.rssi,DEC);
		Serial.print("\t");
		
		Serial.write(mac.payload,mac.payload_len);
		// print ln
		Serial.println("");
		
		digitalWrite(ORANGE_LED, HIGH);
	}

	return;
}

