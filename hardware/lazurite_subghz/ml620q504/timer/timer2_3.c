/* FILE NAME: timer2_3.c
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
#include "timer2_3.h"
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
 * Initialize the value to be compared with the timer2 counter and an 8-bit/16-bit binary counter
 * @note Set TMnD when the timer stops.
 *
 * @param[in]   cnt     If timer2 is 8bit mode, then this program set compare value to TM2D register.<br>
 *                      If timer2 is 16bit mode, then this program set compare value to TM23D register.
 * @return      None
 */
void timer2_setCnt( unsigned short cnt )
{
	/*--- mode select ---*/
	if( get_bit( T23M16 ) == 0 ) {
	    /*--- 8bit timer mode ---*/
	    write_reg8( TM2C, 0x00 );                           /* counter clear */
	    write_reg8( TM2D, (unsigned char)cnt );             /* compare value set */
	}
	else {
	    /*--- 16bit timer mode ---*/
	    write_reg16( TM23C, 0x0000 );                       /* counter clear */
	    write_reg16( TM23D, (unsigned short)cnt );          /* compare value set */
	}
}

/**
 * Initialize the value to be compared with the timer3 counter and an 8-bit binary counter
 * @note Set TMnD when the timer stops.
 *
 * @param[in]   cnt     the value of TM3D register
 * @return      None
 */
void timer3_setCnt( unsigned short cnt )
{
	/*--- Only 8bit timer mode ---*/
	write_reg8( TM3C, 0x00 );                               /* counter clear */
	write_reg8( TM3D, (unsigned char)cnt );                 /* compare value set */
}

