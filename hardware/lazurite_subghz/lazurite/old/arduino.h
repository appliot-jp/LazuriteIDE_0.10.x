/* FILE NAME: arduino.h
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

#ifndef _ARDUINO_H_
#define _ARDUINO_H_

// 
#include "lazurite.h"
#include "arduino_api.c"


#ifdef    _ML620Q504
#include "pin_assignment.h"
#include "digitalio.h"
#include "analogio.h"
#include "spi.h"
#include "arduino_system.h"
#include "wiring_shift.h"
#include "WMath.h"
#include "WInterrpts.h"
#include "tone.h"
#include <math.h>
#include <stdlib.h>

//#include "wire.h"
#endif  // _ML620Q504

#endif /* _ARDUINO_H_ */



