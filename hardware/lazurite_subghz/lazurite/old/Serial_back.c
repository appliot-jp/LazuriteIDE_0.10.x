/* FILE NAME: Serial_back.c
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
#include "rdwr_reg.h"
#include "mcu.h"
#include "driver_watch_dog.h"

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
void serial_init(void)
{
	CHAR text[16] = "";
	CHAR n;
	clear_bit(DUA0);			// enable UART0
	clear_bit(DUA1);			// enable UART0
	write_reg16(UA0BRT, 1667);	// ボーレート  8.192MHzで9600bps
	write_reg8(UA0MOD1, 0x08);	// 9600bps, パリティ なし, stop bit = 1, フロー制御 なし
	write_reg8(UA0MOD0, 0x04);	// OSCLKを使用
	set_bit(U0EN);	// UART Enable
	for(n=0;n<16;n++)
	{
		while(QUA0 == 0)
		{
			watchdog_clear();
		}
		clear_bit(QUA0);
		text[n]=UA0BUF;
	}
	for (n=0;n<16;n++)
	{
		write_reg8(UA1BUF, n);	// send
		while(QUA1 == 0)
		{
			watchdog_clear();
		}
		clear_bit(QUA1);
	}
	while(QUA1 == 0)
	{
		watchdog_clear();
	}
	clear_bit(QUA1);
	
	clear_bit(U0EN);	// UART Enable
	set_bit(DUA0);			// disenable UART0
	set_bit(DUA1);			// disenable UART0
	return;
}

void print(void)
{
	serial_init();
}