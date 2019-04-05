/* FILE NAME: FeRAM.h
 *
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
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

#ifndef _LAZUTITE_FERAM_H_
#define _LAZUTITE_FERAM_H_

#include "lazurite.h"

//********************************************************************************
//   global definitions
//********************************************************************************
//********************************************************************************
//   global parameters
//********************************************************************************
typedef struct {
	void (*begin)(uint8_t cs, uint8_t wp, uint8_t hold);
	void (*write)(uint32_t addr, uint8_t data_write);
	uint8_t (*read)(uint32_t addr);
	void (*burstWrite)(uint32_t addr, uint8_t *data_write_ptr, uint32_t total_data_count);
	void (*burstRead)(uint32_t addr, uint8_t *data_read_ptr, uint32_t total_data_count );
	void (*statusRegWrite)(uint8_t data_write);
	uint8_t (*statusRegRead)(void);
} FERAMClass;

//********************************************************************************
//   extern function definitions
//********************************************************************************
extern const FERAMClass FeRAM;

#endif // _LAZUTITE_FERAM_H_

