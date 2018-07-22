/* FILE NAME: HardwareServo.c
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
#include "HardwareServo.h"
#include "driver_irq.h"

#include "digitalio.h"

extern const unsigned char pin_to_pwm[];
extern const unsigned char ml620504f_tmout_to_pin[];
extern unsigned char *ml620504f_pin_to_port();
extern const unsigned char ml620504f_pin_to_bit[];


static unsigned short dt[4];
static unsigned short hsv_val2[4];
static unsigned char hsv_use_pwm=0;

static void timer5_isr(void)
{
	unsigned char use_ftm = (unsigned char)((hsv_use_pwm + 1) >> 1);
	
	digitalWrite(13,(~digitalRead(13))&0x01);
	
	switch(use_ftm)
	{
	case 4:
		set_bit(FT3ICP);
	case 3:
		set_bit(FT2ICP);
	case 2:
		set_bit(FT1ICP);
	case 1:
		set_bit(FT0ICP);
	default:
		break;
	}
}


static bool hsv_init(unsigned char num, unsigned short period)
{
	int i;
	int use_ftm;
	pinMode(13,OUTPUT);
	digitalWrite(13,LOW);
	if((num > 8)||(num==0)) return false;
	
	hsv_use_pwm = num;
	use_ftm = (++num) >> 1;
	
	// base timer setting
	clear_bit(DTM4);
	clear_bit(DTM5);
	TM4CON = 0x40|0x20|0x01;						//16bit mode, div16,HSCLK (1us)
	TM45D = period;
	set_bit(ETM5);
	irq_sethandler(IRQ_NO_TM5INT,timer5_isr);
	
	// base timer init
	for(i=0;i<use_ftm;i++)
	{
		dt[i] = 1;
		hsv_val2[i] = 0;
		BLKCON1 |= (unsigned char)(1<<i);							// BLKCON1 pwm reset
		BLKCON1 &= (unsigned char)(~(1<<i));						// BLKCON1 pwm on
		*(&FT0CLKL + (i << (5-sizeof(FT0CLKL)+1))) = 0x42;			// clock source = HSCLK / 16
		*(&FT0DT   + (i << (5-sizeof(FT0DT)+1)))   = 1;			// FT0DT = 1;  (for masking pulse)
		
		switch(i)
		{
		case 0:
			FT0TRG0H = 0x85;		// triggered by TM5
			break;
		case 1:
			FT1TRG0H = 0x88;		// triggered by FTM0
			break;
		case 2:
			FT2TRG0H = 0x89;		// triggered by FTM1
			break;
		case 3:
			FT3TRG0H = 0x8A;		// triggered by FTM2
			break;
		default:
			break;
		}
		*(&FT0TRG0L + (i << (5-sizeof(FT0TRG0L)+1))) |= 0x01;			//	waiting trigger
		*(&FT0INTE + (i << (5-sizeof(FT0INTE)+1))) |= 0x0101;			//	FTnIEP = 1, FTnIOP = 1;
		*(&FT0MODL + (i << (5-sizeof(FT0MODL)+1))) = 0xC3;			// FTnST0 = One shot, FTnDTEN = 1, PWM2
		*(&FT0CON0 + (i << (5-sizeof(FT0CON0)+1))) |= 0x02;			// FTnST0 = One shot, FTnDTEN = 1, PWM2
	}
	return true;
}

static void hsv_start(void)
{
	set_bit(T4RUN);
}

static void hsv_stop(void)
{
	set_bit(T4STP);
}

static void hsv_update(void)
{
	set_bit(FT0UD);
	set_bit(FT1UD);
	set_bit(FT2UD);
	set_bit(FT3UD);	
}


static void hsv_write(unsigned char ch, unsigned short val)
{
	unsigned char ftm_ch;
	unsigned char a_b;
	unsigned short *ftma;
	unsigned short *ftmp;
	
	if(hsv_use_pwm <= ch) return;
	
	ftm_ch = (unsigned char)(ch >> 1);
	a_b = (unsigned char)(0x01 & ch);
	
	ftma = &FT0EA + (ftm_ch << (5-sizeof(FT0EA)+1));
	ftmp = &FT0P + (ftm_ch << (5-sizeof(FT0P)+1));
	
	switch(a_b)
	{
	case 0:					// ftm_a
		*ftma = val + dt[ftm_ch];
		*ftmp = *ftma + hsv_val2[ftm_ch] + dt[ftm_ch]+1;
		break;
	case 1:					// ftm_b
		hsv_val2[ftm_ch] = val;
		*ftmp = *ftma + hsv_val2[ftm_ch] + dt[ftm_ch]+1;
		break;
	default:
		break;
	}
	return;
}

static bool hsv_attach(unsigned char ch, unsigned char pin)
{
	unsigned char port_num;
	unsigned char ftm;
	unsigned char *port;
	unsigned char bit;

	// check channel
	if(ch >= hsv_use_pwm ) return false;
	
	// check GPIO
	if(pin_to_pwm[pin] == 0xFF) return false;

	// GPIO setting
	ftm = pin_to_pwm[pin];
	port_num = ml620504f_tmout_to_pin[ftm];
	port = ml620504f_pin_to_port(port_num);
	bit = ml620504f_pin_to_bit[port_num];
	
	*(port+1) &= ~bit;		// PnDIR  = 0
	*(port+2) |= bit;		// PnCON0 = 1
	*(port+3) |= bit;		// PnCON1 = 1
	*(port+4) |= bit;		// PnMOD0 = 1
	*(port+5) |= bit;		// PnMOD1 = 1
	
	*(&FTO0SL + ftm) = ch;
	return true;
}

static bool hsv_detach(unsigned char pin,unsigned char level)
{
	unsigned char port_num;
	unsigned char ftm;
	unsigned char *port;
	unsigned char bit;
	
	// check GPIO
	if(pin_to_pwm[pin] == 0xFF) return false;

	// GPIO setting
	ftm = pin_to_pwm[pin];
	port_num = ml620504f_tmout_to_pin[ftm];
	port = ml620504f_pin_to_port(port_num);
	bit = ml620504f_pin_to_bit[port_num];
	if(level == 0)
		*(port+0) &= ~bit;		// PnD    = 0
	else
		*(port+0) |= bit;		// PnD    = 1
	*(port+1) &= ~bit;		// PnDIR  = 0
	*(port+2) |= bit;		// PnCON0 = 1
	*(port+3) |= bit;		// PnCON1 = 1
	*(port+4) &= ~bit;		// PnMOD0 = 0
	*(port+5) &= ~bit;		// PnMOD1 = 0
	
	return true;
}


static void hsv_close(void)
{
	int i;
	unsigned char use_ftm = (unsigned char)((hsv_use_pwm + 1) >> 1);
	
	clear_bit(ETM5);				// disenable timer interrupt
	clear_bit(T4RUN);				// stop base time
	set_bit(DTM4),set_bit(DTM5);	// set timer to power down
	
	for(i=0;i<use_ftm;i++)
	{
		BLKCON1 |= (unsigned char)(1<<i);			// set FTMn to power down
	}
}

static void hsv_dt(unsigned char ch,unsigned short deadtime)
{
	if(ch>3) return;				// error.
	if(deadtime == 0) return;		// error!! output signals are shorted.
	*(&FT0DT + (ch << (5-sizeof(FT0DT)+1))) = deadtime;			// FTnST0 = One shot, FTnDTEN = 1, PWM2
	dt[ch] = deadtime;
	return;
}

const HardwareServo hsv ={
	hsv_init,
	hsv_attach,
	hsv_detach,
	hsv_start,
	hsv_stop,
	hsv_close,
	hsv_write,
	hsv_dt,
	hsv_update
};

