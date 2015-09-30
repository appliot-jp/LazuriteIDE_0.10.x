/* FILE NAME: timer4_5.h
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

#ifndef _TIMER4_5_H_
#define _TIMER4_5_H_

#include "rdwr_reg.h"
#include "timer_common.h"

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize timer4
 *
 * @param[in]   tmncon  A value of setting TM4CON register. <br>
 *                      Specify the logical sum of the following items.
 * @arg                     Clock source      : TM_CS_LSCLK, or TM_CS_OSCLK, or TM_CS_EXTCLK
 * @arg                     Division ratio    : TM_DIV1, or TM_DIV4, or TM_DIV8, or TM_DIV16, or TM_DIV32, or TM_DIV64
 * @arg                     Timer             : TM_MODE_8BIT, or TM_MODE_16BIT
 * @arg                     Mode              : TM_OST_REROAD, or TM_OST_ONESHOT
 * @return      None
 */
#define         timer4_init( tmncon )   write_reg8( TM4CON, tmncon )
/**
 * Initialize timer5
 *
 * @param[in]   tmncon  A value of setting TM5CON register. <br>
 *                      Specify the logical sum of the following items.
 * @arg                     Clock source      : TM_CS_LSCLK, or TM_CS_OSCLK, or TM_CS_EXTCLK
 * @arg                     Division ratio    : TM_DIV1, or TM_DIV4, or TM_DIV8, or TM_DIV16, or TM_DIV32, or TM_DIV64
 * @arg                     Timer             : Only TM_MODE_8BIT
 * @arg                     Mode              : TM_OST_REROAD, or TM_OST_ONESHOT
 * @return      None
 */
#define         timer5_init( tmncon )   write_reg8( TM5CON, tmncon )
/**
 * Start timer4
 *
 * @param       -
 * @return      None
 */
#define         timer4_start()          set_bit( T4RUN )
/**
 * Start timer5
 *
 * @param       -
 * @return      None
 */
#define         timer5_start()          set_bit( T5RUN )
/**
 * Stop timer4
 *
 * @param       -
 * @return      None
 */
#define         timer4_stop()           set_bit( T4STP )
/**
 * Stop timer5
 *
 * @param       -
 * @return      None
 */
#define         timer5_stop()           set_bit( T5STP )

void            timer4_setCnt( unsigned short cnt );
void            timer5_setCnt( unsigned short cnt );

#endif /*_TIMER4_5_H_*/

