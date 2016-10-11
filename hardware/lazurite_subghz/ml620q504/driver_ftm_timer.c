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


/* --- Common Header --- */
#include "common.h"

/* --- Platfrom Header --- */
#include "driver_ftm_timer.h"
#include "driver_irq.h"
#include "mcu.h"
#include "rdwr_reg.h"
#include "lazurite.h"

//*************************************************************************************
//
// FUNCTION: void ftm_timer_set(unsigned char ch, unsigned char ftmclk, unsigned short timeout, void (*func)(void))
// Paramterers:
// API for timer of ML620Q504
// 8bit timer
// ch = 0 ~ 3
// ftmclk = FTMnCLK register
// timeout = FTMnP register
// RETURN:  none
//*************************************************************************************
#define FTnC_OFFSET  	0x08/2			// 16bit access
#define FTnCON0_OFFSET	0x0A/2			// 16bit access (access to FTnCON1 also)
//#define FTnCON1_OFFSET  0x0B  not in use
#define FTnMOD_OFFSET	0x0C/2			// 16bit access
#define FTnCLK_OFFSET	0x0E/2			// 16bit access
#define FTnINTE_OFFSET	0x18/2			// 16bit access
#define FTnINTC_OFFSET	0x1C/2			// 16bit access

void ftm_timer_set(unsigned char ch, unsigned char ftmclk, unsigned short timeout, void (*func)(void))
{
	unsigned short pOffset;
	uint8_t ch_bitmask;
	unsigned short* pFTM0P = &FT0P;
	
	ch_bitmask = 0x01 << ch;
	BLKCON1 &= ~ch_bitmask;					// enable FTMn
	
	
	pOffset = ch * 32/sizeof(short);		// 16bit access
	
	*(pFTM0P+pOffset) = timeout;						// set timer
	// timer set in working state
	if(*(pFTM0P+pOffset+FTnCON0_OFFSET)&0x0001)
	{
		*(pFTM0P+pOffset+FTnCON0_OFFSET) = 0x0101;		// timer update
	}
	// start timer
	else
	{
		IE6 &= ~ch_bitmask;							// disenable IFQ
		IRQ6 &= ~ch_bitmask;						// clear IRQ

		*(pFTM0P+pOffset+FTnCLK_OFFSET) = ftmclk;			// set clock mode
		*(pFTM0P+pOffset+FTnMOD_OFFSET) = 0x0000;			// set mode timer,auto reload
		*(pFTM0P+pOffset+FTnC_OFFSET) =   0x0000;				// counter reset
		*(pFTM0P+pOffset+FTnINTC_OFFSET) = 0x0001;			// enable FTM0P interrupt
		*(pFTM0P+pOffset+FTnINTE_OFFSET) = 0x0001;			// enable FTM0P interrupt

		irq_sethandler((unsigned char)(IRQ_NO_FTM0INT + ch),func);				// set IRQ handler
		
		IE6 |= ch_bitmask;							// Enable IRQ
		
		*(pFTM0P+pOffset+FTnCON0_OFFSET) = 0x0001;			// timer start
	}
	return;
}

void ftm_timer_stop(unsigned char ch)
{	
	uint8_t ch_bitmask;
	
	ch_bitmask = 0x01 << ch;
	IE6 &= ~ch_bitmask;
	
	switch(ch)
	{
	case 0:
		FT0CON0 = 0;
		FT0INTE = 0;
		break;
	case 1:
		FT1CON0 = 0;
		FT1INTE = 0;
		break;
	case 2:
		FT2CON0 = 0;
		FT2INTE = 0;
		break;
	case 3:
		FT3CON0 = 0;
		FT3INTE = 0;
		break;
	default:
		return;
	}
	BLKCON1 |= ch_bitmask;
	return;
}

void ftm_timer_clear_irq(unsigned char ch)
{
	switch(ch)
	{
	case 0:
		FT0INTC = 0x01;
		break;
	case 1:
		FT1INTC = 0x01;
		break;
	case 2:
		FT2INTC = 0x01;
		break;
	case 3:
		FT3INTC = 0x01;
		break;
	default:
		return;
	}
		
	return;
}

