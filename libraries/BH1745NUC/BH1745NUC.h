/* FILE NAME: BH1745NUC.h
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


#ifndef _BH1745NUC_H_
#define _BH1745NUC_H_

#define BH1745NUC_DEVICE_ADDRESS_38            (0x38)    // 7bit Addrss
#define BH1745NUC_DEVICE_ADDRESS_39            (0x39)    // 7bit Addrss
#define BH1745NUC_PART_ID_VAL                  (0x0B)
#define BH1745NUC_MANUFACT_ID_VAL              (0xE0)

#define BH1745NUC_SYSTEM_CONTROL               (0x40)
#define BH1745NUC_MODE_CONTROL1                (0x41)
#define BH1745NUC_MODE_CONTROL2                (0x42)
#define BH1745NUC_MODE_CONTROL3                (0x43)
#define BH1745NUC_RED_DATA_LSB                 (0x50)
#define BH1745NUC_MANUFACTURER_ID              (0x92)

#define BH1745NUC_MODE_CONTROL1_MEAS_TIME160MS (0x00)

#define BH1745NUC_MODE_CONTROL2_ADC_GAIN_X1    (0)
#define BH1745NUC_MODE_CONTROL2_ADC_GAIN_X2    (1)
#define BH1745NUC_MODE_CONTROL2_ADC_GAIN_X16   (2)
#define BH1745NUC_MODE_CONTROL2_RGBC_EN        (1 << 4)

#define BH1745NUC_MODE_CONTROL1_VAL            (BH1745NUC_MODE_CONTROL1_MEAS_TIME160MS)
#define BH1745NUC_MODE_CONTROL2_VAL            (BH1745NUC_MODE_CONTROL2_RGBC_EN | BH1745NUC_MODE_CONTROL2_ADC_GAIN_X16)
#define BH1745NUC_MODE_CONTROL3_VAL            (0x02)

typedef struct {
	byte (*init)(int slave_address) ;
	byte (*get_val)(unsigned short *data);
} t_BH1745NUC;

extern const t_BH1745NUC bh1745;

#endif // _BH1745NUC_H_
