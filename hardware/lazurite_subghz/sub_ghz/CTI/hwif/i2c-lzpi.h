/*
 * BP3596 I2C Header
 * 
 * File:  i2c-bp3596.h
 * 
 * Copyright 2015 Lapis Semiconductor Co.,Ltd.
 * Author: Naotaka Saito
 * 
 * The program is based on BP3596 driver by Communication Technology
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef	_I2C_BP3596_H_
#define	_I2C_BP3596_H_

#define I2C_OK		0

extern int lzpi_i2c_init(void);
extern int lzpi_i2c_adapter_init(uint8_t i2c_addr);
extern int lzpi_i2c_del_driver(void);
extern int lzpi_i2c_write(const uint8_t* data, uint8_t length);
extern int lzpi_i2c_read(uint8_t* data, uint8_t length);

#endif	// _I2C_BP3596_H_
