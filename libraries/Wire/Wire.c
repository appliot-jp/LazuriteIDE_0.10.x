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
#include "wire.h"
#include "driver_i2c.h"
#include <stdlib.h>
#include <stddef.h>

//********************************************************************************
//   local definitions
//********************************************************************************

//********************************************************************************
//   global parameters
//********************************************************************************


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
} wire;
//static BOOL delay_status;
static uint16_t timeout=0;
//********************************************************************************
//   local function definitions
//********************************************************************************

//********************************************************************************
//   functions
//********************************************************************************
// Arduino compatible I2C ( by I2C1)

// Initializing I2C hardware & reset parameter
static void _wire_begin(void)
{
	i2c_init(1);
	i2c_begin(1);
	timeout=0;
	return;
}

// set trasmittion mode and I2C address
static void _wire_beginTransmission(uint8_t address)
{
	// indicate that we are transmitting
	i2c_set_address(1, address);					// set I2C address
	wire.tx.index = 0;
	wire.tx.length = 0;
}

// set trasmittion mode and I2C address
static size_t _wire_write(const uint8_t *data, size_t quantity)
{
	if((quantity-wire.tx.index) > I2C_BUFFER_LENGTH)
	{
		quantity = I2C_BUFFER_LENGTH - wire.tx.index;
	}
	memcpy(wire.tx.buffer + wire.tx.index,data, quantity);
	wire.tx.index += (UCHAR)quantity;
	wire.tx.length += (UCHAR)quantity;	
	return quantity;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
static size_t _wire_write_byte(uint8_t data)
{
	if(wire.tx.index > I2C_BUFFER_LENGTH)
	{
		return 0;
	}
	wire.tx.buffer[wire.tx.index] = data;
	wire.tx.index++;
	wire.tx.length = wire.tx.index;
	
	return 1;
}

static uint8_t _wire_endTransmission(uint8_t sendStop)
{
	// set tx pointer
	i2c_set_tx_buf(1,wire.tx.buffer,wire.tx.length);

	// start transmittion
	i2c_start(1,true,sendStop);
	
	// wait end of communication
	HALT_Until_Event(HALT_I2C1_END,timeout);
	
	wire.tx.index = 0;
	wire.tx.length = 0;
	
	return (uint8_t)i2c_get_err_status(1);
}

// Read sequence
static size_t _wire_requestFrom(UCHAR address,UCHAR quantity, UCHAR sendStop)
{
	// Check quantity
	if(quantity > I2C_BUFFER_LENGTH)
	{
	    return 0;
	}
	// reset buffer pointer
	wire.rx.index = 0;
	wire.rx.length = quantity;

	// set rx pointer
	i2c_set_rx_buf(1,wire.rx.buffer,quantity);
	
	// Send I2C Slave Address
	i2c_set_address(1, address);					// set I2C address

	// start transmittion
	i2c_start(1,false,sendStop);						// ch, write, cont
	
	// wait end of communication
	HALT_Until_Event(HALT_I2C1_END,timeout);

	return i2c_read_amount(1);
}

// return number of byte in rx buffer
static int _wire_available(void)
{
	return wire.rx.length - wire.rx.index;
}

// Read from rx buffer
static int _wire_read(void)
{
	int value = -1;
	if(wire.rx.index < wire.rx.length)
	{
		value = wire.rx.buffer[wire.rx.index];
		wire.rx.index++;
	}
	return value;
}
static void _wire_setTimeout(uint16_t ms)
{
	timeout = ms;
}

const TwoWire Wire ={
	_wire_begin,
	_wire_requestFrom,
	_wire_available,
	_wire_read,
	_wire_beginTransmission,
	_wire_write_byte,
	_wire_write,
	_wire_endTransmission,
	_wire_setTimeout
};

