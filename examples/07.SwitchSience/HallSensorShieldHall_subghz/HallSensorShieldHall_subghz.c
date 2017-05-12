#include "HallSensorShieldHall_subghz_ide.h"		// Additional Header

/* FILE NAME: HallSensorShieldHall_subghz.c
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

#define SUBGHZ_CH			36
#define SUBGHZ_PANID		0xabcd
#define SUBGHZ_BPS			100
#define SUBGHZ_PWR			20
#define SUBGHZ_HOST_ADDR	0x5f59

uint8_t txdata[256];
bool hall = false;

void isr_hall(void)
{
  hall = true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SubGHz.init();

  pinMode(3, INPUT);
  attachInterrupt(1, isr_hall, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  wait_event(&hall);

  Print.init(txdata,sizeof(txdata));
  if (digitalRead(3)) {
    Print.p("Released");
  } else {
    Print.p("Detected");
  }
  Print.ln();

  SubGHz.begin(SUBGHZ_CH,SUBGHZ_PANID,SUBGHZ_BPS,SUBGHZ_PWR);
  SubGHz.send(SUBGHZ_PANID,SUBGHZ_HOST_ADDR,txdata,Print.len(),NULL);
  SubGHz.close();

  Serial.print(txdata);
}
