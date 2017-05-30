/* FILE NAME: lazurite.h
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


#ifndef _LAZURITE_H_
#define _LAZURITE_H_

#include "common.h"

//****************************************************
//        APIs
//****************************************************
extern void setup(void);				// Arduino startup function
extern void loop(void);					// Arduino loop function

//****************************************************
//        External API
//****************************************************
#include "analogio.h"
#include "digitalio.h"
#include "lazurite_system.h"
#include "serial.h"
#include "print.h"
#include "wiring_shift.h"
#include "wiring_pulse.h"
#include "WInterrupts.h"
#include "WMath.h"
#include "flash.h"
#include "tone.h"
#include <stdlib.h>
#include <math.h>

#ifdef SUBGHZ
	#include "subghz_api.h"
#endif
#ifdef LAZURITE_BLE
	#include "mk71251.h"
#endif

#endif /* _LAZURITE_H_ */

