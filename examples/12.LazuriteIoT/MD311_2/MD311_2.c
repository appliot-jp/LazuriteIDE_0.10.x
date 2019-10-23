#include "MD311_2_ide.h"		// Additional Header

/* FILE NAME: MD311_2.c
 * The MIT License (MIT)
 *
 * Copyright (c) 2019  Lapis Semiconductor Co.,Ltd.
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

#include <driver_gpio.h>

#define CHB						( 2 )
#define MEAS 					( 3 )
#define REASON_INT_PIN1			( 11 )
#define REASON_INT_PIN2			( 12 )
#define REASON_INT_PIN3			( 13 )

void reason_init(void) {
	pinMode(REASON_INT_PIN1,INPUT_PULLUP);	// Active LOW
	pinMode(REASON_INT_PIN2,INPUT_PULLUP);	// Active LOW
	pinMode(REASON_INT_PIN3,INPUT_PULLUP);	// Active LOW
}

/*
 * initializaing function
 * this function is called in initalizing process
 * return filename
 */
char* sensor_init() {
	static char filename[] = __FILE__;
	pinMode(CHB,OUTPUT);
	pinMode(MEAS,OUTPUT);
	analogReadResolution(12);
	reason_init();
	return filename;
}
/*
 * callback function of activation
 * return  true : sensor_meas is called after interval
 *         false: sensor_meas is called immidialtely
 */
bool sensor_activate(void) {
	return false;
}
/*
 * callback function of deactivation
 */
void sensor_deactivate(void) {
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
	int *reason = &(s[0].reason), reason_val;
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
		sleep(1);
		en_voltage = analogRead(A0);
		en_time = millis();
	} while(((en_time - st_time) < 1000) && (en_voltage < 1024));
	digitalWrite(CHB,LOW);
	digitalWrite(MEAS,LOW);
	amps = 3.3*1.1;
	dif_vol = en_voltage-st_voltage;
	dif_vol = (dif_vol < 0) ? 0 : dif_vol;
	amps = amps * dif_vol /4.096/(en_time - st_time);
	val->data.double_val = amps;
	val->type = DOUBLE_VAL;
	val->digit = 2;
	// check reason
	reason_val = digitalRead(REASON_INT_PIN1) ? 0 : 1;
	reason_val |= digitalRead(REASON_INT_PIN2) ? 0 : 1 << 1;
	reason_val |= digitalRead(REASON_INT_PIN3) ? 0 : 1 << 2;
	Serial.println_long(reason_val,DEC);
	if (reason_val) {
		*reason = reason_val;
	} else {
		*reason = INVALID_REASON;
	}
	return;
}
