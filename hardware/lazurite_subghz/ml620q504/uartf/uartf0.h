/* FILE NAME: uartf0.h
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


#ifndef _UARTF0_H_
#define _UARTF0_H_

#include "rdwr_reg.h"
#include "uartf_common.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Getting reception data
 *
 * @param       -
 * @return      A reception data <br>
 *              (A value of UAF0BUFL register)
 */
#define         uartf0_getc()           read_reg8( UAF0BUFL )

/**
 * Sending transmission of data
 *
 * @param[in]   data    A transmission data
 *              (Setting value of UAF0BUFL register)
 * @return  None
 */
#define         uartf0_putc( data )     write_reg8( UAF0BUFL, (unsigned char)(data) )

/**
 * Checking UARTF(Writing) busy status
 *
 * @param       -
 * @retval      0   busy status : Normal(able to write)
 * @retval      1   busy status : Busy
 */
#define         uartf0_checkWriteBusy() ( (unsigned char)( ! get_bit( UF0THRE )) )

/**
 * Checking UARTF(Reading) ready status
 *
 * @param       -
 * @retval      0   ready status : Normal
 * @retval      1   ready status : Ready(able to read, and reception data is valid)
 */
#define         uartf0_checkReadReady() ( (unsigned char)get_bit( UF0DR ) )

/**
 * Trig interrupt
 *
 * Request UARTF interrupt.
 *
 * @param       -
 * @return      None
 */
#define         uartf0_trigIRQ()        set_bit( UF0IRQ )

/**
 * Getting interrupt cause
 *
 * Get causes which UARTF interrupt is requested.
 *
 * @param       -
 * @return      Interrupt causes <br>
 *              (A value of UAF0IIR register)
 */
#define         uartf0_getIntCause()    read_reg16( UAF0IIR )

/**
 * Getting UARTF status
 *
 *
 * @param       -
 * @return      UARTF line status <br>
 *              (A value of UAF0LSR register)
 */
#define         uartf0_getStatus()      read_reg16( UAF0LSR )

/**
 * Clear RBR interrupt cause
 *
 * Read RBR to clear interrupt cause. <br>
 * ex. Character timeout is occurred.
 *
 * @param       -
 * @return      RBR stored data(Dummy read) <br>
 *              (A value of UAF0BUFL register)
 */
#define         uartf0_clearRbrIntCause() uartf0_getc()

/**
 * Crear Write FIFO
 *
 * @param       -
 * @return      None
 */
 #define        uartf0_clearWriteFifo()   set_bit( UF0TFR );

/**
 * Crear Read FIFO
 *
 * @param       -
 * @return      None
 */
 #define        uartf0_clearReadFifo()    set_bit( UF0RFR );

void            uartf0_init( unsigned short uafnmod, unsigned short uafncaj, unsigned short brDivisorLatch );
void            uartf0_write( unsigned char *data, unsigned int size, cbfUartF_t func );
void            uartf0_read( unsigned char *data, unsigned int size, cbfUartF_t func );
int             uartf0_continueWrite( void );
int             uartf0_continueRead( void );

#endif /*_UARTF0_H_*/

