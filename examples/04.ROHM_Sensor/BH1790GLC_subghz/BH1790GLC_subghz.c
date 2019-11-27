#include "BH1790GLC_subghz_ide.h"		// Additional Header

/* FILE NAME: BH1790GLC_subghz.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2019  Lapis Semiconductor Co.,Ltd.
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
#define SUBGHZ_CH		36			// channel number (frequency)
#define SUBGHZ_PANID	0xabcd		// panid
#define HOST_ADDRESS	0x4566		// distination address

uint8_t send_data[250];

void setup() {
  // put your setup code here, to run once:
    SubGHz.init();
	Serial.begin(115200);
    Serial.println("LEDON Data, LEDOFF Data");
	Wire.begin();
	bh1790glc.init();
	SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);		// start Sub-GHz

}

void loop() {
  // put your main code here, to run repeatedly:
 	uint16_t data[2];

	bh1790glc.get_val(data);
	Print.init(send_data,sizeof(send_data));
	Print.p("STX,");
	Print.l((long)data[0], DEC);
	Print.p(",");
	Print.l((long)data[1], DEC);
	Print.p(",ETX");
	Print.ln();
	Serial.println(send_data);
	digitalWrite(LED,LOW);														// LED ON
	SubGHz.send(SUBGHZ_PANID, HOST_ADDRESS, send_data, Print.len(),NULL);// send data
	digitalWrite(LED,HIGH);														// LED off

	sleep(30);
}
