/* FILE NAME: tone.h
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


#ifndef _TONE_H_
#define _TONE_H_

#include "common.h"
#include "lazurite.h"
#include "mcu.h"


//********************************************************************************
//   global definitions
//********************************************************************************
//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************
extern void tone(uint8_t _pin, uint16_t frequency, long duration);
extern void noTone();
extern void setToneCh(uint8_t toneCh);

#endif // _TONE_H_

