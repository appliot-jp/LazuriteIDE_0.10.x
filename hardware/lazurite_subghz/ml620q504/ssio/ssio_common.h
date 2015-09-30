/* FILE NAME: ssio_common.h
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

#ifndef _SSIO_COMMON_H_
#define _SSIO_COMMON_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* SIOnMOD */
#define SIOnMOD_SnDIR           ( 0x0001 )                          /**< SIOnMOD Register SnDIR bit                             */
#define SIOnMOD_SnMD0           ( 0x0002 )                          /**< SIOnMOD Register SnMD0 bit                             */
#define SIOnMOD_SnMD1           ( 0x0004 )                          /**< SIOnMOD Register SnMD1 bit                             */
#define SIOnMOD_SnLG            ( 0x0008 )                          /**< SIOnMOD Register SnLG bit                              */
#define SIOnMOD_SnCK0           ( 0x0100 )                          /**< SIOnMOD Register SnCK0 bit                             */
#define SIOnMOD_SnCK1           ( 0x0200 )                          /**< SIOnMOD Register SnCK1 bit                             */
#define SIOnMOD_SnCK2           ( 0x0400 )                          /**< SIOnMOD Register SnCK2 bit                             */
#define SIOnMOD_SnCKT           ( 0x1000 )                          /**< SIOnMOD Register SnCKT bit                             */
#define SIOnMOD_SnNEG           ( 0x2000 )                          /**< SIOnMOD Register SnNEG bit                             */

/*=== API return value ===*/
#define SSIO_R_OK               (  0 )                              /**< Status of OK                                           */
#define SSIO_R_ERR              ( -1 )                              /**< Noticed error                                          */
#define SSIO_R_DATA_SIZE_NG     ( -2 )                              /**< Value of data size is illegality                       */
#define SSIO_R_TRANS_FIN        (  1 )                              /**< Reading/writing communication is finished              */
#define SSIO_R_TRANS_CONT_OK    (  0 )                              /**< Reading/writing communication is continued             */

/*=== API parameters value ===*/
/* Initial configuration parameters for ssio*_init() */
#define SSIO_DIR_LSB            ( 0 )                               /**< Endian        : LSB first                              */
#define SSIO_DIR_MSB            ( SIOnMOD_SnDIR )                   /**< Endian        : MSB first                              */
#define SSIO_LG_8BIT            ( 0 )                               /**< Buffer mode   : 8bit                                   */
#define SSIO_LG_16BIT           ( SIOnMOD_SnLG )                    /**< Buffer mode   : 16bit                                  */
#define SSIO_CLK_LS             ( 0 )                               /**< Clock         : 1/1LSCLK                               */
#define SSIO_CLK_LS2            ( SIOnMOD_SnCK0 )                   /**< Clock         : 1/2LSCLK                               */
#define SSIO_CLK_HS4            ( SIOnMOD_SnCK1 )                   /**< Clock         : 1/4HSCLK                               */
#define SSIO_CLK_HS8            ( SIOnMOD_SnCK1 | SIOnMOD_SnCK0 )   /**< Clock         : 1/8HSCLK                               */
#define SSIO_CLK_HS16           ( SIOnMOD_SnCK2 )                   /**< Clock         : 1/16HSCLK                              */
#define SSIO_CLK_HS32           ( SIOnMOD_SnCK2 | SIOnMOD_SnCK0 )   /**< Clock         : 1/32HSCLK                              */
#define SSIO_CLK_EX             ( SIOnMOD_SnCK2 | SIOnMOD_SnCK1 )   /**< Clock         : External clock                         */
#define SSIO_CKT_HI             ( 0 )                               /**< Clock type    : default high                           */
#define SSIO_CKT_LOW            ( SIOnMOD_SnCKT )                   /**< Clock type    : default low                            */
#define SSIO_NEG_POS            ( 0 )                               /**< Logical value : positive                               */
#define SSIO_NEG_NEG            ( SIOnMOD_SnNEG )                   /**< Logical value : negative                               */

/* Select SSIO mode parameter for ssio*_start() */
#define SSIO_MODE1              ( SIOnMOD_SnMD0 )                   /**< Operation of SSIO mode : Reading(receive)              */
#define SSIO_MODE2              ( SIOnMOD_SnMD1 )                   /**< Operation of SSIO mode : Writing(send)                 */
#define SSIO_MODE3              ( SIOnMOD_SnMD1 | SIOnMOD_SnMD0 )   /**< Operation of SSIO mode : Reading and writing           */

/*=== Callback function ===*/
/**
 * SSIO callback function
 *
 * @param[in]   size        size of transsmition/reception operation data                           <br>
 *                          Unit is BYTE.
 * @return      None
 */
typedef void (*cbfSsio_t)( unsigned int size );

#endif /*_SSIO_COMMON_H_*/

