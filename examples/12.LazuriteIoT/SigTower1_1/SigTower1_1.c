#include "SigTower1_1_ide.h"		// Additional Header

/* FILE NAME: ALSensor_6.c
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

static int data_buf_index = 0;
#define DATA_BUF_LENGTH  8
static float data_buf[DATA_BUF_LENGTH];
static bool timer2_flag = false;

static void mstimer2_isr(void) {
	timer2_flag = true;
	return;
}

char* sensor_init() {
	static char filename[] = __FILE__;
	analogReadResolution(12);

	return filename;
}

/*
 * callback function of activation
 * argument interval: sense interval during initialization
 * return  true : sensor_meas is called after interval
 *         false: sensor_meas is called immidialtely
 */
bool sensor_activate(uint32_t *interval) {	
	*interval = 5000ul; // dummy
	Serial.begin(115200);
	return true;
}
/*
 * callback function of deactivation
 */
void sensor_deactivate(void) {
	timer2.stop();
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
void sensor_meas(SensorState s[]) {
	SENSOR_VAL *val = &(s[0].sensor_val);
	double sum_als = 0;
	double als_val = 0;
	int i;

	uint32_t remain_time;

	timer2.set(100L,mstimer2_isr);
	timer2.start();

	for(i = 0; i < 20; i++) {
		remain_time = wait_event_timeout(&timer2_flag,1000);
		if(remain_time == 0) {
			break;
		}
		sum_als  += analogRead(14);
	}
	if(i>0) {
		als_val = sum_als/i;
	} else {
		als_val = 0;
	}
	val->data.double_val=als_val;  val->type = DOUBLE_VAL; val->digit = 2;

	Serial.print("STX,");
	Serial.print_double((double)als_val,2);
	Serial.println(",ETX");

	return;
}
