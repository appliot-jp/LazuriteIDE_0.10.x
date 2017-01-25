/* FILE NAME: RPR0521.h
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


#ifndef _RPR0521RS_H_
#define _RPR0521RS_H_

#include "lazurite.h"

#define RPR0521RS_DEVICE_ADDRESS      (0x38)    // 7bit Addrss
#define RPR0521RS_PART_ID_VAL         (0x0A)
#define RPR0521RS_MANUFACT_ID_VAL     (0xE0)

#define RPR0521RS_SYSTEM_CONTROL      (0x40)
#define RPR0521RS_MODE_CONTROL        (0x41)
#define RPR0521RS_ALS_PS_CONTROL      (0x42)
#define RPR0521RS_PS_CONTROL          (0x43)
#define RPR0521RS_PS_DATA_LSB         (0x44)
#define RPR0521RS_ALS_DATA0_LSB       (0x46)
#define RPR0521RS_MANUFACT_ID         (0x92)

// ƒNƒ‰ƒX‚Ì’è‹`
typedef struct {
	byte (*init)(void) ;
    byte (*get_rawpsalsval)(unsigned char *data);
    byte (*get_psalsval)(unsigned short *ps, float *als);
    byte (*get_oneShot)(unsigned short *ps, float *als);
    float (*convert_lux)(unsigned short *data);
    byte (*write)(unsigned char memory_address, unsigned char *data, unsigned char size);
    byte (*read)(unsigned char memory_address, unsigned char *data, int size); 
} t_RPR0521RS;

extern const t_RPR0521RS rpr0521rs;

#endif // _RPR0521RS_H_
