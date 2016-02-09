/* FILE NAME: LedDotMatrix.h
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

#ifndef _LEDDOTMATRIX_H_
#define  _LEDDOTMATRIX_H_

#include "lazurite.h"

typedef struct {
	bool (*init)(void);
	void (*setMemory)(uint8_t *up, uint16_t up_size, uint8_t *lo, uint16_t lo_size);
	void (*setFlash)(uint8_t up_sector, uint16_t up_offset, uint16_t up_size, uint8_t lo_sector,uint16_t lo_offset, uint16_t lo_size);
	void (*shift)(int speed, bool up_shift, bool lo_shift);
} LED_DOT_MATRIX;

extern const LED_DOT_MATRIX LedDotMatrix;

#endif //_LEDDOTMATRIX_H_

