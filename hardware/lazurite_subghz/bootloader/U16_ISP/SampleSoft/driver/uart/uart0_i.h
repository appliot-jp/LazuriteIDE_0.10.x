/*****************************************************************************
 uart0.h

 Copyright (C) 2014 LAPIS Semiconductor Co., LTD.
 All rights reserved.

 This software is provided "as is" and any expressed or implied
  warranties, including, but not limited to, the implied warranties of
  merchantability and fitness for a particular purpose are disclaimed.
 LAPIS SEMICONDUCTOR shall not be liable for any direct, indirect,
 consequential or incidental damages arising from using or modifying
 this software.
 You (customer) can modify and use this software in whole or part on
 your own responsibility, only for the purpose of developing the software
 for use with microcontroller manufactured by LAPIS SEMICONDUCTOR.

 History
    2014.03.31 ver.1.00

******************************************************************************/
/**
 * @file    uart0.h
 *
 * This file is API definition for 'UART'(channel No.0) drivers.
 *
 */

#ifndef _UART0_H_
#define _UART0_H_

#include "rdwr_reg.h"
#include "uart_common_i.h"

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
 *              (Setting value of UA1BUF register)
 * @return  None
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
#ifdef UART_BYTE_CALLBACK
void            uart0_read( unsigned char *data, unsigned int size, cbfUartByte_t func1Byte, cbfUart_t func);
#else
void            uart0_read( unsigned char *data, unsigned int size, cbfUart_t func );
#endif
int             uart0_continueWrite( void );
int             uart0_continueRead( void );

#endif /*_UART0_H_*/

