#include "HallDoor_2_ide.h"		// Additional Header

/* FILE NAME: HallDoor_2.c
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

#define HALL_PIN				( 3 )
#define HALL_INT				( 1 )

void hall_isr(void)
{
	waitEventFlag = true;
}

/*
 * initializaing function
 * this function is called in initalizing process
 * return filename
 */
char* sensor_init() {
	static char filename[] = __FILE__;

	pinMode(HALL_PIN, INPUT);
	
	return filename;
}

/*
 * callback function of activation
 * return  true : sensor_meas is called after interval
 *         false: sensor_meas is called immidialtely
 */
bool sensor_activate(void) {
	attachInterrupt(HALL_INT, hall_isr, CHANGE);
	return false;
}
/*
 * callback function of deactivation
 */
void sensor_deactivate(void) {
	detachInterrupt(HALL_INT);
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

	val->data.uint8_val=!digitalRead(HALL_PIN);  val->type = UINT8_VAL;
	
	return;
}

