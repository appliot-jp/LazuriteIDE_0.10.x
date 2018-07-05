/* FILE NAME: flash.h
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


#ifndef _FLASH_H_
#define _FLASH_H_

//********************************************************************************
//   global definitions
//********************************************************************************
//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************

typedef struct {
	void (*write)(unsigned char sector, unsigned short address, unsigned short data);
	unsigned short (*read)(unsigned char sector, unsigned short address);
	void (*erase)(unsigned char sector);
	void (*write_byte)(unsigned char sector, unsigned short address, unsigned char data);
	unsigned char (*read_byte)(unsigned char sector, unsigned short address);
	
} DATAFLASH;

extern const DATAFLASH Flash;


#endif  // _ANALOGIO_H_

