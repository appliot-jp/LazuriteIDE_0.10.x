#include "RFTEST_1_ide.h"		// Additional Header

/* FILE NAME: RFTEST_1.c
 * The MIT License (MIT)
 *
 * Copyright (c) 2021  Lapis Technology Co.,Ltd.
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

static uint8_t ch = 36;
static uint8_t baud = 100;
#define DEFAULT_PAYLOAD		"factory-iot"
#define SUBGHZ_PANID	0xffff
#define SUBGHZ_DST      0xffff
static uint16_t panid = SUBGHZ_PANID;
static uint16_t dst_addr = SUBGHZ_DST;
static char serialBuf[256];
static int serialBufIndex = 0;
static uint8_t payload[256];
static char txmode = 0;
static uint32_t txinterval = 1000;
uint8_t rx_data[256];

uint8_t antmode = 0;
#define PAYLOAD_AMTMODE0 "factory-iot,BUILTIN"
#define PAYLOAD_AMTMODE1  "factory-iot,EXT"

SUBGHZ_STATUS rx;							// structure for getting rx status
#define ORANGE_LED	25
#define BLUE_LED	26

static const unsigned char *aes_key = NULL;		// disable AES key
static unsigned long last_tx_time;

struct PER {
	int req;
	int ack;
};

struct PER builtin_ant;
struct PER ext_ant;

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

#include "mcu.h"
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

	memset(&builtin_ant,0,sizeof(builtin_ant));
	memset(&ext_ant,0,sizeof(ext_ant));

	SubGHz.getMyAddr64(myAddr);
	print_hex_func(myAddr[0]);
	print_hex_func(myAddr[1]);
	print_hex_func(myAddr[2]);
	print_hex_func(myAddr[3]);
	print_hex_func(myAddr[4]);
	print_hex_func(myAddr[5]);
	print_hex_func(myAddr[6]);
	print_hex_func(myAddr[7]);
	Serial.println("");

	sleep(5000);

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

	if (txmode == 0) {
		if ((millis() - last_tx_time) > txinterval) {
			Serial.print("PER(BUILTIN,EXT) = ");
			Serial.print_double((float)(builtin_ant.req-builtin_ant.ack)/(float)builtin_ant.req*100,2);
			Serial.print(",");
			Serial.print_double((float)(ext_ant.req - ext_ant.ack)/(float)ext_ant.req*100,2);
			Serial.println("");
			digitalWrite(BLUE_LED, LOW);
			SubGHz.close();
			SubGHz.antSwitch(antmode);
			msg = SubGHz.begin(ch, SUBGHZ_PANID,  baud, SUBGHZ_PWR_20MW);
			if (antmode == 0) {
				antmode = 1;
				memcpy(payload, PAYLOAD_AMTMODE0, sizeof(PAYLOAD_AMTMODE0));
				builtin_ant.req++;
			} else {
				antmode = 0;
				memcpy(payload, PAYLOAD_AMTMODE1, sizeof(PAYLOAD_AMTMODE1));
				ext_ant.req++;
			}
			Serial.println(payload);
			msg = SubGHz.send(panid, dst_addr, payload, strlen(payload), NULL);
			last_tx_time = millis();
			digitalWrite(BLUE_LED, HIGH);
		}
	}
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
		Serial.println("");
		if(strncmp(mac.payload,"activate,",sizeof("activate,")-1) == 0){		
			if(antmode == 1) {
				builtin_ant.ack++;
			} else {
				ext_ant.ack++;
			}
		}
		
		// print ln
		digitalWrite(ORANGE_LED, HIGH);
	}
	rtmp = Serial.read();
	if (rtmp >= 0) {
		if (serialBufIndex < (sizeof(serialBuf) - 2)) {
			serialBuf[serialBufIndex] = (uint8_t)rtmp;
			serialBufIndex++;
		}
	}
	if ((rtmp == 10) || (rtmp == 13)) {
		bool update = false;
		serialBuf[serialBufIndex - 1] = NULL;
		if (strncmp(serialBuf, "p0x", 3) == 0) {
			rtmp = strtol(serialBuf + 1, NULL, 0);
			panid = (uint16_t) rtmp;
			update = true;
		} else if (strncmp(serialBuf, "d0x", 3) == 0) {
			rtmp = strtol(serialBuf + 1, NULL, 0);
			dst_addr = (uint16_t) rtmp;
		} else {
			rtmp = strtol(serialBuf, NULL, 0);
			if ((rtmp >= 24) && (rtmp <= 62)) {
				ch = (uint8_t)rtmp;
				update = true;
			} else	if (rtmp == 1) {
				baud = 50;
				update = true;
			} else if (rtmp == 2) {
				baud = 100;
				update = true;
			} else if (rtmp == 3) {
				Serial.println("one shot");
				txmode = 1;
			} else if (rtmp == 4) {
				Serial.println("10ms");
				txinterval = 10;
				txmode = 0;
			} else if (rtmp == 5) {
				Serial.println("100ms");
				txinterval = 100;
				txmode = 0;
			} else if (rtmp == 6) {
				txinterval = 1000;
				txmode = 0;
			} else if (rtmp == 7) {
				txinterval = 10000;
				txmode = 0;
			} else {
				memcpy(payload, serialBuf, serialBufIndex);
				if (txmode == 1) {
					digitalWrite(BLUE_LED, LOW);
					msg = SubGHz.send(SUBGHZ_PANID, SUBGHZ_DST, payload, strlen(payload), NULL);
					SubGHz.msgOut(msg);
					last_tx_time = millis();
					digitalWrite(BLUE_LED, HIGH);
				}
			}
		}
		serialBufIndex = 0;
		if (update) {
			Serial.print("change RF params  CH=");
			Serial.print_long((long)ch, DEC);
			Serial.print(", baud=");
			Serial.print_long((long)baud, DEC);
			Serial.println("");
			SubGHz.rxDisable();
			msg = SubGHz.begin(ch, SUBGHZ_PANID,  baud, SUBGHZ_PWR_20MW);
			if (msg != SUBGHZ_OK)
			{
				SubGHz.msgOut(msg);
			}
			msg = SubGHz.rxEnable(NULL);
			if (msg != SUBGHZ_OK)
			{
				SubGHz.msgOut(msg);
			}
		}
	}
	return;
}


