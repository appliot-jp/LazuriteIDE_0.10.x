#include "ecompus2_ide.h"		// Additional Header


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

static  float amax[3];
static  float amin[3];
static  float mmax[3];
static  float mmin[3];
static  float moff[3];

static volatile float adata[3];
static volatile float mdata[3];

void setup() {
  byte rc;
  int i;

  Serial.begin(115200);
  
  Wire.begin();
  
//  rc = kxg03.set_acc_range(KXG03_ACC_RANGE_16G);
//  if(rc!=0) while(1);
//  rc = kxg03.set_gyro_range(KXG03_GYRO_RANGE_2048);
//  if(rc!=0) while(1);
//  rc = kxg03.set_acc_avr(KXG03_ACC_AVR_128);
//  if(rc!=0) while(1);
  rc = kxg03.sync_init(KXG03_DEVICE_ADDRESS_4E, KXG03_ODR_6_25HZ,kxg03_isr);
  rc = bm1422.init(0);
//  rc = bm1383a.init(0)

  Serial.println("initializing parameters......");
  
  // waiting first data
  wait_event(&kxg03_irq);

  // initializing min/max
  kxg03.get_acc(amax);
  bm1422.get_val(mmax);
  for(i=0;i<3;i++)
  {
  	amin[i] = amax[i];
  	mmin[i] = mmax[i];
  }

  Serial.println("calibrating......");

  while(1)
  {
	  wait_event(&kxg03_irq);
	
	  // update data
	  kxg03.get_acc(adata);
	  bm1422.get_val(mdata);
	  for(i=0;i<3;i++)
	  {
	  	if(adata[i] > amax[i]) amax[i] = adata[i];
	  	if(adata[i] < amin[i]) amin[i] = adata[i];
	  	if(mdata[i] > mmax[i]) mmax[i] = mdata[i];
	  	if(mdata[i] < mmin[i]) mmin[i] = mdata[i];
	  }

	  if((amax[0]>0.5) && (amax[1]>0.5) && (amax[2]>0.5) &&
	     (amin[0]<-0.5) && (amin[1]<-0.5) && (amin[2]<-0.5))
	  	   break;
  }
  
  for(i=0;i<3;i++)
  {
  	moff[i] = (mmax[i]+mmin[i])/2;
  }
  
  rc = kxg03.angle_init(KXG03_MODE_KALMAN | KXG03_MODE_PR, 1, -2, 3, 2,1,3);
  kxg03.set_kalman_out(true);
//  kxg03.set_acc_out(true);
//  kxg03.set_gyro_out(true);
  Serial.println("end of calibration");
}

float radPitch,radRoll;
float mx2,my2,gmag;

void loop() {
	byte rc;
	
	static float val[11];

	wait_event(&kxg03_irq);
	Print.init(txdata,sizeof(txdata));
	
	rc = kxg03.get_angle(&val[0]);
	bm1422.get_val(&val[3]);
	
    radPitch = val[0] / RAD_TO_DEG;
    radRoll = val[1] / RAD_TO_DEG;

	mx2 = (val[3]-moff[0]) * cos(radRoll) + (val[5]-moff[2]) * sin(radRoll);
	my2 = -1 * (val[3] - moff[0]) * sin(radPitch) * sin(radRoll) + (val[4]-moff[1]) * cos(radPitch) - (val[5]-moff[2]) * sin(radPitch) * cos(radRoll);
	gmag = 90 - atan2(my2,mx2)*RAD_TO_DEG;
	
    
    Print.p("STX,");
    Print.d((double)val[0], 2);		// pitch(x)
    Print.p(",");
    Print.d((double)val[1], 2);		// roll(y)
    Print.p(",");
    Print.d((double)val[3]-moff[0],0);		// mag(x)
    Print.p(",");
    Print.d((double)val[4]-moff[1],0);		// mag(y)
    Print.p(",");
    Print.d((double)val[5]-moff[2],0);		// mag(z)
    Print.p(",");
    Print.d((double)gmag, 2);				// geomag(x)

        
    Print.p(",ETX\r\n");
 
    Serial.print(txdata);
    
}
