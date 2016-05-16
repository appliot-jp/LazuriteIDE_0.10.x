/* FILE NAME: Wire0.h
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

#ifndef _WIRE0_H_
#define _WIRE0_H_

#include "common.h"
#include "lazurite.h"

#define I2C_BUFFER_LENGTH 32

//********************************************************************************
//   global definitions
//********************************************************************************

//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************
// to match API with arduino 
typedef struct {
	void (*begin)(void);
	size_t (*requestFrom)(UCHAR address,UCHAR quantity, UCHAR sendStop);
	int (*available)(void);
	int (*read)(void);
	void (*beginTransmission)(uint8_t address);
	size_t (*write_byte)(uint8_t data);
	size_t (*write)(const uint8_t *data, size_t quantity);
	uint8_t (*endTransmission)(uint8_t sendStop);
}TwoWire0;

extern const TwoWire0 Wire0;

#endif // _WIRE_H_

