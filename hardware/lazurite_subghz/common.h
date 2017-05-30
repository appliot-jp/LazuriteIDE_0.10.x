/* FILE NAME: common.h
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

#ifndef _COMMON_H_
#define _COMMON_H_
#include <ml620504F.h>
#include <string.h>


/************************************************/
/* Type Definition                              */
/************************************************/
typedef unsigned char   BOOLEAN;
typedef unsigned char   boolean;
typedef unsigned char   bool;
typedef unsigned char   BOOL;

typedef unsigned char   UCHAR;
typedef signed char     CHAR;

typedef unsigned char   UINT8;
typedef unsigned int    UINT16;
typedef unsigned long   UINT32;

typedef signed char     INT8;
typedef signed int      INT16;
typedef signed long     INT32;

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned long uint32_t;
typedef signed long int32_t;
typedef uint8_t		boolean;
typedef uint8_t		byte;
typedef unsigned int word;


#define true 0x1
#define false 0x0
#define TRUE 0x1
#define FALSE 0x0

#define HIGH (UCHAR)0x1
#define LOW  (UCHAR)0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define HIZ 0x03
#define OPEN_DRAIN 0x04
#define INPUT_PULLDOWN 0x5

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

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define bit(b) (1UL << (b))
#define BIT(n)				(0x01<<n)

#define _BV(bit) (1 << (bit))


/************************************************/
/* Definition                                   */
/************************************************/
#endif /* _COMMON_H_ */
