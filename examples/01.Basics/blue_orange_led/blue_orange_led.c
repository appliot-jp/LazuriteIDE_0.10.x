#include "blue_orange_led_ide.h"		// Additional Header

/* FILE NAME: blue_orange_led.c
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

#define BLUE_LED		26
#define ORANGE_LED		25

//*****************************************************
// Local parameters
//*****************************************************

//*****************************************************
// Local func declaration
//*****************************************************


//*****************************************************
// Arduino Start up sequence
//*****************************************************
////////////// HOST ////////

void setup(void)
{
	digitalWrite(BLUE_LED,HIGH);
	pinMode(BLUE_LED,OUTPUT);
	digitalWrite(ORANGE_LED,LOW);
	pinMode(ORANGE_LED,OUTPUT);
}

// Arduino loop sequence
void loop(void)
{
	digitalWrite(BLUE_LED,HIGH);
	digitalWrite(ORANGE_LED,LOW);
	delay(1000);
	digitalWrite(BLUE_LED,LOW);
	digitalWrite(ORANGE_LED,HIGH);
	delay(1000);
}
