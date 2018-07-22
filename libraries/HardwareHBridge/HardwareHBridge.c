/* FILE NAME: HardwareHBridge.c
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
#include "mcu.h"
#include "rdwr_reg.h"
#include "HardwareHBridge.h"

extern const unsigned char pin_to_pwm[];
extern const unsigned char ml620504f_tmout_to_pin[];
extern unsigned char *ml620504f_pin_to_port();
extern const unsigned char ml620504f_pin_to_bit[];

#define OVF16BIT(a) ((a > 65535)? 0xffff : ((a < -65535) ? -65535 : a))

static unsigned short dt[4];

static void hhb_init(unsigned char ch, unsigned short period)
{
	if(ch>3) return;				// error
		
	BLKCON1 |=  (unsigned char)(1 << ch);						// 	set_bit(DFTM0);   reset
	BLKCON1 &= ~(unsigned char)(1 << ch);						// 	clear_bit(DFTM0);
	*(&FT0CLKL + (ch << (5-sizeof(FT0CLKL)+1))) = 0x42;			//	FT0CLKL = 0x42;  HSCLK / 16
	*(&FT0P    + (ch << (5-sizeof(FT0P)+1)))    = period;		//	FT0P = period;
	*(&FT0DT   + (ch << (5-sizeof(FT0DT)+1)))   = 1;			//	FT0DT = 1;  (for masking pulse)
	*(&FT0MODL + (ch << (5-sizeof(FT0MODL)+1))) = 0x42;			// PWM1, FTnDTEN = 1
	dt[ch] = 1;
	return;
}

static bool hhb_attach(unsigned char ch,unsigned char pin1,unsigned char pin2)
{
	unsigned char port_num;
	unsigned char ftm;
	unsigned char *port;
	unsigned char bit;
	int i;
	unsigned char pin[2];
	pin[0] = pin1;
	pin[1] = pin2;
	// check channel
	if(ch > 3) return false;
	
	// check GPIO
	if((pin_to_pwm[pin[0]] == 0xFF) || (pin_to_pwm[pin[1]] == 0xFF)) return false;

	// GPIO setting
	for(i=0;i<2;i++)
	{
		ftm = pin_to_pwm[pin[i]];
		port_num = ml620504f_tmout_to_pin[ftm];
		port = ml620504f_pin_to_port(port_num);
		bit = ml620504f_pin_to_bit[port_num];
		
		*(port+1) &= ~bit;		// PnDIR  = 0
		*(port+2) |= bit;		// PnCON0 = 1
		*(port+3) |= bit;		// PnCON1 = 1
		*(port+4) |= bit;		// PnMOD0 = 1
		*(port+5) |= bit;		// PnMOD1 = 1
		
		*(&FTO0SL + ftm) = (unsigned char)((ch << 1) + i);
	}
	return true;
}

static bool hhb_detach(unsigned char pin1,unsigned char pin2,unsigned char level1, unsigned char level2)
{
	unsigned char port_num;
	unsigned char ftm;
	unsigned char *port;
	unsigned char bit;
	int i;
	unsigned char pin[2];
	unsigned char level[2];
	pin[0] = pin1;
	pin[1] = pin2;
	level[0] = level1;
	level[1] = level2;
	// check GPIO
	if((pin_to_pwm[pin[0]] == 0xFF) || (pin_to_pwm[pin[1]] == 0xFF)) return false;

	// GPIO setting
	for(i=0;i<2;i++)
	{
		ftm = pin_to_pwm[pin[i]];
		port_num = ml620504f_tmout_to_pin[ftm];
		port = ml620504f_pin_to_port(port_num);
		bit = ml620504f_pin_to_bit[port_num];
		if(level[i] == 0)
			*(port+0) &= ~bit;		// PnD    = 0
		else
			*(port+0) |= bit;		// PnD    = 1
		*(port+1) &= ~bit;		// PnDIR  = 0
		*(port+2) |= bit;		// PnCON0 = 1
		*(port+3) |= bit;		// PnCON1 = 1
		*(port+4) &= ~bit;		// PnMOD0 = 0
		*(port+5) &= ~bit;		// PnMOD1 = 0
	}
	return true;
}
static void hhb_start(unsigned char ch)
{
	if(ch>3) return;				// error
	*(&FT0CON0 + (ch << (5-sizeof(FT0CON0)+1))) = 1;			// set_bit(FT0RUN);
}
static void hhb_stop(unsigned char ch)
{
	if(ch>3) return;				// error
	*(&FT0CON0 + (ch << (5-sizeof(FT0CON0)+1))) = 0;			// clear_bit(FT0RUN);
}
static void hhb_close(unsigned char ch)
{
	BLKCON1 |=  (unsigned char)(1 << ch);						// 	set_bit(DFTM0);   reset
	if(ch>3) return;				// error
}
static void hhb_write(unsigned char ch, long time)
{
	if(ch>3) return;				// error
	
	if(time == 0)
	{
		*(&FT0EA + (ch << (5-sizeof(FT0EA)+1))) = 0;
		*(&FT0EB + (ch << (5-sizeof(FT0EB)+1))) = 0;
	}
	else if(time > 0)
	{
		*(&FT0EA + (ch << (5-sizeof(FT0EA)+1))) = (unsigned short)OVF16BIT(time + dt[ch]);
		*(&FT0EB + (ch << (5-sizeof(FT0EB)+1))) = 0;
	}
	
	else
	{
		*(&FT0EA + (ch << (5-sizeof(FT0EA)+1))) = 0;
		*(&FT0EB + (ch << (5-sizeof(FT0EB)+1))) = (unsigned short)OVF16BIT(abs(time) + dt[ch]);
	}
	
	if( (*(&FT0CON1 + (ch << (5-sizeof(FT0CON1)+1))) & 0x01) == 0)
	{
		*(&FT0CON1 + (ch << (5-sizeof(FT0CON1)+1))) |= 0x01;
	}
}

static void hhb_dt(unsigned char ch,unsigned short deadtime)
{
	if(ch>3) return;				// error.
	if(deadtime == 0) return;		// error!! output signals are shorted.
	*(&FT0DT + (ch << (5-sizeof(FT0DT)+1))) = deadtime;			// FTnST0 = One shot, FTnDTEN = 1, PWM2
	dt[ch] = deadtime;
	return;
}


static void hhp_update(void)
{
	set_bit(FT0UD);
	set_bit(FT1UD);
	set_bit(FT2UD);
	set_bit(FT3UD);	
}


const HardwareHBridge hhb =
{
	hhb_init,
	hhb_attach,
	hhb_detach,
	hhb_start,
	hhb_stop,
	hhb_close,
	hhb_write,
	hhb_dt,
	hhp_update
};

