/* FILE NAME: MsTimer2.c
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

#include "MsTimer2.h"
#include "driver_timer.h"

void ms_timer2_init(void)
{
	timer2.current_time_h = 0;
	TM23D = (timer2.target_time_h ? 0xFFFF : timer2.target_time_l);
	return;
}

void ms_timer2_isr()
{
	unsigned short tm_count;
	if(timer2.current_time_h >= timer2.target_time_h)
	{
		timer2.callback();
		ms_timer2_init();
	}
	else
	{
		timer2.current_time_h++;
		TM23D = ((timer2.current_time_h != timer2.target_time_h) ?0xFFFF : timer2.target_time_l);
		QTM3 = 0;
		tm_count = TM23C;
		if(timer2.target_time_l < tm_count)
		{
			QTM2 = 1;
		}
	}
}

void ms_timer2_set(unsigned long ms, void (*f)())
{
	// calcurate count number of target time
//	double ms_float;
	unsigned long tmp_target_time_l;
	unsigned short tm_data;
	
	if(ms == 0) return;

//	ms_float = ms;
//	ms_float = ms_float * F_LSCLK / 32000 + 0.5;

//	timer2.target_time_h = (unsigned long)(ms_float / 65536);
//	if(ms_float > 4294967295) 
//	{
//		ms_float -= 4294967295;
//		ms_float -= 1;
//	}
//	timer2.target_time_l = (unsigned short)ms_float;	// 
	timer2.target_time_h = ms / 64000;
	tmp_target_time_l = ((ms % 64000) <<8) / 250;
	timer2.target_time_l = (unsigned short)tmp_target_time_l;
	if(tmp_target_time_l==0)
	{
		timer2.target_time_l=0xFFFF;
		timer2.target_time_h--;
	}
	else
	{
		timer2.target_time_l--;								// decrease 1 to set register
	}
	
	tm_data = timer2.target_time_h ? 0xFFFF:timer2.target_time_l;

#if 0
	Serial.print_long(timer2.target_time_h,DEC);
	Serial.print("\t");
	Serial.print_long(timer2.target_time_l,DEC);
	Serial.println("");
#endif
	ms_timer2_init();
	timer2.callback = f;

	// setting timer
	timer_16bit_set(2,0x68, tm_data, ms_timer2_isr);

	return;
}

void ms_timer2_start(void)
{
	timer_16bit_start(2);
}

void ms_timer2_stop(void)
{
	timer_16bit_stop(2);
}

extern char uart_tx_sending;
extern char uartf_tx_sending;

MsTimer2 timer2 ={
	0,					// unsigned long  current_time_h;
	0,					// unsigned long  target_time_h;
	0,					// unsigned short target_time_l;
	NULL,				// void (*callback)(void);
	ms_timer2_set,		// void (*set)(unsigned long ms, void (*f)());
	ms_timer2_start,	// void (*start)(void);
	ms_timer2_stop,		// void (*stop)(void);
};
