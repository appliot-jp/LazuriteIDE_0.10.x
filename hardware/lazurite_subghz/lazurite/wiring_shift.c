/* FILE NAME: wiring_shift.c
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
#include "mcu.h"


//********************************************************************************
//   global parameters
//********************************************************************************

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

UCHAR shiftIn(UCHAR dataPin, UCHAR clockPin, UCHAR bitOrder)
{
	UCHAR value = 0;
	UCHAR i;
	for(i = 0; i < 8 ; i++)
	{
		digitalWrite(clockPin, HIGH);
		if(bitOrder == LSBFIRST)
		{
			value >>= 1;
			value |= (digitalRead(dataPin) << 7);
		}
		else
		{
			value <<= 1;
			value |= digitalRead(dataPin);
		}
		digitalWrite(clockPin, LOW);
	}
	return value;
}

void shiftOut(UCHAR dataPin, UCHAR clockPin, UCHAR bitOrder, UCHAR val)
{
	UCHAR i;
	
	for (i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST)
		{
			digitalWrite(dataPin,(unsigned char)(val & 0x01));
			val >>= 1;
		}
		else
		{
			digitalWrite(dataPin, (val >= 0x80));
			val <<= 1;
		}
		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);
	}
	
	return;
}
