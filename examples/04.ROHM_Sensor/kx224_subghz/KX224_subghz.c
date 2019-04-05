#include "kx224_subghz_ide.h"		// Additional Header

/*****************************************************************************
  KX224_subghz.c

 Copyright (c) 2017 LAPIS Semiconductor Co.,Ltd.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
******************************************************************************/#include <Wire.h>

#define LED 26						// pin number of Blue LED
#define SUBGHZ_CH		36			// channel number (frequency)
#define SUBGHZ_PANID	0xABCD		// panid
#define HOST_ADDRESS	0xC345		// distination address


char tx_buf[128];

void setup() {
  byte rc;

  Serial.begin(119200);
  SubGHz.init();
 // while (!Serial);

  Wire.begin();

  rc = kx224.init(KX224_DEVICE_ADDRESS_1E);
  pinMode(LED, OUTPUT);			// setting of LED
  digitalWrite(LED, HIGH);			// setting of LED

  if (rc != 0) {
    Serial.println("KX224 initialization failed");
    Serial.flush();
  }
}

void loop() {
  byte rc;

  static float val[3];

  rc = kx224.get_val(&val[0]);

  Print.init(tx_buf, sizeof(tx_buf));
  Print.p("STX,");
  Print.f(val[0], 2);
  Print.p(",");
  Print.f(val[1], 2);
  Print.p(",");
  Print.f(val[2], 2);
  Print.p(",ETX,");
  Print.ln();
  
//  Serial.print(tx_buf);

	// Initializing
	SubGHz.setAckReq(false);
	SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);		// start Sub-GHz
	
	// preparing data
	digitalWrite(LED,LOW);														// LED ON
	SubGHz.send(SUBGHZ_PANID, HOST_ADDRESS, tx_buf, Print.len(),NULL);// send data
	digitalWrite(LED,HIGH);														// LED off
	
	// close
	SubGHz.close();																// Sub-GHz module sets into power down mode.

  delay(100);

}
