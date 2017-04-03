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
extern uint16_t halt_event_timeout;

//********************************************************************************
//   extern function definitions
//********************************************************************************
extern void HALT_Until_Event(HALT_EVENT halt_event,uint16_t timeout);
#define delay(v) delay_long((unsigned long)v)
#define sleep(v) sleep_long((unsigned long)v)
#define delayMicroseconds(v) delay_microseconds((unsigned long)v)
extern void init(void);
extern void delay_long(unsigned long ms);
extern void sleep_long(unsigned long ms);
extern volatile void delay_microseconds(unsigned long us);
extern volatile unsigned long millis(void);
extern volatile unsigned long micros(void);
extern void interrupts(void);
extern void noInterrupts(void);
extern void wait_event(bool *flag);
extern bool wait_event_timeout(bool *flag,uint32_t time);
extern void set_timer0_function(void (*func)(uint32_t sys_timer_count));
extern uint8_t voltage_check(uint8_t level);

#define VLS_UNDER_1_898 (  2 )
#define VLS_1_898		(  3 )
#define VLS_2_000		(  4 )
#define VLS_2_093		(  5 )
#define VLS_2_196		(  6 )
#define VLS_2_309		(  7 )
#define VLS_2_409		(  8 )
#define VLS_2_605		(  9 )
#define VLS_2_800		( 10 )
#define VLS_3_068		( 11 )
#define VLS_3_394		( 12 )
#define VLS_3_797		( 13 )
#define VLS_4_226		( 14 )
#define VLS_4_667		( 15 )

#endif // _ARDUINO_SYSTEM_H_

