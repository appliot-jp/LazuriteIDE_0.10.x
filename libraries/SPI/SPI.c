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

static volatile unsigned char _spi0_transfer(UCHAR _data);
static void _spi0_attachInterrupt(void);
static void _spi0_detachInterrupt(void); // Default
static void _spi0_begin(void); // Default
static void _spi0_end(void);
static void _spi0_setBitOrder(uint8_t);
static void _spi0_setDataMode(uint8_t);
static void _spi0_setClockDivider(UINT16 ckdiv);


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

// SPI0 can not be used from Arduino connector.
// The interface can be used for BLE or SLPR connected internally in Lazurite.
const SPIClass SPI0 =
{
	_spi0_transfer,
	_spi0_attachInterrupt,
	_spi0_detachInterrupt,
	_spi0_begin,
	_spi0_end,
	_spi0_setBitOrder,
	_spi0_setDataMode,
	_spi0_setClockDivider,
};
//********************************************************************************
//   local definitions
//********************************************************************************
#define SPI0_MSBFIRST		0x0001	// 1: MSBFIRST,  0: LSBFIRST(default)
#define SPI0_TX_RX			0x0006
#define SPI0_8BIT			0x0000
#define SPI0_16BIT			0x0001

#define SPI0_CLOCK_MASK		0x0700
#define SPI0_CLOCK_DIV4		0x0200
#define SPI0_CLOCK_DIV8		0x0300
#define SPI0_CLOCK_DIV16	0x0400
#define SPI0_CLOCK_DIV32	0x0500

#define SPI0_MODE_MASK 		0x3000
#define SPI0_MODE0			0x1000
#define SPI0_MODE1			0x2000
#define SPI0_MODE2			0x3000
#define SPI0_MODE3 			0x0000

#define SPI0_GPIO_MASK16	0x0707
#define SPI0_GPIO_MASK8		0x07
#define SPI0_GPIO_DIR		0x02
#define SPI0_GPIO_CON		0x0707
#define SPI0_GPIO_MOD		0x0700
//********************************************************************************
//   local parameters
//********************************************************************************
static UINT16 _spi_brr = SPI_CLOCK_DIV4;
static UCHAR  _spi_ctrl= ( SPI_MODE0 | SPI_MSBFIRST );
static UINT16 _spi0_mod = (SPI0_MODE0 | SPI0_CLOCK_DIV4 | SPI0_TX_RX | SPI0_8BIT | SPI0_MSBFIRST);

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


// SPI0  not supported by Arduino
// for BLE or SLPR
static volatile unsigned char _spi0_transfer(UCHAR _data)
{
	UCHAR res;
	write_reg8(SIO0BUFL,_data);
	set_bit(S0EN);
	// set data
	
	// wait end of transfer
	while(get_bit(S0EN)==1)
	{
		continue;
	} 

	// return rx data
	res = SIO0BUFL;
	return res;
}
static void _spi0_attachInterrupt()
{
	
}
static void _spi0_detachInterrupt(void)
{
	
}


static void _spi0_begin(void)
{
//	unsigned char tmp8;
//	unsigned char tmp16;
	clear_bit(DSIO0);					// BLKCON  SPI enable
	write_reg16(SIO0MOD, _spi0_mod);		// 
	
	// GPIO setting		DIR  CON1  CON0 MD1 MD0
	//	P00 SOUT0  OUT   0   1     1    1    0
	//	P01 SIN0   IN    1   1     1    1    0
	//	P02 SCK0   OUT   0   1     1    1    0
	//  P11 CSB    OUT   O   1     1    0    0
	P0DIR &= ~SPI0_GPIO_MASK8;
	P0DIR |= SPI0_GPIO_DIR;

	P0CON &= ~SPI0_GPIO_MASK16;
	P0CON |= SPI0_GPIO_CON;

	P0MOD &= ~SPI0_GPIO_MASK16;
	P0MOD |= SPI0_GPIO_MOD;
	
	// CSB GPIO setting
	// SS setting
	SPI0_CSB(HIGH);
	clear_bit(P11DIR);
	set_bit(P11C1);
	set_bit(P11C0);
	
}
static void _spi0_end()
{
	SPI0_CSB(HIGH);
	set_bit(DSIO0);			// BLKCON  SPI disenable
}

static void _spi0_setBitOrder(uint8_t bo)
{
	if(bo == LSBFIRST)
	{
		_spi0_mod &= (~SPI0_MSBFIRST );
	}
	else
	{
		_spi0_mod |= SPI0_MSBFIRST ;
	}
}

static void _spi0_setDataMode(uint8_t mode)
{
	_spi0_mod &= (~SPI0_MODE_MASK);
	switch(mode)
	{
	case SPI_MODE0:
		_spi0_mod |= SPI0_MODE0;
		break;
	case SPI_MODE1:
		_spi0_mod |= SPI0_MODE1;
		break;
	case SPI_MODE2:
		_spi0_mod |= SPI0_MODE2;
		break;
	case SPI_MODE3:
	default:
		_spi0_mod |= SPI0_MODE3;
		break;
	}
}

// Some of the clock settings are not supported in SIO.
static void _spi0_setClockDivider(UINT16 ckdiv)
{
	_spi0_mod &= (~SPI0_CLOCK_MASK);
	switch(ckdiv)
	{
	case SPI_CLOCK_DIV2:				// not supported
	case SPI_CLOCK_DIV4:
		_spi0_mod |= SPI0_CLOCK_DIV4;
		break;
	case SPI_CLOCK_DIV8:
		_spi0_mod |= SPI0_CLOCK_DIV8;
		break;
	case SPI_CLOCK_DIV16:
		_spi0_mod |= SPI0_CLOCK_DIV16;
		break;
	case SPI_CLOCK_DIV32:
	case SPI_CLOCK_DIV64:				// not supported
	case SPI_CLOCK_DIV128:				// not supported:
	default:
		_spi0_mod |= SPI0_CLOCK_DIV32;
		break;
	}
}


