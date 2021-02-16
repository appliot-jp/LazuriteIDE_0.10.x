#include "SigTower3_1_ide.h"		// Additional Header

/* FILE NAME: SignalTower_1.c
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

/*
 * initializaing function
 * this function is called in initalizing process
 * return filename
 */
#define ORANGE_LED				( 25 )
#define BLUE_LED				( 26 )

#define SENSOR_NUM 3

static bool timer2_flag = false;

static void mstimer2_isr(void) {
	timer2_flag = true;
	return;
}

char* sensor_init() {
	static char filename[] = __FILE__;
//	analogReadResolution(14);
	return filename;
}

/*
 * callback function of activation
 * argument interval: sense interval during initialization
 * return  true : sensor_meas is called after interval
 *         false: sensor_meas is called immidialtely
 */

extern SensorState Sensor[MAX_SENSOR_NUM];
static uint16_t thrs0,thrs1;
bool sensor_activate(uint32_t *interval) {
	SensorState *ssp = &Sensor[0];
	thrs0 = ssp->thrs_on_val/2;
	thrs1 = ssp->thrs_on_val;	
	Serial.begin(115200);
	return true;
}
/*
 * callback function of deactivation
 */
void sensor_deactivate(void) {
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
static const uint8_t led_brink[] = {1,3,9,27};
static const uint8_t led_on[] = {2,6,18,54};
static int chat_reason = -1;
void sensor_meas(SensorState s[]) {
	SensorState *ssp = &Sensor[0];
	SENSOR_VAL *val = &(s[0].sensor_val);
	uint16_t raw[4] = {0,0,0,0};
	int i,j;
	uint32_t remain_time;
	int reason = 0;

	timer2.set(100L,mstimer2_isr);
	timer2.start();

	for(i = 0; i < 16; i++) {
		remain_time = wait_event_timeout(&timer2_flag,1000);
		for(j=0;j<SENSOR_NUM;j++) {
			raw[j] += analogRead(14+j)/16;
		}
	}
	timer2.stop();
	Serial.print("STX,");
	for(j=0;j<SENSOR_NUM;j++) {
		if(raw[j] < thrs0) {
		} else if( raw [j] < thrs1) {
			reason += led_brink[j];
		} else {
			reason += led_on[j];			
		}
		Serial.print_long(raw[j],DEC);
		Serial.print(",");
	}
	Serial.print_long(val->reason,DEC);
	Serial.println(",ETX");
	if(chat_reason == -1) {
		chat_reason = reason;
		val->reason = reason;
	} else if(reason == chat_reason) {
		val->reason = chat_reason;
	} else {
		chat_reason = reason;
	}
	if(val->reason != 2) {
		val->data.uint16_val= 0;  val->type = UINT16_VAL;
	} else {
		val->data.uint16_val=raw[0];  val->type = UINT16_VAL;
	}
	return;
}
