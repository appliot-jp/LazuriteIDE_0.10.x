#include "ALSensor2_3_ide.h"		// Additional Header

/* FILE NAME: ALSensor2_3.c
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

static int data_buf_index = 0;
#define DATA_BUF_LENGTH  8
#define ORANGE_LED 25
#define BLUE_LED 26
static float data_buf[DATA_BUF_LENGTH];

static void mstimer2_isr(void) {
	waitEventFlag = true;
	return;
}

char* sensor_init() {
	static char filename[] = __FILE__;
	//digitalWrite(ORANGE_LED,LOW);
	Wire.begin();
	return filename;
}

/*
 * callback function of activation
 * return  true : sensor_meas is called after interval
 *         false: sensor_meas is called immidialtely
 */
bool sensor_activate(void) {
	uint8_t reg = 0x86;
	for(data_buf_index=0;data_buf_index < DATA_BUF_LENGTH; data_buf_index++) {
		data_buf[data_buf_index] = 0;
	}
	data_buf_index=0;
	timer2.set(100L,mstimer2_isr);
	timer2.start();
	rpr0521rs.write(RPR0521RS_MODE_CONTROL, &reg, sizeof(reg));
	Serial.begin(115200);
	return false;
}
/*
 * callback function of deactivation
 */
void sensor_deactivate(void) {
	timer2.stop();
	//digitalWrite(ORANGE_LED,HIGH);
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
	float als_val;
	int i;
	uint8_t rawval[4];
	uint16_t rawals[2];

	rpr0521rs.read(RPR0521RS_ALS_DATA0_LSB, rawval, 4);

	rawals[0] = ((unsigned short)rawval[1] << 8) | rawval[0];
	rawals[1] = ((unsigned short)rawval[3] << 8) | rawval[2];

	als_val = rpr0521rs.convert_lux(rawals);
	data_buf[data_buf_index] = als_val;
	data_buf_index++;

	if(data_buf_index >= 8) data_buf_index = 0;

	als_val = 0;
	for(i=0;i<DATA_BUF_LENGTH;i++) {
		als_val += data_buf[i];
	}
	als_val = als_val/DATA_BUF_LENGTH;
	val->data.double_val=als_val;  val->type = DOUBLE_VAL; val->digit = 2;

	Serial.print("STX,");
	Serial.print_double((double)als_val,2);
	Serial.println(",ETX");

	return;
}
