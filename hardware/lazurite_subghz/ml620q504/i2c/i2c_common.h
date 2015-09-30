/* FILE NAME: i2c_common.h
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

#ifndef _I2C_COMMON_H_
#define _I2C_COMMON_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* I2CnMOD */
#define I2CnMOD_I2nEN           ( 0x0001 )                          /**< I2CnMOD Register I2nEN bit                             */
#define I2CnMOD_I2nMD           ( 0x0002 )                          /**< I2CnMOD Register I2nMD bit                             */
#define I2CnMOD_I2nDW0          ( 0x0004 )                          /**< I2CnMOD Register I2nDW0 bit                            */
#define I2CnMOD_I2nDW1          ( 0x0008 )                          /**< I2CnMOD Register I2nDW1 bit                            */
#define I2CnMOD_I2nCD0          ( 0x0100 )                          /**< I2CnMOD Register I2nCD0 bit                            */
#define I2CnMOD_I2nCD1          ( 0x0200 )                          /**< I2CnMOD Register I2nCD1 bit                            */

/* I2CnCON */
#define I2CnCON_I2nST           ( 0x0001 )                          /**< I2CnCON Register I2nST bit                             */
#define I2CnCON_I2nSP           ( 0x0002 )                          /**< I2CnCON Register I2nSP bit                             */
#define I2CnCON_I2nRS           ( 0x0004 )                          /**< I2CnCON Register I2nRS bit                             */
#define I2CnCON_I2nACT          ( 0x0080 )                          /**< I2CnCON Register I2nACT bit                            */

/* I2CnSA */
#define I2CnSA_I2nRW            ( 0x0001 )                          /**< I2CnSA Register I2nRW bit                              */

/*=== API parameters value ===*/
/* Initial configuration parameters for i2c*_init() */
#define I2C_MOD_STD             ( 0 )                               /**< Transmission mode : Standard mode(100kHz@4MHz)         */
#define I2C_MOD_FST             ( I2CnMOD_I2nMD )                   /**< Transmission mode : Fast mode(400kHz@4MHz)             */
#define I2C_DW_ATTENUAT_NONE    ( 0 )                               /**< Attenuation rate  : None                               */
#define I2C_DW_ATTENUAT_10      ( I2CnMOD_I2nDW0 )                  /**< Attenuation rate  : 10%                                */
#define I2C_DW_ATTENUAT_20      ( I2CnMOD_I2nDW1 )                  /**< Attenuation rate  : 20%                                */
#define I2C_DW_ATTENUAT_30      ( I2CnMOD_I2nDW1 | I2CnMOD_I2nDW0 ) /**< Attenuation rate  : 30%                                */
#define I2C_CD_OSCLK            ( 0 )                               /**< Clock division    : OSCLK                              */
#define I2C_CD_OSCLK2           ( I2CnMOD_I2nCD0 )                  /**< Clock division    : 1/2 OSCLK                          */
#define I2C_CD_OSCLK4           ( I2CnMOD_I2nCD1 )                  /**< Clock division    : 1/4 OSCLK                          */
#define I2C_CON_ACT_0           ( 0 )                               /**< Acknowledgment data : 0                                */
#define I2C_CON_ACT_1           ( I2CnCON_I2nACT )                  /**< Acknowledgment data : 1                                */

/* Parameters for i2c*_setSlaveAdrs() */
#define I2C_RW_WRITE            ( 0 )                               /**< Transmit mode     : Write                              */
#define I2C_RW_READ             ( I2CnSA_I2nRW )                    /**< Transmit mode     : Read                               */

/*=== API return value ===*/
#define I2C_R_OK                (  0 )                              /**< Status of OK                                                        */
#define I2C_R_TRANS_FIN         (  1 )                              /**< Communication of writing/reading data is finished (success/failure) */
#define I2C_R_TRANS_CONT_OK     (  0 )                              /**< Communication of writing/reading data is continued (success)        */

/*=== Error value ===*/
#define I2C_ERR_ACR             (  1 )                              /**< Error : Nack reception                                 */
#define I2C_ERR_SEND_ERR        (  2 )                              /**< Error : Transmit error                                 */


/*=== callback function ===*/
/**
 * I2C callback function
 *
 * @param[in]   size        size of reading/writing data
 * @param[in]   errStatus   status of I2C operation result      <br>
 *                          0                : Success          <br>
 *                          I2C_ERR_ACR      : Nack reception   <br>
 *                          I2C_ERR_SEND_ERR : Transmit error
 * @return      None
 */
typedef void (*cbfI2c_t)( unsigned int size, unsigned char errStat );

#endif /*_I2C_COMMON_H_*/
