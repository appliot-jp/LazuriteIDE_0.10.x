#include "pr_serial_ide.h"		// Additional Header

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

bool kxg03_irq = false;
static char txdata[256];

static void kxg03_isr(void)
{
	kxg03_irq=true;
}

static float val[16];
void setup() {
  byte rc;
  int i;
  Serial.begin(115200);
  
  Wire.begin();
  
  rc = kxg03.sync_init(KXG03_DEVICE_ADDRESS_4E, KXG03_ODR_50HZ,kxg03_isr);

  /* raw data output
  kxg03.angle_init(KXG03_MODE_RAW ,3,2,1,2,-3,-1);
  kxg03.set_acc_out(true);
  kxg03.set_gyro_out(true);
  */
  
  kxg03.angle_init(KXG03_MODE_PR | KXG03_MODE_KALMAN ,3,2,1,2,-3,-1);
  kxg03.set_acc_out(true);
  kxg03.set_gyro_out(true);
  kxg03.set_kalman_out(true);
  kxg03.set_deg_out(true);

  for(i=0;i<16;i++)
  {
  	val[i] = 0;
  }
  
}

void loop() {
	byte rc;
	
	wait_event(&kxg03_irq);
	kxg03.get_angle(val);
	Print.init(txdata,sizeof(txdata));
		
    Print.p("STX,");
    Print.d((double)val[0], 2);		// kalman pitch
    Print.p(",");
    Print.d((double)val[1], 2);		// kalman roll
    Print.p(",");
    Print.d((double)val[2], 2);		// acc deg
    Print.p(",");
    Print.d((double)val[3], 2);		// acc deg
    Print.p(",");
    Print.d((double)val[4], 2);		// gyro x
    Print.p(",");
    Print.d((double)val[5], 2);		// gyro y
    Print.p(",");
    Print.d((double)val[6], 2);		// gyro z
    Print.p(",");
    Print.d((double)val[7], 2);		// acc x
    Print.p(",");
    Print.d((double)val[8], 2);		// acc y
    Print.p(",");
    Print.d((double)val[9], 2);		// acc z
    Print.p(",ETX");
    Print.ln();

    Serial.print(txdata);
    
}
