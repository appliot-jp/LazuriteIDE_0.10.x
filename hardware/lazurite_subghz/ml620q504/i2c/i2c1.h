/* FILE NAME: i2c1.h
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

#ifndef _I2C1_H_
#define _I2C1_H_

#include "rdwr_reg.h"
#include "i2c_common.h"

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Setting slave address
 *
 * @param[in]   i2cnsa      Slave address                                       <br>
 *                          (Setting value of I2C1SA register)                  <br>
 *                          Bit0(=I21RW) means read/write mode.                 <br>
 * @return      None
 */
#define         i2c1_setSlaveAdrs( i2cnsa ) write_reg8( I2C1SA, i2cnsa )

/**
 * Writing transmission of data
 *
 * @param[in]   data        A transmission data <br>
 *                          (Setting value of I2C1TD register)
 * @return      None
 */
#define         i2c1_putc( data )           write_reg8( I2C1TD, data )

/**
 * Reading reception data
 *
 * @param       -
 * @return      A reception data <br>
 *              (A value of I2C1RD register)
 */
#define         i2c1_getc()                 read_reg8( I2C1RD )

/**
 * Trig I2C
 *
 * Trig the operation of I2C.                                                   <br>
 * I21ST = 0 to 1 : exec StartCondition                                         <br>
 * I21ST = 1 to 1 : restart operation of I2C                                    <br>
 *
 * @param       -
 * @return      None
 */
#define         i2c1_trigStart()            set_bit( I21ST )

/**
 * Trig StopCondition
 *
 * @param       -
 * @return      None
 */
#define         i2c1_trigStopCondition()    set_bit( I21SP )

/**
 * Trig RestartCondition
 *
 * @param       -
 * @return      None
 */
#define         i2c1_trigRestartCondition() set_bit( I21RS )

/**
 * Getting Ack/Nack status
 *
 * @param       -
 * @retval      0   Ack status : Received ack '0'
 * @retval      1   Ack status : Received ack '1'
 */
#define         i2c1_getReceivedAck()       ( (unsigned char)get_bit( I21ACR ) )

/**
 * Checking I2C send-error status
 *
 * @param       -
 * @retval      0   send-error : Normal
 * @retval      1   send-error : Error
 */
#define         i2c1_checkError()           ( (unsigned char)get_bit( I21ER ) )

/**
 * Stop I2C
 *
 * @param       -
 * @return      None
 */
#define         i2c1_stop()                 clear_bit( I21EN )

void            i2c1_init( unsigned short i2cnmod );
int             i2c1_write( unsigned char slaveAddr, unsigned char *addr, unsigned int addrSize, unsigned char *buf, unsigned short size, cbfI2c_t func );
int             i2c1_read( unsigned char slaveAddr, unsigned char *addr, unsigned int addrSize, unsigned char *buf, unsigned short size, cbfI2c_t func );
int             i2c1_continue( void );

#endif /*_I2C1_H_*/
