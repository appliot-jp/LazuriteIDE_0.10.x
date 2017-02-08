#include "voltage_check_ide.h"		// Additional Header

/* FILE NAME: voltage_check.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
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

void print_voltage(uint8_t level) {
	Serial.print("Current VDD voltage : ");
	switch (level) {
	case VLS_UNDER_1_898:
		Serial.println("< 1.898V");
		break;
	case VLS_1_898:
		Serial.println("1.898 ~ 2.000V");
		break;
	case VLS_2_000:
		Serial.println("2.000 ~ 2.093V");
		break;
	case VLS_2_093:
		Serial.println("2.093 ~ 2.196V");
		break;
	case VLS_2_196:
		Serial.println("2.196 ~ 2.309V");
		break;
	case VLS_2_309:
		Serial.println("2.309 ~ 2.409V");
		break;
	case VLS_2_409:
		Serial.println("2.409 ~ 2.605V");
		break;
	case VLS_2_605:
		Serial.println("2.605 ~ 2.800V");
		break;
	case VLS_2_800:
		Serial.println("2.800 ~ 3.068V");
		break;
	case VLS_3_068:
		Serial.println("3.068 ~ 3.394V");
		break;
	case VLS_3_394:
		Serial.println("3.394 ~ 3.797V");
		break;
	case VLS_3_797:
		Serial.println("3.797 ~ 4.226V");
		break;
	case VLS_4_226:
		Serial.println("4.226 ~ 4.667V");
		break;
	case VLS_4_667:
		Serial.println("> 4.667V");
		break;
	default:
		Serial.println("unknown");
		break;
	}
}

void setup() {
	Serial.begin(115200);
}

void loop() {
	uint8_t level;

	level = voltage_check(VLS_4_667);		// start checking from 4.667V
	print_voltage(level);
	sleep(3000);
}

