/* FILE NAME: timer0_1.h
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

#ifndef _TIMER0_1_H_
#define _TIMER0_1_H_

#include "rdwr_reg.h"
#include "timer_common.h"

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize timer0
 *
 * @param[in]   tmncon  A value of setting TM0CON register. <br>
 *                      Specify the logical sum of the following items.
 * @arg                     Clock source      : TM_CS_LSCLK, or TM_CS_OSCLK, or TM_CS_EXTCLK
 * @arg                     Division ratio    : TM_DIV1, or TM_DIV4, or TM_DIV8, or TM_DIV16, or TM_DIV32, or TM_DIV64
 * @arg                     Timer             : TM_MODE_8BIT, or TM_MODE_16BIT
 * @arg                     Mode              : TM_OST_REROAD, or TM_OST_ONESHOT
 * @return      None
 */
#define         timer0_init( tmncon )   write_reg8( TM0CON, tmncon )
/**
 * Initialize timer1
 *
 * @param[in]   tmncon  A value of setting TM1CON register. <br>
 *                      Specify the logical sum of the following items.
 * @arg                     Clock source      : TM_CS_LSCLK, or TM_CS_OSCLK, or TM_CS_EXTCLK
 * @arg                     Division ratio    : TM_DIV1, or TM_DIV4, or TM_DIV8, or TM_DIV16, or TM_DIV32, or TM_DIV64
 * @arg                     Timer             : Only TM_MODE_8BIT
 * @arg                     Mode              : TM_OST_REROAD, or TM_OST_ONESHOT
 * @return      None
 */
#define         timer1_init( tmncon )   write_reg8( TM1CON, tmncon )
/**
 * Start timer0
 *
 * @param       -
 * @return      None
 */
#define         timer0_start()          set_bit( T0RUN )
/**
 * Start timer1
 *
 * @param       -
 * @return      None
 */
#define         timer1_start()          set_bit( T1RUN )
/**
 * Stop timer0
 *
 * @param       -
 * @return      None
 */
#define         timer0_stop()           set_bit( T0STP )
/**
 * Stop timer1
 *
 * @param       -
 * @return      None
 */
#define         timer1_stop()           set_bit( T1STP )

void            timer0_setCnt( unsigned short cnt );
void            timer1_setCnt( unsigned short cnt );

#endif /*_TIMER0_1_H_*/

