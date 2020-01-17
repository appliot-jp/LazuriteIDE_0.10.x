/* FILE NAME: random.c
 *
 * Copyright (c) 2018  Lapis Semiconductor Co.,Ltd.
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

#ifdef SUBGHZ_OTA
	#pragma SEGCODE "OTA_SEGCODE"
	#pragma SEGINIT "OTA_SEGINIT"
	#pragma SEGNOINIT "OTA_SEGNOINIT"
	#pragma SEGCONST "OTA_SEGCONST"
#endif

#define RAND_MAX ( 32767 )

unsigned long seeds = 1;

int rand(void)
{	/* compute pseudo-random value */
	seeds = seeds * 1103515245 + 12345;
	return ((unsigned int)(seeds >> 16) & RAND_MAX);
}

void srand(unsigned int data)
{	/* alter the seeds */
	seeds = data;
}
