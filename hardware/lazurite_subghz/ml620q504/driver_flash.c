/* FILE NAME: driver_flash.c
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

#include "common.h"
#include "lazurite.h"
#include "driver_gpio.h"
#include "driver_adc.h"
#include "driver_pin_assignment.h"
#include "mcu.h"
#include "rdwr_reg.h"


void flash_write(unsigned char sector, unsigned short address, unsigned short data)
{
	set_bit(FSELF);
	write_reg8( FLASHACP, 0xFA );
	write_reg8( FLASHACP, 0xF5 );
	write_reg8( FLASHSEG, 7 );
	if(sector != 0) address += 0x400;
	write_reg16( FLASHA, address );
	write_reg16( FLASHD, data );
	__asm("nop");
	__asm("nop");
	clear_bit(FSELF);

}

void flash_write_byte(unsigned char sector, unsigned short address, unsigned char data)
{
	unsigned short tmp_data;
	unsigned short tmp_addr = address&~(0x0001);
	
	tmp_data = flash_read(sector, address&0xFE);
	if(tmp_addr&0x0001)
	{
		tmp_data <<= 8;
		tmp_data |= 0x00FF;
	}
	else
	{
		tmp_data |= 0xFF00;
	}
	set_bit(FSELF);
	write_reg8( FLASHACP, 0xFA );
	write_reg8( FLASHACP, 0xF5 );
	write_reg8( FLASHSEG, 7 );
	if(sector != 0) address += 0x400;
	write_reg16( FLASHA, tmp_addr );
	write_reg16( FLASHD, tmp_data );
	__asm("nop");
	__asm("nop");
	clear_bit(FSELF);

}


unsigned short flash_read(unsigned char sector, unsigned short address)
{
	unsigned short __far *far_addr;
	unsigned long laddr;
	
	if(sector == 0)	laddr = 0x000F0000 + address;
	else			laddr = 0x000F0400 + address;
	
	far_addr = (unsigned short __far *)laddr;
	return *far_addr;
}

unsigned char flash_read_byte(unsigned char sector, unsigned short address)
{
	unsigned char __far *far_addr;
	unsigned long laddr;
	
	if(sector == 0)	laddr = 0x000F0000 + address;
	else			laddr = 0x000F0400 + address;
	
	far_addr = (unsigned char __far *)laddr;
	return *far_addr;
}

void flash_erase(unsigned char sector)
{
	set_bit(FSELF);
	switch(sector)
	{
	case 0:
		/* set FLASH Accepter */
		write_reg8( FLASHACP, 0xFA );
		write_reg8( FLASHACP, 0xF5 );
		write_reg8( FLASHSEG, 7);
		write_reg8( FLASHAH, 0);
		set_bit( FSERS );
			break;
	case 1:
		/* set FLASH Accepter */
		write_reg8( FLASHACP, 0xFA );
		write_reg8( FLASHACP, 0xF5 );
		write_reg8( FLASHSEG, 7);
		write_reg16( FLASHA, 0x0400);
		set_bit( FSERS );
		break;
	}
	clear_bit(FSELF);
}
