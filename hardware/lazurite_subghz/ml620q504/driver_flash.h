/* FILE NAME: driver_flash.h
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

#ifndef _DRIVER_FLASH_H_
#define _DRIVER_FLASH_H_

//********************************************************************************
//   global definitions
//********************************************************************************
//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************
extern void flash_write(unsigned char sector, unsigned short address, unsigned short data);
extern unsigned short flash_read(unsigned char sector, unsigned short address);
extern void flash_erase(unsigned char sector);
extern void flash_write_byte(unsigned char sector, unsigned short address, unsigned char data);
extern unsigned char flash_read_byte(unsigned char sector, unsigned short address);

#endif //_DRIVER_FLASH_H_

