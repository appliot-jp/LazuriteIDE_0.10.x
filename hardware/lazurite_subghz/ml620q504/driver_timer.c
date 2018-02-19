/* FILE NAME: driver_timer.c
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

#ifdef SUBGHZ_OTA
	#pragma SEGCONST "OTA_SEGCONST"
#endif
/* --- Common Header --- */
#include "common.h"

/* --- Platfrom Header --- */
#include "driver_timer.h"
#include "driver_irq.h"
#include "mcu.h"
#include "rdwr_reg.h"

const char num_to_bit[]=
{
	0x01,	0x02,	0x04,	0x08,
	0x10,	0x20,	0x40,	0x80,
};
//*************************************************************************************
//
// FUNCTION: void timer_8bit_set(unsigned char ch, unsigned char TMnCON, unsigned char TMnD, void (*func)(void)):
// FUNCTION: void timer_16bit_set(unsigned char ch, unsigned char TMnCON, unsigned char TMnD, void (*func)(void)):
// Paramterers:
// API for timer of ML620Q504
// 8bit timer
// ch = 0 ~ 7
//   In case of 8bit, timer =  0 ~ 7
//   In case of 16bit, timer =  0,2,4,6
// TMnCON = value of TMnCON register
// TMnD   = value of TMnD register
// func   = callback function of interrupt
// RETURN:  none
//*************************************************************************************
void timer_8bit_set(unsigned char ch, unsigned char TMnCON, unsigned char TMnD, void (*func)(void))
{
	unsigned char* pTMnD;
	unsigned char* pTMnC;
	unsigned char* pTMnCON;
	
	// check ch
	if(ch >= 8) return;
	
	pTMnD = &TM0D;
	pTMnD += ch;
	pTMnC = &TM0C;
	pTMnC += ch;
	pTMnCON = &TM0CON;
	pTMnCON += ch;
	
	BLKCON0 &= ~num_to_bit[ch];					// enabling timer0, timer1
	TMSTP0  |=  num_to_bit[ch];					// stop timer0, timer1
	
	IRQ5 &= ~num_to_bit[ch];						// clear irq of timer0, timer1
	IE5  &= ~num_to_bit[ch];						// clear ie of timer0, timer1
	
	*pTMnD = TMnD;
	*pTMnC = 0;
	*pTMnCON = TMnCON;
	
	irq_sethandler((unsigned char)(IRQ_NO_TM0INT + ch),func);		// set IRQ handler
	
	return;
}

void timer_8bit_start(unsigned char ch)
{
	// check ch
	if(ch >= 8) return;
	
	IE5    |= num_to_bit[ch];						// set ie of timer0, timer1
	TMSTR0 |= num_to_bit[ch];						// start timer0, timer1
	return;
}

void timer_8bit_stop(unsigned char ch)
{
	// check ch
	if(ch >= 8) return;
	
	TMSTP0 |= num_to_bit[ch];						// stop timer0, timer1
	IE5 &= ~num_to_bit[ch];						// disenable timer0, timer1
	BLKCON0 |= num_to_bit[ch];					// power down timer0, timer1
	
}

#ifdef SUBGHZ_OTA
	#pragma SEGCODE "OTA_SEGCODE"
#endif

void timer_16bit_set(unsigned char ch, unsigned char TMnCON, unsigned short TMnnD, void (*func)(void))
{
	unsigned short* pTMnnD;
	unsigned short* pTMnnC;
	unsigned char* pTMnCON;
	
	// check ch
	if(ch >= 8) return;
	ch &= 0xFE;

	pTMnnD = &TM01D;
	pTMnnD += (ch>>1);
	pTMnnC = &TM01C;
	pTMnnC += (ch>>1);
	pTMnCON = &TM0CON;
	pTMnCON += ch;
	
	BLKCON0 &= ~(num_to_bit[ch] | num_to_bit[ch+1]);		// enabling timer0, timer1
	TMSTP0  |=  (num_to_bit[ch] | num_to_bit[ch+1]);		// stop timer0, timer1
	
	IRQ5 &= ~(num_to_bit[ch] | num_to_bit[ch+1]);			// clear irq of timer0, timer1
	IE5  &= ~(num_to_bit[ch] | num_to_bit[ch+1]);			// clear ie of timer0, timer1
	
	*pTMnnD = TMnnD;
	*pTMnnC = 0;
	*pTMnCON = TMnCON;
	
	irq_sethandler((unsigned char)(IRQ_NO_TM0INT + ch + 1),func);				// set IRQ handler
	
	return;
}

void timer_16bit_start(unsigned char ch)
{
	// check ch
	if(ch >= 8) return;
	ch &= 0xFE;
	
	IE5    |= (num_to_bit[ch+1]);							// set ie of timer0, timer1
	TMSTR0 |= (num_to_bit[ch]);							// start timer0, timer1
	return;
}

void timer_16bit_stop(unsigned char ch)
{
	// check ch
	if(ch >= 8) return;
	ch &= 0xFE;
	
	TMSTP0 |= (num_to_bit[ch]);							// stop timer0, timer1
	IE5 &= ~(num_to_bit[ch] | num_to_bit[ch+1]);		// disenable timer0, timer1
	BLKCON0 |= (num_to_bit[ch] | num_to_bit[ch+1]);		// power down timer0, timer1
	
	return;
}


