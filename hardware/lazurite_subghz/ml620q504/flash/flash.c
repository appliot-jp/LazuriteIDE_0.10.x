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

#include "mcu.h"
#include "rdwr_reg.h"
#include "flash.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/


/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/


/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/

/**
 * Erase Flash Memory (Block)
 *
 * Erase the flash memory space which include the specified the address by a block.
 *
 * @param[in]   addr    Erase Block Address
 * @return      None
 * @note        Enable self programming, before call this function.
 */
void flash_eraseBlock( unsigned short __far *addr )
{
	/* set FLASH Accepter */
	write_reg8( FLASHACP, 0xFA );
	write_reg8( FLASHACP, 0xF5 );

	/* set FLASH Seg */
	write_reg8( FLASHSEG, (unsigned char)(((unsigned long)addr & 0xFF0000) >> 16) );

	/* set FLASH Address */
	write_reg8( FLASHAH, (unsigned char)(((unsigned long)addr & 0xFF00) >> 8) );

	/* start FLASH Erase */
	set_bit( FERS );
	__asm("nop");
	__asm("nop");
}

/**
 * Erase Flash Memory (Sector)
 *
 * Erase the flash memory space which include the specified the address by a sector.
 *
 * @param[in]   addr    Erase Sector Address
 * @return      None
 * @note        Enable self programming, before call this function.
 */
void flash_eraseSector( unsigned short __far *addr )
{
	/* set FLASH Accepter */
	write_reg8( FLASHACP, 0xFA );
	write_reg8( FLASHACP, 0xF5 );

	/* set FLASH Seg */
	write_reg8( FLASHSEG, (unsigned char)(((unsigned long)addr & 0xFF0000) >> 16) );

	/* set FLASH Address */
	write_reg8( FLASHAH, (unsigned char)(((unsigned long)addr & 0xFF00) >> 8) );

	/* start FLASH Erase */
	set_bit( FSERS );
	__asm("nop");
	__asm("nop");
}

/**
 * Write Flash Memory
 *
 * Write the one word data to the flash memory.
 *
 * @param[in]   addr    Write Address
 * @param[in]   data    Write Data
 * @return      None
 * @note        Enable self programming, before call this function.
 */
void flash_writeWord( unsigned short __far *addr, unsigned short data )
{
	/* set FLASH Accepter */
	write_reg8( FLASHACP, 0xFA );
	write_reg8( FLASHACP, 0xF5 );

	/* set FLASH Seg */
	write_reg8( FLASHSEG, (unsigned char)(((unsigned long)addr & 0xFF0000) >> 16) );

	/* set FLASH Address */
	write_reg16( FLASHA, (unsigned short)((unsigned long)addr & 0xFFFF) );

	/* Write Data */
	write_reg16( FLASHD, data );
	__asm("nop");
	__asm("nop");
}

/**
 * Execute Remap
 *
 * Remap the program memory space at the restart address.
 *
 * @param[in]   remapAddr   Remap Address
 * @return      None
 */
void flash_executeRemap( unsigned long remapAddr )
{
	unsigned char   reg;

	/* Set Remap Address */
	reg = (unsigned char)(remapAddr >> 12);
	if(remapAddr == (unsigned long)0xFC00){
	    reg |= 0x80;
	}
	write_reg8( REMAPADD, reg );

	/* Set ELEVEL 2 */
	__asm("mov r0,psw\n or r0,#2\n mov psw,r0\n");

	/* Software Reset */
	__asm("brk");
	/* endless loop */
	while(1);
}

