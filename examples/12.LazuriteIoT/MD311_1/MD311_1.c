#include "MD311_1_ide.h"		// Additional Header

/* FILE NAME: MD311_1.c
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
#include <driver_extirq.h>

//#define DEBUG	// uncomment, if using debug message

#define CHB						( 2 )
#define MEAS 					( 3 )
#define REASON_INT_PIN1			( 24 )	// pin #11
#define REASON_INT_PIN2			( 25 )	// pin #12
#define REASON_INT_PIN3			( 18 )	// pin #13
#define STOP_SIGNAL_PIN			( 10 )	// stop signal output pin
#define STOP_SIG_INTERVAL		( 200 )	// ms, assertion interval of stop signal
#define INT_PIN_DEAD_TIME		( 100 )	// ms, dead time of input pin

uint32_t int_pin_timestamp=0;
uint8_t reason_val;

void reason_isr1(void) {
	if (millis()-int_pin_timestamp > INT_PIN_DEAD_TIME) {
		int_pin_timestamp = millis();
		reason_val = 1;
	}
}

void reason_isr2(void) {
	if (millis()-int_pin_timestamp > INT_PIN_DEAD_TIME) {
		int_pin_timestamp = millis();
		reason_val = 2;
	}
}

void reason_isr3(void) {
	if (millis()-int_pin_timestamp > INT_PIN_DEAD_TIME) {
		int_pin_timestamp = millis();
		reason_val = 3;
	}
}

void reason_enable_interrupt(void) {
	drv_attachInterrupt(REASON_INT_PIN1,0,reason_isr1,FALLING,false,false);
	drv_attachInterrupt(REASON_INT_PIN2,1,reason_isr2,FALLING,false,false);
	drv_attachInterrupt(REASON_INT_PIN3,2,reason_isr3,FALLING,false,false);
}

void reason_disable_interrupt(void) {
	drv_detachInterrupt(0);
	drv_detachInterrupt(1);
	drv_detachInterrupt(2);
}

void timer_isr(void) {
	digitalWrite(STOP_SIGNAL_PIN,LOW);
	timer2.stop();
}

void state_change_cb(uint8_t state) {
	reason_val = 0;
	if (state == STATE_TO_OFF) {
		timer2.stop();
		timer2.set((long)STOP_SIG_INTERVAL,timer_isr);
		digitalWrite(STOP_SIGNAL_PIN,HIGH);
		timer2.start();
	}
	return;
}

void reason_init(void) {
	pinMode(STOP_SIGNAL_PIN,OUTPUT);
	digitalWrite(STOP_SIGNAL_PIN,LOW);		// Active HIGH
	drv_pinMode(REASON_INT_PIN1,INPUT_PULLUP);	// Active LOW
	drv_pinMode(REASON_INT_PIN2,INPUT_PULLUP);	// Active LOW
	drv_pinMode(REASON_INT_PIN3,INPUT_PULLUP);	// Active LOW
	reason_enable_interrupt();
	sensor_set_state_change_callback(state_change_cb);
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
	reason_enable_interrupt();
	return false;
}
/*
 * callback function of deactivation
 */
void sensor_deactivate(void) {
	reason_disable_interrupt();
}

/*
 * function of sensor measurement
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
void sensor_meas(SENSOR_VAL *val) {
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
	if (reason_val) {
#ifdef DEBUG
		Serial.print("stop reason: ");
		Serial.println_long((long)reason_val, DEC);
#endif
		val->reason = reason_val;
	}
	return;
}
