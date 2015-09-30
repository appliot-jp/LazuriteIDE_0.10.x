/* FILE NAME: ML8511.h
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

#ifndef _ML8511_H_
#define _ML8511_H_
#include "lazurite.h"

// ƒNƒ‰ƒX‚Ì’è‹`
typedef struct 
{
	void (*init)(int ana_pin);
	void (*get_val)(float *data);
} t_ML8511;

extern const t_ML8511 ml8511;

#endif // _ML8511_H_
