/* FILE NAME: rst.h
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

#ifndef _RST_H_
#define _RST_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* RSTAT */
#define RSTAT_POR           ( 0x01 )        /**< RSTAT Register POR bit       */
#define RSTAT_WDTR          ( 0x04 )        /**< RSTAT Register WDTR bit      */
#define RSTAT_VLSR          ( 0x08 )        /**< RSTAT Register VLSR bit      */
#define RSTAT_LLDR          ( 0x10 )        /**< RSTAT Register LLDR bit      */

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Get reset causes
 * 
 * Get causes which system-reset is generated.
 *
 * @param       -
 * @return      The value of RSTAT register
 * @see         MCU Users manual 'Reset Function'
 */

#define         rst_getResetCause()             read_reg8( RSTAT )

#endif /*_RST_H_*/

