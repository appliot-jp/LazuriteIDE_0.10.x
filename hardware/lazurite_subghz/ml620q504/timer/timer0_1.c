/* FILE NAME: timer0_1.c
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
#include "timer0_1.h"
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
 * Initialize the value to be compared with the timer0 counter and an 8-bit/16-bit binary counter
 * @note Set TMnD when the timer stops.
 *
 * @param[in]   cnt     If timer0 is 8bit mode, then this program set compare value to TM0D register.<br>
 *                      If timer0 is 16bit mode, then this program set compare value to TM01D register.
 * @return      None
 */
void timer0_setCnt( unsigned short cnt )
{
	/*--- mode select ---*/
	if( get_bit( T01M16 ) == 0 ) {
	    /*--- 8bit timer mode ---*/
	    write_reg8( TM0C, 0x00 );                           /* counter clear */
	    write_reg8( TM0D, (unsigned char)cnt );             /* compare value set */
	}
	else {
	    /*--- 16bit timer mode ---*/
	    write_reg16( TM01C, 0x0000 );                       /* counter clear */
	    write_reg16( TM01D, (unsigned short)cnt );          /* compare value set */
	}
}

/**
 * Initialize the value to be compared with the timer1 counter and an 8-bit binary counter
 * @note Set TMnD when the timer stops.
 *
 * @param[in]   cnt     the value of TM1D register
 * @return      None
 */
void timer1_setCnt( unsigned short cnt )
{
	/*--- Only 8bit timer mode ---*/
	write_reg8( TM1C, 0x00 );                               /* counter clear */
	write_reg8( TM1D, (unsigned char)cnt );                 /* compare value set */
}

