/* FILE NAME: driver_pin_assignment.h
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

#ifndef _DRIVER_PIN_ASSIGNMENT_H_
#define _DRIVER_PIN_ASSIGNMENT_H_

#include "mcu.h"

#ifdef SUBGHZ_OTA
	extern unsigned char *ml620504f_pin_to_port();
#else
	extern const unsigned char *ml620504f_pin_to_port[];
#endif
extern const unsigned char ml620504f_pin_to_bit[];
extern const unsigned char ml620504f_ain_to_pin[];
extern const unsigned char ml620504f_tmout_to_pin[];
extern const char num_to_bit[];



#define	ML620504F_MAX_PIN_NO	35
#define ML620504F_MAX_AIN_NO	11
#define ML620504F_MAX_TMOUT_NO	15
#define ML620504F_MAX_FTM_NO	7
#define ML620504F_MAX_EXTIRQ_NO	7

#endif //_DRIVER_PIN_ASSIGNMENT_H_

