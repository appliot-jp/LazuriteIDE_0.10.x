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

#ifndef	_SPI_LZPI_H_
#define	_SPI_LZGW_H_

#ifdef		LAZURITE_IDE
#else		//	LAZURITE_IDE
#include <linux/types.h>
#endif		//	LAZURITE_IDE

#define SPI_OK				0
#define SPI_ERR_UNKNOWN		-1

extern int lzpi_spi_init(int (*callback)(void));
extern int lzpi_spi_del_driver(void);
extern int lzpi_spi_transfer(const uint8_t* wdata,uint16_t wsize,uint8_t* rdata, uint16_t rsize);

#endif	// _SPI_LZPI_H_
