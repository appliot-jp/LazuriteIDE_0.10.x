#include "multihop_host_rx_ide.h"		// Additional Header
/* FILE NAME: Welcome_SubGHz.c
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

static uint8_t ch = SUBGHZ_HOPPING_TS_H;
static uint8_t baud = 100;
#define DEFAULT_PAYLOAD		__FILE__
#define SUBGHZ_PANID	0xABCD
#define SUBGHZ_DST      0x8e6d
static uint16_t panid = SUBGHZ_PANID;
static uint16_t dst_addr = SUBGHZ_DST;
static char serialBuf[256];
static int serialBufIndex = 0;
static uint8_t payload[256];
static char txmode = 1;
static uint32_t txinterval=5000;
uint8_t rx_data[256];

SUBGHZ_STATUS rx;							// structure for getting rx status
#define ORANGE_LED	25
#define BLUE_LED	26

static const unsigned char *aes_key = NULL;		// disable AES key
static unsigned long last_tx_time;


void print_hex_func(uint8_t data)
{
	if (data == 0) {
		Serial.print("00");
	}
	else
		if (data < 16) {
			Serial.print("0");
			Serial.print_long(data, HEX);
		} else {
			Serial.print_long(data, HEX);
		}
}

void setup(void)
{
	SUBGHZ_MSG msg;
	uint8_t myAddr[8];

	Serial.begin(115200);

	msg = SubGHz.init();
	if (msg != SUBGHZ_OK)
	{
		SubGHz.msgOut(msg);
		while (1) { }
	}
	memcpy(payload, DEFAULT_PAYLOAD, sizeof(DEFAULT_PAYLOAD));

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

	digitalWrite(BLUE_LED, HIGH);
	digitalWrite(ORANGE_LED, HIGH);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(ORANGE_LED, OUTPUT);

	Serial.println("TIME	HEADER	SEQ	PANID	RX_ADDR	TX_ADDR	RSSI	PAYLOAD");
	Serial.println("-----------------------------------------------------------------------------------------------------------------");

	// リセット後にデータを送信させるためのダミーデータ
	last_tx_time = 0x8000;

	SubGHz.begin(ch, panid,  baud, SUBGHZ_PWR_20MW);
	SubGHz.rxEnable(NULL);

	return;
}

void loop(void)
{
	SUBGHZ_MSG msg;
	SUBGHZ_MAC_PARAM mac;
	short rx_len;
	short index = 0;
	uint16_t data16;
	long rtmp = 0;
	
	//受信時の処理
	rx_len = SubGHz.readData(rx_data, sizeof(rx_data));
	if (rx_len > 0)
	{
		//受信データの最後にNULLを付与する
		rx_data[rx_len] = 0;
		digitalWrite(ORANGE_LED, LOW);
		SubGHz.getStatus(NULL, &rx);
		SubGHz.decMac(&mac, rx_data, rx_len);
		Serial.print_long(millis(), DEC);
		Serial.print("\t");

		Serial.print_long(mac.mac_header.fc16, HEX);
		Serial.print("\t");

		Serial.print_long(mac.seq_num, HEX);
		Serial.print("\t");

		Serial.print_long(mac.dst_panid, HEX);
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

		Serial.print_long(rx.rssi, DEC);
		Serial.print("\t");

		Serial.print(mac.payload);
		// print ln
		Serial.println("");
		digitalWrite(ORANGE_LED, HIGH);
	}

	return;
}


