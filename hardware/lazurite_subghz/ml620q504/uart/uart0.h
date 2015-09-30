/* FILE NAME: uart0.h
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

#ifndef _UART0_H_
#define _UART0_H_

#include "rdwr_reg.h"
#include "uart_common.h"

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Getting reception data
 *
 * @param   -
 * @return  A reception data <br>
 *          (A value of UA0BUF register)
 */
#define         uart0_getc()            read_reg8( UA0BUF )

/**
 * Sending transmission of data
 *
 * @param[in]   data    A transmission data <br>
 *                      (Setting value of UA1BUF register)
 * @return      None
 */
#define         uart0_putc( data )      write_reg8( UA1BUF, (unsigned char)(data) )

/**
 * Stopping UART communication
 *
 * @param   -
 * @return  None
 */
#define         uart0_stop()            do {                    \
                                            clear_bit( U0EN );  \
                                            clear_bit( U1EN );  \
                                        } while(0)

/**
 * Checking UART(Writing mode) busy status
 *
 * @param   -
 * @retval  0   busy status : Normal(able to next writing)
 * @retval  1   busy status : Busy
 */
#define         uart0_checkWriteBusy()  ( (unsigned char)get_bit( U1EN ) )

void            uart0_init( unsigned char uanmod0, unsigned char uanmod1, unsigned short brCnt );
void            uart0_write( unsigned char *data, unsigned int size, cbfUart_t func );
void            uart0_read( unsigned char *data, unsigned int size, cbfUart_t func );
int             uart0_continueWrite( void );
int             uart0_continueRead( void );

#endif /*_UART0_H_*/

