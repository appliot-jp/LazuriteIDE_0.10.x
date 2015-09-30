/* FILE NAME: analogio.c
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
#include "pin_assignment.h"
#include <stdlib.h>
#include "driver_adc.h"
#include "driver_tmout.h"
//********************************************************************************
//   local definitions
//********************************************************************************
//#define PWM_TEST

#define ADC_RESOLUTION 12

//********************************************************************************
//   local parameters
//********************************************************************************

CHAR analogRead_resolution = 10;

// assignment of Arduino A0-A5 to AIN0-11 in ML620504F
const unsigned char analog_pin_to_port[MAX_AIN_NO+1] =
{
	4,	//	AIN4
	5,	//	AIN5
	6,	//	AIN6
	11,	//	AIN11
	0,	//	AIN0
	1,	//	AIN1
};

// assignment of pin to TMOUTn
// 255 means TMOUTn is not assigned
// 0=TMOUT0, 1=TMOUT1, ...  15=TMOUTF
const unsigned char pin_to_pwm[MAX_PIN_NO+1] =
{
	255,	//	-
	255,	//	-
	255,	//	-
	13,		//	TMOUTD *
	8,		//	TMOUT8 *
	9,		//	TMOUT9 *
	4,		//	TMOUT4 *
	5,		//	TMOUT5 *
	15,		//	TMOUTF *
	12,		//	TMOUTC *
	7,		//	TMOUT7 *
	255,	//	-
	255,	//	-
	6,		//	TMOUT6
	255,	//	-
	255,	//	-
	2,		//	TMOUT2
	1,		//	TMOUT1
	255,	//	-
	255,	//	-
	3,		//	TMOUT3
	255,	//	-
	255,	//	-
	255,	//	-
	255,	//	-
	255,	//	-
	14,		//	TMOUTE
	10,		//	TMOUTA
	255,	//	-
	255,	//	-
	255,	//	-
	0,		//	TMOUT0
	255,	//	-
	255,	//	-
	11,		//	TMOUTB
	255,	//	-
};

// assignment of TMOUTn to FTnP/FTnN 
const unsigned char pwm_to_ftm[16] =
{
	255,	//	TMOUT0	-
	255,	//	TMOUT1	-
	255,	//	TMOUT2	-
	255,	//	TMOUT3	-
	3,		//	TMOUT4	FTM1N
	4,		//	TMOUT5	FTM2P
	255,	//	TMOUT6	-
	7,		//	TMOUT7	FTM3N
	1,		//	TMOUT8	FTM0N
	2,		//	TMOUT9	FTM1P
	255,	//	TMOUTA	-
	255,	//	TMOUTB	-
	6,		//	TMOUTC	FTM3P
	0,		//	TMOUTD	FTM0P
	255,	//	TMOUTE	-
	5,		//	TMOUTF	FTM2N
};


//********************************************************************************
//   local function definitions
//********************************************************************************
//********************************************************************************
//   functions
//********************************************************************************
int analogRead(uint8_t pin)
{
	int res = -1;
	char shift;
	if((pin > (A5-A0)) &&  (pin < A0)) return -1;
	else if((pin >= A0) && (pin <= A5))
	{
		pin -= A0;
	}
	else if(pin > A5) return res;
	res = drv_analogRead(analog_pin_to_port[pin]);
	
	// Change resolution
	// ADC resolution is fixed to 12bit.
	// Data is only shifted according to resolution.
	shift = (CHAR)(analogRead_resolution - ADC_RESOLUTION);	// bit shift according to resolution
	if(shift < 0)
	{
		res >>= abs(shift);
	}
	else if(shift > 0)
	{
		res <<= shift;
	}
	
	return res;
}

void analogReadResolution(UCHAR mode)
{
	if(mode >= 16) mode = 16;
	else if(mode <= 8) mode = 8;
	analogRead_resolution = (CHAR)mode;
	return;
}
//*******************************************************
// PWM output function
// pin = 3-10 are supported
// when val = 0 or 255, FTM is stoped. In the case, PWM block is turn to power down mode.
// When val = 1 - 254, FTM start or update.
// If hardware PWM is not supported, no operation.
// The process include power down control of FTM.
//*******************************************************
void analogWrite(UCHAR pin, UCHAR val)
{
	if(pin > MAX_PIN_NO) return;
	if(pin_to_pwm[pin] < 255)
	{
		drv_analogWrite(pin_to_pwm[pin], pwm_to_ftm[pin_to_pwm[pin]],val);
	}
}

