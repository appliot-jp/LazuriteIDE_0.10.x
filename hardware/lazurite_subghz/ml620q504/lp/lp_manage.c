/* FILE NAME: lp_manage.c
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
#include "lp_manage.h"

/**
 * Setting STOP mode
 *
 * @param       -
 * @return      None
 */
void lp_setStopMode( void )
{
	/* set StopCode Accepter */
	write_reg8( STPACP, 0x50 );
	write_reg8( STPACP, 0xA0 );

	/* The CPU mode is changed to the STOP mode. */
	/* When the mode switch to STOP mode at High speed oscillator is
	 * used, Frequency Status Register (FSTAT) HOSCS bit must be "0". */
	set_bit( STP );
	__asm("nop\n");
	__asm("nop\n");
}

/**
 * Setting HALT mode
 *
 * @param       -
 * @return      None
 */
void lp_setHaltMode( void )
{
	/* The CPU mode is changed to the HALT mode. */
	set_bit( HLT );
	__asm("nop\n");
	__asm("nop\n");
}

/**
 * Setting HALT-H mode
 *
 * @param       -
 * @return      None
 */
void lp_setHaltHMode( void )
{
	/* The CPU mode is changed to the HALT-H mode. */
	/* When the mode switch to HALT-H mode at High speed oscillator 
	 * is used, Frequency Status Register (FSTAT) HOSCS bit must be
	 * "0". */
	set_bit( HLTH );
	__asm("nop\n");
	__asm("nop\n");
}

/**
 * Setting DEEP-HALT mode
 *
 * @param       -
 * @return      None
 */
void lp_setDeepHaltMode( void )
{
	/* The CPU mode is changed to the DEEP-HALT mode. */
	/* When High speed crystal/ceramic oscillator or Low speed 
	 * crystal/ceramic oscillator is used and the mode switch to 
	 * DEEP - HALT mode, Frequency Status Register (FSTAT) HOSCS bit
	 * must be "0". */
	set_bit( DHLT );
	__asm("nop\n");
	__asm("nop\n");
}

