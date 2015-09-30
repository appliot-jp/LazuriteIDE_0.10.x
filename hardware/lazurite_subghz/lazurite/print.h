/* FILE NAME: print.c
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

#ifndef _PRINT_H_
#define _PRINT_H_

#include "common.h"
#include "lazurite.h"
#include "mcu.h"

typedef struct {
	void (*init)(char* buf, unsigned short size);
	int (*p)(char* data);
	int (*l)(long data, unsigned char base);
	int (*f)(float data, unsigned char digit);
	int (*d)(double data, unsigned char digit);
	int (*ln)(void);
	int (*status)(void);
	int (*len)(void);
} t_print;

extern const t_print Print;

extern size_t printFloat(char* x, double data, unsigned char digit);
extern size_t printNumber(char * x, long data, unsigned char base);

#endif
