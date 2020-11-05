#include "multihop_slave_tx64_ide.h"		// Additional Header

/* FILE NAME: Print_SubGHz.c
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

#define SUBGHZ_CH	SUBGHZ_HOPPING_TS_S

#define SUBGHZ_PANID	0xabcd		// panid
#define SUBGHZ_DST	{0x00,0x1d,0x12,0x90,0x00,0x04,0x2f,0x10}
static const uint8_t host[] = SUBGHZ_DST;
//#define SUBGHZ_DST		0x7fb2
uint8_t rx_data[256];
uint32_t last_recv_time = 0;
SUBGHZ_STATUS rx;							// structure for getting rx status
#define ORANGE_LED	25
#define BLUE_LED	26

static const unsigned char *aes_key = NULL;		// disable AES key
static const char send_data[] = __FILE__;

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
	uint8_t myAddr[8];
	uint32_t bo = 2000;

	Serial.begin(115200);
	
	msg = SubGHz.init();
	if(msg != SUBGHZ_OK)
	{
		SubGHz.msgOut(msg);
		while(1){ }
	}
	
	SubGHz.getMyAddr64(myAddr);
	Serial.print("myAddress = ");
	print_hex_func(myAddr[0]);
	print_hex_func(myAddr[1]);
	Serial.print(" ");
	print_hex_func(myAddr[2]);
	print_hex_func(myAddr[3]);
	Serial.print(" ");
	Serial.print(" ");
	print_hex_func(myAddr[4]);
	print_hex_func(myAddr[5]);
	Serial.print(" ");
	print_hex_func(myAddr[6]);
	print_hex_func(myAddr[7]);
	Serial.println("");


	SubGHz.setKey(aes_key);
	
	msg = SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);
	if(msg != SUBGHZ_OK)
	{
		SubGHz.msgOut(msg);
		while(1){ }
	}	
	pinMode(BLUE_LED,OUTPUT);
	digitalWrite(BLUE_LED,HIGH);
	
	pinMode(ORANGE_LED,OUTPUT);
	digitalWrite(ORANGE_LED,HIGH);
	
	return;
}

void loop(void)
{
	SUBGHZ_MSG msg;
	
	// preparing data
	digitalWrite(BLUE_LED,LOW);														// LED ON
	msg=SubGHz.send64be(host, &send_data, sizeof(send_data),NULL);// send data
	digitalWrite(BLUE_LED,HIGH);														// LED off
	SubGHz.msgOut(msg);	
	// close
	SubGHz.close();																// Sub-GHz module sets into power down mode.
	
	sleep(1000);																// sleep

	return;
}

