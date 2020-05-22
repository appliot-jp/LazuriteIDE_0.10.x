#include "DC2PLC6_subghz_ide.h"		// Additional Header

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

#define ORANGE_LED 25						// pin number of ORANGE LED
#define BLUE_LED 26						// pin number of Blue LED

#define SUBGHZ_CH		36			// channel number (frequency)
#define RX_PANID	0xabcd		// panid
#define RX_ADDR	0x902b		// distination address

uint8_t txbuf[256];

void setup(void)
{
	int i;
	SubGHz.init();					// initializing Sub-GHz
	Serial.begin(115200);
	pinMode(BLUE_LED,OUTPUT);			// setting of LED
	digitalWrite(BLUE_LED,HIGH);		// setting of LED
	
	for(i=8;i<=13;i++) {
		pinMode(i,INPUT);
	}
}

void loop(void)
{
	SUBGHZ_MSG msg;
	int i;
	// Initializing
	Print.init(txbuf,sizeof(txbuf));
	for(i=8;i<=13;i++) {
		Print.l(digitalRead(i),DEC);
		Print.p(",");
	}
	Print.l(analogRead(14),DEC);
	Print.p(",");
	Print.l(analogRead(15),DEC);
	Print.ln();
	
	SubGHz.begin(SUBGHZ_CH, RX_PANID,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);		// start Sub-GHz
	
	digitalWrite(BLUE_LED,LOW);
	SubGHz.send(RX_PANID,RX_ADDR,txbuf,Print.len(),NULL);
	digitalWrite(BLUE_LED,HIGH);
	SubGHz.close();
	sleep(1000);																// sleep

	return;
}
