/* FILE NAME: MsTimer4.h
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


#ifndef _MS_TIMER4_H_
#define _MS_TIMER4_H_

#include "lazurite.h"

#include "driver_timer.h"

//********************************************************************************
//   global definitions
//********************************************************************************
typedef struct {
	unsigned long  current_time_h;
	unsigned long  target_time_h;
	unsigned short target_time_l;
	void (*callback)(void);
	void (*set)(unsigned long ms, void (*f)());
	void (*start)(void);
	void (*stop)(void);
} MsTimer4;

extern MsTimer4 timer4;
//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************


#endif // _MS_TIMER4_H_

