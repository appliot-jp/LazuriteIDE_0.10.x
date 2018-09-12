/* FILE NAME: wiring_pulse.c
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


#include "common.h"
#include "lazurite.h"
#include "digitalio.h"

// header for hardware access
#include "mcu.h"
#include "driver_gpio.h"
#include "wiring_pulse.h"

//********************************************************************************
//   global parameters
//********************************************************************************
extern const unsigned char digital_pin_to_port[];
extern const unsigned char ml620504f_pin_to_bit[];
extern const int ml620504f_pin_to_port[];

//********************************************************************************
//   local definitions
//********************************************************************************

//********************************************************************************
//   local parameters
//********************************************************************************

//********************************************************************************
//   local function definitions
//********************************************************************************

//********************************************************************************
//   local functions
//********************************************************************************

unsigned long pulseIn(UCHAR pin, UCHAR value, UINT32 timeout)
{
	volatile uint32_t st;
	volatile uint32_t en;
	volatile uint32_t duration;
	volatile uint8_t* raw_pin;
	volatile uint8_t raw_bit;
	volatile uint8_t raw_val;
	
	raw_bit = ml620504f_pin_to_bit[digital_pin_to_port[pin]];
	raw_pin = (uint8_t*)ml620504f_pin_to_port[digital_pin_to_port[pin]];
	st=micros();
#if 0
	do
	{
		raw_val = ((*raw_pin)&raw_bit)!=0 ? HIGH : LOW;
		duration=micros()-st;

	} while(duration>timeout);
#else
	while(1)
	{
		
		en = micros();
		duration = en - st;
		raw_val = ((*raw_pin)&raw_bit)!=0 ? HIGH : LOW;
		if((duration > timeout)||(raw_val != value))
		{
			break;
		}

	}
#endif
	return duration;
}

