#include "Axis10_serial_ide.h"		// Additional Header


/* FILE NAME: Axis10_serial.c
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
  
  rc = kxg03.init(KXG03_DEVICE_ADDRESS_4E);
  rc = bm1422.init(0);
  rc = bm1383.init(0);
}



void loop() {
	byte rc;
	
	static float val[11];

	rc = kxg03.get_val(&val[0]);
	bm1422.get_val(&val[6]);
	bm1383.get_val(&val[9]);  

    Serial.print("STX,");
    Serial.print_double((double)val[0], 2);		// Gyro(x)
    Serial.print(",");
    Serial.print_double((double)val[1], 2);		// Gyro(y)
    Serial.print(",");
    Serial.print_double((double)val[2], 2);		// Gyro(z)
    Serial.print(",");
    Serial.print_double((double)val[3], 2);		// Acc(x)
    Serial.print(",");
    Serial.print_double((double)val[4], 2);		// Acc(y)
    Serial.print(",");
    Serial.print_double((double)val[5], 2);		// Acc(z)
    Serial.print(",");
    Serial.print_double((double)val[6], 2);		// Mag(x)
    Serial.print(",");
    Serial.print_double((double)val[7], 2);		// Mag(y)
    Serial.print(",");
    Serial.print_double((double)val[8], 2);		// Mag(z)
    Serial.print(",");
    Serial.print_double((double)val[10], 4);		// Pressure
    Serial.println(",ETX");
    
    delay(100);
}
