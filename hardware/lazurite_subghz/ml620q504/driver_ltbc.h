/* FILE NAME: driver_ltbc.h
 *
 * Copyright (c) 2020  Lapis Semiconductor Co.,Ltd.
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

#ifndef _DRIVER_LTBC_H_
#define _DRIVER_LTBC_H_

#include "common.h"
#include "driver_irq.h"

extern void ltbc_init(void);
extern unsigned short ltbc_get_count(void);
extern void ltbc_attach_handler(unsigned char num, unsigned short expire, unsigned short (*func)(unsigned short count));
extern void ltbc_detach_handler(unsigned char num);

#endif  /* _DRIVER_LTBC_H_ */
