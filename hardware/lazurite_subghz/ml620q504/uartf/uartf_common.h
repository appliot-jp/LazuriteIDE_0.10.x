/* FILE NAME: uartf_common.h
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

#ifndef _UARTF_COMMON_H_
#define _UARTF_COMMON_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* UAFnMOD */
#define UAFnMOD_UFnLG0      ( 0x0001 )        /**< UAFnMOD Register UFnLG0 bit  */
#define UAFnMOD_UFnLG1      ( 0x0002 )        /**< UAFnMOD Register UFnLG1 bit  */
#define UAFnMOD_UFnSTP      ( 0x0004 )        /**< UAFnMOD Register UFnSTP bit  */
#define UAFnMOD_UFnPT0      ( 0x0008 )        /**< UAFnMOD Register UFnPT0 bit  */
#define UAFnMOD_UFnPT1      ( 0x0010 )        /**< UAFnMOD Register UFnPT1 bit  */
#define UAFnMOD_UFnPT2      ( 0x0020 )        /**< UAFnMOD Register UFnPT2 bit  */
#define UAFnMOD_UFnBC       ( 0x0040 )        /**< UAFnMOD Register UFnBC bit   */
#define UAFnMOD_UFnDLAB     ( 0x0080 )        /**< UAFnMOD Register UFnDLAB bit */
#define UAFnMOD_UFnFEN      ( 0x0100 )        /**< UAFnMOD Register UFnFEN bit  */
#define UAFnMOD_UFnRFR      ( 0x0200 )        /**< UAFnMOD Register UFnRFR bit  */
#define UAFnMOD_UFnTFR      ( 0x0400 )        /**< UAFnMOD Register UFnTFR bit  */
#define UAFnMOD_UFnFTL0     ( 0x4000 )        /**< UAFnMOD Register UFnFTL0 bit */
#define UAFnMOD_UFnFTL1     ( 0x8000 )        /**< UAFnMOD Register UFnFTL1 bit */

/* UAFnIER */
#define UAFnIIR_UFnERBFI    ( 0x0001 )        /**< UAFnIIR Register UFnERBFI bit*/
#define UAFnIIR_UFnETBEI    ( 0x0002 )        /**< UAFnIIR Register UFnETBEI bit*/
#define UAFnIIR_UFnELSI     ( 0x0004 )        /**< UAFnIIR Register UFnELSI bit */

/* UAFnIIR */
#define UAFnIIR_UFnIRP      ( 0x0001 )        /**< UAFnIIR Register UFnIRP bit  */
#define UAFnIIR_UFnIRID0    ( 0x0002 )        /**< UAFnIIR Register UFnIRID0 bit*/
#define UAFnIIR_UFnIRID1    ( 0x0004 )        /**< UAFnIIR Register UFnIRID1 bit*/
#define UAFnIIR_UFnIRID2    ( 0x0008 )        /**< UAFnIIR Register UFnIRID2 bit*/
#define UAFnIIR_UFnFMD0     ( 0x0040 )        /**< UAFnIIR Register UFnFMD0 bit */
#define UAFnIIR_UFnFMD1     ( 0x0080 )        /**< UAFnIIR Register UFnFMD1 bit */

/* UAFnLSR */
#define UAFnLSR_UFnDR       ( 0x0001 )        /**< UAFnLSR Register UFnDR bit   */
#define UAFnLSR_UFnOER      ( 0x0002 )        /**< UAFnLSR Register UFnOER bit  */
#define UAFnLSR_UFnPER      ( 0x0004 )        /**< UAFnLSR Register UFnPER bit  */
#define UAFnLSR_UFnFER      ( 0x0008 )        /**< UAFnLSR Register UFnFER bit  */
#define UAFnLSR_UFnBI       ( 0x0010 )        /**< UAFnLSR Register UFnBI bit   */
#define UAFnLSR_UFnTHRE     ( 0x0020 )        /**< UAFnLSR Register UFnTHRE bit */
#define UAFnLSR_UFnTEMT     ( 0x0040 )        /**< UAFnLSR Register UFnTEMT bit */
#define UAFnLSR_UFnRFE      ( 0x0080 )        /**< UAFnLSR Register UFnRFE bit  */


/*=== API parameters value ===*/
/* Initial configuration parameters for uartf0_init() */
/* UART Mode */
#define UARTF_LG_5BIT       (              0 )                                /**< Data length : 5bit                                              */
#define UARTF_LG_6BIT       (              0 |  UAFnMOD_UFnLG0 )              /**< Data length : 6bit                                              */
#define UARTF_LG_7BIT       ( UAFnMOD_UFnLG1 |               0 )              /**< Data length : 7bit                                              */
#define UARTF_LG_8BIT       ( UAFnMOD_UFnLG1 |  UAFnMOD_UFnLG0 )              /**< Data length : 8bit                                              */
#define UARTF_STP_1BIT      (              0 )                                /**< Stop bit : 1bit                                                 */
#define UARTF_STP_2BIT      ( UAFnMOD_UFnSTP )                                /**< Stop bit : 1.5bit(Data length=5bit), 2bit(Data length=6,7,8bit) */
#define UARTF_PT_NON        (              0 )                                /**< Parity bit : none                                               */
#define UARTF_PT_ODD        (              0 |  UAFnMOD_UFnPT0 )              /**< Parity bit : odd                                                */
#define UARTF_PT_EVEN       ( UAFnMOD_UFnPT1 |  UAFnMOD_UFnPT0 )              /**< Parity bit : even                                               */
#define UARTF_PT_FIXED_1    ( UAFnMOD_UFnPT2 |  UAFnMOD_UFnPT0 )              /**< Parity bit : fixed '1'                                          */
#define UARTF_PT_FIXED_0    ( UAFnMOD_UFnPT2 |  UAFnMOD_UFnPT1 |  UAFnMOD_UFnPT0 ) /**< Parity bit : fixed '0'                                     */
#define UARTF_BC_DIS        (              0 )                                /**< Break control : disable                                         */
#define UARTF_BC_ENA        ( UAFnMOD_UFnBC  )                                /**< Break control : enable                                          */
#define UARTF_DLAB_RBR_THR  (               0 )                               /**< UAFnBUF : RBR,THR                                               */
#define UARTF_DLAB_DLR      ( UAFnMOD_UFnDLAB )                               /**< UAFnBUF : DLR                                                   */
#define UARTF_FEN_DIS       (              0 )                                /**< FIFO : disable                                                  */
#define UARTF_FEN_ENA       ( UAFnMOD_UFnFEN )                                /**< FIFO : enable                                                   */
#define UARTF_RFR_KEEP      (              0 )                                /**< FIFO(Reception) : normal(data in FIFO is keeped)                */
#define UARTF_RFR_CLR       ( UAFnMOD_UFnRFR )                                /**< FIFO(Reception) : clear (data in FIFO is cleared)               */
#define UARTF_TFR_KEEP      (              0 )                                /**< FIFO(Transmit)  : normal(data in FIFO is keeped)                */
#define UARTF_TFR_CLR       ( UAFnMOD_UFnTFR )                                /**< FIFO(Transmit)  : clear (data in FIFO is cleared)               */
#define UARTF_FTL_1BYTE     (               0 )                               /**< Trigger level (Reception) : 1byte                               */
#define UARTF_FTL_2BYTE     ( UAFnMOD_UFnFTL0 )                               /**< Trigger level (Reception) : 2byte                               */
#define UARTF_FTL_3BYTE     ( UAFnMOD_UFnFTL1 )                               /**< Trigger level (Reception) : 3byte                               */
#define UARTF_FTL_4BYTE     ( UAFnMOD_UFnFTL1 | UAFnMOD_UFnFTL0 )             /**< Trigger level (Reception) : 4byte                               */

/* Interrupt */
#define UARTF_ERBFI_DIS     (                0 )                              /**< ERBFI (reception interuput) : disable                           */
#define UARTF_ERBFI_ENA     ( UAFnIIR_UFnERBFI )                              /**< ERBFI (reception interuput) : enable                            */
#define UARTF_ETBEI_DIS     (                0 )                              /**< ETBEI (transmittion interuput): disable                         */
#define UARTF_ETBEI_ENA     ( UAFnIIR_UFnETBEI )                              /**< ETBEI (transmittion interuput): enable                          */
#define UARTF_ELSI_DIS      (               0 )                               /**< ELSI (reception data error interuput) : disable                 */
#define UARTF_ELSI_ENA      ( UAFnIIR_UFnELSI )                               /**< ELSI (reception data error interuput) : enable                  */

#define UARTF_IRID_NONE         (                0 )                          /**< UFnIRID  : interrupt none                                       */
#define UARTF_IRID_WRITE_REQ    ( UAFnIIR_UFnIRID0 )                          /**< UFnIRID  : request of writing data                              */
#define UARTF_IRID_READ_REQ     ( UAFnIIR_UFnIRID1 )                          /**< UFnIRID  : request of reading data                              */
#define UARTF_IRID_CHAR_TIMEOUT ( UAFnIIR_UFnIRID2 | UAFnIIR_UFnIRID1 )       /**< UFnIRID  : character time out                                   */
#define UARTF_IRID_DATA_ERR     ( UAFnIIR_UFnIRID1 | UAFnIIR_UFnIRID0 )       /**< UFnIRID  : received data error                                  */


/*=== API return value ===*/
#define UARTF_R_OK              (  0 )  /**< Status of OK                               */
#define UARTF_R_TRANS_FIN       (  1 )  /**< writing or reading is finished             */
#define UARTF_R_TRANS_CONT_OK   (  0 )  /**< writing or reading is continued (success)  */

/*=== Callback function ===*/
/**
 * UARTF callback function
 *
 * @param[in]   size        size of reading/writing data
 * @param[in]   errStatus   status of UARTF operation result                    <br>
 *                          The logical sum of the following items.             <br>
 * @arg                         UAFnLSR_UFnFER
 * @arg                         UAFnLSR_UFnOER
 * @arg                         UAFnLSR_UFnPER
 * @return      None
 */
typedef void (*cbfUartF_t)( unsigned int size, unsigned short errStatus );

#endif /*_UARTF_COMMON_H_*/

