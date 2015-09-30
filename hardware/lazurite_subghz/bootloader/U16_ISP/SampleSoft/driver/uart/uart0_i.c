/*****************************************************************************
 uart0.c

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
 * @file    uart0.c
 *
 * This module is 'UART' drivers(channel No.0).
 *
 */
#include "mcu.h"
#include "rdwr_reg.h"
#include "uart0_i.h"
/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/** control parameters for UART */
typedef struct {
	unsigned char  *data;       /**< pointer to area where the writing/reading data is stored   */
	unsigned int    size;       /**< size of writing/reading data                               */
	unsigned int    cnt;        /**< size of data which is writing/reading                      */
	cbfUart_t       callBack;   /**< callback function                                          */
#ifdef UART_BYTE_CALLBACK
	cbfUartByte_t	callBackByte; /**< callback function                                        */
#endif
	unsigned char   errStat;    /**< error status                                               */
} uartCtrlParam_t;


/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
static uartCtrlParam_t  s_writeCtrlParam;   /**< write control parameters for UART */
static uartCtrlParam_t  s_readCtrlParam;    /**< read control parameters for UART  */

/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize UART
 *
 * @param[in]   uanmod0     Setting value of UA0MOD0 register <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Clock source            : UART_CS_LSCLK, or UART_CS_HSCLK
 * @arg                         Received data input pin : UART_RSEL_PORT_P02, or UART_RSEL_PORT_P42, or UART_RSEL_PORT_P86, or UART_RSEL_PORT_P54
 * @arg                         Sampling timing         : UART_RSS_BR_DIV2, or UART_RSS_BR_DIV2_M1
 * 
 * @param[in]   uanmod1     Setting value of UA0MOD1 register <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Data length             : UART_LG_8BIT, or UART_LG_7BIT, or UART_LG_6BIT, or UART_LG_5BIT
 * @arg                         Parity bit              : UART_PT_EVEN, or UART_PT_ODD, or UART_PT_NON
 * @arg                         Stop bit                : UART_STP_1BIT, or UART_STP_2BIT
 * @arg                         Logical value           : UART_NEG_POS, or UART_NEG_NEG
 * @arg                         Endian                  : UART_DIR_LSB, or UART_DIR_MSB
 * 
 * @param[in]   brCnt       The count value of the baud-rate generator
 * @see                     MCU Users manual 'Baud rate' in chapter 'UART'
 *
 * @return      None
 */
void uart0_init( unsigned char uanmod0, unsigned char uanmod1, unsigned short brCnt )
{
	/*=== Register setting ===*/
	/*---   An uart communication stop  ---*/
	uart0_stop();

	/*---   Input clock choice to the baud rate generator   ---*/
	write_reg8( UA0MOD0, uanmod0 );

	/*---   Communication setting   ---*/
	write_reg8( UA0MOD1, uanmod1 );

	/*---   Baud rate count value setting   ---*/
	write_reg16( UA0BRT, brCnt );
}


/**
 * Start to sending write data
 *
 * @param[in]   *data   pointer to area where the send data is stored
 * @param[in]   size    size of data (unit is byte)
 * @param[in]   *func   pointer to callback function
 * @return      None
 */
void uart0_write( unsigned char *data, unsigned int size, cbfUart_t func )
{
	/*=== Transmission of a message system order parameter setting. ===*/
	s_writeCtrlParam.data      = data;
	s_writeCtrlParam.size      = size;
	s_writeCtrlParam.cnt       = 0;
	s_writeCtrlParam.callBack  = func;
#ifdef UART_BYTE_CALLBACK
	s_writeCtrlParam.callBackByte = (void *)0;
#endif
	s_writeCtrlParam.errStat   = 0;

	/*=== transmit a message, and it is worked to start. ===*/
	uart0_putc( *data );
	s_writeCtrlParam.data++;
	s_writeCtrlParam.cnt++;
	set_bit( U0EN );
	set_bit( U1EN );
}

/**
 * Start to receiving read data
 *
 * @param[in]   *data   pointer to area where the receive data is stored
 * @param[in]   size    receive buffer size (the unit is byte)
 * @param[in]   *func   pointer to callback function
 * @return      None
 */
#ifdef UART_BYTE_CALLBACK
void uart0_read( unsigned char *data, unsigned int size, cbfUartByte_t func1Byte, cbfUart_t func)
#else
void uart0_read( unsigned char *data, unsigned int size, cbfUart_t func )
#endif
{
	/*===   Clear status    ===*/
	clear_bit( U0FER );
	clear_bit( U0OER );
	clear_bit( U0PER );

	/*=== Transmission of a message system order parameter setting. ===*/
	s_readCtrlParam.data       = data;
	s_readCtrlParam.size       = size;
	s_readCtrlParam.cnt        = 0;
	s_readCtrlParam.callBack   = func;
#ifdef UART_BYTE_CALLBACK
	s_readCtrlParam.callBackByte = func1Byte;
#endif
	s_readCtrlParam.errStat    = 0;

	/*=== I receive it, and it is worked to start. ===*/
	set_bit( U0EN );
}

/**
 * Process to continue writing
 *
 * @param   -
 * @retval  UART_R_TRANS_FIN(=1)      : writing communication is finished
 * @retval  UART_R_TRANS_CONT_OK(=0)  : writing communication is continued (success)
 * @retval  UART_R_TRANS_CONT_NG(=-1) : writing communication is continued (failure)
 * @retval  UART_R_TRANS_FIN_NG(=-2)  : writing communication is finished (failure)
 */
int uart0_continueWrite( void )
{
	/*---   Are transmission of a message data left?    ---*/
	if( s_writeCtrlParam.size != s_writeCtrlParam.cnt ){
		/*=== I transmit a message, and it is worked to continue. ===*/
		/*---   There are data in the transmission of a message buffer? ---*/
		if( get_bit( U1FUL ) == 0 ){
			uart0_putc( *s_writeCtrlParam.data );
			s_writeCtrlParam.data++;
			s_writeCtrlParam.cnt++;
			set_bit( U0EN );
			set_bit( U1EN );
			return ( UART_R_TRANS_CONT_OK );
		}
		else{
			return ( UART_R_TRANS_CONT_NG );
		}
	}
	else{
		/*---   Did the transmission of data end?   ---*/
		if( uart0_checkWriteBusy() == 0 ){
			/*=== It is returned that the transmission ended. ===*/
			/* Callback */
			if( s_writeCtrlParam.callBack != (void *)0 ){
				s_writeCtrlParam.callBack( s_writeCtrlParam.cnt, s_writeCtrlParam.errStat );
			}
			return ( UART_R_TRANS_FIN );
		}
		return ( UART_R_TRANS_FIN_NG );
	}
}

/**
 * Process to continue reading
 * 
 * @param   -
 * @retval  UART_R_TRANS_FIN(=1)      : reading communication is finished
 * @retval  UART_R_TRANS_CONT_OK(=0)  : reading communication is continued (success)
 */
int uart0_continueRead( void )
{
	unsigned char   errStat;

	/*---   Are reception data left?    ---*/
	if( s_readCtrlParam.size != s_readCtrlParam.cnt ){
		errStat = (unsigned char)( read_reg8( UA0STAT ) & (UAnSTAT_UnFER|UAnSTAT_UnOER|UAnSTAT_UnPER) );
		s_readCtrlParam.errStat |= errStat;
		/* Clear status */
		clear_bit( U0FER );
		clear_bit( U0OER );
		clear_bit( U0PER );
		/* Getting reception data */
		*s_readCtrlParam.data = uart0_getc();
		s_readCtrlParam.data++;
		s_readCtrlParam.cnt++;

#ifdef UART_BYTE_CALLBACK
		if(s_readCtrlParam.callBackByte != (void *)0 )
		{
			if(s_readCtrlParam.callBackByte( *(s_readCtrlParam.data - 1), errStat ) == UART_R_STOP){
				return ( UART_R_TRANS_FIN );
			}
		}
#endif

		if( ( errStat & ( unsigned char )( UAnSTAT_UnFER ) ) == 0 ) {
			if( s_readCtrlParam.size != s_readCtrlParam.cnt ){
				return ( UART_R_TRANS_CONT_OK );
			}
		} else {
			/*---   Frame Error ---*/
			/* This program execute callback and return UART_R_TRANS_FIN, if "frame error" occurs. */
		}
	}

	/* Callback */
	if( s_readCtrlParam.callBack != (void *)0 ){
		s_readCtrlParam.callBack( s_readCtrlParam.cnt, s_readCtrlParam.errStat );
	}
	return ( UART_R_TRANS_FIN );
}
