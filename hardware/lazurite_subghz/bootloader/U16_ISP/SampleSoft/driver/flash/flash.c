/*****************************************************************************
 flash.c

 Copyright (C) 2014 LAPIS Semiconductor Co., LTD.
 All rights reserved.

 This software is provided "as is" and any expressed or implied
  warranties, including, but not limited to, the implied warranties of
  merchantability and fitness for a particular purpose are disclaimed.
 LAPIS SEMICONDUCTOR shall not be liable for any direct, indirect,
 consequential or incidental damages arising from using or modifying
 this software.
 You (customer) can modify and use this software in whole or part on
 your own responsibility, only for the purpose of developing the software
 for use with microcontroller manufactured by LAPIS SEMICONDUCTOR.

 History
    2014.03.31 ver.1.00

******************************************************************************/
/**
 * @file    flash.c
 *
 * This module is 'Flash Memory' drivers.
 *
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

