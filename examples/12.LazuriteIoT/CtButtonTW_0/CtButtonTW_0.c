#include "CtButtonTW_0_ide.h"		// Additional Header

/* FILE NAME: CTSensorTW2_10.c
 * The MIT License (MIT)
 *
 * Copyright (c) 2018  Lapis Semiconductor Co.,Ltd.
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

#define CHB						( 2 )
#define MEAS 					( 3 )

#define ORANGE_LED		(25)
#define BLUE_LED			(26)
#define SW_AC100V			(19)

/*
 * initializaing function
 * this function is called in initalizing process
 * return filename
 */

struct button btn0[] = {
	{
		1,	// push
		13,	// pin
		0	// extirq
	},
	{
		1,	// push
		12,	// pin
		1	// extirq
	},
	{
		1,	// push
		11,	// pin
		2	// extirq
	},
	{
		1,	// push
		10,	// pin
		4	// extirq
	}
};
struct button_config btn = {
	4,		// num
	0,		// event
	false,	// wait_flag
	1,		// chat
	0,		// cont
	1,		// cont_interval
	100,	// timer = 100ms
	btn0
};

static uint8_t button_state = 0;
static uint8_t buttonLedList[] = {
	9,
	8,
	7,
	6,
};

static const uint8_t buttonBit[]={0x01,0x02,0x04,0x10};
void buttonCallback() {	
	uint8_t i=0;
	digitalWrite(BLUE_LED,!digitalRead(BLUE_LED));
//	Serial.print_long(btn.event,HEX);
	for(i=0;i<sizeof(buttonBit);i++) {
//		Serial.print(",");
//		Serial.print_long(btn.event & buttonBit[i],HEX);
		if(btn.event & buttonBit[i]) {
			if(button_state == (i+1)) {
				button_state = 0;
			} else {
				button_state = i+1;
			}
			break;
		}
	}
	for(i=0;i<sizeof(buttonBit);i++) {
		if(button_state == (i+1)) {
			digitalWrite(buttonLedList[i],HIGH);
		} else {
			digitalWrite(buttonLedList[i],LOW);
		}
	}
	btn.event = 0;
//	Serial.println_long(button_state,DEC);
}

 
char* sensor_init() {
	int i;
	static char filename[] = __FILE__;
	pinMode(CHB,OUTPUT);
	pinMode(MEAS,OUTPUT);
	digitalWrite(19,LOW);
	pinMode(19,OUTPUT);
	analogReadResolution(12);

	buttonDetect(&btn,buttonCallback);
	for(i=0;i<sizeof(buttonLedList);i++) {
		pinMode(buttonLedList[i],OUTPUT);
		digitalWrite(buttonLedList[i],LOW);
	}
	
	return filename;
}
/*
 * callback function of activation
 * return  true : sensor_meas is called after interval
 *         false: sensor_meas is called immidialtely
 */
bool sensor_activate(uint32_t *interval) {
	*interval = 5000ul;
	return false;
}
/*
 * callback function of deactivation
 */
void sensor_deactivate(void) {
	return;
}

/*
 * function of sensor measurement
 *
 * s[]: Array of SensorState is passed. If single sensor type, array size is always '1'.
 *
 * val->data is settled depends on data type
 * data type is set into val->type
 * val->digit shows digit of floating number.
 * 
 * val->data.uint8_val=xxx;   val->type = UINT8_VAL;
 * val->data.int8_val=xxx;    val->type = INT8_VAL;
 * val->data.uint16_val=xxx;  val->type = UINT16_VAL;
 * val->data.int16_val=xxx;   val->type = INT16_VAL;
 * val->data.uint32_val=xxx;  val->type = UINT32_VAL;
 * val->data.int32_val=xxx;   val->type = UINT32_VAL;
 * val->data.float_val=xxx;   val->type = FLOAT_VAL;  val->digit = d;
 * val->data.double_val=xxx;  val->type = DOUBLE_VAL; val->digit = d;
 */
void sensor_meas(SensorState s[]) {
	SENSOR_VAL *val = &(s[0].sensor_val);
	unsigned long  st_time,en_time;
	double amps;
	volatile int st_voltage,en_voltage,dif_vol;
	digitalWrite(MEAS,HIGH);
	digitalWrite(CHB,HIGH);

	st_voltage = analogRead(A0);
	sleep(1);
	st_voltage = analogRead(A0);
	sleep(1);
	st_voltage = analogRead(A0);
	sleep(1);
	st_voltage = analogRead(A0);
	st_time = millis();
	do {
//		sleep(1);
		en_voltage = analogRead(A0);
		en_time = millis();
	} while(((en_time - st_time) < 1000) && (en_voltage < 1024));
	digitalWrite(CHB,LOW);
	digitalWrite(MEAS,LOW);
	amps = 3.3*1.1;
	dif_vol = en_voltage-st_voltage;
	dif_vol = (dif_vol < 0) ? 0 : dif_vol;
	amps = amps * dif_vol /4.096/(en_time - st_time);
	
//	Serial.println_double(amps,2);
	val->data.double_val = amps;
	val->type = DOUBLE_VAL;
	val->digit = 2;
	val->reason = button_state;

	if(button_state != 0) {
		digitalWrite(19,HIGH);
	} else {
		digitalWrite(19,LOW);
	}
	
	return;
}

