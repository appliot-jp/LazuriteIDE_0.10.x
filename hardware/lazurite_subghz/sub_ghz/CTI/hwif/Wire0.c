/* FILE NAME: Wire.c
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
#include "binary.h"
#include "lazurite_system.h"
#include "lazurite.h"
#include "wire0.h"
#include "driver_i2c.h"
#include <stdlib.h>
#include <stddef.h>

//********************************************************************************
//   local definitions
//********************************************************************************

//********************************************************************************
//   global parameters
//********************************************************************************

static void _wire0_begin(void);
static size_t _wire0_requestFrom(UCHAR address,UCHAR quantity, UCHAR sendStop);
static int _wire0_available(void);
static int _wire0_read(void);
static size_t _wire0_write(const uint8_t *data, size_t quantity);
static size_t _wire0_write_byte(uint8_t data);
static uint8_t _wire0_endTransmission(uint8_t sendStop);
static void _wire0_beginTransmission(uint8_t address);

const TwoWire0 Wire0 ={
	_wire0_begin,
	_wire0_requestFrom,
	_wire0_available,
	_wire0_read,
	_wire0_beginTransmission,
	_wire0_write_byte,
	_wire0_write,
	_wire0_endTransmission
};

//********************************************************************************
//   local parameters
//********************************************************************************
typedef struct {
	unsigned char buffer[I2C_BUFFER_LENGTH];
	unsigned char index;
	unsigned char length;
} WIRE_PARAM;

static struct {
	WIRE_PARAM tx;
	WIRE_PARAM rx;
}wire0;
//static BOOL delay_status;

//********************************************************************************
//   local function definitions
//********************************************************************************

//********************************************************************************
//   functions
//********************************************************************************
// Arduino compatible I2C ( by I2C1)

//**********************************************

// Initializing I2C hardware & reset parameter
static void _wire0_begin(void)
{
	i2c_init(0);
	i2c_begin(0);
	return;
}

// set trasmittion mode and I2C address
static void _wire0_beginTransmission(uint8_t address)
{
	// indicate that we are transmitting
	i2c_set_address(0, address);					// set I2C address
	wire0.tx.index = 0;
	wire0.tx.length = 0;
}

// set trasmittion mode and I2C address
static size_t _wire0_write(const uint8_t *data, size_t quantity)
{
	if((quantity-wire0.tx.index) > I2C_BUFFER_LENGTH)
	{
		quantity = I2C_BUFFER_LENGTH - wire0.tx.index;
	}
	memcpy(wire0.tx.buffer + wire0.tx.index,data, quantity);
	wire0.tx.index += (UCHAR)quantity;
	wire0.tx.length += (UCHAR)quantity;	
	return quantity;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
static size_t _wire0_write_byte(uint8_t data)
{
	if(wire0.tx.index > I2C_BUFFER_LENGTH)
	{
		return 0;
	}
	wire0.tx.buffer[wire0.tx.index] = data;
	wire0.tx.index++;
	wire0.tx.length = wire0.tx.index;
	
	return 1;
}

static uint8_t _wire0_endTransmission(uint8_t sendStop)
{
	// set tx pointer
	i2c_set_tx_buf(0,wire0.tx.buffer,wire0.tx.length);

	// start transmittion
	i2c_start(0,true,sendStop);
	
	// wait end of communication
	HALT_Until_Event(HALT_I2C0_END,0);
	
	wire0.tx.index = 0;
	wire0.tx.length = 0;
	
	return (uint8_t)i2c_get_err_status(0);
}

// Read sequence
static size_t _wire0_requestFrom(UCHAR address,UCHAR quantity, UCHAR sendStop)
{
	// Check quantity
	if(quantity > I2C_BUFFER_LENGTH)
	{
	    return 0;
	}
	// reset buffer pointer
	wire0.rx.index = 0;
	wire0.rx.length = quantity;

	// set rx pointer
	i2c_set_rx_buf(0,wire0.rx.buffer,quantity);
	
	// Send I2C Slave Address
	i2c_set_address(0, address);					// set I2C address

	// start transmittion
	i2c_start(0,false,sendStop);						// ch, write, cont
	
	// wait end of communication
	HALT_Until_Event(HALT_I2C0_END,0);

	return i2c_read_amount(0);
}

// return number of byte in rx buffer
static int _wire0_available(void)
{
	return wire0.rx.length - wire0.rx.index;
}

// Read from rx buffer
static int _wire0_read(void)
{
	int value = -1;
	if(wire0.rx.index < wire0.rx.length)
	{
		value = wire0.rx.buffer[wire0.rx.index];
		wire0.rx.index++;
	}
	return value;
}
