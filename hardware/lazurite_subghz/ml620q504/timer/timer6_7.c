/* FILE NAME: timer6_7.c
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
#include "timer6_7.h"
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
 * Initialize the value to be compared with the timer6 counter and an 8-bit/16-bit binary counter
 * @note Set TMnD when the timer stops.
 *
 * @param[in]   cnt     If timer6 is 8bit mode, then this program set compare value to TM6D register.<br>
 *                      If timer6 is 16bit mode, then this program set compare value to TM67D register.
 * @return      None
 */
void timer6_setCnt( unsigned short cnt )
{
	/*--- mode select ---*/
	if( get_bit( T67M16 ) == 0 ) {
	    /*--- 8bit timer mode ---*/
	    write_reg8( TM6C, 0x00 );                           /* counter clear */
	    write_reg8( TM6D, (unsigned char)cnt );             /* compare value set */
	}
	else {
	    /*--- 16bit timer mode ---*/
	    write_reg16( TM67C, 0x0000 );                       /* counter clear */
	    write_reg16( TM67D, (unsigned short)cnt );          /* compare value set */
	}
}

/**
 * Initialize the value to be compared with the timer7 counter and an 8-bit binary counter
 * @note Set TMnD when the timer stops.
 *
 * @param[in]   cnt     the value of TM7D register
 * @return      None
 */
void timer7_setCnt( unsigned short cnt )
{
	/*--- Only 8bit timer mode ---*/
	write_reg8( TM7C, 0x00 );                               /* counter clear */
	write_reg8( TM7D, (unsigned char)cnt );                 /* compare value set */
}

