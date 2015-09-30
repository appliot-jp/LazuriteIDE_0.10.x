/* FILE NAME: vls.h
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

#ifndef _VLS_H_
#define _VLS_H_

#include "rdwr_reg.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* Initial configuration parameters for vls_init() */
/* VLSCONL */
#define VLSCONL_VLSLV0          ( 0x01 )                            /**< VLSCONL Register VLSLV0 bit  */
#define VLSCONL_VLSLV1          ( 0x02 )                            /**< VLSCONL Register VLSLV1 bit  */
#define VLSCONL_VLSLV2          ( 0x04 )                            /**< VLSCONL Register VLSLV2 bit  */
#define VLSCONL_VLSLV3          ( 0x08 )                            /**< VLSCONL Register VLSLV3 bit  */

/* VLSMODL */
#define VLSMODL_VLSSEL0         ( 0x01 )                            /**< VLSMODL Register VLSSEL0 bit */
#define VLSMODL_VLSSEL1         ( 0x02 )                            /**< VLSMODL Register VLSSEL1 bit */

/* VLSMODH */
#define VLSMODH_VLSSM0          ( 0x01 )                            /**< VLSMODH Register VLSSM0 bit  */
#define VLSMODH_VLSAMD0         ( 0x02 )                            /**< VLSMODH Register VLSAMD0 bit */
#define VLSMODH_VLSAMD1         ( 0x04 )                            /**< VLSMODH Register VLSAMD1 bit */

/*=== API parameters value ===*/
#define VLS_SINGLE_1            ( 0 )                               /**< VLS mode : Single mode 1     */
#define VLS_SINGLE_2            ( VLSMODH_VLSAMD0 )                 /**< VLS mode : Single mode 2     */
#define VLS_SUPERVISOR          ( VLSMODH_VLSAMD1 )                 /**< VLS mode : Supervisor mode   */
#define VLS_SUPERVISOR2         ( VLSMODH_VLSAMD0 | VLSMODH_VLSAMD1 )

#define VLS_SAMPLING_DIS        ( 0 )                               /**< Sampling : Without sampling  */
#define VLS_SAMPLING_ENA        ( VLSMODH_VLSSM0 )                  /**< Sampling : With sampling     */

#define VLS_RST_DIS_INT_DIS  ( 0 )                                  /**< Reset and interrupt : disable          */
#define VLS_RST_ENA_INT_DIS1 ( VLSMODL_VLSSEL0 )                    /**< Reset : enable,  Interrupt : disable   */
#define VLS_RST_DIS_INT_ENA  ( VLSMODL_VLSSEL1 )                    /**< Reset : disable, Interrupt : enable    */
#define VLS_RST_ENA_INT_DIS2 ( VLSMODL_VLSSEL1 | VLSMODL_VLSSEL0 )  /**< Reset : enable,  Interrupt : disable   */

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Activates VLS (ON)
 *
 * @param       -
 * @return      None
 */
#define         vls_on()                    set_bit( ENVLS )

/**
 * Checking VLS result
 *
 * Getting the result of compare voltage.
 *
 * @param       -
 * @retval      0 : VDD > VVLS
 * @retval      1 : otherwise
 */
#define         vls_check()                 ( (unsigned char)get_bit( VLSF ) )

/**
 * Deactivates VLS (OFF)
 * @note        Make sure that the VLSRF bit is set to "1" before enabling the STOP mode.
 *
 * @param       -
 * @return      None
 */
#define         vls_off()                   clear_bit( ENVLS )

/**
 * Getting VLS status
 *
 * Getting the status of VLS result.
 *
 * @param       -
 * @retval      0 : VLS result is invalid (VLS-OFF or Judgment is in progress)
 * @retval      1 : VLS result is valid
 */
#define         vls_getResultStatus()       ( (unsigned char)get_bit( VLSRF ) )


void            vls_init( unsigned char vlsconl, unsigned char vlsmodl, unsigned char vlsmodh );

#endif /*_VLS_H_*/

