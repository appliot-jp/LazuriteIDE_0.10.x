/* FILE NAME: serial.h
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


#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "common.h"
#include "lazurite.h"
#include "mcu.h"


//********************************************************************************
//   global definitions
//********************************************************************************
#define BIN					2
#define OCT					8
#define DEC					10
#define HEX					16
//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************

typedef struct {
	void (*begin)(UINT32 baud);
	void (*end)(void);
	int (*available)(void);
	int (*read)(void);
	int (*peak)(void);
	void (*flush)(void);
	volatile size_t (*print)(char* data);
	volatile size_t (*println)(char* data);
	volatile size_t (*print_long)(long data, UCHAR fmt);
	volatile size_t (*println_long)(long data, UCHAR fmt);
	volatile size_t (*print_double)(double data, UCHAR digit);
	volatile size_t (*println_double)(double data, UCHAR digit);
	volatile size_t (*write)(char* data, size_t quantity);
	volatile size_t (*write_byte)(char data);
	int (*tx_available)(void);								// add 
} HardwareSerial;

extern const HardwareSerial Serial;
extern const HardwareSerial Serial1;
extern const HardwareSerial Serial2;
extern const HardwareSerial Serial3;

#endif // _SERIAL_H_

