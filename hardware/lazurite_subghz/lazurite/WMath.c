/* FILE NAME: WMath.c
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


#include <stdlib.h>

//********************************************************************************
//   global parameters
//********************************************************************************

//********************************************************************************
//   local definitions
//********************************************************************************
//********************************************************************************
//   local parameters
//********************************************************************************
//********************************************************************************
//   local function definitions
//********************************************************************************
//********************************************************************************
//   local functions
//********************************************************************************
void randomSeed(unsigned int seed)
{
	srand(seed);
	return;
}

long random(long howsmall, long howbig)
{
	long diff;
	if(howsmall >= howbig) return howsmall;
	diff = howbig - howsmall;
	return (rand()%diff + howsmall);
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (out_min + (x - in_min) * (out_max - out_min) / (in_max - in_min));
}

