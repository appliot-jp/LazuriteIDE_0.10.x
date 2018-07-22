/* FILE NAME: BH1790GLC.h
 *
 * Copyright (c) 2018  Lapis Semiconductor Co.,Ltd.
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
#ifndef _BH1790GLC_H_
#define _BH1790GLC_H_

#include "lazurite.h"

#define BH1790GLC_DEVICE_ADDRESS      (0x5B)    // 7bit Addrss
#define BH1790GLC_MID_VAL             (0xE0)
#define BH1790GLC_PID_VAL             (0x0D)

#define BH1790GLC_MANUFACTURER_ID     (0x0F)
#define BH1790GLC_PART_ID             (0x10)
#define BH1790GLC_MEAS_CONTROL1       (0x41)
#define BH1790GLC_DATAOUT_LEDOFF      (0x54)

#define BH1790GLC_MEAS_CONTROL1_RDY                     (1 << 7)
#define BH1790GLC_MEAS_CONTROL1_LED_LIGHTING_FREQ_128HZ (0 << 2)
#define BH1790GLC_MEAS_CONTROL1_RCYCLE_32HZ             (2 << 0)

#define BH1790GLC_MEAS_CONTROL2_LED_EN_00               (0 << 6)
#define BH1790GLC_MEAS_CONTROL2_LED_ON_TIME_0_3MS       (0 << 5)
#define BH1790GLC_MEAS_CONTROL2_LED_CURRENT_10MA        (12 << 0)

#define BH1790GLC_MEAS_START_MEAS_ST                    (1 << 0)

#define BH1790GLC_MEAS_CONTROL1_VAL   (BH1790GLC_MEAS_CONTROL1_RDY | BH1790GLC_MEAS_CONTROL1_LED_LIGHTING_FREQ_128HZ | BH1790GLC_MEAS_CONTROL1_RCYCLE_32HZ)
#define BH1790GLC_MEAS_CONTROL2_VAL   (BH1790GLC_MEAS_CONTROL2_LED_EN_00 | BH1790GLC_MEAS_CONTROL2_LED_ON_TIME_0_3MS | BH1790GLC_MEAS_CONTROL2_LED_CURRENT_10MA)
#define BH1790GLC_MEAS_START_VAL      (BH1790GLC_MEAS_START_MEAS_ST)

typedef struct 
{
    byte (*init)(void);
    byte (*get_rawval)(unsigned char *data);
    byte (*get_val)(unsigned short *data);
    byte (*write)(unsigned char memory_address, unsigned char *data, unsigned char size);
    byte (*read)(unsigned char memory_address, unsigned char *data, int size);
} f_BH1790GLC;
extern const f_BH1790GLC bh1790glc;

#endif // _BH1790GLC_H_
