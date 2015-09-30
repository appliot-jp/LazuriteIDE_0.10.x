/* FILE NAME: cmp0.h
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

#ifndef _CMP0_H_
#define _CMP0_H_

#include "rdwr_reg.h"
#include "cmp_common.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize Comparator
 *
 * @param[in]   cmpnmod     Configuration of Comparator mode , and so on        <br>
 *                          (Setting value of CMP0MOD register)                 <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Mode                            : CMP_MD_SINGLE, or CMP_MD_SINGLE_MONITOR, or CMP_MD_SUPERVISOR
 * @arg                         Interrupt trigger                                                                     <br>
 *                                  [for Single mode]           : CMP_INT_SINGLE_CMPnD0, or CMP_INT_SINGLE_CMPnD1     <br>
 *                                  [for Supervisor mode]       : CMP_INT_SVISOR_DIS, or CMP_INT_SVISOR_FALLING, or CMP_INT_SVISOR_RISING, or CMP_INT_SVISOR_BOTH
 * @arg                         Sampling clock                  : CMP_CLK_SAMPL_LS, or CMP_CLK_SAMPL_LS2, or CMP_CLK_SAMPL_OSCLK, or CMP_CLK_SAMPL_OSCLK64, or CMP_CLK_SAMPL_OSCLK128
 * 
 * @return      None
 * @note        Don't stop HSCLK oscillation which CPU is HALT mode,            <br>
 *              if 'Sampling clock' is CMP_CLK_SAMPL_OSCLK or CMP_CLK_SAMPL_OSCLK64 or CMP_CLK_SAMPL_OSCLK128.
 */

#define         cmp0_init( cmpnmod )        write_reg16( CMP0MOD, (unsigned short)(cmpnmod) )

/**
 * Activates Comparator (ON)
 *
 * @param       -
 * @return      None
 */
#define         cmp0_on()                   set_bit( CMP0EN )

/**
 * Deactivates Comparator (OFF)
 *
 * @param       -
 * @return      None
 */
#define         cmp0_off()                  clear_bit( CMP0EN )

/**
 * Getting Comparator result status
 *
 * Getting the status of Comparator result.
 *
 * @param       -
 * @retval      0 : Comparator result(CMPnD) is invalid
 * @retval      1 : Comparator result(CMPnD) is valid
 * @note        [for Supervisor mode] Check this macro returns '1', before the CPU mode is changed to the STOP mode.
 */
#define         cmp0_checkFin()             ( (unsigned char)get_bit( CMP0RF ) )

/**
 * Getting Comparator result
 *
 * @param       -
 * @retval      0 : CMPnP < CMPnM
 * @retval      1 : CMPnP > CMPnM
 */
#define         cmp0_getResult()            ( (unsigned char)get_bit( CMP0D ) )

/**
 * Getting Comparator running status
 *
 * @param       -
 * @retval      0 : Comparator is stop 
 * @retval      1 : Comparator is running
 * @note        [for Single mode and Single monitor mode]                   <br>
 *                  Check this macro returns '0', before the CPU mode is changed to the STOP mode.
 */
#define         cmp0_checkRunning()         ( (unsigned char)get_bit( CMP0EN ) )

#endif /*_CMP0_H_*/

