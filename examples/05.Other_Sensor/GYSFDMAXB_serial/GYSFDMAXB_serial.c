/* FILE NAME: GYSFDMAXB_serial.c
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
 
#include "GYSFDMAXB_serial_ide.h"		// Additional Header

#define GPS_BUF_SIZE	( 220 )

uint8_t bufp = 0;
uint8_t gps_buf[GPS_BUF_SIZE];

void gps_output(void)
{
	Serial.println(gps_buf);
}

void setup() {
  // put your setup code here, to run once:
	Serial.begin(115200);
	Serial2.begin(9600);
	Serial2.flush();
}

void loop() {
  // put your main code here, to run repeatedly:
	uint8_t data;

	if (Serial2.available() > 0) {
		data = (uint8_t) Serial2.read();
		if ((data == 0x0D) || (data == 0x0A)) {			// if end of line
			if ((bufp > 0) && (bufp < GPS_BUF_SIZE)) {
				gps_buf[bufp] = NULL;
				gps_output();							// output gps data
			}
			bufp = 0;
		} else if (bufp < GPS_BUF_SIZE) {
			gps_buf[bufp++] = data;
		}
	}
}
