#include "SwitchSheild_ide.h"		// Additional Header

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

bool buttoned = false;


void isr_button(void)
{
  buttoned = true;
}

void setup() {
  // put your setup code here, to run once:
  SubGHz.init();
  buttoned = false;

  Serial.begin(115200);
  //  Serial.println_long(SubGHz.getMyAddress(), HEX);
  sleep(100);

  pinMode(2, INPUT);

  attachInterrupt(0, isr_button, CHANGE);

  pinMode(26, OUTPUT);

}

char payload[256];

void loop() {
  // put your main code here, to run repeatedly:
  wait_event(&buttoned);
//  Print.init(payload, sizeof(payload));

  digitalWrite(26, LOW);

  if(digitalRead(2)) Serial.println("release");
  else Serial.println("push");

  sleep(10);				// for chatering
  
  buttoned = false;


  //  SubGHz.begin(36,0xabcd,100,20);
  //  SubGHz.send(0xabcd,0x7fba,payload,Print.len(),NULL);
  //  SubGHz.close();



  digitalWrite(26, HIGH);
  /*
    Print.init(payload,sizeof(payload));

    digitalWrite(26,LOW);
    Print.l(digitalRead(3),DEC);
    Print.ln();

    SubGHz.begin(36,0xabcd,100,20);
    SubGHz.send(0xabcd,0x7fba,payload,Print.len(),NULL);
    SubGHz.close();
    Serial.write(payload,Print.len());
    Serial.println("");
    digitalWrite(26,HIGH);

  	sleep(10000);
  	*/
}
