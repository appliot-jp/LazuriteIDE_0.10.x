/* FILE NAME: digitalio.h
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


#include "common.h"
#include "lazurite.h"
#include "pin_assignment.h"

#ifndef _DIGITALIO_H_
#define _DIGITALIO_H_
//********************************************************************************
//   global definitions
//********************************************************************************
//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************
extern volatile void pinMode(uint8_t pin, uint8_t mode);
extern int digitalRead(uint8_t pin);
extern void digitalWrite(uint8_t pin, uint8_t val);

#endif // _DIGITALIO_H_

