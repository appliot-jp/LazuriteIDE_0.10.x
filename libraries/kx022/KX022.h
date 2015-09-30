/* FILE NAME: KX022.h
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



#ifndef _KX022_H_
#define _KX022_H_
#include "lazurite.h"

#define KX022_DEVICE_ADDRESS_1E   (0x1E)    // 7bit Addrss
#define KX022_DEVICE_ADDRESS_1F   (0x1F)    // 7bit Address
#define KX022_WAI_VAL             (0x14)

#define KX022_XOUT_L              (0x06)
#define KX022_WHO_AM_I            (0x0F)
#define KX022_CNTL1               (0x18)
#define KX022_CNTL3               (0x1A)
#define KX022_ODCNTL              (0x1B)
#define KX022_TILT_TIMER          (0x22)

typedef struct {
	byte (*init)(int slave_address) ;
	byte (*get_val)(float *data) ;    
} t_KX022;

extern const t_KX022 kx022;


#endif // _KX022_H_
