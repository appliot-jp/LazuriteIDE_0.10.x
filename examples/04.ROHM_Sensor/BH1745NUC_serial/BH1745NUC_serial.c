#include "BH1745NUC_serial_ide.h"		// Additional Header

/* FILE NAME: BH1745_serial.c
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


void setup() {
  byte rc;

  Serial.begin(115200);
  Wire.begin();
  rc = bh1745.init(BH1745NUC_DEVICE_ADDRESS_39);
}

void loop() {
  byte rc;
  unsigned short rgbc[4];

  rc = bh1745.get_val(rgbc);
  /*  if (rc == 0) {
      Serial.print("BH1745NUC (RED)   = ");
      Serial.println_long(rgbc[0],DEC);
      Serial.print("BH1745NUC (GREEN) = ");
      Serial.println_long(rgbc[1],DEC);
      Serial.print("BH1745NUC (BLUE)  = ");
      Serial.println_long(rgbc[2],DEC);
      Serial.print("BH1745NUC (CLEAR) = ");
      Serial.println_long(rgbc[3],DEC);
      Serial.println("");
    }
    */
  Serial.print("STX,");
  Serial.print_long(rgbc[0], 0);
  Serial.print(",");
  Serial.print_long(rgbc[1], 0);
  Serial.print(",");
  Serial.print_long(rgbc[2], 0);
  Serial.print(",");
  Serial.print_long(rgbc[3], 0);
  Serial.println(",ETX");
  sleep(50);

}
