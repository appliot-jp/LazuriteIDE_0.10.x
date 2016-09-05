/* FILE NAME: driver_i2c.h
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

#ifndef _DRIVER_I2C_H_
#define _DRIVER_I2C_H_

#include "common.h"
#include "mcu.h"
//#include <stdlib.h>
#include <stddef.h>

#define I2C_BUFFER_LENGTH 32

//********************************************************************************
//   global definitions
//********************************************************************************
#define I2C_MODE_DIS		0		// I2C is not available
#define I2C_MODE_IDLE		1		// Initialized I2C hardware, but parameter is not
#define I2C_MODE_READY		2
#define I2C_MODE_STOPBIT	3
#define I2C_MODE_ERROR		4
#define I2C_MODE_ADDRESS	5
#define I2C_MODE_TX			6
#define I2C_MODE_RX_ADR		7
#define I2C_MODE_RX			8
#define I2C_MODE_RX_END		9


#define I2C_RESULT_OK		0
#define I2C_RESULT_LEN_ERR	1		// length is over. may not use
#define I2C_RESULT_ANACK	2		// address send, nack receive
#define I2C_RESULT_DNACK	3		// data send, nack receive
#define I2C_RESULT_ERROR	4		// other error


//********************************************************************************
//   global parameters
//********************************************************************************

//********************************************************************************
//   extern function definitions
//********************************************************************************
// parameters
//	UCHAR ch:  channel of I2C   must be 0 or 1
//		0 = I2C0
//		1 = I2C1
//	UCHAR *rxbufp  : buffer pointer to store recieving data
//	UCHAR *txbufp  : buffer pointer to send
//  UINT16 length  : length of buffer
//  UCHAR   write   : true = transmit,   false = recieve
//  UCHAR   cont    : true = does not send slave address, false = send slave address
// UCHAR  address  : I2C slave address(7bit)
extern void i2c_init(UCHAR ch);						// initializing I2C
extern void i2c_begin(UCHAR ch);					// reset I2C parameter
extern void i2c_set_rx_buf(UCHAR ch,UCHAR *rxbufp, UINT16 length);		// set buffer length and pointer for receiver
extern void i2c_set_tx_buf(UCHAR ch,UCHAR *txbufp, UINT16 length);		// set buffer length and pointer for tranceiver
extern void i2c_set_address(UCHAR ch, UCHAR address);					// set I2C address
extern void i2c_send_stopbit(UCHAR ch);									// send stop bit
extern void i2c_start(UCHAR ch, BOOLEAN write, BOOLEAN cont);				// start transmittion or receive. restart condition is "I2CnST bit ==1" and "cont == false"
extern CHAR i2c_get_status(UCHAR ch);									// get status
extern void i2c_isr(UCHAR ch);
extern UINT16 i2c_read_amount(UCHAR ch);
extern UINT16 i2c_write_amount(UCHAR ch);
extern UINT8 i2c_get_err_status(UCHAR ch);
extern void i2c_force_stop(UCHAR ch);


#endif // _DRIVER_I2C_H_

