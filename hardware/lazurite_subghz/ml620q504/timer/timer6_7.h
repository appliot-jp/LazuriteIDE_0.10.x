/* FILE NAME: timer6_7.h
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

#ifndef _TIMER6_7_H_
#define _TIMER6_7_H_

#include "rdwr_reg.h"
#include "timer_common.h"

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize timer6
 *
 * @param[in]   tmncon  A value of setting TM6CON register. <br>
 *                      Specify the logical sum of the following items.
 * @arg                     Clock source      : TM_CS_LSCLK, or TM_CS_OSCLK, or TM_CS_EXTCLK
 * @arg                     Division ratio    : TM_DIV1, or TM_DIV4, or TM_DIV8, or TM_DIV16, or TM_DIV32, or TM_DIV64
 * @arg                     Timer             : TM_MODE_8BIT, or TM_MODE_16BIT
 * @arg                     Mode              : TM_OST_REROAD, or TM_OST_ONESHOT
 * @return      None
 */
#define         timer6_init( tmncon )   write_reg8( TM6CON, tmncon )
/**
 * Initialize timer7
 *
 * @param[in]   tmncon  A value of setting TM7CON register. <br>
 *                      Specify the logical sum of the following items.
 * @arg                     Clock source      : TM_CS_LSCLK, or TM_CS_OSCLK, or TM_CS_EXTCLK
 * @arg                     Division ratio    : TM_DIV1, or TM_DIV4, or TM_DIV8, or TM_DIV16, or TM_DIV32, or TM_DIV64
 * @arg                     Timer             : Only TM_MODE_8BIT
 * @arg                     Mode              : TM_OST_REROAD, or TM_OST_ONESHOT
 * @return      None
 */
#define         timer7_init( tmncon )   write_reg8( TM7CON, tmncon )
/**
 * Start timer6
 *
 * @param       -
 * @return      None
 */
#define         timer6_start()          set_bit( T6RUN )
/**
 * Start timer7
 *
 * @param       -
 * @return      None
 */
#define         timer7_start()          set_bit( T7RUN )
/**
 * Stop timer6
 *
 * @param       -
 * @return      None
 */
#define         timer6_stop()           set_bit( T6STP )
/**
 * Stop timer7
 *
 * @param       -
 * @return      None
 */
#define         timer7_stop()           set_bit( T7STP )

void            timer6_setCnt( unsigned short cnt );
void            timer7_setCnt( unsigned short cnt );

#endif /*_TIMER6_7_H_*/

