/*****************************************************************************
 wdt.c

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
    2014.03.31 ver.0.90

******************************************************************************/
/**
 * @file    wdt.c
 *
 * This module is WDT(Watch Dog Timer) drivers.
 *
 */
#include "mcu.h"
#include "rdwr_reg.h"
#include "wdt.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*--- bit field of registers ---*/
/* WDTCON */
#define WDTCON_WDP          ( 0x01 )        /**< WDTCON Register WDP bit      */

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
 * Clear WDT
 *
 * @param       -
 * @return      None
 */
void wdt_clear( void )
{
	do {
		write_reg8( WDTCON, 0x5A );
	} while ( (read_reg8( WDTCON ) & WDTCON_WDP) == 0 );
	write_reg8( WDTCON, 0xA5 );
}


