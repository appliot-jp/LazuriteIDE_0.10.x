/* FILE NAME: _draft_header.h
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

#ifndef _IOT_H_
#define _IOT_H_

#include "common.h"
#include "lazurite.h"
#include "mcu.h"


//********************************************************************************
//   global definitions
//********************************************************************************
//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************

#define INT8_VAL		( 1 )
#define UINT8_VAL		( 2 )
#define INT16_VAL		( 3 )
#define UINT16_VAL		( 4 )
#define INT32_VAL		( 5 )
#define UINT32_VAL		( 6 )
#define FLOAT_VAL		( 7 )
#define DOUBLE_VAL		( 8 )
#define STATE_TO_OFF	( 0 )
#define STATE_TO_ON		( 1 )

typedef struct {
	union  {
		int8_t int8_val;
		uint8_t uint8_val;
		int16_t int16_val;
		uint16_t uint16_val;
		int32_t int32_val;
		uint32_t uint32_val;
		float float_val;
		double double_val;
	} data;
	uint8_t type;
	uint8_t digit;
	uint16_t reason;
} SENSOR_VAL;

extern char* sensor_init(void);
extern void sensor_meas(int index, SENSOR_VAL *sensor_val);
extern bool sensor_activate(void);
extern void sensor_deactivate(void);
extern bool waitEventFlag;
extern void sensor_set_state_change_callback(void (*f)(uint8_t state));

#endif // _IOT_H_
