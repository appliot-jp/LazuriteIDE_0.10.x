#include "RPR0521_subghz_ide.h"		// Additional Header

/* FILE NAME: rpr0521_serial.c
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
#define HOST_ADDRESS	0xAC4E		// distination address


void setup(void) {
  byte rc;

  Serial.begin(115200);
  SubGHz.init();					// initializing Sub-GHz

  pinMode(LED, OUTPUT);			// setting of LED
  digitalWrite(LED, HIGH);			// setting of LED

  Wire.begin();

  rc = rpr0521rs.init();
}

char tx_buf[128];

void loop(void) {
  byte rc;
  unsigned short ps_val;
  float als_val;

  rc = rpr0521rs.get_psalsval(&ps_val, &als_val);
  if (rc == 0) {
    /*
    Serial.print("STX,");
    Serial.print_long((long)ps_val,DEC);
    Serial.print(",");
    Serial.print_double((double)als_val,0);
    Serial.println(",ETX");
    */
    Print.init(tx_buf, sizeof(tx_buf));
    Print.p("STX,");
    Print.l(ps_val, DEC);
    Print.p(",");
    Print.d(als_val, 2);
    Print.p(",ETX");
    Print.ln();
  }
  // Initializing
  SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);		// start Sub-GHz
  // preparing data
  digitalWrite(LED, LOW);														// LED ON
  SubGHz.send(SUBGHZ_PANID, HOST_ADDRESS, tx_buf, Print.len(), NULL); // send data
  digitalWrite(LED, HIGH);														// LED off

  // close
  SubGHz.close();																// Sub-GHz module sets into power down mode.

  delay(100);
}

