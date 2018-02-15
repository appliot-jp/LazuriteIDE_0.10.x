/* FILE NAME: wdt.c
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

#pragma SEGCODE "OTA_SEGCODE1"

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


