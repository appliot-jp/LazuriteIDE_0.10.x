#include "CT_Sensor_SubGHz_ide.h"		// Additional Header

/* FILE NAME: CT_Sensor.c
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

#define CHB	2
#define MEAS 3
#define MEAS_LED	25
#define TX_LED		26
#define TX_INTERVAL	30*1000

#define SUBGHZ_CH	36
#define MY_PANID	0xABCD

#define RX_ADDR		0xFFFF
#define RX_PANID	0xABCD
#define BAUD		100
#define PWR			20


bool event = false;
void callback(void)
{
	event = true;
}

const char vls_val[][8] ={
	"0",		//	"invalid",
	"0",		//	"invalid",
	"1.8",		//	"< 1.898V"
	"1.95",		//	"1.898 ~ 2.000V"
	"2.05",		//	"2.000 ~ 2.093V"
	"2.15",		//	"2.093 ~ 2.196V",
	"2.25",		//	"2.196 ~ 2.309V",
	"2.35",		//	"2.309 ~ 2.409V",
	"2.5",		//	"2.409 ~ 2.605V",
	"2.7",		//	"2.605 ~ 2.800V",
	"2.9",		//	"2.800 ~ 3.068V",
	"3.2",		//	"3.068 ~ 3.394V",
	"3.6",		//	"3.394 ~ 3.797V",
	"4.0",		//	"3.797 ~ 4.226V",
	"4.4",		//	"4.226 ~ 4.667V",
	"4.7"		//	"> 4.667V"
};

void ct_init() {
	pinMode(CHB,OUTPUT);
	pinMode(MEAS,OUTPUT);
	analogReadResolution(12);
}
double ct_meas() {
	unsigned long  st_time,en_time;
	double amps;
	volatile int st_voltage,en_voltage,dif_vol;
	digitalWrite(MEAS,HIGH);
	digitalWrite(CHB,HIGH);
	st_voltage = analogRead(A0);
	delayMicroseconds(1000);
	st_voltage = analogRead(A0);
	delayMicroseconds(1000);
	st_voltage = analogRead(A0);
	delayMicroseconds(1000);
	st_voltage = analogRead(A0);
	st_time = millis();
	do {
		delayMicroseconds(1000);
		en_voltage = analogRead(A0);
		en_time = millis();
	} while(((en_time - st_time) < 1000) && (en_voltage < 1024));
	digitalWrite(CHB,LOW);
	digitalWrite(MEAS,LOW);
	amps = 3.3*1.1;
	dif_vol = en_voltage-st_voltage;
	dif_vol = (dif_vol < 0) ? 0 : dif_vol;
	amps = amps * dif_vol /4.096/(en_time - st_time);  
	return amps;
}

void setup() {
	// put your setup code here, to run once:
	int i=0;
	SUBGHZ_PARAM param;
	
	ct_init();
	
	digitalWrite(MEAS_LED,HIGH);
	pinMode(MEAS_LED,OUTPUT);
	digitalWrite(TX_LED,HIGH);
	pinMode(TX_LED,OUTPUT);
  
	timer2.set(TX_INTERVAL,callback);
	event = true;

	timer2.start();
	SubGHz.init();
	SubGHz.getSendMode(&param);
	param.txRetry = 3;
	SubGHz.setSendMode(&param);
}
char txbuf[250];
void loop() {
	// put your main code here, to run repeatedly:
	double amps;
	uint8_t level;
	
	Print.init(txbuf,sizeof(txbuf));

	// waiting timeout
	wait_event(&event);
	event = false;

	// start measuring
	amps = ct_meas();
	level = voltage_check(VLS_4_667);	
	
	Print.d(amps,2);
	Print.p(",");
	Print.p(vls_val[level]);
	Print.ln();

	digitalWrite(TX_LED,LOW);
	SubGHz.begin(SUBGHZ_CH,MY_PANID,BAUD,PWR);
	SubGHz.send(RX_PANID,RX_ADDR,txbuf,Print.len(),NULL);
	SubGHz.close();
	digitalWrite(TX_LED,HIGH);
	
}
