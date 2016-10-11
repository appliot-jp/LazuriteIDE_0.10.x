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
#include "driver_gpio.h"
#include "driver_ftm_timer.h"
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
static volatile int32_t tone_toggle_count;
static uint8_t drv_tone_pin;
static uint8_t tone_val;
static uint8_t tone_ch=0;

//********************************************************************************
//   local function definitions
//********************************************************************************
void timer_tone_isr(void);
//********************************************************************************
//   local functions
//********************************************************************************

#define TONE_BASE_CLOCK		4000000L
#define TONE_FTM_CH			3
void timer_tone_isr(void)
{
	if(tone_toggle_count>0)
	{
		tone_toggle_count--;
		if(tone_toggle_count == 0)
		{
			drv_digitalWrite(drv_tone_pin, LOW);		// reset port to 0
			ftm_timer_stop(tone_ch);
			return;
		}
	}
	
	if(tone_val)			// invert port
	{
		tone_val = LOW;
	}
	else
	{
		tone_val = HIGH;
	}
	drv_digitalWrite(drv_tone_pin, tone_val);
	
	// check duration
	ftm_timer_clear_irq(tone_ch);
	
	return;
}

void noTone()
{
	ftm_timer_stop(tone_ch);
	ftm_timer_clear_irq(tone_ch);
}

void tone(uint8_t pin, uint16_t frequency, long duration)
{
	uint16_t timer_count;
	
	// base clock 4MHz
	timer_count = (uint16_t)(TONE_BASE_CLOCK / (frequency*2));
	if(duration>=0)
	{
		tone_toggle_count = duration * frequency * 2/1000;
	}
	else 
	{
		tone_toggle_count = -1;			// continue
	}
	
	drv_tone_pin=digital_pin_to_port[pin];
	tone_val = 0;
	drv_digitalWrite(drv_tone_pin,tone_val);
	drv_pinMode(drv_tone_pin,OUTPUT);
	
	// time setting(direct access to ML620Q504H)
	ftm_timer_set(tone_ch,0x0021,timer_count,timer_tone_isr);
	
	return;
}

void setToneCh(uint8_t toneCh)
{
	tone_ch = toneCh;
}
