#include "axis10_subghz_ide.h"		// Additional Header



/* FILE NAME: Axis10_subghz.c
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
#define SUBGHZ_CH		36			// channel number (frequency)
#define SUBGHZ_PANID	0xABCD		// panid
#define HOST_ADDRESS	0x8e46	// distination address

void setup() {
  byte rc;

  Serial.begin(115200);
  
  SubGHz.init();					// initializing Sub-GHz

  pinMode(LED, OUTPUT);			// setting of LED
  digitalWrite(LED, HIGH);			// setting of LED

  Wire.begin();

  rc = kxg03.init(KXG03_DEVICE_ADDRESS_4E);
  rc = bm1422.init(0);
  rc = bm1383.init(0);
}

char tx_buf[128];


void loop() {
  byte rc;

  static float val[11];

  rc = kxg03.get_val(&val[0]);
  bm1422.get_val(&val[6]);
  bm1383.get_val(&val[9]);

  Print.init(tx_buf, sizeof(tx_buf));
  Print.p("STX,");
  Print.f(val[0], 2);
  Print.p(",");
  Print.f(val[1], 2);
  Print.p(",");
  Print.f(val[2], 2);
  Print.p(",");
  Print.f(val[3], 2);
  Print.p(",");
  Print.f(val[4], 2);
  Print.p(",");
  Print.f(val[5], 2);
  Print.p(",");
  Print.f(val[6], 2);
  Print.p(",");
  Print.f(val[7], 2);
  Print.p(",");
  Print.f(val[8], 2);
  Print.p(",");
  Print.f(val[10], 2);
  Print.p(",ETX");
  Print.ln();
  
//  Serial.print(tx_buf);

	// Initializing
	SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);		// start Sub-GHz
	
	// preparing data
	digitalWrite(LED,LOW);														// LED ON
	SubGHz.send(SUBGHZ_PANID, HOST_ADDRESS, tx_buf, Print.len(),NULL);// send data
	digitalWrite(LED,HIGH);														// LED off
	
	// close
	SubGHz.close();																// Sub-GHz module sets into power down mode.

  delay(100);
}
