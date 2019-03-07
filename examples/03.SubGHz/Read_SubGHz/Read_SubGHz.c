#include "Read_SubGHz_ide.h"		// Additional Header

/* FILE NAME: Read_SubGHz.c
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

#define SUBGHZ_CH	36
#define SUBGHZ_PANID	0xABCD
uint8_t rx_data[256];
uint32_t last_recv_time = 0;
uint8_t myAddr64[8];
SUBGHZ_STATUS rx;

#define BLUE_LED	26

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
	SUBGHZ_MSG msg;
	long myAddress;

	Serial.begin(115200);
	
	msg = SubGHz.init();
	if(msg != SUBGHZ_OK)
	{
		SubGHz.msgOut(msg);
		while(1){ }
	}
	
	myAddress = SubGHz.getMyAddress();
	SubGHz.getMyAddr64(myAddr64);
	Serial.print("myAddress = ");
	Serial.println_long(myAddress,HEX);
	Serial.print_long(myAddr64[0],HEX);
	Serial.print(" ");
	Serial.print_long(myAddr64[1],HEX);
	Serial.print(" ");
	Serial.print_long(myAddr64[2],HEX);
	Serial.print(" ");
	Serial.print_long(myAddr64[3],HEX);
	Serial.print(" ");
	Serial.print_long(myAddr64[4],HEX);
	Serial.print(" ");
	Serial.print_long(myAddr64[5],HEX);
	Serial.print(" ");
	Serial.print_long(myAddr64[6],HEX);
	Serial.print(" ");
	Serial.println_long(myAddr64[7],HEX);
	msg = SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);
	if(msg != SUBGHZ_OK)
	{
		SubGHz.msgOut(msg);
		while(1){ }
	}
	msg = SubGHz.rxEnable(NULL);
	if(msg != SUBGHZ_OK)
	{
		SubGHz.msgOut(msg);
		while(1){ }
	}
	
	pinMode(BLUE_LED,OUTPUT);
	digitalWrite(BLUE_LED,HIGH);
	
	Serial.println("TIME	HEADER	SEQ	PANID	RX_ADDR	TX_ADDR	RSSI	PAYLOAD");
	Serial.println("-----------------------------------------------------------------------------------------------------------------");
	
	return;
}

void loop(void)
{
	SUBGHZ_MAC_PARAM mac;
	short rx_len;
	short index=0;
	uint16_t data16;
	int i;
	
	rx_len = SubGHz.readData(rx_data,sizeof(rx_data));
	rx_data[rx_len]=0;
	if(rx_len>0)
	{
		digitalWrite(BLUE_LED, LOW);
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

		for(i=0;i<mac.payload_len;i++) {
			print_hex_func(mac.payload[i]);
			Serial.print(" ");
		}
		// print ln
		Serial.println("");
		digitalWrite(BLUE_LED, HIGH);

	}
	
	return;
}
	

