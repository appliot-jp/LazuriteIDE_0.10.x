
/* FILE NAME: cmp_common.h
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

#ifndef _CMP_COMMON_H_
#define _CMP_COMMON_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* CMPnMOD */
#define CMPnMOD_CMPnE0          ( 0x0001 )                          /**< CMPnMOD Register CMPnE0 bit                            */
#define CMPnMOD_CMPnE1          ( 0x0002 )                          /**< CMPnMOD Register CMPnE0 bit                            */
#define CMPnMOD_CMPnMD0         ( 0x0010 )                          /**< CMPnMOD Register CMPnMD0 bit                           */
#define CMPnMOD_CMPnMD1         ( 0x0020 )                          /**< CMPnMOD Register CMPnMD1 bit                           */
#define CMPnMOD_CMPnSM0         ( 0x0100 )                          /**< CMPnMOD Register CMPnSM0 bit                           */
#define CMPnMOD_CMPnSM1         ( 0x0200 )                          /**< CMPnMOD Register CMPnSM1 bit                           */
#define CMPnMOD_CMPnCK          ( 0x1000 )                          /**< CMPnMOD Register CMPnCK bit                            */

/*=== API parameters value ===*/
/* Initial configuration parameters for cmp*_init() */
/* Mode */
#define CMP_MD_SINGLE           ( 0 )                               /**< Mode              : Single mode                        */
#define CMP_MD_SINGLE_MONITOR   ( CMPnMOD_CMPnMD0 )                 /**< Mode              : Single monitor mode                */
#define CMP_MD_SUPERVISOR       ( CMPnMOD_CMPnMD1 )                 /**< Mode              : Supervisor mode                    */
/* Interrupt */
#define CMP_INT_SINGLE_CMPnD0   ( 0 )                               /**< Interrupt trigger : [for Single mode] CMPnD=0          */
#define CMP_INT_SINGLE_CMPnD1   ( CMPnMOD_CMPnE1 )                  /**< Interrupt trigger : [for Single mode] CMPnD=1          */
#define CMP_INT_SVISOR_DIS      ( 0 )                               /**< Interrupt trigger : [for Supervisor mode] disable      */
#define CMP_INT_SVISOR_FALLING  ( CMPnMOD_CMPnE0 )                  /**< Interrupt trigger : [for Supervisor mode] falling edge */
#define CMP_INT_SVISOR_RISING   ( CMPnMOD_CMPnE1 )                  /**< Interrupt trigger : [for Supervisor mode] rising edge  */
#define CMP_INT_SVISOR_BOTH     ( CMPnMOD_CMPnE1 | CMPnMOD_CMPnE0 ) /**< Interrupt trigger : [for Supervisor mode] both edge    */
/* Sampling clock */
#define CMP_CLK_SAMPL_LS        ( 0 )                                                   /**< Sampling clock    : LSCLK          */
#define CMP_CLK_SAMPL_LS2       ( CMPnMOD_CMPnSM0 )                                     /**< Sampling clock    : 1/2LSCLK       */
#define CMP_CLK_SAMPL_OSCLK     ( CMPnMOD_CMPnCK )                                      /**< Sampling clock    : OSCCLK         */
#define CMP_CLK_SAMPL_OSCLK64   ( CMPnMOD_CMPnCK | CMPnMOD_CMPnSM0 )                    /**< Sampling clock    : 1/64  OSCCLK   */
#define CMP_CLK_SAMPL_OSCLK128  ( CMPnMOD_CMPnCK | CMPnMOD_CMPnSM1 | CMPnMOD_CMPnSM0 )  /**< Sampling clock    : 1/128 OSCCLK   */


#endif /*_CMP_COMMON_H_*/

