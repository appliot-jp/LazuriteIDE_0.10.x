/* FILE NAME: main.c
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

#pragma SEGCODE "OTA_SEGCODE2"
#pragma SEGINIT "OTA_SETUP_LOOP"

/* --- Common Header --- */
#include <mcu.h>
#include "lazurite_system.h"

static void (*fn_p[2])(void) = {setup, loop};

/*******************************************************************************
*
* Function  :   Main
*
* Input     :   None
*
* Output    :   None
*
* Return    :   None
*
*******************************************************************************/
void main( void )
{
	/* --- Platform Initial Setting --- */
	init();
#ifdef SUBGHZ_OTA
	OTA.boot();
#endif
	fn_p[0]();			// setup()
	for(;;)
	{
		fn_p[1]();		// loop()
	}
}
