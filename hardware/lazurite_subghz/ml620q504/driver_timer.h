/* FILE NAME: driver_timer.h
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

#ifndef _DRIVER_TIMER_H_
#define _DRIVER_TIMER_H_

#include "common.h"
#include "driver_irq.h"

/************************************************/
/* Definition                                   */
/************************************************/
/************************************************/
/* Enumerated Type                              */
/************************************************/
/* --- Timer Channel Number --- */
typedef enum {
    TM_CH0 = 0,                 /* Timer 0              */
    TM_CH1,                     /* Timer 1              */
    TM_CH2,                     /* Timer 2              */
    TM_CH3,                     /* Timer 3              */
    TM_CH4,                     /* Timer 4              */
    TM_CH5,                     /* Timer 5              */
    TM_CH6,                     /* Timer 6              */
    TM_CH7                      /* Timer 7              */
} TM_CH;

extern void timer_8bit_set(unsigned char ch, unsigned char TMnCON, unsigned char TMnD, void (*func)(void));
extern void timer_8bit_start(unsigned char ch);
extern void timer_8bit_stop(unsigned char ch);
extern void timer_16bit_set(unsigned char ch, unsigned char TMnCON, unsigned short TMnnD, void (*func)(void));
extern void timer_16bit_start(unsigned char ch);
extern void timer_16bit_stop(unsigned char ch);

#endif  /* _DRIVER_TIMER_H_ */
