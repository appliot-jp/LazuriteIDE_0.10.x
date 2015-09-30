/* FILE NAME: tone.c
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
#include "driver_timer.h"
#include "serial.h"
#include "mcu.h"

//********************************************************************************
//   global parameters
//********************************************************************************

//********************************************************************************
//   local definitions
//********************************************************************************
//#define TONE_TEST
//********************************************************************************
//   local parameters
//********************************************************************************

// About _tone_toggle_count
// _tone_toggle_count < 0  : continue
// _tone_toggle_count = 0  : stop
// _tone_toggle_count > 0  : counting
INT32 _tone_toggle_count;
UCHAR* _tone_port;
UCHAR  _tone_bit;

#ifdef TONE_TEST
UCHAR* _tonetest_port;
UCHAR  _tonetest_bit;
#endif // TONE_TEST

//********************************************************************************
//   local function definitions
//********************************************************************************
void timer_tone_isr(void);
//********************************************************************************
//   local functions
//********************************************************************************

void tone(UCHAR pin, UINT16 frequency, UINT32 duration)
{
	UINT32 timer_count;
	UINT32 frequency32 = frequency;
	if(pin > MAX_PIN_NO) return;
	_tone_port = digital_pin_to_port[pin];
	pinMode(pin,OUTPUT);
	
	// BLKCON  Enabling TM6,7

	if(frequency > 250)
	{
		timer_count = 16000000 / (frequency32 * 2);				//timec clock = 1/1clock
		timer_16bit_set(TONE_TIMER,0x41,(unsigned short)timer_count,timer_tone_isr);
	}
	else if(frequency > 30)
	{
		timer_count = 16000000 / (frequency32 * 8 * 2);			//timec clock = 1/8 clock
		timer_16bit_set(TONE_TIMER,0x59,(unsigned short)timer_count,timer_tone_isr);
	}
	else
	{
		timer_count = 16000000 / (frequency32 * 64 * 2);			//timec clock = 1/64 clock
		if(timer_count > 0xFFFF) timer_count = 0xFFFF;
		timer_16bit_set(TONE_TIMER,0x71,(unsigned short)timer_count,timer_tone_isr);
	}
	
	if(duration == 0)
	{
		_tone_toggle_count = -1;
	}
	else
	{
		_tone_toggle_count = frequency * duration * 2 / 1000;			// toggle cycle
	}
	
#ifdef TONE_TEST
	Serial.print("frequency =");
	Serial.print_long(frequency, DEC);
	Serial.print("\ttimer count =");
	Serial.print_long(timer_count, DEC);
	Serial.print("\tduration = ");
	Serial.println_long(_tone_toggle_count,DEC);
	pinMode(22,OUTPUT);
	_tonetest_port = digital_pin_to_port[22];
	_tonetest_bit = digital_pin_to_bit[22];
#endif // TONE_TEST
	timer_16bit_start(TONE_TIMER);
	
}

void timer_tone_isr(void)
{
	if(digitalRead(_tone_port) == HIGH)			// invert port
	{
		digitalWrite(_tone_port,LOW);
	}
	else
	{
		digitalWrite(_tone_port, HIGH);
	}
	
	// check duration
	if(_tone_toggle_count>0)
	{
		_tone_toggle_count--;
		if(_tone_toggle_count == 0)
		{
			digitalWrite(_tone_port, LOW);		// reset port to 0
			timer_16bit_stop(TONE_TIMER);
		}
	}
	
	return;
}

void noTone(UCHAR _pin)
{
	digitalWrite(_pin,LOW);					// reset port to 0
	timer_16bit_stop(TONE_TIMER);
}


