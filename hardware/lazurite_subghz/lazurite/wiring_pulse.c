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
#include "driver_timer.h"
#include "driver_irq.h"
#include "wdt.h"
#include "lp_manage.h"
#include "wiring_pulse.h"

//********************************************************************************
//   global parameters
//********************************************************************************

//********************************************************************************
//   local definitions
//********************************************************************************

// hardware resources
#define PULSEIN_TIMER			3						// use TM6 and TM7 16bit timer mode
#define PULSEIN_EXT_NUM			4						// external IRQ number

#define pulsein_irq_ext_init(edge,sampling,filter,port)		irq_ext4_init(edge,sampling,filter,port)			//
#define pulsein_irq_ext_dis()								irq_ext4_dis()										// EEXI = 0
#define pulsein_irq_ext_ena()								irq_ext4_ena()										// EEXI = 1
#define pulsein_irq_ext_clearIRQ()							irq_ext4_clearIRQ()									// QEXI = 0
#define pulsein_irq_sethandler(f)							irq_sethandler(IRQ_NO_EXI4INT,f);

#define pulsein_cal_port_num()								(((UCHAR)(digital_pin_to_port[pin]-&P0D))<<1)
#define pulsein_get_timer_val()								read_reg16(TM67C)

//********************************************************************************
//   local parameters
//********************************************************************************
// pulsein_status
#define PULSEIN_WAITING			0
#define PULSEIN_DETECT_1		1
#define PULSEIN_DETECT_2		2
#define PULSEIN_TIMEOUT			3
/*
static UCHAR _pulsein_flag;
static UCHAR _pulsein_value;
static UCHAR* _pulsein_port;
static UCHAR _pulsein_bit;
static UINT32 _pulsein_timer_now;
static UINT32 _pulsein_timer_start;
static UINT32 _pulsein_timer_end;
*/
//********************************************************************************
//   local function definitions
//********************************************************************************

//********************************************************************************
//   local functions
//********************************************************************************

void timer_pulsein_isr(void)
{
	_pulsein_timer_now += 0x00010000;					// increment timer counter
	return;
}

void extin_pulsein_isr(void)
{
	UCHAR port_data;
	port_data = (UCHAR)(((*_pulsein_port)&_pulsein_bit)!=0 ? HIGH : LOW);
	if( _pulsein_flag == PULSEIN_WAITING )												// detecting 1st edge
	{
		if(port_data == _pulsein_value)
		{
			_pulsein_timer_start = _pulsein_timer_now + pulsein_get_timer_val();		// set starting time
			_pulsein_flag = PULSEIN_DETECT_1;											// 
		}
	}
	else																				// 2nd edge
	{
		_pulsein_timer_end = _pulsein_timer_now + pulsein_get_timer_val();				// set ending time
		_pulsein_flag = PULSEIN_DETECT_2;
	}
}

unsigned long pulseIn(UCHAR pin, UCHAR value, UINT32 timeout)
{
	UCHAR n;
	UCHAR m;
	
	/// initializing timing counter
	_pulsein_timer_now = 0;
	_pulsein_timer_end = timeout;
	if( _pulsein_timer_end >= 0x7FFFFFFF) _pulsein_timer_end = 0x7FFFFFFF; 
	if(timeout >= 0x0000FFFF) timeout = 0x0000FFFF;

	// initializing parameter
	_pulsein_port = digital_pin_to_port[pin];
	_pulsein_bit  = digital_pin_to_bit[pin];
	_pulsein_value = value;
	_pulsein_flag = PULSEIN_WAITING;

	// initializing interrupt setting
	m= _pulsein_bit;		// Calculate bit position of GPIO to bit number
	for(n=0;n<8;n++)
	{
		if(m == 0x01) break;
		m>>=1;
	}
	if(n == 8) return 0;	// error
	n += pulsein_cal_port_num();	// Calculate GPIO number.  P00 = 0, P10 = 8, P57 = 0x3F
	
	// BLKCON  Enabling TM6,7
	timer16[PULSEIN_TIMER].open(true);
	
	// Timer initialization
	timer16_set(PULSEIN_TIMER, (UINT16)(TM_CS_OSCLK | TM_DIV16 | TM_MODE_16BIT | TM_OST_REROAD), (UINT16)timeout, timer_pulsein_isr);
	
	// Setting of EXINT
	pulsein_irq_sethandler(extin_pulsein_isr);
	
//	__DI();
	dis_interrupts(DI_WIRING_PULSE);
	
	pulsein_irq_ext_init( EXIn_EDGE_BOTH, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, n);
	pulsein_irq_ext_clearIRQ();
	pulsein_irq_ext_ena();
	
	// Prepare data for interrupt handler
	
	// Start timer
	timer16_start(PULSEIN_TIMER);
	
//	__EI();
	enb_interrupts(DI_WIRING_PULSE);
	
	while( _pulsein_flag < PULSEIN_DETECT_2 )
	{
		wdt_clear();
		lp_setHaltMode();
		if(_pulsein_timer_end <= _pulsein_timer_now )
		{
			_pulsein_flag = PULSEIN_TIMEOUT;
		}
	}
	
	// disenabling EXTINT
	pulsein_irq_ext_init( EXIn_EDGE_DISABLE, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, n);
	pulsein_irq_ext_dis();
	pulsein_irq_ext_clearIRQ();
	
	// end of measuring process
	timer16_stop(PULSEIN_TIMER);
	
	// BLKCON  Disenable TM6,7
	timer16[PULSEIN_TIMER].open(false);
	
	// output duration of pulse
	if(_pulsein_flag == PULSEIN_TIMEOUT) return 0;
	else return (_pulsein_timer_end - _pulsein_timer_start - 4);
}

