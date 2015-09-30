/* FILE NAME: rcAdc.c
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
#include "rcAdc.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define RCADC_MAX_CNT_VAL       ( 0x01000000UL )

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
 * Initialize RC-ADC
 *
 * @param[in]   radmod      Configuration of RC-ADC                             <br>
 *                          (Setting value of RADMOD register)                  <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Mode        : RCADC_MODE0, or RCADC_MODE1, or ( ... ), or RCADC_MODE6, or RCADC_MODE7
 * @arg                         Clock       : RCADC_CLK_LS, or RCADC_CLK_HS, or ( ... ), or RCADC_CLK_HS32, or RCADC_CLK_HS64
 * @arg                         Interrupt   : RCADC_INT_A, or RCADC_INT_B       <br>
 *                                            (Interrupt request by counter A or B overflow)
 *
 *
 * @param[in]   radc        The value of counter A or counter B.                <br>
 *                          (from 0x00000001 to 0x00FFFFFF)
 *
 * @return      None
 *
 * @see         MCU Users manual 'Description of Operation' in chapter 'RC Oscillation Type A/D Converter'.
 */
void rcAdc_init( unsigned short radmod, unsigned long radc )
{
	radc = (unsigned long)RCADC_MAX_CNT_VAL - radc;

	/*--- Interrupt mode, A/D counter ---*/
	/* Interrupt request by counter A overflow */
	if( (radmod & RADMOD_RADI) == RCADC_INT_A ) {
		/* Set Counter A */
		write_reg16( RADCA0, (unsigned short)( radc &  0x0000FFFF ) );
		write_reg8( RADCA1L, (unsigned char) ( radc >> 16 ) );
		/* Clear Counter B */
		write_reg16( RADCB0, (unsigned short)0x0000 );
		write_reg8( RADCB1L, (unsigned char)0x00 );
	}
	/* Interrupt request by counter B overflow */
	else {
		/* Set Counter B */
		write_reg16( RADCB0, (unsigned short)( radc &  0x0000FFFF ) );
		write_reg8( RADCB1L, (unsigned char) ( radc >> 16 ) );
		/* Clear Counter A */
		write_reg16( RADCA0, (unsigned short)0x0000 );
		write_reg8( RADCA1L, (unsigned char)0x00 );
	}

	/*--- Setting RC-ADC mode ---*/
	write_reg16( RADMOD, radmod );
}

