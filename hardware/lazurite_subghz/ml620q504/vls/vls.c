/* FILE NAME: vls.c
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

#include "mcu.h"
#include "rdwr_reg.h"
#include "vls.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/

/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/

/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize VLS
 *
 * @param[in]   vlsconl     Selection of VLS voltage                            <br>
 *                          (Setting value of VLSCONL register (VLSLV0-3)bit )  <br>
 *                          Specify the logical sum of the following items.
 * @arg                         VLSCONL_VLSLV0
 * @arg                         VLSCONL_VLSLV1
 * @arg                         VLSCONL_VLSLV2
 * @arg                         VLSCONL_VLSLV3
 *
 * @param[in]   vlsmodl     Configuration of VLS reset and interrupt            <br>
 *                          (Setting value of VLSMODL register)                 <br>
 * @arg                         VLS_RST_DIS_INT_DIS  : Disables reset and interrupt             <br>
 * @arg                         VLS_RST_ENA_INT_DIS1 : Enables reset, and disables interrupt    <br>
 * @arg                         VLS_RST_DIS_INT_ENA  : Disables reset, and enables interrupt    <br>
 * @arg                         VLS_RST_ENA_INT_DIS2 : Enables reset and interrupt              <br>
 *
 * @param[in]   vlsmodh     Configuration of VLS mode, and so on                <br>
 *                          (Setting value of VLSMODH register)                 <br>
 *                          Specify the logical sum of the following items.
 * 
 * @note        In the STOP mode, no sampling performed regardless of the value set in VLSSM0 since the sampling clock stops. <br>
 *              The sampling depends on the frequency of LSCLK.
 *
 * @arg                         Sampling        : VLS_SAMPLING_DIS, or VLS_SAMPLING_ENA
 * @arg                         VLS mode        : VLS_SINGLE_1, or VLS_SINGLE_2, or VLS_SUPERVISOR
 *
 * @return      None
 *
 * @note        Don't call this function, in the state that enabled VLS(ENVLS=1).
 */
void vls_init( unsigned char vlsconl, unsigned char vlsmodl, unsigned char vlsmodh )
{
	/* Configuration of VLS  */
	write_reg8( VLSMODL, vlsmodl );
	write_reg8( VLSMODH, vlsmodh );
	write_reg8( VLSCONL, vlsconl );
}

