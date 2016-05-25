/* FILE NAME: SPI.h
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

#ifndef _ARDUINO_SPI_H_
#define _ARDUINO_SPI_H_

#include "common.h"
#include "lazurite.h"
#include "mcu.h"

//********************************************************************************
//   global definitions
//********************************************************************************
#define SPI_CLOCK_DIV2 0x01
#define SPI_CLOCK_DIV4 0x02
#define SPI_CLOCK_DIV8 0x04
#define SPI_CLOCK_DIV16 0x08
#define SPI_CLOCK_DIV32 0x10
#define SPI_CLOCK_DIV64 0x20
#define SPI_CLOCK_DIV128 0x40

#define SPI_MODE0 0x00
#define SPI_MODE1 0x20
#define SPI_MODE2 0x40
#define SPI_MODE3 0x60

#define SPI_MODE_MASK 		0x60	// SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_MSBFIRST		0x10	// 1: MSBFIRST,  0: LSBFIRST(default)

//********************************************************************************
//   global parameters
//********************************************************************************
// For equivalent to Arduino API
typedef struct {
	volatile unsigned char (*transfer)(UCHAR _data);
	void (*attachInterrupt)(void);
	void (*detachInterrupt)(void); // Default
	void (*begin)(void); // Default
	void (*end)(void);
	void (*setBitOrder)(uint8_t);
	void (*setDataMode)(uint8_t);
	void (*setClockDivider)(UINT16 ckdiv);
} SPIClass;

//********************************************************************************
//   extern function definitions
//********************************************************************************
extern const SPIClass SPI;

#endif // _ARDUINO_SPI_H_

