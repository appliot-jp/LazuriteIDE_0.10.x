/* FILE NAME: lazurite_api.c
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
#include "driver_api.h"

//********************************************************************************
//   local definition
//********************************************************************************
//********************************************************************************
//   local function
//********************************************************************************
static void (*_loopHs)(void);
//********************************************************************************
//   local parameters
//********************************************************************************

//*******************************************************************************/
//      void set_LoopHS_callback(unsigned short loophs_interval,void (*callback_func)(void));
//      function: timer interrupt of ms order.
//                On this BLE application, main loop function is called per about 1 second.
//                Some of sensors require many of actual data from sensor.
//                The function can be used to get data from sensor for short interval.
//                After compless process, data can be sent per 1 sec.
//      parameters:
//           loophs_interval =  interval of function.
//                              interval = 1/32768 x n [s] = about 30us x n
//                              minimum value is 33 ( about 1ms ).
//           callback_func   =  call back function.
//********************************************************************************/
void _handler_LoopHS_callback(void)
{
	// HSCLK setting
	// HALT setting
	// execute call back setting
	_loopHs();
	
	
}
void set_LoopHS_callback(unsigned short loophs_interval,void (*callback_func)(void))
{
	_loopHs = callback_func;
	// BLKCON Enable
	// set interval
	// use timer 4-5
	// time base clock = 1LSCLK = 
//	irq_sethandler(IRQ_NO,_loopHs);
	return;
}

void init_LoopHS_callback(void)
{
//	_loopHs = _intNullHdr;
//	irq_sethandler(IRQ_NO,_loopHs);
	return;
}

void start_LoopHS_callback(void)
{
	return;
}

void stop_LoopHS_callback(void)
{
	return;
}
void arduino_setup(void)
{
	setup();
	return;
}

void arduino_main(void)
{
	loop();										// Prepare sensor data
	return;
}
