/* FILE NAME: timer_common.h
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

#ifndef _TIMER_COMMON_H_
#define _TIMER_COMMON_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* TMnCON */
#define TMnCON_TnCS0        ( 0x01 )       /**< TMnCON Register TnCS0 bit  */
#define TMnCON_TnCS1        ( 0x02 )       /**< TMnCON Register TnCS1 bit  */
#define TMnCON_TnDIV0       ( 0x08 )       /**< TMnCON Register TnDIV0 bit */
#define TMnCON_TnDIV1       ( 0x10 )       /**< TMnCON Register TnDIV1 bit */
#define TMnCON_TnDIV2       ( 0x20 )       /**< TMnCON Register TnDIV2 bit */
#define TMnCON_TmM16        ( 0x40 )       /**< TMnCON Register Tn1M16 bit */
#define TMnCON_TnOST        ( 0x80 )       /**< TMnCON Register TnOST bit  */

/*=== API parameters value ===*/
/* Initial configuration parameters for timerX_init() */
#define TM_CS_LSCLK         (               0 |               0 )                      /**< Clock source   :LSCLK        */
#define TM_CS_OSCLK         (               0 |    TMnCON_TnCS0 )                      /**< Clock source   :OSCLK        */
#define TM_CS_EXTCLK        (    TMnCON_TnCS1 |    TMnCON_TnCS0 )                      /**< Clock source   :EXTCLK       */
#define TM_DIV1             (               0 |               0 |               0 )    /**< Division ratio :Not divide   */
#define TM_DIV2             (               0 |               0 |   TMnCON_TnDIV0 )    /**< Division ratio :1/2          */
#define TM_DIV4             (               0 |   TMnCON_TnDIV1 |               0 )    /**< Division ratio :1/4          */
#define TM_DIV8             (               0 |   TMnCON_TnDIV1 |   TMnCON_TnDIV0 )    /**< Division ratio :1/8          */
#define TM_DIV16            (   TMnCON_TnDIV2 |               0 |               0 )    /**< Division ratio :1/16         */
#define TM_DIV32            (   TMnCON_TnDIV2 |               0 |   TMnCON_TnDIV0 )    /**< Division ratio :1/32         */
#define TM_DIV64            (   TMnCON_TnDIV2 |   TMnCON_TnDIV1 |               0 )    /**< Division ratio :1/64         */
#define TM_MODE_8BIT        (               0 )                                        /**< Timer mode     :8-bit timer  */
#define TM_MODE_16BIT       (    TMnCON_TmM16 )                                        /**< Timer mode     :16-bit timer */
#define TM_OST_REROAD       (               0 )                                        /**< Mode           :auto reload  */
#define TM_OST_ONESHOT      (    TMnCON_TnOST )                                        /**< Mode           :one shout    */

#endif /*_TIMER_COMMON_H_*/

