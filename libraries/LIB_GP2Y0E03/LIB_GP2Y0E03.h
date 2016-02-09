/* FILE NAME: LIB_GP2Y0E03.h
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

#ifndef _GP2Y0E03_H_
#define _GP2Y0E03_H_

#include "lazurite.h"

typedef struct {
	void (*init)(unsigned char i2c_addr);
	unsigned char (*measure)(void);
	void (*meas_mode)(unsigned char mode);
	void (*stby_mode)(bool mode);
} tGP2Y0E03;

extern tGP2Y0E03 gp2y0e03;

#endif // _GP2Y0E03_H_
