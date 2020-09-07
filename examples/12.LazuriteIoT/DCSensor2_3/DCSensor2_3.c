#include "DCSensor2_3_ide.h"		// Additional Header

/* FILE NAME: DCSensor2_3.c
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

#define PLC_IN_NUM			( 6 )
#define PLC_IN0				( 8 )
#define DC_SENSOR_NUM		( 2 )
#define DC_SENSOR_AIN0		( 14 )

#if DC_SENSOR_NUM+PLC_IN_NUM > MAX_SENSOR_NUM
	#error The number of sensor exceeds MAX_SENSOR_NUM.
#endif

void callback(void)
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
	int i;

	analogReadResolution(12);
	useInterruptFlag = true;
	for (i=0; i<PLC_IN_NUM; i++) {
		pinMode((uint8_t)(PLC_IN0+i), INPUT);
	}

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
	timer2.set(100L,callback);
	timer2.start();
	Serial.begin(115200);

	return false;
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
	SENSOR_VAL *val;
	uint16_t data[DC_SENSOR_NUM+PLC_IN_NUM];
	int i;

	for (i=0; i<DC_SENSOR_NUM; i++) {
		val = &(s[i].sensor_val);
		data[i] = analogRead((uint8_t)(DC_SENSOR_AIN0+i));
		val->data.uint16_val = data[i];
		val->type = UINT16_VAL;
	}

	for (i=0; i<PLC_IN_NUM; i++) {
		val = &(s[DC_SENSOR_NUM+i].sensor_val);
		data[DC_SENSOR_NUM+i] = digitalRead((uint8_t)(PLC_IN0+i)) == 0 ? 1 : 0; // invert, active low
		val->data.uint16_val = data[DC_SENSOR_NUM+i];
		val->type = UINT16_VAL;
	}

	Serial.print("STX");
	for (i=0; i<DC_SENSOR_NUM+PLC_IN_NUM; i++) {
		Serial.print(",");
		Serial.print_long((long)data[i],DEC);
	}
	Serial.println(",ETX");
	return;
}

