/* FILE NAME: KXG03.h
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

#ifndef _KXG03_H_
#define _KXG03_H_

#include "lazurite.h"

#define KXG03_DEVICE_ADDRESS_4E   (0x4E)    // 7bit Addrss
#define KXG03_DEVICE_ADDRESS_4F   (0x4F)    // 7bit Address
// #define KXG03_WHO_AM_I_VAL        (0x24)
#define KXG03_WHO_AM_I_VAL        (0xED)    // sample

#define KXG03_GYRO_XOUT_L         (0x02)
#define KXG03_ACC_XOUT_L          (0x08)
#define KXG03_WHO_AM_I            (0x30)
#define KXG03_STATUS1             (0x36)
#define KXG03_ACCEL_ODR_WAKE      (0x3E)
#define KXG03_STDBY               (0x43)

// ƒNƒ‰ƒX‚Ì’è‹`
typedef struct 
{
	byte (*init)(int slave_address);
	byte (*get_val)(float *data) ;    
} t_KXG03;
extern const t_KXG03 kxg03;

#endif // _KXG03_H_
