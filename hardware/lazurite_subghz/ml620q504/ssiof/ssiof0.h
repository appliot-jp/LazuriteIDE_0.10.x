/* FILE NAME: ssiof0.h
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

#ifndef _SSIOF0_H_
#define _SSIOF0_H_

#include "rdwr_reg.h"
#include "ssiof_common.h"

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize SSIOF
 *
 * @param[in]   sfnctrl     Configuration of SSIOF mode , and so on             <br>
 *                          (Setting value of SF0CTRL register)                 <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Master/Slave                : SSIOF_MST_MASTER, or SSIOF_MST_SLAVE
 * @arg                         Buffer mode                 : SSIOF_LG_8BIT, or SSIOF_LG_16BIT
 * @arg                         Mode fault                  : SSIOF_MDF_DIS, or SSIOF_MDF_ENA
 * @arg                         Endian                      : SSIOF_DIR_LSB, or SSIOF_DIR_MSB
 * @arg                         Clock phase                 : SSIOF_CPHA_1SM_2SH, or SSIOF_CPHA_1SH_2SM
 * @arg                         Clock type                  : SSIOF_CPOL_LOW, or SSIOF_CPOL_HIGH
 * @arg                         SSn output                  : SSIOF_SSZ_OUTPUT, or SSIOF_SSZ_HIZ
 * @arg                         MISO output                 : SSIOF_SOZ_OUTPUT, or SSIOF_SOZ_HIZ
 * @arg                         MOSI output                 : SSIOF_MOZ_OUTPUT, or SSIOF_MOZ_HIZ
 *
 * @param[in]   sfnintc     Configuration of interrupt                          <br>
 *                          (Setting value of SF0INTC register)                 <br>
 *                          Specify the logical sum of the following items.
 * @arg                         SFnTFIE                     : SSIOF_INT_TFIE_DIS
 * @arg                         SFnRFIE                     : SSIOF_INT_RFIE_DIS
 * @arg                         SFnFIE                      : SSIOF_INT_FIE_DIS
 * @arg                         SFnORIE                     : SSIOF_INT_ORIE_DIS
 * @arg                         SFnMFIE                     : SSIOF_INT_MFIE_DIS
 * @arg                         Interrupt threshold (Write) : SSIOF_INT_WR_THRESH_0, or SSIOF_INT_WR_THRESH_1, or SSIOF_INT_WR_THRESH_2, or SSIOF_INT_WR_THRESH_3
 * @arg                         Interrupt threshold (Read)  : SSIOF_INT_RD_THRESH_1
 * 
 * @return      None
 * @note        If you set master mode(=SSIOF_MST_MASTER), wait over 100ns.
 * @note        Trigger-Level of the Reading FIFO is fixed at 1Byte.
 * @note        Initial state to disable interrupts.
 */
#define         ssiof0_init( sfnctrl, sfnintc )           do {                                                   \
                                                            ssiof0_clearFifo();                                  \
                                                            write_reg16( SF0CTRL,  (unsigned short)(sfnctrl) );  \
                                                            write_reg16( SF0INTC,  (unsigned short)(sfnintc & SSIOF_INT_WR_THRESH_3) ); \
                                                             }while(0)

/**
 * Setting baud-rate
 *
 * @param[in]   sfnbrr  Setting baud-rate                                       <br>
 *                      (Setting value of SF0BRR register)
 * @note
 * @arg         About baud-rate clock calculation                               <br>
 *                  Baud-rate clock = 'HSCLK' / ( 2 * 'sfnbrr')                 <br>
 * @arg         About maximum of baud-rate clock frequency                      <br>
 *                  Don't set greater than 4MHz baud-rate.
 * @return      None
 *
 * @note        The maximum SSIOF transfer frequency is 4MHz. This setting should not exceed 4MHz. <br>
 *              If using P22 as SCKF0 in master mode, the max frequency is 2MHz.
 */
#define         ssiof0_setBaudrate( sfnbrr )              write_reg16( SF0BRR, (unsigned short)(sfnbrr) )

/**
 * Setting interval time
 *
 * @param[in]   sfntrac Setting interval time <br>
 *                      (Setting value of SF0TRAC register)
 * @return      None
 * @note        This macro is master mode only.
 */
#define         ssiof0_setIntervalTime( sfntrac )         write_reg16( SF0TRAC, (unsigned short)(sfntrac) )

/**
 * Writing transmission of data (for BYTE access)
 *
 * @param[in]   data    A transmission data (8bit length)                       <br>
 *                      (Setting value of SF0DWRL register)
 * @return      None
 */
#define         ssiof0_putcByte( data )   write_reg8( SF0DWRL, (unsigned char)(data) )

/**
 * Writing transmission of data (for WORD access)
 *
 * @param[in]   data    A transmission data (16bit length)                      <br>
 *                      (Setting value of SF0DWR register)
 * @return      None
 */
#define         ssiof0_putcWord( data )   write_reg16( SF0DWR, (unsigned short)(data) )

/**
 * Reading reception data (for BYTE access)
 *
 * @param       -
 * @return      A reception data (8bit length)                                  <br>
 *              (A value of SF0DRRL register)
 */
#define         ssiof0_getcByte()         read_reg8( SF0DRRL )

/**
 * Reading reception data (for WORD access)
 *
 * @param       -
 * @return      A reception data (16bit length)                                 <br>
 *              (A value of SF0DRR register)
 */
#define         ssiof0_getcWord()         read_reg16( SF0DRR )

/**
 * Getting SSIOF status
 *
 * @param       -
 * @return      SSIOF status <br>
 *              (A value of SF0SRR register)
 */
#define         ssiof0_getStatus()        read_reg16( SF0SRR )

/**
 * Crear SSIOF status
 *
 * @param       sfnsrc  Mask value of the status you want to clear      <br>
 *                      Specify the logical sum of the following items.
 * @arg                     SF0TFC      : SFnSRC_SFnTFC
 * @arg                     SF0RFC      : SFnSRC_SFnRFC
 * @arg                     SF0FC       : SFnSRC_SFnFC
 * @arg                     SF0ORFC     : SFnSRC_SFnORFC
 * @arg                     SF0MDFC     : SFnSRC_SFnMDFC
 * @arg                     SF0SSIOFFC  : SFnSRC_SFnSSIOFFC
 * @arg                     SF0WOFC     : SFnSRC_SFnWOFC
 *
 * @return      None
 */
#define         ssiof0_clearStatus( sfnsrc )  write_reg16( SF0SRC, (unsigned short)(sfnsrc) )

/**
 * Checking SSIOF busy status
 *
 * @param       -
 * @return      Mask value of status
 * @retval      0   busy status : Ready(able to read/write next operation)
 * @retval      1   busy status : Busy
 */
#define         ssiof0_checkReady()       ( (unsigned char)( ! get_bit( SF0SPIF ) ) )

/**
 * Trig interrupt
 *
 * Request SSIOF interrupt(for SSIOF), again.
 *
 * @param       -
 * @return      None
 */
#define         ssiof0_trigIRQ()          set_bit( SF0IRQ )

/**
 * Getting FIFO(for read) size
 *
 * @param       -
 * @return      Received data size in the FIFO
 */
#define         ssiof0_getReadFifoSize()  read_reg8( SF0FSRH )

/**
 * Getting FIFO(for write) size
 *
 * @param       -
 * @return      The size of data remaining in the FIFO
 */
#define         ssiof0_getWriteFifoSize() read_reg8( SF0FSRL )

void            ssiof0_clearFifo();
int             ssiof0_start( unsigned char mode, void *rxData, void *txData, unsigned int dataCnt, cbfSsiof_t func );
int             ssiof0_continue( void );

#endif /*_SSIOF0_H_*/

