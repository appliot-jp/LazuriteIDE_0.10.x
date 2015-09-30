/* FILE NAME: WInterrupts.c
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
#include "driver_extirq.h"
#include "mcu.h"
#include "Winterrupts.h"

//********************************************************************************
//   global parameters
//********************************************************************************

//********************************************************************************
//   local definitions
//********************************************************************************

//********************************************************************************
//   local parameters
//********************************************************************************
//********************************************************************************
//   local function definitions
//********************************************************************************
//********************************************************************************
//   local functions
//********************************************************************************

void attachInterrupt(UCHAR irqnum, void (*func)(void), int mode)
{
	unsigned char pin;
	if(irqnum > 1) return;
	
	if(irqnum == 0) pin = 2;
	else pin = 3;
	
	if(func == NULL) return;

	//	void drv_attachInterrupt(unsigned char pin,unsigned char irqnum, void (*func)(void), int mode,bool sampling, bool filter)
	drv_attachInterrupt(digital_pin_to_port[pin], irqnum, func, mode,false,false);
	return;
}

void detachInterrupt(UCHAR irqnum)
{
	if(irqnum > 1) return;
	// void drv_detachInterrupt(unsigned char irqnum)
	drv_detachInterrupt(irqnum);
	
	return;
}

