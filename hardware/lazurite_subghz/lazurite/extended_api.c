/* FILE NAME: extended_api.c
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
#include "lazurite.h"
//#include "lazurite_system.h"
#include "wdt.h"
#include "driver_timer.h"
#include "driver_irq.h"
//#include "driver_i2c.h"
#include "lp_manage.h"

//********************************************************************************
//   global parameters
//********************************************************************************
//BOOL	delay_irq_mode;					//
//********************************************************************************
//   local definitions
//********************************************************************************
//********************************************************************************
//   local parameters
//********************************************************************************

static boolean flag_waitPinCondition;
static unsigned char pin_to_portnum(unsigned char pin);

//********************************************************************************
//   local function definitions
//********************************************************************************
//********************************************************************************
//   local functions
//********************************************************************************

void isr_waitPinCondition(void)
{
	flag_waitPinCondition = TRUE;
}

// 
void waitPinCondition(unsigned char pin, int condition)
{
	unsigned char *exisel;
	if(pin > MAX_PIN_NO) return;
	if((condition <= 0) || (condition >= 4)) return;
	
	port_num = pin_to_port_num(pin);
	irq_ext5_dis();
	pinMode(pin, INPUT);
	irq_ext5_clearIRQ();
	irq_sethandler(IRQ_NO_EXI5INT,isr_waitPinCondition);
	irq_ext5_ena();
	
	flag_waitPinCondition = FALSE;
	
	switch(condition)
	{
	case LOW:
		irq_ext5_init( EXIn_EDGE_FALLING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, port_num);
		if(digitalRead(pin) == LOW) set_bit(QEXI5);						// if pin is already LOW, set trigger of interrupt;
		break;
	case RISING:
		irq_ext5_init( EXIn_EDGE_RISING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, port_num);
		break;
	case FALLING:
		irq_ext5_init( EXIn_EDGE_FALLING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, port_num);
		break;
	case CHANGE:
		irq_ext5_init( EXIn_EDGE_BOTH, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, port_num);
		break;
	default :
		flag_waitPinCondition = TRUE;
		break;
	}
	
	while(flag_waitPinCondition == FALSE)
	{
		lp_setDeepHaltMode();
	}
	irq_ext5_clearIRQ();
	irq_sethandler(IRQ_NO_EXI5INT,_intNullHdr);
	irq_ext5_dis();
}

