/* FILE NAME: LSM9DS1.c
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
#define DEBUG
#include "LSM9DS1.h"
#include "Wire.h"
#include "driver_i2c.h"
#include "libdebug.h"
#include "lazurite.h"
/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
// device
#define LSM9DS1_DEV_XLG				( 0x0 )
#define LSM9DS1_DEV_MAG				( 0x1 )
#define LSM9DS1_WHO_AM_I_VAL		( 0x68 )
#define LSM9DS1_WHO_AM_I_M_VAL		( 0x3D )
// accelerometer and gyroscope register definition
#define LSM9DS1_ACT_THS				LSM9DS1_DEV_XLG, 0x04
#define LSM9DS1_ACT_DUR				LSM9DS1_DEV_XLG, 0x05
#define LSM9DS1_INT_GEN_CFG_XL		LSM9DS1_DEV_XLG, 0x06
#define LSM9DS1_INT_GEN_THS_X_XL	LSM9DS1_DEV_XLG, 0x07
#define LSM9DS1_INT_GEN_THS_Y_XL	LSM9DS1_DEV_XLG, 0x08
#define LSM9DS1_INT_GEN_THS_Z_XL	LSM9DS1_DEV_XLG, 0x09
#define LSM9DS1_INT_GEN_DUR_XL		LSM9DS1_DEV_XLG, 0x0A
#define LSM9DS1_REFERENCE_G			LSM9DS1_DEV_XLG, 0x0B
#define LSM9DS1_INT1_CTRL			LSM9DS1_DEV_XLG, 0x0C
#define LSM9DS1_INT2_CTRL			LSM9DS1_DEV_XLG, 0x0D
#define LSM9DS1_WHO_AM_I			LSM9DS1_DEV_XLG, 0x0F
#define LSM9DS1_CTRL_REG1_G			LSM9DS1_DEV_XLG, 0x10
#define LSM9DS1_CTRL_REG2_G			LSM9DS1_DEV_XLG, 0x11
#define LSM9DS1_CTRL_REG3_G			LSM9DS1_DEV_XLG, 0x12
#define LSM9DS1_ORIENT_CFG_G		LSM9DS1_DEV_XLG, 0x13
#define LSM9DS1_INT_GEN_SRC_G		LSM9DS1_DEV_XLG, 0x14
#define LSM9DS1_OUT_TEMP_L			LSM9DS1_DEV_XLG, 0x15
#define LSM9DS1_OUT_TEMP_H			LSM9DS1_DEV_XLG, 0x16
#define LSM9DS1_STATUS_REG0			LSM9DS1_DEV_XLG, 0x17
#define LSM9DS1_OUT_X_L_G			LSM9DS1_DEV_XLG, 0x18
#define LSM9DS1_OUT_X_H_G			LSM9DS1_DEV_XLG, 0x19
#define LSM9DS1_OUT_Y_L_G			LSM9DS1_DEV_XLG, 0x1A
#define LSM9DS1_OUT_Y_H_G			LSM9DS1_DEV_XLG, 0x1B
#define LSM9DS1_OUT_Z_L_G			LSM9DS1_DEV_XLG, 0x1C
#define LSM9DS1_OUT_Z_H_G			LSM9DS1_DEV_XLG, 0x1D
#define LSM9DS1_CTRL_REG4			LSM9DS1_DEV_XLG, 0x1E
#define LSM9DS1_CTRL_REG5_XL		LSM9DS1_DEV_XLG, 0x1F
#define LSM9DS1_CTRL_REG6_XL		LSM9DS1_DEV_XLG, 0x20
#define LSM9DS1_CTRL_REG7_XL		LSM9DS1_DEV_XLG, 0x21
#define LSM9DS1_CTRL_REG8			LSM9DS1_DEV_XLG, 0x22
#define LSM9DS1_CTRL_REG9			LSM9DS1_DEV_XLG, 0x23
#define LSM9DS1_CTRL_REG10			LSM9DS1_DEV_XLG, 0x24
#define LSM9DS1_INT_GEN_SRC_XL		LSM9DS1_DEV_XLG, 0x26
#define LSM9DS1_STATUS_REG1			LSM9DS1_DEV_XLG, 0x27
#define LSM9DS1_OUT_X_L_XL			LSM9DS1_DEV_XLG, 0x28
#define LSM9DS1_OUT_X_H_XL			LSM9DS1_DEV_XLG, 0x29
#define LSM9DS1_OUT_Y_L_XL			LSM9DS1_DEV_XLG, 0x2A
#define LSM9DS1_OUT_Y_H_XL			LSM9DS1_DEV_XLG, 0x2B
#define LSM9DS1_OUT_Z_L_XL			LSM9DS1_DEV_XLG, 0x2C
#define LSM9DS1_OUT_Z_H_XL			LSM9DS1_DEV_XLG, 0x2D
#define LSM9DS1_FIFO_CTRL			LSM9DS1_DEV_XLG, 0x2E
#define LSM9DS1_FIFO_SRC			LSM9DS1_DEV_XLG, 0x2F
#define LSM9DS1_INT_GEN_CFG_G		LSM9DS1_DEV_XLG, 0x30
#define LSM9DS1_INT_GEN_THS_XH_G	LSM9DS1_DEV_XLG, 0x31
#define LSM9DS1_INT_GEN_THS_XL_G	LSM9DS1_DEV_XLG, 0x32
#define LSM9DS1_INT_GEN_THS_YH_G	LSM9DS1_DEV_XLG, 0x33
#define LSM9DS1_INT_GEN_THS_YL_G	LSM9DS1_DEV_XLG, 0x34
#define LSM9DS1_INT_GEN_THS_ZH_G	LSM9DS1_DEV_XLG, 0x35
#define LSM9DS1_INT_GEN_THS_ZL_G	LSM9DS1_DEV_XLG, 0x36
#define LSM9DS1_INT_GEN_DUR_G		LSM9DS1_DEV_XLG, 0x37
// magnetic sensor register definition
#define LSM9DS1_OFFSET_X_REG_L_M	LSM9DS1_DEV_MAG, 0x05
#define LSM9DS1_OFFSET_X_REG_H_M	LSM9DS1_DEV_MAG, 0x06
#define LSM9DS1_OFFSET_Y_REG_L_M	LSM9DS1_DEV_MAG, 0x07
#define LSM9DS1_OFFSET_Y_REG_H_M	LSM9DS1_DEV_MAG, 0x08
#define LSM9DS1_OFFSET_Z_REG_L_M	LSM9DS1_DEV_MAG, 0x09
#define LSM9DS1_OFFSET_Z_REG_H_M	LSM9DS1_DEV_MAG, 0x0A
#define LSM9DS1_WHO_AM_I_M			LSM9DS1_DEV_MAG, 0x0F
#define LSM9DS1_CTRL_REG1_M			LSM9DS1_DEV_MAG, 0x20
#define LSM9DS1_CTRL_REG2_M			LSM9DS1_DEV_MAG, 0x21
#define LSM9DS1_CTRL_REG3_M			LSM9DS1_DEV_MAG, 0x22
#define LSM9DS1_CTRL_REG4_M			LSM9DS1_DEV_MAG, 0x23
#define LSM9DS1_CTRL_REG5_M			LSM9DS1_DEV_MAG, 0x24
#define LSM9DS1_STATUS_REG_M		LSM9DS1_DEV_MAG, 0x27
#define LSM9DS1_OUT_X_L_M			LSM9DS1_DEV_MAG, 0x28
#define LSM9DS1_OUT_X_H_M			LSM9DS1_DEV_MAG, 0x29
#define LSM9DS1_OUT_Y_L_M			LSM9DS1_DEV_MAG, 0x2A
#define LSM9DS1_OUT_Y_H_M			LSM9DS1_DEV_MAG, 0x2B
#define LSM9DS1_OUT_Z_L_M			LSM9DS1_DEV_MAG, 0x2C
#define LSM9DS1_OUT_Z_H_M			LSM9DS1_DEV_MAG, 0x2D
#define LSM9DS1_INT_CFG_M			LSM9DS1_DEV_MAG, 0x30
#define LSM9DS1_INT_SRC_M			LSM9DS1_DEV_MAG, 0x31
#define LSM9DS1_INT_THS_L_M			LSM9DS1_DEV_MAG, 0x32
#define LSM9DS1_INT_THS_H_M			LSM9DS1_DEV_MAG, 0x33

/*############################################################################*/
/*#                            Local variable                                #*/
/*############################################################################*/
static uint8_t _slave_addr_xlg;
static uint8_t _slave_addr_m;
static bool _acc_gyro_mode = false;
static bool _acc_only_mode = false;
static bool _mag_mode = false;
static uint8_t _acc_fs_range = LSM9DS1_XL_FS_RANGE_2G;
static uint8_t _gyro_fs_range = LSM9DS1_G_FS_RANGE_245DPS;
static uint8_t _mag_fs_range = LSM9DS1_M_FS_RANGE_4GAUSS;
static float _acc_sense[4] =		// accelerometer sensitivity
{
	0.000061,
	0.000122,
	0.000244,
	0.000732
};
static float _gyro_sense[3] = {		// gyroscope sensitivity
/*	0.00875,
	0.0175,
	0.07
	*/
	(float)32768/245,
	(float)32768/500,
	(float)32768/2000
};
static uint32_t _mag_sense[4] = {	// magnetic sensitivity
	0.00014,
	0.00029,
	0.00043,
	0.00058
};

/* for accelerometer and gyroscope registers */
// CTRL_REG1_G
static uint8_t _fs_g = 0x0;			// 00: 245 dps; 01: 500 dps; 10: Not Available; 11: 2000 dps
static uint8_t _bw_g = 0x0;
// CTRL_REG2_G
static uint8_t _int_sel = 0x0;
static uint8_t _out_sel = 0x0;
// CTRL_REG3_G
static uint8_t _lp_mode = 1;
static uint8_t _hp_en = 0;
static uint8_t _hp_cfg = 0x0;
// CTRL_REG3_4
static uint8_t _xyzen_g = 0x0;
static uint8_t _lir_xl1 = 0x0;
// CTRL_REG6_XL
static uint8_t _fs_xl = 0x0;		// 00: ± 2 g; 01: ±16 g; 10: ±4 g; 11: ±8 g
static uint8_t _bw_scal_odr = 0;
static uint8_t _bw_xl = 0x0;		// 00: 408 Hz; 01: 211 Hz; 10: 105 Hz; 11: 50 Hz	

/* for magnetometer registers */
// CTRL_REG1_M
static uint8_t _temp_comp = 0x0;
static uint8_t _om = 0x3;			// POR 0x0
// CTRL_REG2_M
static uint8_t _fs_m = 0x0;			// 00: ± 4 gauss, 01: ± 8 gauss, 10: ± 12 gauss, 11: ± 16 gauss
// CTRL_REG3_M
static uint8_t _lp = 0x0;
static uint8_t _md = 0x0;			// POR 0x3
// CTRL_REG4_M
static uint8_t _omz = 0x3;			// POR 0x0

/*############################################################################*/
/*#                                Pototype                                  #*/
/*############################################################################*/
static bool lsm9ds1_availableAcc(void);
static bool lsm9ds1_availableGyro(void);
static bool lsm9ds1_availableMag(void);
static void lsm9ds1_getAcc(float *val);
static void lsm9ds1_getGyro(float *val);
static void lsm9ds1_getMag(float *val);
static void lsm9ds1_powerDownAccGyro(void);
static void lsm9ds1_powerDownAccOnly(void);
static void lsm9ds1_powerDownMag(void);
static void break_reg_check(uint8_t dev, uint8_t addr);

/*############################################################################*/
/*#                             Local function                               #*/
/*############################################################################*/
static uint8_t _i2c_read(uint8_t dev, uint8_t addr, uint8_t* data, uint8_t size)
{
	uint8_t slave_addr, res, i=0;

	if (dev == LSM9DS1_DEV_XLG) {
		slave_addr = _slave_addr_xlg;
	} else if (dev == LSM9DS1_DEV_MAG) {
		slave_addr = _slave_addr_m;
	} else {
		return I2C_RESULT_ERROR;
	}

	Wire.beginTransmission(slave_addr);
	Wire.write_byte((uint8_t)addr);
	res = Wire.endTransmission(false);
	if (res == I2C_RESULT_OK) {
		Wire.requestFrom(slave_addr, size, true);
		while (Wire.available())
		{
			data[i] = (uint8_t)Wire.read();
			i++;
		}
	}
	return res;
}

static uint8_t _i2c_write(uint8_t dev, uint8_t addr, uint8_t* data, uint8_t size)
{
	uint8_t slave_addr, res;

	if (dev == LSM9DS1_DEV_XLG) {
		slave_addr = _slave_addr_xlg;
	} else if (dev == LSM9DS1_DEV_MAG) {
		slave_addr = _slave_addr_m;
	} else {
		return I2C_RESULT_ERROR;
	}

	Wire.beginTransmission(_slave_addr_xlg);
	Wire.write_byte((uint8_t)addr);
	Wire.write(data, size);
	res = Wire.endTransmission(true);

	return res;
}

static void _soft_reset_acc_gyro(void)
{
	uint8_t data;

	// software reset
	_i2c_read(LSM9DS1_CTRL_REG8, &data, 1);
	data |= 0x01;
	_i2c_write(LSM9DS1_CTRL_REG8, &data, 1);
}

static void _soft_reset_mag(void)
{
	uint8_t data;

	// software reset
	_i2c_read(LSM9DS1_CTRL_REG2_M, &data, 1);
	data |= 0x04;
	_i2c_write(LSM9DS1_CTRL_REG2_M, &data, 1);
}

static void _config_acc_gyro(uint8_t odr_rate)
{
	uint8_t data;
	float f[3];

	// 0 means power-down
	if (odr_rate == 0x00) return;

	// output disable
	data = 0x00;
	_i2c_write(LSM9DS1_CTRL_REG4, &data, 1);

	// reset INT 1_A/G pin control
	data = 0x00;
	_i2c_write(LSM9DS1_INT1_CTRL, &data, 1);

	// dummy read and clear data ready INT if exist
	if (lsm9ds1_availableGyro()) lsm9ds1_getGyro(f);

	// power-down
	lsm9ds1_powerDownAccGyro();

	// do software reset
	_soft_reset_acc_gyro();

	// set ODR for both accelerometer and gyroscope
	data = (uint8_t)(odr_rate << 5 | _fs_g << 3 | _bw_g);
	_i2c_write(LSM9DS1_CTRL_REG1_G, &data, 1);
	data = (uint8_t)(_fs_xl << 3 | _bw_scal_odr << 2 | _bw_xl);
	_i2c_write(LSM9DS1_CTRL_REG6_XL, &data, 1);

	// set INT_SEL and OUT_SEL
	data = (uint8_t)(_int_sel << 2 | _out_sel);
	_i2c_write(LSM9DS1_CTRL_REG2_G, &data, 1);

	// set LP_mode and high-pass filter
	data = (uint8_t)(_lp_mode << 7 | _hp_en << 6 |  _hp_cfg);
	_i2c_write(LSM9DS1_CTRL_REG3_G, &data, 1);

	// assign gyroscope data ready on INT 1_A/G pin
	data = 0x02;
	_i2c_write(LSM9DS1_INT1_CTRL, &data, 1);
}

static void _config_acc_only(uint8_t odr_rate)
{
	uint8_t data;
	float f[3];
//	int i;

	// 0 means power-down
	if (odr_rate == 0x00) return;

	// output disable
	data = 0x00;
	_i2c_write(LSM9DS1_CTRL_REG5_XL, &data, 1);

	// dummy read and clear data ready INT if exist
	if (lsm9ds1_availableAcc()) lsm9ds1_getAcc(f);

	// reset INT 1_A/G pin control
	data = 0x00;
	_i2c_write(LSM9DS1_INT1_CTRL, &data, 1);

	// power-down
	lsm9ds1_powerDownAccOnly();

	// do software reset
	_soft_reset_acc_gyro();

	// set ODR for both accelerometer and gyroscope
	data = (uint8_t)(odr_rate << 5 | _fs_xl << 3 | _bw_scal_odr << 2 | _bw_xl);
	_i2c_write(LSM9DS1_CTRL_REG6_XL, &data, 1);

	// set gyroscope sleep mode
	data = 0x80;
	_i2c_write(LSM9DS1_CTRL_REG9, &data, 1);

	// assign accelerometer data ready on INT 1_A/G pin
	data = 0x01;
	_i2c_write(LSM9DS1_INT1_CTRL, &data, 1);

	// set gyroscope output and accelerometer latched interrupt
	data = (uint8_t)(_xyzen_g << 3 | _lir_xl1 << 1);
	_i2c_write(LSM9DS1_CTRL_REG4, &data, 1);

/* 	for (i=4;i<0x38;i++) {
		break_reg_check(0,(uint8_t)i);
	}	 */
}

static void _config_mag(uint8_t odr_rate)
{
	uint8_t data;
	float f[3];

	// 0 means power-down
	if (odr_rate == 0x00) return;

 	// dummy read and clear data ready
	if (lsm9ds1_availableMag()) lsm9ds1_getMag(f);

	// power-down
	lsm9ds1_powerDownMag();

	// do software reset
	_soft_reset_mag();

	// set temperature compensation, X/Y-operative mode and ODR for magnetometer
	data = (uint8_t)(_temp_comp << 7 | _om << 5 | odr_rate << 2);
	_i2c_write(LSM9DS1_CTRL_REG1_M, &data, 1);

	// set full scale configuration
	data = (uint8_t)(_fs_m << 5);
	_i2c_write(LSM9DS1_CTRL_REG2_M, &data, 1);

	// set low-power mode and operating mode selection
	data = (uint8_t)(_lp << 5 | _md);
	_i2c_write(LSM9DS1_CTRL_REG3_M, &data, 1);


	// set Z-axis operative mode
	data = (uint8_t)(_omz << 2);
	_i2c_write(LSM9DS1_CTRL_REG4_M, &data, 1);
}

// for debug purpose only
uint8_t lsm9ds1_break_reg_read(uint8_t dev, uint8_t addr)
{
	uint8_t data;

	_i2c_read(dev, addr, &data, 1);
	Serial.print_long((long)addr, HEX);
	BREAKL(" addr = ", (long)data, HEX);
	return data;
}

uint8_t lsm9ds1_reg_read(uint8_t dev, uint8_t addr)
{
    uint8_t data;

    _i2c_read(dev, addr, &data, 1);
    return data;
}

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
static void lsm9ds1_init(uint8_t op_mode, uint8_t i2c_addr_xlg, uint8_t i2c_addr_m)
{
	uint8_t data, res;

	if (op_mode & LSM9DS1_OPMODE_XLG) {
		if (i2c_addr_xlg == LSM9DS1_I2C_ADDR_XLG_6A) {
			_slave_addr_xlg = LSM9DS1_I2C_ADDR_XLG_6A;
		} else {
			_slave_addr_xlg = LSM9DS1_I2C_ADDR_XLG_6B;
		}
		res = _i2c_read(LSM9DS1_WHO_AM_I, &data, 1);
		if ((res == I2C_RESULT_OK) && (data == LSM9DS1_WHO_AM_I_VAL)) {
			_acc_gyro_mode = true;
#ifdef DEBUG
			Serial.println("_acc_gyro_mode");
#endif
		}
	} else if (op_mode & LSM9DS1_OPMODE_XL) {
		if (i2c_addr_xlg == LSM9DS1_I2C_ADDR_XLG_6A) {
			_slave_addr_xlg = LSM9DS1_I2C_ADDR_XLG_6A;
		} else {
			_slave_addr_xlg = LSM9DS1_I2C_ADDR_XLG_6B;
		}
		res = _i2c_read(LSM9DS1_WHO_AM_I, &data, 1);
		if ((res == I2C_RESULT_OK) && (data == LSM9DS1_WHO_AM_I_VAL)) {
			_acc_only_mode = true;
		}
	} else {
		// do nothing
	}

 	if (op_mode & LSM9DS1_OPMODE_MAG) {
		if (i2c_addr_m == LSM9DS1_I2C_ADDR_M_1C) {
			_slave_addr_m = LSM9DS1_I2C_ADDR_M_1C;
		} else {
			_slave_addr_m = LSM9DS1_I2C_ADDR_M_1E;
		}
		res = _i2c_read(LSM9DS1_WHO_AM_I_M, &data, 1);
		if ((res == I2C_RESULT_OK) && (data == LSM9DS1_WHO_AM_I_M_VAL)) {
			_mag_mode = true;
		}
	}
}

static void lsm9ds1_begin(uint8_t odr_rate_xlg, uint8_t odr_rate_m)
{
	if (_acc_gyro_mode) {
		_config_acc_gyro(odr_rate_xlg);
	} else if (_acc_only_mode) {
		_config_acc_only(odr_rate_xlg);
	} else {
		// do nothing
	}
	if (_mag_mode) _config_mag(odr_rate_m);
}

static void lsm9ds1_setAccRange(uint8_t range)
{
	_acc_fs_range = range;
	switch (range) {
	case LSM9DS1_XL_FS_RANGE_2G:
		_fs_xl = 0x0;
		break;
	case LSM9DS1_XL_FS_RANGE_4G:
		_fs_xl = 0x2;
		break;
	case LSM9DS1_XL_FS_RANGE_8G:
		_fs_xl = 0x3;
		break;
	case LSM9DS1_XL_FS_RANGE_16G:
		_fs_xl = 0x1;
		break;
	default:
		break;
	}
}

static void lsm9ds1_setGyroRange(uint8_t range)
{
	_gyro_fs_range = range;
	switch (range) {
	case LSM9DS1_G_FS_RANGE_245DPS:
		_fs_g = 0x0;
		break;
	case LSM9DS1_G_FS_RANGE_500DPS:
		_fs_g = 0x1;
		break;
	case LSM9DS1_G_FS_RANGE_2000DPS:
		_fs_g = 0x3;
		break;
	default:
		break;
	}
}

static void lsm9ds1_setMagRange(uint8_t range)
{
	_mag_fs_range = range;
	switch (range) {
	case LSM9DS1_M_FS_RANGE_4GAUSS:
		_fs_m = 0x0;
		break;
	case LSM9DS1_M_FS_RANGE_8GAUSS:
		_fs_m = 0x1;
		break;
	case LSM9DS1_M_FS_RANGE_12GAUSS:
		_fs_m = 0x2;
		break;
	case LSM9DS1_M_FS_RANGE_16GAUSS:
		_fs_m = 0x3;
		break;
	default:
		break;
	}
}

static bool lsm9ds1_availableAcc(void)
{
	uint8_t data;

	_i2c_read(LSM9DS1_STATUS_REG1, &data, 1);

	return (bool)(data & 0x1);
}

static bool lsm9ds1_availableGyro(void)
{
	uint8_t data;

	_i2c_read(LSM9DS1_STATUS_REG1, &data, 1);

	return (bool)((data & 0x2) >> 1);
}

static bool lsm9ds1_availableMag(void)
{
	uint8_t data;

	_i2c_read(LSM9DS1_STATUS_REG_M, &data, 1);

	return (bool)((data & 0x8) >> 4);
}

static void lsm9ds1_getAcc(float *val)
{
	int data[3];

	_i2c_read(LSM9DS1_OUT_X_L_XL, (uint8_t*)data, 6);
	val[0] = (float)data[0] * _acc_sense[_acc_fs_range];
	val[1] = (float)data[1] * _acc_sense[_acc_fs_range];
	val[2] = (float)data[2] * _acc_sense[_acc_fs_range];
}

static void lsm9ds1_getGyro(float *val) //degree per second
{
	int data[3];

	_i2c_read(LSM9DS1_OUT_X_L_G, (uint8_t*)data, 6);
	
	val[0] = (float)data[0] / _gyro_sense[_gyro_fs_range]; 
	val[1] = (float)data[1] / _gyro_sense[_gyro_fs_range];
	val[2] = (float)data[2] / _gyro_sense[_gyro_fs_range];
}

static void lsm9ds1_getMag(float *val)
{
	int data[3];

	_i2c_read(LSM9DS1_OUT_X_L_M, (uint8_t*)data, 6);
	val[0] = (float)data[0] * _mag_sense[_mag_fs_range];
	val[1] = (float)data[1] * _mag_sense[_mag_fs_range];
	val[2] = (float)data[2] * _mag_sense[_mag_fs_range];
}

static void lsm9ds1_powerDownAccGyro(void)
{
	uint8_t data;

	if (_acc_gyro_mode) {
		_i2c_read(LSM9DS1_CTRL_REG1_G, &data, 1);
		data &= 0x1F;
		_i2c_write(LSM9DS1_CTRL_REG1_G, &data, 1);
	}
}

static void lsm9ds1_powerDownAccOnly(void)
{
	uint8_t data;

	if (_acc_only_mode) {
		_i2c_read(LSM9DS1_CTRL_REG6_XL, &data, 1);
		data &= 0x1F;
		_i2c_write(LSM9DS1_CTRL_REG6_XL, &data, 1);
	}
}

static void lsm9ds1_powerDownMag(void)
{
	uint8_t data;

	if (_mag_mode) {
		_i2c_read(LSM9DS1_CTRL_REG3_M, &data, 1);
		data |= 0x03;
		_i2c_write(LSM9DS1_CTRL_REG3_M, &data, 1);
	}
}

/*############################################################################*/
/*#                            Global variable                               #*/
/*############################################################################*/
const st_LSM9DS1_f LSM9DS1 = {
	lsm9ds1_init,				// void (*init)(uint8_t op_mode, uint8_t i2c_addr_xlg, uint8_t i2c_addr_m);
	lsm9ds1_begin,				// void (*begin)(uint8_t odr_rate_xlg, uint8_t odr_rate_m);
	lsm9ds1_setAccRange,		// void (*setAccRange)(uint8_t range);
	lsm9ds1_setGyroRange,		// void (*setGyroRange)(uint8_t range);
	lsm9ds1_setMagRange,		// void (*setMagRange)(uint8_t range);
	lsm9ds1_availableAcc,		// bool (*availableAcc)(void);
	lsm9ds1_availableGyro,		// bool (*availableGyro)(void);
	lsm9ds1_availableMag,		// bool (*availableMag)(void);
	lsm9ds1_getAcc,				// void (*getAcc)(float *val);
	lsm9ds1_getGyro,			// void (*getGyro)(float *val);
	lsm9ds1_getMag,				// void (*getMag)(float *val);
	lsm9ds1_powerDownAccGyro,	// void (*powerDownAccGyro)(void);
	lsm9ds1_powerDownAccOnly,	// void (*powerDownAccOnly)(void);
	lsm9ds1_powerDownMag		// void (*powerDownMag)(void);
};
	
	
