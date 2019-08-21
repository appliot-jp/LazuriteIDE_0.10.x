/* FILE NAME: LSM9DS1.h
 *
 * Copyright (c) 2017  LAPIS Semiconductor Co.,Ltd.
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

#ifndef _LSM9DS1_H_
#define _LSM9DS1_H_
#include <lazurite.h>

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
// operating mode
#define LSM9DS1_OPMODE_XLG			( 0x1 )
#define LSM9DS1_OPMODE_XL			( 0x2 )
#define LSM9DS1_OPMODE_MAG			( 0x4 )
// i2c device address
#define LSM9DS1_I2C_ADDR_XLG_6A		( 0x6A )
#define LSM9DS1_I2C_ADDR_XLG_6B		( 0x6B )
#define LSM9DS1_I2C_ADDR_M_1C		( 0x1C )
#define LSM9DS1_I2C_ADDR_M_1E		( 0x1E )
// accel and gyro mode odr parameter
#define LSM9DS1_ODR_XLG_14_9Hz		( 0x1 )
#define LSM9DS1_ODR_XLG_59_5Hz		( 0x2 )
#define LSM9DS1_ODR_XLG_119Hz		( 0x3 )
#define LSM9DS1_ODR_XLG_238Hz		( 0x4 )
#define LSM9DS1_ODR_XLG_476Hz		( 0x5 )
#define LSM9DS1_ODR_XLG_952Hz		( 0x6 )
// accel only mode odr parameter
#define LSM9DS1_ODR_XL_10Hz			( 0x1 )
#define LSM9DS1_ODR_XL_50Hz			( 0x2 )
#define LSM9DS1_ODR_XL_119Hz		( 0x3 )
#define LSM9DS1_ODR_XL_238Hz		( 0x4 )
#define LSM9DS1_ODR_XL_476Hz		( 0x5 )
#define LSM9DS1_ODR_XL_952Hz		( 0x6 )
// mag odr parameter
#define LSM9DS1_ODR_M_0_625Hz		( 0x0 )
#define LSM9DS1_ODR_M_1_25Hz		( 0x1 )
#define LSM9DS1_ODR_M_2_5Hz			( 0x2 )
#define LSM9DS1_ODR_M_5Hz			( 0x3 )
#define LSM9DS1_ODR_M_10Hz			( 0x4 )
#define LSM9DS1_ODR_M_20Hz			( 0x5 )
#define LSM9DS1_ODR_M_40Hz			( 0x6 )
#define LSM9DS1_ODR_M_80Hz			( 0x7 )
// accel full-scale range
#define LSM9DS1_XL_FS_RANGE_2G		( 0x00 )
#define LSM9DS1_XL_FS_RANGE_4G		( 0x01 )
#define LSM9DS1_XL_FS_RANGE_8G		( 0x02 )
#define LSM9DS1_XL_FS_RANGE_16G		( 0x03 )
// gyroscope full-scale range
#define LSM9DS1_G_FS_RANGE_245DPS	( 0x00 )
#define LSM9DS1_G_FS_RANGE_500DPS	( 0x01 )
#define LSM9DS1_G_FS_RANGE_2000DPS	( 0x02 )
// accel full-scale range
#define LSM9DS1_M_FS_RANGE_4GAUSS	( 0x00 )
#define LSM9DS1_M_FS_RANGE_8GAUSS	( 0x01 )
#define LSM9DS1_M_FS_RANGE_12GAUSS	( 0x02 )
#define LSM9DS1_M_FS_RANGE_16GAUSS	( 0x03 )

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/

typedef struct {
	void (*init)(uint8_t op_mode, uint8_t i2c_addr_xlg, uint8_t i2c_addr_m);
	void (*begin)(uint8_t odr_rate_xlg, uint8_t odr_rate_m);
	void (*setAccRange)(uint8_t range);
	void (*setGyroRange)(uint8_t range);
	void (*setMagRange)(uint8_t range);
	bool (*availableAcc)(void);
	bool (*availableGyro)(void);
	bool (*availableMag)(void);
	void (*getAcc)(float *val);
	void (*getGyro)(float *val);
	void (*getMag)(float *val);
	void (*powerDownAccGyro)(void);
	void (*powerDownAccOnly)(void);
	void (*powerDownMag)(void);
} st_LSM9DS1_f;

extern const st_LSM9DS1_f LSM9DS1;

#endif // _RTC_H_
