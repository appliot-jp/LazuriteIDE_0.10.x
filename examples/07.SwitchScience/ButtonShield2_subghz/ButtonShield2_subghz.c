#include "ButtonShield2_subghz_ide.h"		// Additional Header

/* FILE NAME: ButtonShield_subghz.c
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
#define SUBGHZ_HOST_ADDR	0xFFFF

uint8_t txdata[256];

#define ORANGE_LED	25
#define BLUE_LED	26

const char vls_val[][8] ={
	"0",		//	"invalid",
	"0",		//	"invalid",
	"1.8",		//	"< 1.898V"
	"1.95",		//	"1.898 ~ 2.000V"
	"2.05",		//	"2.000 ~ 2.093V"
	"2.15",		//	"2.093 ~ 2.196V",
	"2.25",		//	"2.196 ~ 2.309V",
	"2.35",		//	"2.309 ~ 2.409V",
	"2.5",		//	"2.409 ~ 2.605V",
	"2.7",		//	"2.605 ~ 2.800V",
	"2.9",		//	"2.800 ~ 3.068V",
	"3.2",		//	"3.068 ~ 3.394V",
	"3.6",		//	"3.394 ~ 3.797V",
	"4.0",		//	"3.797 ~ 4.226V",
	"4.4",		//	"4.226 ~ 4.667V",
	"4.7"		//	"> 4.667V"
};
static uint8_t level=0;

struct button btn0[] = {

	{
		1,	// push
		2,	// 2pin
		1	// extirq 0
	}
};
struct button_config btn = {
	1,		// num
	0,		// event
	false,	// wait_flag
	1,		// chat
	5,		// cont
	1,		// cont_interval
	100,	// timer = 100ms
	btn0
};

void setup() {
  // put your setup code here, to run once:
	SubGHz.init();
	Serial.begin(115200);  
	buttonDetect(&btn,NULL);
	pinMode(BLUE_LED,OUTPUT);
	digitalWrite(BLUE_LED,HIGH);
	pinMode(ORANGE_LED,OUTPUT);
	digitalWrite(BLUE_LED,HIGH);
}

void loop() {
	static bool began = false;
	wait_event(&btn.wait_flag);
	if((began == true) && (btn0[0].state == 0)) {
		SubGHz.close();
		digitalWrite(ORANGE_LED,HIGH);
		began = false;
	} else if((began == false) && (btn0[0].state == 2)) {
		began = true;
		SubGHz.begin(SUBGHZ_CH,SUBGHZ_PANID,SUBGHZ_BPS,SUBGHZ_PWR);
		digitalWrite(ORANGE_LED,LOW);
	}
	if(btn.event != 0) {
		if(level < 2) {
			level = voltage_check(VLS_4_667);
		} else {
			level = voltage_check(level);		
		}
		Print.init(txdata,sizeof(txdata));
		Print.l(btn.event,HEX);
		btn.event = 0;
		Print.p(",");
		Print.p(vls_val[level]);
		Print.ln();
		digitalWrite(BLUE_LED,LOW);
		SubGHz.send(SUBGHZ_PANID,SUBGHZ_HOST_ADDR,txdata,Print.len(),NULL);
		digitalWrite(BLUE_LED,HIGH);
		Serial.print(txdata);
	}	
}
