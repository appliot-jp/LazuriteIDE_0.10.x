/* FILE NAME: uartf0.c
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

#include "mcu.h"
#include "rdwr_reg.h"
#include "uartf0.h"
/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define UARTFn_WR_FIFO_MAX  ( 4 )             /**< Size of FIFO (Write)       */

/* control parameters for UARTF */
typedef struct {
	unsigned char   *data;      /**< pointer to area where the writing/reading data is stored   */
	unsigned int    size;       /**< size of writing/reading data                               */
	unsigned int    cnt;        /**< size of data which is writing/reading                      */
	cbfUartF_t      callBack;   /**< callback function                                          */
	unsigned short  errStat;    /**< error status                                               */
	unsigned char   blockSize;  /**< size of data which is one-operation writing/reading size   */
} uartfCtrlParam_t;


/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
static uartfCtrlParam_t s_writeCtrlParam;   /**< write control parameters for UARTF */
static uartfCtrlParam_t s_readCtrlParam;    /**< read control parameters for UARTF  */

/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
static void           s_writeSingleData( uartfCtrlParam_t *param );
static void           s_writeFifo( uartfCtrlParam_t *param );
static unsigned short s_readSingleData( uartfCtrlParam_t *param );
static unsigned short s_readFifo( uartfCtrlParam_t *param );

/*############################################################################*/
/*#                              Subroutine                                  #*/
/*############################################################################*/
/**
 * Writing a data
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @return          None
 */
#pragma INLINE s_writeSingleData
static void s_writeSingleData( uartfCtrlParam_t *param )
{
	uartf0_putc( *param->data );
	param->data++;
	param->cnt++;
}

/**
 * Writing data to FIFO
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @return          None
 */
#pragma INLINE s_writeFifo
static void s_writeFifo( uartfCtrlParam_t *param )
{
	unsigned char   wr_size;

	wr_size = param->blockSize;     /* size of next writing */
	if( (param->cnt + wr_size) > param->size ) {
		wr_size = (unsigned char)(param->size - param->cnt);
	}
	/* Write data (to FIFO) */
	while( uartf0_checkWriteBusy() != 0){};
	for( ; wr_size != 0; wr_size-- ) {
		s_writeSingleData( param );
	}
}

/**
 * Reading a data
 *
 * @param[in,out]   *param Pointer to control parameters
 * @return          Status (the value of UAF0LSR register)
 */
#pragma INLINE s_readSingleData
static unsigned short s_readSingleData( uartfCtrlParam_t *param )
{
	unsigned short  status;

	status = uartf0_getStatus();
	*param->data = uartf0_getc();
	param->data++;
	param->cnt++;
	return status;
}

/**
 * Reading data from FIFO
 *
 * @param[in,out]   *param Pointer to control parameters
 * @return          Status (the value of UAF0LSR register)      <br>
 *                  Status is logical sum of all reception data in FIFO.
 */
#pragma INLINE s_readFifo
static unsigned short s_readFifo( uartfCtrlParam_t *param )
{
	unsigned short  status;
	unsigned char   rd_size;

	status = 0;
	rd_size = param->blockSize;     /* size of reading */
	if( (param->cnt + rd_size) > param->size ) {
		rd_size = (unsigned char)(param->size - param->cnt);
	}
	/* Read data (from FIFO) */
	for( ; rd_size != 0; rd_size-- ) {
		if ( uartf0_checkReadReady() == 1) {
			status |= s_readSingleData( param );
		}
	}
	return status;
}

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize UARTF
 *
 * @param[in]   uafnmod     Configuration UART mode                             <br>
 *                          (Setting value of UAF0MOD)                          <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Data length             : UARTF_LG_5BIT, or UARTF_LG_6BIT, or UARTF_LG_7BIT, or UARTF_LG_8BIT
 * @arg                         Stop bit                : UARTF_STP_1BIT, or UARTF_STP_2BIT
 * @arg                         Parity bit              : UARTF_PT_NON, or UARTF_PT_ODD, or UARTF_PT_EVEN, or UARTF_PT_FIXED_1, or UARTF_PT_FIXED_0
 * @arg                         Enable 'break control'  : UARTF_BC_DIS, or UARTF_BC_ENA
 * @arg                         UAFnBUF access          : UARTF_DLAB_RBR_THR, or UARTF_DLAB_DLR
 * @arg                         FIFO data (read)        : UARTF_RFR_KEEP, or UARTF_RFR_CLR
 * @arg                         FIFO data (write)       : UARTF_TFR_KEEP, or UARTF_TFR_CLR
 * @arg                         Read FIFO trigger level : UARTF_FTL_1BYTE, or UARTF_FTL_2BYTE, or UARTF_FTL_3BYTE, or UARTF_FTL_4BYTE
 * 
 * @param[in]   uafncaj         Clock adjustment value (for baud-rate generator) <br>
 *                              (Setting value of UAF0CAJ)
 *
 * @param[in]   brDivisorLatch  Divisor latch value    (for baud-rate generator) <br>
 *                              (set to 0, or greater than 2)
 *
 * @param[in]   uafnier     Configuration interrupt                             <br>
 *                          (Setting value of UAF0IER)                          <br>
 *                          Specify the logical sum of the following items.
 * @arg                         ERBFI (reception interuput)     : UARTF_ERBFI_DIS, or UARTF_ERBFI_ENA
 * @arg                         ETBEI (transmittion interuput)  : UARTF_ETBEI_DIS, or UARTF_ETBEI_ENA
 * @arg                         ELSI  (reception data error interuput)  : UARTF_ELSI_DIS, or UARTF_ELSI_ENA
 *
 * @return      None
 * 
 * @note
 * @arg         About baud-rate clock calculation                               <br>
 *                  Baud-rate clock = 'Sysytem clock' * ( ('uafncaj' - 1) / 'uafncaj' ) / ('brDivisorLatch' * 16)
 *
 * @arg         About system clock frequency                                    <br>
 *                  If you watnt to use UARTF, Set sysytem clock over 4MHz.
 *
 * @arg         About using FIFO                                                <br>
 *                  This module is FIFO-mode only. You cannot disable FIFO.
 *
 * @arg         About interrupt                                                 <br>
 *                  Interrupt is prohibited except during communication.
 *
 */
void uartf0_init( unsigned short uafnmod, unsigned short uafncaj, unsigned short brDivisorLatch )
{
	volatile unsigned short  status;

	/*=== Register setting ===*/
	/*---   Baud rate clock setting   ---*/
	set_bit( UF0DLAB );                         /* enable DLR(Divisor Latch Resister) access */
	write_reg16( UAF0BUF, brDivisorLatch );     /* set DLR */
	clear_bit( UF0DLAB );                       /* disable DLR(Divisor Latch Resister) access */
	write_reg16( UAF0CAJ, uafncaj );            /* set adjustment value */

	/*---   Communication setting   ---*/
	uafnmod |= UAFnMOD_UFnFEN;                  /* enable FIFO */
	write_reg16( UAF0MOD, uafnmod );
	write_reg16( UAF0IER, (unsigned short)(UARTF_ERBFI_DIS | UARTF_ETBEI_DIS | UARTF_ELSI_DIS) );
	s_writeCtrlParam.blockSize    = (unsigned char)UARTFn_WR_FIFO_MAX;
	s_readCtrlParam.blockSize     = (unsigned char)( (get_bit( UF0FTL1 ) << 1) | get_bit( UF0FTL0 ) );
	s_readCtrlParam.blockSize++;

	/*---   Communication status is clear   ---*/
	uartf0_clearReadFifo();                     /* clear Read FIFO */
	uartf0_clearWriteFifo();                    /* clear Write FIFO */
	status = uartf0_getStatus();                /* clear UF0OER,UF0PER,UF0FER */
	status = uartf0_getIntCause();              /* clear write request */
}

/**
 * Start to sending write data
 *
 * @param[in]   *data   pointer to area where the send data is stored
 * @param[in]   size    size of data (unit is byte)
 * @param[in]   *func   pointer to callback function
 * @return      None
 * @note        Transmit FIFO is an empty state, but there is the case that all transmit        <br>
 *              processing doesn't complete. Confirm that transmit shift register (TSR) became  <br>
 *              empty in UF0TEMT bit of UAF0LSR register before stopping high-speed clock       <br>
 *              (Transition to modes such as STOP / DEEP-HALT / HALT-H).
 */
void uartf0_write( unsigned char *data, unsigned int size, cbfUartF_t func )
{
	/*=== Transmission of a message system order parameter setting. ===*/
	s_writeCtrlParam.data      = data;
	s_writeCtrlParam.size      = size;
	s_writeCtrlParam.cnt       = 0;
	s_writeCtrlParam.callBack  = func;
	s_writeCtrlParam.errStat   = 0;

	/*=== transmit a message, and it is worked to start. ===*/
	/* Write next data block */
	s_writeFifo( &s_writeCtrlParam );

	/* Interrupt enable */
	write_reg16( UAF0IER, UARTF_ERBFI_DIS | UARTF_ETBEI_ENA | UARTF_ELSI_DIS );
}

/**
 * Start to receiving read data
 *
 * @param[in]   *data   pointer to area where the receive data is stored
 * @param[in]   size    receive buffer size (the unit is byte)
 * @param[in]   *func   pointer to callback function
 * @return      None
 */
void uartf0_read( unsigned char *data, unsigned int size, cbfUartF_t func )
{
	unsigned short  status;

	/*===   Clear status    ===*/
	status = uartf0_getStatus();                /* clear UF0OER,UF0PER,UF0FER */

	/*=== Transmission of a message system order parameter setting. ===*/
	s_readCtrlParam.data       = data;
	s_readCtrlParam.size       = size;
	s_readCtrlParam.cnt        = 0;
	s_readCtrlParam.callBack   = func;
	s_readCtrlParam.errStat    = 0;

	/* Interrupt enable */
	write_reg16( UAF0IER, UARTF_ERBFI_ENA | UARTF_ETBEI_DIS | UARTF_ELSI_ENA );
}

/**
 * Process to continue writing
 *
 * @param   -
 * @retval  UARTF_R_TRANS_FIN(=1)      : writing communication is finished
 * @retval  UARTF_R_TRANS_CONT_OK(=0)  : writing communication is continued (success)
 */
int uartf0_continueWrite( void )
{
	int             ret;

	/*---   Are transmission of a message data left?    ---*/
	if( s_writeCtrlParam.size > s_writeCtrlParam.cnt ){
		/*=== I transmit a message, and it is worked to continue. ===*/
		/*--- There are data in the transmission of a message buffer? ---*/
		/* Write next data block */
		s_writeFifo( &s_writeCtrlParam );
		ret = (int)( UARTF_R_TRANS_CONT_OK );
	}
	else{
		/*=== It is returned that the transmission ended. ===*/
		/* interrupt disable */
		write_reg16( UAF0IER, UARTF_ERBFI_DIS | UARTF_ETBEI_DIS | UARTF_ELSI_DIS );

		/* Callback */
		if( s_writeCtrlParam.callBack != (void *)0 ){
			s_writeCtrlParam.callBack( s_writeCtrlParam.cnt, s_writeCtrlParam.errStat );
		}
		ret = (int)( UARTF_R_TRANS_FIN );
	}
	return ret;
}

/**
 * Process to continue reading
 * 
 * @param   -
 * @retval  UARTF_R_TRANS_FIN(=1)     : reading communication is finished
 * @retval  UARTF_R_TRANS_CONT_OK(=0) : reading communication is continued (success)
 */
int uartf0_continueRead( void )
{
	int             ret;
	unsigned short  status;
	unsigned char   flgReadEnd=0;

	status = uartf0_getStatus();
	/*---   Are reception data left?    ---*/
	if( s_readCtrlParam.size > s_readCtrlParam.cnt ){
		/* Getting reception data */
		/* Read next data block */
		status |= s_readFifo( &s_readCtrlParam );
		s_readCtrlParam.errStat |= status & (unsigned short)(UAFnLSR_UFnFER|UAFnLSR_UFnOER|UAFnLSR_UFnPER|UAFnLSR_UFnBI);

		/* End of communication */
		if( s_readCtrlParam.size <= s_readCtrlParam.cnt ){
			flgReadEnd = 1;
		}
		/*---   Frame Error ---*/
		else if( ( status & ( unsigned char )( UAFnLSR_UFnFER ) ) != 0 ) {
			/* This program execute callback and return UARTF_R_TRANS_FIN, if "frame error" occurs. */
			flgReadEnd = 1;
		}
	}
	/* overflow */
	else {
		s_readCtrlParam.errStat |= status;
		/* FIFO reset */ 
		uartf0_clearReadFifo();
		flgReadEnd = 1;
	}

	ret = (int)( UARTF_R_TRANS_CONT_OK );
	/* If communication is completed, then callback function is called. */
	if( flgReadEnd != 0 ){
		/* interrupt disable */
		write_reg16( UAF0IER, UARTF_ERBFI_DIS | UARTF_ETBEI_DIS | UARTF_ELSI_DIS );

		ret = (int)( UARTF_R_TRANS_FIN );
		/* Callback */
		if( s_readCtrlParam.callBack != (void *)0 ){
			s_readCtrlParam.callBack( s_readCtrlParam.cnt, s_readCtrlParam.errStat );
		}
	}
	return ret;
}
