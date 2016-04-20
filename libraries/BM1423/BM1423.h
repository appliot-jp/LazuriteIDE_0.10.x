/* FILE NAME: BM1422.h
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

#ifndef _BM1422_H_
#define _BM1422_H_
#include "lazurite.h"

#define BM1422_DEVICE_ADDRESS_0E   (0x0E)    // 7bit Addrss
#define BM1422_DEVICE_ADDRESS_0F   (0x0F)    // 7bit Address
#define BM1422_WIA_VAL             (0x41)

#define BM1422_WIA                 (0x0F)
#define BM1422_DATAX               (0x10)
#define BM1422_CNTL1               (0x1B)
#define BM1422_CNTL2               (0x1C)
#define BM1422_CNTL3               (0x1D)
#define BM1422_AVE_A               (0x40)
#define BM1422_CNTL4               (0x5C)

// ƒNƒ‰ƒX‚Ì’è‹`
typedef struct {
	byte (*init)(int slave_address) ;
	byte (*get_val)(float *data) ; 
}t_BM1423;

extern const t_BM1423 bm1423;

#endif // _BM1422_H_
