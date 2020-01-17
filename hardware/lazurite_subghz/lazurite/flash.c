/* FILE NAME: flash.c
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


#include "driver_flash.h"
#include "flash.h"
#include "wdt.h"

void flash_write_word(unsigned char sector, unsigned short address, unsigned short data)
{
	address <<= 1;
	flash_write(sector, address, data);
	
}

short flash_read_word(unsigned char sector, unsigned short address)
{
	address <<= 1;
	return flash_read(sector, address);
}


const DATAFLASH Flash =
{
	flash_write_word,
	flash_read_word,
	flash_erase,
	flash_write_byte,
	flash_read_byte
};
