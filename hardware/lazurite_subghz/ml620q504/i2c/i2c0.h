/* FILE NAME: i2c0.h
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

#ifndef _I2C0_H_
#define _I2C0_H_

#include "rdwr_reg.h"
#include "i2c_common.h"

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Setting slave address
 *
 * @param[in]   i2cnsa      Slave address                                       <br>
 *                          (Setting value of I2C0SA register)                  <br>
 *                          Bit0(=I20RW) means read/write mode.                 <br>
 * @return      None
 */
#define         i2c0_setSlaveAdrs( i2cnsa ) write_reg8( I2C0SA, i2cnsa )

/**
 * Writing transmission of data
 *
 * @param[in]   data        A transmission data <br>
 *                          (Setting value of I2C0TD register)
 * @return      None
 */
#define         i2c0_putc( data )           write_reg8( I2C0TD, data )

/**
 * Reading reception data
 *
 * @param       -
 * @return      A reception data <br>
 *              (A value of I2C0RD register)
 */
#define         i2c0_getc()                 read_reg8( I2C0RD )

/**
 * Trig I2C
 *
 * Trig the operation of I2C.                                                   <br>
 * I20ST = 0 to 1 : exec StartCondition                                         <br>
 * I20ST = 1 to 1 : restart operation of I2C                                    <br>
 *
 * @param       -
 * @return      None
 */
#define         i2c0_trigStart()            set_bit( I20ST )

/**
 * Trig StopCondition
 *
 * @param       -
 * @return      None
 */
#define         i2c0_trigStopCondition()    set_bit( I20SP )

/**
 * Trig RestartCondition
 *
 * @param       -
 * @return      None
 */
#define         i2c0_trigRestartCondition() set_bit( I20RS )

/**
 * Getting Ack/Nack status
 *
 * @param       -
 * @retval      0   Ack status : Received ack '0'
 * @retval      1   Ack status : Received ack '1'
 */
#define         i2c0_getReceivedAck()       ( (unsigned char)get_bit( I20ACR ) )

/**
 * Checking I2C send-error status
 *
 * @param       -
 * @retval      0   send-error : Normal
 * @retval      1   send-error : Error
 */
#define         i2c0_checkError()           ( (unsigned char)get_bit( I20ER ) )

/**
 * Stop I2C
 *
 * @param       -
 * @return      None
 */
#define         i2c0_stop()                 clear_bit( I20EN )

void            i2c0_init( unsigned short i2cnmod );
int             i2c0_write( unsigned char slaveAddr, unsigned char *addr, unsigned int addrSize, unsigned char *buf, unsigned short size, cbfI2c_t func );
int             i2c0_read( unsigned char slaveAddr, unsigned char *addr, unsigned int addrSize, unsigned char *buf, unsigned short size, cbfI2c_t func );
int             i2c0_continue( void );

#endif /*_I2C0_H_*/
