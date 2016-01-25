/* FILE NAME: lazurite_system.h
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


#ifndef _ARDUINO_SYSTEM_H_
#define _ARDUINO_SYSTEM_H_

#include "common.h"
#include "lazurite.h"

//********************************************************************************
//   global definitions
//********************************************************************************
typedef enum {
	HALT_I2C1_END = 1,
	HALT_I2C0_END,
	HALT_DELAY
} HALT_EVENT;

//********************************************************************************
//   global parameters
//********************************************************************************
extern BOOL	delay_irq_mode;
//********************************************************************************
//   extern function definitions
//********************************************************************************
extern void HALT_Until_Event(HALT_EVENT halt_event);
#define delay(v) delay_long((unsigned long)v)
#define sleep(v) sleep_long((unsigned long)v)
#define delayMicroseconds(v) delay_microseconds((unsigned long)v)
extern void init(void);
extern void delay_long(unsigned long ms);
extern void sleep_long(unsigned long ms);
extern void delay_microseconds(unsigned long us);
extern unsigned long millis(void);
extern unsigned long micros(void);
extern void interrupts(void);
extern void noInterrupts(void);
extern void wait_event(bool *flag);

#endif // _ARDUINO_SYSTEM_H_

