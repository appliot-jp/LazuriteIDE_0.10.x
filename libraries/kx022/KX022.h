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

#define KX022_DEVICE_ADDRESS_1E   (0x1E)    // 7bit Addrss
#define KX022_DEVICE_ADDRESS_1F   (0x1F)    // 7bit Address
#define KX022_WAI_VAL             (0x14)

#define KX022_XOUT_L              (0x06)
#define KX022_WHO_AM_I            (0x0F)
#define KX022_CNTL1               (0x18)
#define KX022_ODCNTL              (0x1B)

#define KX022_CNTL1_TPE           (1 << 0)
#define KX022_CNTL1_WUFE          (1 << 1)
#define KX022_CNTL1_TDTE          (1 << 2)
#define KX022_CNTL1_GSELMASK      (0x18)
#define KX022_CNTL1_GSEL_2G       (0x00)
#define KX022_CNTL1_GSEL_4G       (0x08)
#define KX022_CNTL1_GSEL_8G       (0x10)
#define KX022_CNTL1_DRDYE         (1 << 5)
#define KX022_CNTL1_RES           (1 << 6)
#define KX022_CNTL1_PC1           (1 << 7)

#define KX022_ODCNTL_OSA_50HZ     (2)
#define KX022_ODCNTL_LPRO         (1 << 6)
#define KX022_IIR_BYPASS          (1 << 7)

#define KX022_CNTL1_VAL           (KX022_CNTL1_RES | KX022_CNTL1_GSEL_2G)
#define KX022_ODCNTL_VAL          (KX022_ODCNTL_OSA_50HZ)

typedef struct {
	byte (*init)(int slave_address) ;
	byte (*get_val)(float *data) ;    
} t_KX022;

extern const t_KX022 kx022;


#endif // _KX022_H_
