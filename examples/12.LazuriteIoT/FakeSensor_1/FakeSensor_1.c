#include "FakeSensor_1_ide.h"		// Additional Header

/* FILE NAME: FakeSensor_1.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2020  Lapis Semiconductor Co.,Ltd.
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
 * sensor value: 0 or 1 (default: random)
 * interval range(min): on [1 ~ 30], off [0.5 ～ 15]
 */
#define INT_ON_MIN ( 1*60 )
#define INT_ON_MAX ( 30*60 )
#define INT_OFF_MIN ( 30 )
#define INT_OFF_MAX ( 15*60 )
uint8_t value[MAX_SENSOR_NUM];
uint32_t last[MAX_SENSOR_NUM]={0},interval[MAX_SENSOR_NUM]={0};

/*
 * initializaing function
 * this function is called in initalizing process
 * return filename
 */
char* sensor_init() {
	static char filename[] = __FILE__;
	return filename;
}
/*
 * callback function of activation
 * return  true : sensor_meas is called after interval
 *         false: sensor_meas is called immidialtely
 */
bool sensor_activate(void) {
	SUBGHZ_STATUS rx;
	int i;

	Serial.begin(115200);
	SubGHz.getStatus(NULL,&rx);
	srand(rand()+rx.rssi);
	for (i=0;i<MAX_SENSOR_NUM;i++) value[i] = (uint8_t)(rand() & 1);
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
	SENSOR_VAL *val;
	int i;

	for (i=0;i<MAX_SENSOR_NUM;i++) {
		if (s[i].id == INVALID_ID) continue;
		val = &(s[i].sensor_val);
		if (millis() - last[i] > interval[i]) {
			last[i] = millis();
			Serial.print("idx=");
			Serial.print_long((long)i,DEC);
			Serial.print(", last: ");
			Serial.print_long((long)last[i],DEC);
			if (value[i] == 0) { // off -> on
				interval[i] = ((rand() & (INT_ON_MAX - INT_ON_MIN)) + INT_ON_MIN) * 1000ul;
				interval[i] *= fabs(sin((double)interval[i]));
				value[i] = 1;
			} else { // on -> off
				interval[i] = ((rand() & (INT_OFF_MAX - INT_OFF_MIN)) + INT_OFF_MIN) * 1000ul;
				interval[i] *= fabs(cos((double)interval[i]));
				value[i] = 0;
			}
//			interval[i] = 15*1000L; // test, fixed interval
			Serial.print(",\tint: ");
			Serial.print_long((long)interval[i],DEC);
			Serial.print(",\tval: ");
			Serial.println_long((long)value[i],DEC);
			delay(5);
		}
		val->data.uint8_val = value[i];
		val->type = UINT8_VAL;
	}
	return;
}

