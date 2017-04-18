#include "HallSensor_serial_ide.h"		// Additional Header

/* FILE NAME: Serial_Echo.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015  Lapis Semiconductor Co.,Ltd.
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

bool hall = false;

void isr_hall(void)
{
  hall = true;
}

void setup() {
  // put your setup code here, to run once:
//  SubGHz.init();
  Serial.begin(115200);
  
  hall = false;

  pinMode(3, INPUT);

  attachInterrupt(1, isr_hall, CHANGE);

  pinMode(26, OUTPUT);

}

char payload[] = "50\r\n";

void loop() {
  // put your main code here, to run repeatedly:
  wait_event(&hall);
  hall = false;

  digitalWrite(26, LOW);

//  SubGHz.begin(36,0xabcd,100,20);
//  SubGHz.send(0xabcd,0x1002,payload,sizeof(payload),NULL);
//  SubGHz.close();

	if(digitalRead(3) == 0 ) Serial.println("Detecting");
	else Serial.println("Release");

  digitalWrite(26, HIGH);

}
