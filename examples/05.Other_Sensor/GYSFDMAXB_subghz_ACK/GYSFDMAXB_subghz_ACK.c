/* FILE NAME: GYSFDMAXB_subghz_ACK.c
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

/*
 [Premise]

 GPS module(GYSFDMAXB) sends the data every 1 second.
 GPS baud rate = 9600bps ( 1/(9600 bps/10bit) * 1000 = 1.04ms/byte)
 SubGHz transfer rate = 100kbps ( 1/(100000 bps/8bit) * 1000 = 0.08ms/byte)
 Serial FIFO(driver) size = 128 byte

 [Design policy]

 GPS data lost is OK when there is no ACK after 1 retry of SubGHz send.

 Buffer size = 220 byte
 Retry = 1 time
 ACK timeout = 50 ms
 
 [Normal case]

                |<--------------------- 1s --------------------->|
 GPS receive    ****************************                     **************
 Buffer         ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~                     ~~~~~~~~~~ ~~~
 SubGHz send                +            +                                 +
 receive ACK                -            -                                 -
 Serial FIFO                =            =                                 =

  * : gets data from GPS
  ~ : copies data from Serial FIFO until Buffer is full
     (It spends time more than 229ms=220byte * 1.04ms/byte at least)
  + : sends data via SubGHz
     (It spends time more than 17.6ms=220byte * 0.08ms/byte at least)
  - : receive ACK immediately (3ms)
  = : increases Serial FIFO data
     (It increase more than 17byte=17.6ms / 1.04ms/byte at least)

 [Send NG but Retry is OK]

                |<--------------------- 1s --------------------->|
 GPS receive    ****************************                     **************
 Buffer         ~~~~~~~~~~~~     ~~~~~~~ ~~~                     ~~~~~~~~~ ~~~~
 SubGHz send                +  +        +                                 +
 ACK wait                   --- -       -                                 -
 Serial FIFO                =====       =                                 =

  * : gets data from GPS
  ~ : copies data from Serial FIFO until Buffer is full
     (It spends time more than 229ms=220byte * 1.04ms/byte at least)
  + : sends data via SubGHz
     (It spends time more than 17.6ms=220byte * 0.08ms/byte at least)
  - : receive ACK after retry
     (It spends time more than 70.6ms=50ms timeout+17.6ms send time+3ms ACK at least)
  = : increases Serial FIFO data
     (It increase more than 68byte=70.6ms / 1.04ms/byte at least)

 [Send NG and Retry is NG(Worst case)]

                |<--------------------- 1s --------------------->|
 GPS receive    ****************************                     **************
 Buffer         ~~~~~~~~~~~~       ~~~~~ ~~~                     ~~~~~~~~~ ~~~~
 SubGHz send                +  +        +                                 +
 ACK wait                   --- ---     -                                 -
 Serial FIFO                ======X     =                                 =

  * : gets data from GPS
  ~ : copies data from Serial FIFO until Buffer is full
     (It spends time more than 229ms=220byte * 1.04ms/byte at least)
  + : sends data via SubGHz
     (It spends time more than 17.6ms=220byte * 0.08ms/byte at least)
  - : receive no ACK after retry
     (It spends time more than 135.2ms=50ms x2 timeout+17.6ms x2 send time at least)
  = : increases Serial FIFO data, but overflowed!(X)
     (It increase more than 130byte=135.2ms / 1.04ms/byte at least)

*/

#include "GYSFDMAXB_subghz_ACK_ide.h"		// Additional Header

#define LED					( 26 )			// pin number of Blue LED
#define SUBGHZ_CH			( 36 )			// channel number (frequency)
#define SUBGHZ_PANID		( 0xabcd )		// panid
#define HOST_ADDRESS		( 0x5f59 )		// distination address
#define GPS_BUF_SIZE		( 220 )			// more than 128 byte at least
#define SUBGHZ_RETRY_NUM	( 1 )			// number of retry
#define SUBGHZ_ACK_TIMEOUT	( 50 )			// ACK timeout

uint8_t bufp;
uint8_t gps_buf[GPS_BUF_SIZE];

void gps_output()
{
	digitalWrite(LED,LOW);								// LED ON
	Serial.println("send");
	SubGHz.send(SUBGHZ_PANID, HOST_ADDRESS, gps_buf, GPS_BUF_SIZE, NULL);	// send data
	digitalWrite(LED,HIGH);								// LED off
}

void setup() {
  // put your setup code here, to run once:
	SUBGHZ_PARAM param;

	pinMode(LED,OUTPUT);								// setting of LED
	digitalWrite(LED,HIGH);								// setting of LED

	SubGHz.init();										// initializing Sub-GHz
	SubGHz.getSendMode( & param);
	param.txRetry = SUBGHZ_RETRY_NUM;					// change number of retry
	param.txInterval = SUBGHZ_ACK_TIMEOUT;				// change ACK timeout
	SubGHz.setSendMode( & param);
	SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID, SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);		// start Sub-GHz

	Serial.begin(115200);
	Serial2.begin(9600);
	Serial2.flush();
}

void loop() {
  // put your main code here, to run repeatedly:
	if (Serial2.available() > 0) {
		gps_buf[bufp++] = (uint8_t) Serial2.read();
		if (bufp >= GPS_BUF_SIZE) {
			gps_output();
			bufp = 0;
		}
	}
}
