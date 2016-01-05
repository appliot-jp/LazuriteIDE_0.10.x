/* FILE NAME: MsTimer2.c
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

// This program is sample software of MsTimer2
// In the program, callback() is called once per 500ms.
// Then, LED is blinking and CPU operating time is send to serial monitor in PC.
// 本プログラムはMsTimer2用のサンプルソフトウエアです。
// 500msに1回、callback()が呼び出され、LEDを点滅するとともに、その時間をPC上のシリアルモニターにＣＰＵが動作してからの累積動作時間を送信しています。

void callback(void)
{
	digitalWrite(26,(digitalRead(26)?LOW:HIGH));
	Serial.println_long((long)millis(),DEC);
}

void setup(void)
{
	// Setup Serial
	Serial.begin(115200);
	
	// Setup Blue GPIO)
	digitalWrite(26,LOW);
	pinMode(26,OUTPUT)；
	
	// Setup MsTimer2;
	timer2.set(500,callback);
	timer2.start();
}


void loop(void)
{
}
