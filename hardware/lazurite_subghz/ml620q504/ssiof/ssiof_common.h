/* FILE NAME: ssiof_common.h
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

#ifndef _SSIOF_COMMON_H_
#define _SSIOF_COMMON_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* SFnCTRL */
#define SFnCTRL_SFnSPE          ( 0x0001 )                          /**< SFnCTRL Register SFnSPE bit                            */
#define SFnCTRL_SFnMST          ( 0x0002 )                          /**< SFnCTRL Register SFnMST bit                            */
#define SFnCTRL_SFnSIZ          ( 0x0004 )                          /**< SFnCTRL Register SFnSIZ bit                            */
#define SFnCTRL_SFnMDF          ( 0x0008 )                          /**< SFnCTRL Register SFnMDF bit                            */
#define SFnCTRL_SFnLSB          ( 0x0010 )                          /**< SFnCTRL Register SFnLSB bit                            */
#define SFnCTRL_SFnCPHA         ( 0x0020 )                          /**< SFnCTRL Register SFnCPHA bit                           */
#define SFnCTRL_SFnCPOL         ( 0x0040 )                          /**< SFnCTRL Register SFnCPOL bit                           */
#define SFnCTRL_SFnFICL         ( 0x0100 )                          /**< SFnCTRL Register SFnFICL bit                           */
#define SFnCTRL_SFnSSZ          ( 0x0200 )                          /**< SFnCTRL Register SFnSSZ bit                            */
#define SFnCTRL_SFnSOZ          ( 0x0400 )                          /**< SFnCTRL Register SFnSOZ bit                            */
#define SFnCTRL_SFnMOZ          ( 0x0800 )                          /**< SFnCTRL Register SFnMOZ bit                            */

/* SFnINTC */
#define SFnINTC_SFnTFIE         ( 0x0001 )                          /**< SFnINTC Register SFnTFIE bit                           */
#define SFnINTC_SFnRFIE         ( 0x0002 )                          /**< SFnINTC Register SFnRFIE bit                           */
#define SFnINTC_SFnFIE          ( 0x0004 )                          /**< SFnINTC Register SFnFIE bit                            */
#define SFnINTC_SFnORIE         ( 0x0008 )                          /**< SFnINTC Register SFnORIE bit                           */
#define SFnINTC_SFnMFIE         ( 0x0010 )                          /**< SFnINTC Register SFnMFIE bit                           */
#define SFnINTC_SFnTFIC0        ( 0x0100 )                          /**< SFnINTC Register SFnTFIC0 bit                          */
#define SFnINTC_SFnTFIC1        ( 0x0200 )                          /**< SFnINTC Register SFnTFIC1 bit                          */
#define SFnINTC_SFnRFIC0        ( 0x1000 )                          /**< SFnINTC Register SFnRFIC0 bit                          */
#define SFnINTC_SFnRFIC1        ( 0x2000 )                          /**< SFnINTC Register SFnRFIC1 bit                          */

/* SFnSRR */
#define SFnSRR_SFnTFI           ( 0x0001 )                          /**< SFnSRR Register SFnTFI bit                             */
#define SFnSRR_SFnRFI           ( 0x0002 )                          /**< SFnSRR Register SFnRFI bit                             */
#define SFnSRR_SFnFI            ( 0x0004 )                          /**< SFnSRR Register SFnFI bit                              */
#define SFnSRR_SFnORF           ( 0x0008 )                          /**< SFnSRR Register SFnORF bit                             */
#define SFnSRR_SFnMDF           ( 0x0010 )                          /**< SFnSRR Register SFnMDF bit                             */
#define SFnSRR_SFnSSIOFF        ( 0x0020 )                          /**< SFnSRR Register SFnSSIOFF bit                          */
#define SFnSRR_SFnWOF           ( 0x0100 )                          /**< SFnSRR Register SFnWOF bit                             */
#define SFnSRR_SFnTFF           ( 0x0200 )                          /**< SFnSRR Register SFnTFF bit                             */
#define SFnSRR_SFnTFE           ( 0x0400 )                          /**< SFnSRR Register SFnTFE bit                             */
#define SFnSRR_SFnRFF           ( 0x0800 )                          /**< SFnSRR Register SFnRFF bit                             */
#define SFnSRR_SFnRFE           ( 0x1000 )                          /**< SFnSRR Register SFnRFE bit                             */

/* SFnSRC */
#define SFnSRC_SFnTFC           ( SFnSRR_SFnTFI   )                 /**< SFnSRC Register SFnTFC bit                             */
#define SFnSRC_SFnRFC           ( SFnSRR_SFnRFI   )                 /**< SFnSRC Register SFnRFC bit                             */
#define SFnSRC_SFnFC            ( SFnSRR_SFnFI    )                 /**< SFnSRC Register SFnFC bit                              */
#define SFnSRC_SFnORFC          ( SFnSRR_SFnORF  )                  /**< SFnSRC Register SFnORFC bit                            */
#define SFnSRC_SFnMDFC          ( SFnSRR_SFnMDF  )                  /**< SFnSRC Register SFnMDFC bit                            */
#define SFnSRC_SFnSSIOFFC       ( SFnSRR_SFnSSIOFF )                /**< SFnSRC Register SFnSSIOFFC bit                         */
#define SFnSRC_SFnWOFC          ( SFnSRR_SFnWOF  )                  /**< SFnSRC Register SFnWOFC bit                            */
#define SFnSRC_SFnIRQ           ( 0x8000 )                          /**< SFnSRC Register SFnIRQ bit                             */

/*=== API return value ===*/
#define SSIOF_R_OK                (  0 )                              /**< Status of OK                                           */
#define SSIOF_R_ERR               ( -1 )                              /**< Noticed error                                          */
#define SSIOF_R_TRANS_FIN         (  1 )                              /**< Reading/writing communication is finished              */
#define SSIOF_R_TRANS_CONT_OK     (  0 )                              /**< Reading/writing communication is continued             */

/*=== API parameters value ===*/
/* Initial configuration parameters for ssiof*_init() */
/* Mode */
#define SSIOF_MST_SLAVE           ( 0 )                               /**< Master/Slave  : Slave                                  */
#define SSIOF_MST_MASTER          ( SFnCTRL_SFnMST )                  /**< Master/Slave  : Master                                 */
#define SSIOF_LG_8BIT             ( 0 )                               /**< Buffer mode   : 8bit                                   */
#define SSIOF_LG_16BIT            ( SFnCTRL_SFnSIZ )                  /**< Buffer mode   : 16bit                                  */
#define SSIOF_MDF_DIS             ( 0 )                               /**< Mode fault    : disable                                */
#define SSIOF_MDF_ENA             ( SFnCTRL_SFnMDF )                  /**< Mode fault    : enable                                 */
#define SSIOF_DIR_LSB             ( 0 )                               /**< Endian        : LSB first                              */
#define SSIOF_DIR_MSB             ( SFnCTRL_SFnLSB )                  /**< Endian        : MSB first                              */
#define SSIOF_CPHA_1SM_2SH        ( 0 )                               /**< Clock phase   : 1st edge=sampling, 2nd edge=shift      */
#define SSIOF_CPHA_1SH_2SM        ( SFnCTRL_SFnCPHA )                 /**< Clock phase   : 1st edge=shift,    2nd edge=sampling   */
#define SSIOF_CPOL_LOW            ( 0 )                               /**< Clock type    : default low                            */
#define SSIOF_CPOL_HIGH           ( SFnCTRL_SFnCPOL )                 /**< Clock type    : default high                           */
#define SSIOF_SSZ_OUTPUT          ( 0 )                               /**< SSn output    : 0/1 output                             */
#define SSIOF_SSZ_HIZ             ( SFnCTRL_SFnSSZ )                  /**< SSn output    : Hi-Z                                   */
#define SSIOF_SOZ_OUTPUT          ( 0 )                               /**< MISO output   : 0/1 output                             */
#define SSIOF_SOZ_HIZ             ( SFnCTRL_SFnSOZ )                  /**< MISO output   : Hi-Z                                   */
#define SSIOF_MOZ_OUTPUT          ( 0 )                               /**< MOSI output   : 0/1 output                             */
#define SSIOF_MOZ_HIZ             ( SFnCTRL_SFnMOZ )                  /**< MOSI output   : Hi-Z                                   */
/* Interrupt */
#define SSIOF_INT_TFIE_DIS        ( 0 )                               /**< SFnTFIE       : disable                                */
#define SSIOF_INT_TFIE_ENA        ( SFnINTC_SFnTFIE )                 /**< SFnTFIE       : enable                                 */
#define SSIOF_INT_RFIE_DIS        ( 0 )                               /**< SFnRFIE       : disable                                */
#define SSIOF_INT_RFIE_ENA        ( SFnINTC_SFnRFIE )                 /**< SFnRFIE       : enable                                 */
#define SSIOF_INT_FIE_DIS         ( 0 )                               /**< SFnFIE        : disable                                */
#define SSIOF_INT_FIE_ENA         ( SFnINTC_SFnFIE )                  /**< SFnFIE        : enable                                 */
#define SSIOF_INT_ORIE_DIS        ( 0 )                               /**< SFnORIE       : disable                                */
#define SSIOF_INT_ORIE_ENA        ( SFnINTC_SFnORIE )                 /**< SFnORIE       : enable                                 */
#define SSIOF_INT_MFIE_DIS        ( 0 )                               /**< SFnMFIE       : disable                                */
#define SSIOF_INT_MFIE_ENA        ( SFnINTC_SFnMFIE )                 /**< SFnMFIE       : enable                                 */
#define SSIOF_INT_WR_THRESH_0     ( 0 )                                     /**< Interrupt threshold (Write) : remain 0 byte      */
#define SSIOF_INT_WR_THRESH_1     ( SFnINTC_SFnTFIC0 )                      /**< Interrupt threshold (Write) : remain 1 byte      */
#define SSIOF_INT_WR_THRESH_2     ( SFnINTC_SFnTFIC1 )                      /**< Interrupt threshold (Write) : remain 2 byte      */
#define SSIOF_INT_WR_THRESH_3     ( SFnINTC_SFnTFIC1 | SFnINTC_SFnTFIC0 )   /**< Interrupt threshold (Write) : remain 3 byte      */
#define SSIOF_INT_RD_THRESH_1     ( 0 )                                     /**< Interrupt threshold (Read)  : 1 byte reception   */
#define SSIOF_INT_RD_THRESH_2     ( SFnINTC_SFnRFIC0 )                      /**< Interrupt threshold (Read)  : 2 byte reception   */
#define SSIOF_INT_RD_THRESH_3     ( SFnINTC_SFnRFIC1 )                      /**< Interrupt threshold (Read)  : 3 byte reception   */
#define SSIOF_INT_RD_THRESH_4     ( SFnINTC_SFnRFIC1 | SFnINTC_SFnRFIC0 )   /**< Interrupt threshold (Read)  : 4 byte reception   */


/* Select SSIOF mode parameter for ssiof*_start() */
#define SSIOF_MODE_RD             (  0 )                              /**< Operation of SSIOF mode : Reading(receive)                */
#define SSIOF_MODE_WR             (  1 )                              /**< Operation of SSIOF mode : Writing(send)                   */
#define SSIOF_MODE_RD_WR          (  2 )                              /**< Operation of SSIOF mode : Reading / Writing(receive/send) */

/*=== Callback function ===*/
/**
 * SSIOF callback function
 *
 * @param[in]   dataCnt     Count of writing/reading operation data                                 <br>
 *                          Unit is BYTE or WORD.                                                   <br>
 *                          If bit-length is 8bit(SFnSIZ=0), unit is BYTE in this parameter.        <br>
 *                          If bit-length is 16bit(SFnSIZ=1), unit is WORD in this parameter.
 *
 * @param[in]   errStatus   Status of SSIOF operation result                                        <br>
 *                          The logical sum of the following items.
 * @arg                         SFnSRR_SFnMDF
 * @arg                         SFnSRR_SFnWOF
 * @arg                         SFnSRR_SFnORF
 * @return      None
 */
typedef void (*cbfSsiof_t)( unsigned int dataCnt, unsigned short errStatus );

#endif /*_SSIOF_COMMON_H_*/

