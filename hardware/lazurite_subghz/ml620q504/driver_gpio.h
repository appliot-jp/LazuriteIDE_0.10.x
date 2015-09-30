/* FILE NAME: driver_gpio.h
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

#ifndef _DRIVER_GPIO_H_
#define _DRIVER_GPIO_H_

//********************************************************************************
//   global definitions
//********************************************************************************
//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************

extern volatile void drv_pinMode(unsigned char pin, unsigned char mode);
extern int drv_digitalRead(unsigned char pin);
extern void drv_digitalWrite(unsigned char pin, unsigned char val);

#endif //_DRIVER_GPIO_H_

