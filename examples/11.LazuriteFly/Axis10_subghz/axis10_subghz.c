#include "axis10_subghz_ide.h"		// Additional Header


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

#define SUBGHZ_CH			36
#define SUBGHZ_PANID		0xABCD
#define SUBGHZ_RATE			100
#define SUBGHZ_PWR			20
#define SUBGHZ_RXADDR		0x7fad

bool kxg03_irq = false;
static char txdata[256];

static void kxg03_isr(void)
{
	kxg03_irq=true;
}

void setup() {
  byte rc;

  Serial.begin(115200);
  SubGHz.init();
  SubGHz.begin(SUBGHZ_CH,SUBGHZ_PANID,SUBGHZ_RATE,SUBGHZ_PWR);
  SubGHz.setAckReq(false);
  
  Wire.begin();
  
//  rc = kxg03.set_acc_range(KXG03_ACC_RANGE_16G);
//  if(rc!=0) while(1);
//  rc = kxg03.set_gyro_range(KXG03_GYRO_RANGE_2048);
  if(rc!=0) while(1);
//  rc = kxg03.set_acc_avr(KXG03_ACC_AVR_128);
  if(rc!=0) while(1);
  rc = kxg03.sync_init(KXG03_DEVICE_ADDRESS_4E, KXG03_ODR_100HZ,kxg03_isr);
  rc = bm1422.init(0);
  rc = bm1383.init(0);
}

void loop() {
	byte rc;
	
	static float val[11];

	wait_event(&kxg03_irq);
	Print.init(txdata,sizeof(txdata));
	
	rc = kxg03.get_val(&val[0]);
	bm1422.get_val(&val[6]);
	bm1383.get_val(&val[9]);  
	
    Print.p("STX,");
    Print.d((double)val[0], 2);		// Gyro(x)
    Print.p(",");
    Print.d((double)val[1], 2);		// Gyro(y)
    Print.p(",");
    Print.d((double)val[2], 2);		// Gyro(z)
    Print.p(",");
    Print.d((double)val[3], 2);		// Acc(x)
    Print.p(",");
    Print.d((double)val[4], 2);		// Acc(y)
    Print.p(",");
    Print.d((double)val[5], 2);		// Acc(z)
    Print.p(",");
    Print.d((double)val[6], 2);		// Mag(x)
    Print.p(",");
    Print.d((double)val[7], 2);		// Mag(y)
    Print.p(",");
    Print.d((double)val[8], 2);		// Mag(z)
    Print.p(",");
    Print.d((double)val[9], 2);		// Pressure
    Print.p(",ETX");
    Print.ln();
    SubGHz.send(SUBGHZ_PANID,SUBGHZ_RXADDR,txdata,Print.len(),NULL);
    
}
