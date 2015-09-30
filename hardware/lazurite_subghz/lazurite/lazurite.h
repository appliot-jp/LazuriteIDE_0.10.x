/* FILE NAME: lazurite.h
 *
 * Copyright (c) 2015  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */


#ifndef _LAZURITE_H_
#define _LAZURITE_H_

#include "common.h"

//****************************************************
//        Definitions
//****************************************************
#ifndef   TRUE
#define TRUE									1
#endif // TRUE
#ifndef   FALSE
#define FALSE									0
#endif // FALSE
typedef UCHAR BOOL;
#define HIGH (UCHAR)0x1
#define LOW  (UCHAR)0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define HIZ 0x03
#define OPEN_DRAIN 0x04
#define INPUT_PULLDOWN 0x5

#define true 0x1
#define false 0x0

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

// TIMER

#define		TONE_TIMER			2			// timer 4, 5 16bit mode
#define		TM_MILLIS			0
#define		TM_DELAY			6


#define		F_LSCLK				32768
#define		F_HSCLK				16000000L

typedef unsigned char	uint8_t;
typedef uint8_t		boolean;
typedef uint8_t		byte;

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
//#define abs(x) ((x)>0?(x): (x^(-1)+1))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts() __EI()
#define noInterrupts() __DI()

/*
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )
*/
#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

typedef unsigned int word;

#define bit(b) (1UL << (b))


//****************************************************
//        APIs
//****************************************************
extern void arduino_setup(void);
extern void arduino_main(void);
extern void setup(void);				// Arduino startup function
extern void loop(void);					// Arduino loop function

//****************************************************
//        External API
//****************************************************
#include "analogio.h"
#include "digitalio.h"
#include "lazurite_system.h"
#include "serial.h"
#include "print.h"
#include "wiring_shift.h"
#include "wiring_pulse.h"
#include "WInterrupts.h"
#include "WMath.h"
#include "flash.h"
#include <stdlib.h>
#include <math.h>

#ifdef SUBGHZ
#include "subghz_api.h"
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LAZURITE_H_ */



