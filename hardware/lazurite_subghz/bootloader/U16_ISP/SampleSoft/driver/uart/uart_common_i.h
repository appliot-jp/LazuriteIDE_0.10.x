/*****************************************************************************
 uart_common.h

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
 * @file    uart_common.h
 *
 * This file is common definition for UART drivers.
 *
 */
#ifndef _UART_COMMON_H_
#define _UART_COMMON_H_

#define UART_BYTE_CALLBACK

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* UAnMOD0 */
#define UAnMOD0_UnCK0       ( 0x02 )        /**< UAnMOD0 Register UnCK0 bit   */
#define UAnMOD0_UnCK1       ( 0x04 )        /**< UAnMOD0 Register UnCK1 bit   */
#define UAnMOD0_UnRSEL0     ( 0x10 )        /**< UAnMOD0 Register UnRSEL0 bit */
#define UAnMOD0_UnRSEL1     ( 0x20 )        /**< UAnMOD0 Register UnRSEL1 bit */
#define UAnMOD0_UnRSS       ( 0x40 )        /**< UAnMOD0 Register UnRSS bit   */
#define UAnMOD0_Un1HD       ( 0x80 )        /**< UAnMOD0 Register Un1HD bit   */
/* UAnMOD1 */
#define UAnMOD1_UnLG0       ( 0x01 )        /**< UAnMOD1 Register UnLG0 bit   */
#define UAnMOD1_UnLG1       ( 0x02 )        /**< UAnMOD1 Register UnLG1 bit   */
#define UAnMOD1_UnPT0       ( 0x04 )        /**< UAnMOD1 Register UnPT0 bit   */
#define UAnMOD1_UnPT1       ( 0x08 )        /**< UAnMOD1 Register UnPT1 bit   */
#define UAnMOD1_UnSTP       ( 0x10 )        /**< UAnMOD1 Register UnSTP bit   */
#define UAnMOD1_UnNEG       ( 0x20 )        /**< UAnMOD1 Register UnNEG bit   */
#define UAnMOD1_UnDIR       ( 0x40 )        /**< UAnMOD1 Register UnDIR bit   */
/* UAnSTAT */
#define UAnSTAT_UnFER       ( 0x01 )        /**< UAnSTAT Register UnFER bit   */
#define UAnSTAT_UnOER       ( 0x02 )        /**< UAnSTAT Register UnOER bit   */
#define UAnSTAT_UnPER       ( 0x04 )        /**< UAnSTAT Register UnPER bit   */

/*=== API parameters value ===*/
/* Initial configuration parameters for uart*_init() */
#define UART_CS_LSCLK       (            0 )                                    /**< The clock oscillator inputted into baud rate generator : LSCLK  */
#define UART_CS_HSCLK       ( UAnMOD0_UnCK1 )                                   /**< The clock oscillator inputted into baud rate generator : HSCLK  */
#define UART_RSEL_PORT_P02  (               0 |               0 )               /**< Received data input pin : P02                                   */
#define UART_RSEL_PORT_P42  (               0 | UAnMOD0_UnRSEL0 )               /**< Received data input pin : P42                                   */
#define UART_RSEL_PORT_P86  ( UAnMOD0_UnRSEL1 |               0 )               /**< Received data input pin : P86                                   */
#define UART_RSEL_PORT_P54  ( UAnMOD0_UnRSEL1 | UAnMOD0_UnRSEL0 )               /**< Received data input pin : P54                                   */
#define UART_RSS_BR_DIV2    (            0 )                                    /**< Value set in the UAnBRTH and UAnBRTL registers/2                */
#define UART_RSS_BR_DIV2_M1 ( UAnMOD0_UnRSS )                                   /**< Value set in the UAnBRTH and UAnBRTL registers/2-1              */
#define UART_LG_8BIT        (               0 |               0 )               /**< Data length : 8bit                                              */
#define UART_LG_7BIT        (               0 |   UAnMOD1_UnLG0 )               /**< Data length : 7bit                                              */
#define UART_LG_6BIT        (   UAnMOD1_UnLG1 |               0 )               /**< Data length : 6bit                                              */
#define UART_LG_5BIT        (   UAnMOD1_UnLG1 |   UAnMOD1_UnLG0 )               /**< Data length : 5bit                                              */
#define UART_PT_EVEN        (               0 |               0 )               /**< Parity bit : even                                               */
#define UART_PT_ODD         (               0 |   UAnMOD1_UnPT0 )               /**< Parity bit : odd                                                */
#define UART_PT_NON         (   UAnMOD1_UnPT1 |               0 )               /**< Parity bit : none                                               */
#define UART_STP_1BIT       (             0 )                                   /**< Stop bit : 1bit                                                 */
#define UART_STP_2BIT       ( UAnMOD1_UnSTP )                                   /**< Stop bit : 2bit                                                 */
#define UART_NEG_POS        (             0 )                                   /**< Logical value : positive                                        */
#define UART_NEG_NEG        ( UAnMOD1_UnNEG )                                   /**< Logical value : negative                                        */
#define UART_DIR_LSB        (             0 )                                   /**< Endian : LSB first                                              */
#define UART_DIR_MSB        ( UAnMOD1_UnDIR )                                   /**< Endian : MSB first                                              */

/*=== API return value ===*/
#define UART_R_TRANS_FIN        (  1 )  /**< writing or reading is finished             */
#define UART_R_TRANS_CONT_OK    (  0 )  /**< writing or reading is continued (success)  */
#define UART_R_TRANS_CONT_NG    ( -1 )  /**< writing or reading is continued (failure)  */
#define UART_R_TRANS_FIN_NG     ( -2 )  /**< writing or reading is finished (failure)   */
#ifdef UART_BYTE_CALLBACK
 #define UART_R_CONTINUE        (  0 )  /**< receive is continue                        */
 #define UART_R_STOP            (  1 )  /**< receive is stop                            */
#endif

/*=== Callback function ===*/
/**
 * UART callback function
 *
 * @param[in]   size        size of reading/writing data
 * @param[in]   errStatus   status of UART operation result                     <br>
 *                          The logical sum of the following items.             <br>
 * @arg                         UAnSTAT_UnFER
 * @arg                         UAnSTAT_UnOER
 * @arg                         UAnSTAT_UnPER
 * @return      None
 */
typedef void (*cbfUart_t)( unsigned int size, unsigned char errStatus );
#ifdef UART_BYTE_CALLBACK
typedef char (*cbfUartByte_t)( unsigned char recData, unsigned char errStatByte );
#endif

#endif /*_UART_COMMON_H_*/

