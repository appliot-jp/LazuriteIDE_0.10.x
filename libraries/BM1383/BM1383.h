/* FILE NAME: BM1383.h
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

#ifndef _BM1383GLV_H_
#define _BM1383GLV_H_
#include "lazurite.h"

#define BM1383GLV_DEVICE_ADDRESS      (0x5D)    // 7bit Addrss
#define BM1383GLV_ID_VAL              (0x32)

#define BM1383GLV_ID                  (0x10)
#define BM1383GLV_POWER_DOWN          (0x12)
#define BM1383GLV_SLEEP               (0x13)
#define BM1383GLV_MODE_CONTROL        (0x14)
#define BM1383GLV_TEMPERATURE_MSB     (0x1A)
#define BM1383GLV_PRESSURE_MSB        (0x1C)

// ƒNƒ‰ƒX‚Ì’è‹`
typedef struct {
	byte (*init)(int slave_address) ;
	byte (*get_val)(float *data);
} t_BM1383;

extern const t_BM1383 bm1383;

#endif // _BM1383GLV_H_
