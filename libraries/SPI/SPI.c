/* FILE NAME: SPI.c
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

#include "common.h"
#include "mcu.h"
#include "rdwr_reg.h"
#include "spi.h"
#include "digitalio.h"
#include "pin_assignment.h"
#include "lp_manage.h"

//********************************************************************************
//   global parameters
//********************************************************************************
static volatile unsigned char _spi_transfer(UCHAR _data);
static void _spi_attachInterrupt(void);
static void _spi_detachInterrupt(void); // Default
static void _spi_begin(void); // Default
static void _spi_end(void);
static void _spi_setBitOrder(uint8_t);
static void _spi_setDataMode(uint8_t);
static void _spi_setClockDivider(UINT16 ckdiv);


// SPI can be used from Arduino connector
const SPIClass SPI =
{
	_spi_transfer,
	_spi_attachInterrupt,
	_spi_detachInterrupt,
	_spi_begin,
	_spi_end,
	_spi_setBitOrder,
	_spi_setDataMode,
	_spi_setClockDivider,
};

//********************************************************************************
//   local definitions
//********************************************************************************
//********************************************************************************
//   local parameters
//********************************************************************************
static UINT16 _spi_brr = SPI_CLOCK_DIV4;
static UCHAR  _spi_ctrl= ( SPI_MODE0 | SPI_MSBFIRST );

//********************************************************************************
//   local function definitions
//********************************************************************************
//********************************************************************************
//   local functions
//********************************************************************************
static volatile unsigned char _spi_transfer(UCHAR _data)
{
	UCHAR res;
	set_bit(SF0SPIFC);
	// set data
	write_reg8(SF0DWRL,_data);
	
	// wait end of transfer
	while(get_bit(SF0SPIF)==0)
	{
		continue;
	} 

	set_bit(SF0SPIFC);		// reset flag of tx end
	// return rx data
	res = SF0DRRL;
	return res;
}
static void _spi_attachInterrupt()
{
	
}
static void _spi_detachInterrupt(void)
{
	
}



static void _spi_begin(void)
{
	clear_bit(DSIOF0);			// BLKCON  SPI enable
	write_reg8(SF0CTRLL, _spi_ctrl);	// SIOF0 control
	write_reg16(SF0BRR, _spi_brr);
	// set SPI mode
	set_bit(SF0MST);			// master mode
	// clear FIFO
	set_bit(SF0FICL);
	clear_bit(SF0FICL);
	// GPIO setting
	set_bit(P45MD1); clear_bit(P45MD0);	set_bit(P45DIR);    set_bit(P45C0);	set_bit(P45C1);		// MISO
	set_bit(P44MD1); clear_bit(P44MD0);	clear_bit(P44DIR);	set_bit(P44C0);	set_bit(P44C1);			// MOSI
	set_bit(P36MD1); clear_bit(P36MD0);	clear_bit(P36DIR);	set_bit(P36C0);	set_bit(P36C1);			// SCK
	// SS setting
//	pinMode(SS,OUTPUT);
//	digitalWrite(SS,HIGH);
	// tx rx start
	set_bit(SF0SPE);			// start SPI tx rx
}
static void _spi_end()
{
	set_bit(DSIOF0);			// BLKCON  SPI disenable
}

static void _spi_setBitOrder(uint8_t bo)
{
	if(bo == LSBFIRST)
	{
		_spi_ctrl &= (~SPI_MSBFIRST );
	}
	else
	{
		_spi_ctrl |= SPI_MSBFIRST ;
	}
}

static void _spi_setDataMode(uint8_t mode)
{
	_spi_ctrl = (uint8_t)((_spi_ctrl & (~SPI_MODE_MASK)) | mode);
}

static void _spi_setClockDivider(UINT16 ckdiv)
{
	_spi_brr = ckdiv;
}

