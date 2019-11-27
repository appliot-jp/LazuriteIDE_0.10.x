#include "Multihop_SubGHz_ide.h"		// Additional Header


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


#define LED 26						// pin number of Blue LED
#define SUBGHZ_CH		SUBGHZ_HOPPING_TS_S			// channel number (frequency)
//#define SUBGHZ_CH		24			// channel number (frequency)
#define SUBGHZ_PANID	0xFFFF		// panid
#define HOST_ADDRESS	0xFFFF		// distination address

unsigned char send_data[] = {"Welcome Lazurite SubGHz2!!\r\n"};
//unsigned char send_data[] = {"Welcome SubGHz2!!\r\n"};

bool event=false;

void callback(void)
{
	event = true;
}

void setup(void)
{
	SubGHz.init();					// initializing Sub-GHz
	SubGHz.setBroadcastEnb(false);
	Serial.begin(115200);			// Initializing serial
	
	pinMode(LED,OUTPUT);			// setting of LED
	digitalWrite(LED,HIGH);			// setting of LED
	
	timer2.set(10000L,callback);
	timer2.start();
	
	SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);		// start Sub-GHz
	Serial.println_long(SubGHz.getMyAddress,HEX);
//	SubGHz.rxEnable(NULL);
}

void loop(void)
{
	SUBGHZ_MSG msg;
	
	// Initializing
	
	// preparing data
	digitalWrite(LED,LOW);														// LED ON
	msg = SubGHz.send(SUBGHZ_PANID, HOST_ADDRESS, &send_data, sizeof(send_data),NULL);// send data
	digitalWrite(LED,HIGH);														// LED off
	SubGHz.msgOut(msg);
	// close
//	SubGHz.close();																// Sub-GHz module sets into power down mode.


	wait_event(&event);

	return;
}

