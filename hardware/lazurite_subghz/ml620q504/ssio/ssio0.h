/* FILE NAME: ssio0.h
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

#ifndef _SSIO0_H_
#define _SSIO0_H_

#include "rdwr_reg.h"
#include "ssio_common.h"

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize SSIO
 *
 * @param[in]   sionmod     Setting value of SIO0MOD register <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Endian                  : SSIO_DIR_LSB, or SSIO_DIR_MSB
 * @arg                         Buffer mode             : SSIO_LG1, or SSIO_LG2
 * @arg                         Clock                   : SSIO_CLK_LS, or SSIO_CLK_LS2, or SSIO_CLK_HS4, or SSIO_CLK_HS8, or SSIO_CLK_HS16, or SSIO_CLK_HS32, or SSIO_CLK_EX
 * @arg                         Clock type              : SSIO_CKT_0, or SSIO_CKT_1
 * @arg                         Logical value           : SSIO_NEG_POS, or SSIO_NEG_NEG
 * 
 * @return      None
 *
 * @note        SCK0 Max clock input frequency is 1/4 of SYSCLK or 2MHz at slave mode.          <br>
 *              SCK0 Max clock output frequency is 2MHz if using P02 as SCK0 in master mode.
 */
#define         ssio0_init( sionmod )   write_reg16( SIO0MOD, (unsigned short)(sionmod) )

/**
 * Writing transmission of data (for BYTE access)
 *
 * @param[in]   data    A transmission data <br>
 *                      (Setting value of SIO0BUFL register)
 * @return      None
 */
#define         ssio0_putcByte( data )  write_reg8( SIO0BUFL, (unsigned char)(data) )

/**
 * Writing transmission of data (for WORD access)
 *
 * @param[in]   data    A transmission data <br>
 *                      (Setting value of SIO0BUF register)
 * @return      None
 */
#define         ssio0_putcWord( data )  write_reg16( SIO0BUF, (unsigned short)(data) )

/**
 * Reading reception data (for BYTE access)
 *
 * @param       -
 * @return      A reception data <br>
 *              (A value of SIO0BUFL register)
 */
#define         ssio0_getcByte()        read_reg8( SIO0BUFL )

/**
 * Reading reception data (for WORD access)
 *
 * @param       -
 * @return      A reception data <br>
 *              (A value of SIO0BUF register)
 */
#define         ssio0_getcWord()        read_reg16( SIO0BUF )

/**
 * Stopping SSIO communication
 *
 * @param       -
 * @return      None
 */
#define         ssio0_stop()            clear_bit( S0EN )

/**
 * Checking SSIO busy status
 *
 * @param       -
 * @retval      0   busy status : Ready(able to read/write next operation)
 * @retval      1   busy status : Busy
 */
#define         ssio0_checkReady()      ( (unsigned char)get_bit( S0EN ) )

int             ssio0_start( unsigned short sionmod, void *rxData, void *txData, unsigned int dataSize, cbfSsio_t func );
int             ssio0_continue( void );

#endif /*_SSIO0_H_*/

