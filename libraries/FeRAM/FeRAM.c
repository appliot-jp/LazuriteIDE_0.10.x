/* FILE NAME: FeRAM.c
 *
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
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

#include "FeRAM.h"
#include "SPI.h"

//********************************************************************************
//   global parameters
//********************************************************************************
static void spiBegin(uint8_t cs, uint8_t wp, uint8_t hold);
static void spiWrite(uint32_t addr, uint8_t data_write);
static uint8_t spiRead(uint32_t addr);
static void spiBurstWrite(uint32_t addr, uint8_t *data_write_ptr, uint32_t total_data_count);
static void spiBurstRead(uint32_t addr, uint8_t *data_read_ptr, uint32_t total_data_count );
static void spiStatusRegWrite(uint8_t data_write);
static uint8_t spiStatusRegRead(void);
static void _feram_wr_ena(void);
static void _feram_wr_dis(void);

// SPI can be used from Lazurite connector
const FERAMClass FeRAM =
{
	spiBegin,
	spiWrite,
	spiRead,
	spiBurstWrite,
	spiBurstRead,
	spiStatusRegWrite,
	spiStatusRegRead,
};

//********************************************************************************
//   local definitions
//********************************************************************************
//FeRAM Command
#define FERAM_CMD_WREN	0x06	//Write Enable 0000 0110
#define FERAM_CMD_WRDI	0x04	//Write Disable 0000 0100
#define FERAM_CMD_RDSR	0x05	//Read Status Register 0000 0101
#define FERAM_CMD_WRSR	0x01	//Write Status Register 0000 0001
#define FERAM_CMD_READ	0x03	//Read from Memory Array 0000 0011
#define FERAM_CMD_WRITE	0x02	//Write to Memory Array 0000 0010

#define DMY_DATA		0x00

//********************************************************************************
//   local parameters
//********************************************************************************
static uint8_t feram_cs;
static uint8_t feram_wp; 
static uint8_t feram_hold;

//********************************************************************************
//   local function definitions
//********************************************************************************
//********************************************************************************
//   local functions
//********************************************************************************
static void spiBegin(uint8_t cs, uint8_t wp, uint8_t hold)
{
	pinMode(cs,OUTPUT);
	pinMode(wp,OUTPUT);
	pinMode(hold,OUTPUT);
	digitalWrite(cs, HIGH);
	digitalWrite(wp, HIGH);
	digitalWrite(hold, HIGH);
	feram_cs = cs;
	feram_wp = wp;
	feram_hold = hold;
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setBitOrder(MSBFIRST);
	SPI.setBitOrder(SPI_MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
}

static void spiWrite(uint32_t addr, uint8_t data_write)
{
	_feram_wr_ena();
	/*--- CS Low ---*/
	digitalWrite(feram_cs, LOW);
	/*--- Command ---*/
	SPI.transfer(FERAM_CMD_WRITE);
	/*--- Address ---*/
	SPI.transfer((unsigned char)(addr>>16 & 0xFF));
	SPI.transfer((unsigned char)(addr>>8 & 0xFF));
	SPI.transfer((unsigned char)(addr & 0xFF));
	/*--- Data ---*/
	SPI.transfer(data_write);
	/*--- CS High ---*/
	digitalWrite(feram_cs, HIGH);
}

static uint8_t spiRead(uint32_t addr)
{
	uint8_t ret;
	/*--- CS Low ---*/
	digitalWrite(feram_cs, LOW);
	/*--- Command ---*/
	SPI.transfer(FERAM_CMD_READ);
	/*--- Address ---*/
	SPI.transfer((unsigned char)(addr>>16 & 0xFF));
	SPI.transfer((unsigned char)(addr>>8 & 0xFF));
	SPI.transfer((unsigned char)(addr & 0xFF));
	/*--- Data ---*/
	ret = SPI.transfer(DMY_DATA);
	/*--- CS High ---*/
	digitalWrite(feram_cs, HIGH);
	return ret;
}

static void spiBurstWrite(uint32_t addr, uint8_t *data_write_ptr, uint32_t total_data_count)
{
	uint32_t n;
	
	_feram_wr_ena();
	/*--- CS Low ---*/
	digitalWrite(feram_cs, LOW);
	/*--- Command ---*/
	SPI.transfer(FERAM_CMD_WRITE);
	/*--- Address ---*/
	SPI.transfer((unsigned char)(addr>>16 & 0xFF));
	SPI.transfer((unsigned char)(addr>>8 & 0xFF));
	SPI.transfer((unsigned char)(addr & 0xFF));
	/*--- Data ---*/
	for(n = 0; n < total_data_count; n++){
		SPI.transfer(*(data_write_ptr + n));
	}
	/*--- CS High ---*/
	digitalWrite(feram_cs, HIGH);
}
	
static void spiBurstRead(uint32_t addr, uint8_t *data_read_ptr, uint32_t total_data_count )
{
	uint32_t n;

	/*--- CS Low ---*/
	digitalWrite(feram_cs, LOW);
	/*--- Command ---*/
	SPI.transfer(FERAM_CMD_READ);
	/*--- Address ---*/
	SPI.transfer((unsigned char)(addr>>16 & 0xFF));
	SPI.transfer((unsigned char)(addr>>8 & 0xFF));
	SPI.transfer((unsigned char)(addr & 0xFF));
	/*--- Data ---*/
	for(n = 0; n < total_data_count; n++){
		*(data_read_ptr + n) = SPI.transfer(DMY_DATA);
	}
	/*--- CS High ---*/
	digitalWrite(feram_cs, HIGH);
}

static void spiStatusRegWrite(uint8_t data_write)
{
	digitalWrite(feram_cs, LOW);
	SPI.transfer(FERAM_CMD_RDSR);
	SPI.transfer(data_write);
	digitalWrite(feram_cs, HIGH);
}

static uint8_t spiStatusRegRead(void)
{
	uint8_t ret;
	digitalWrite(feram_cs, LOW);
	SPI.transfer(FERAM_CMD_RDSR);
	ret = SPI.transfer(DMY_DATA);
	digitalWrite(feram_cs, HIGH);
	return ret;
}
	
static void _feram_wr_ena(void)
{
	digitalWrite(feram_cs, LOW);
	SPI.transfer(FERAM_CMD_WREN);
	digitalWrite(feram_cs, HIGH);
}

static void _feram_wr_dis(void)
{
	digitalWrite(feram_cs, LOW);
	SPI.transfer(FERAM_CMD_WRDI);
	digitalWrite(feram_cs, HIGH);
}
