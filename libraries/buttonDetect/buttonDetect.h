/* FILE NAME: buttonDetect.h
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


#ifndef _BUTTONDETECT_H_
#define _BUTTONDETECT_H_

#include "lazurite.h"

#define PUSH 1
#define RELEASE 2

struct button {
	uint8_t type;		// 1: push , 2 release
	uint8_t pin;		// GPIO
	uint8_t extirq;		// interrupt number 0,1,2,4,5,6,7
	uint8_t state;		// fixed to 0(internal use)
	uint8_t count;		// fixed to 0(internal use)
};
struct button_config {
	uint8_t num;		// button設定の数
	uint8_t event;		// eventの有無
	uint8_t wait_flag;			// eventの有無
	uint8_t chat;		// chattering検出を行うtimer_unitの数
	uint8_t cont;		// ボタン長押し検出時間
	uint8_t cont_interval;	//ボタン長押し検出後のボタン検出時間
	uint16_t timer_unit;	//timerの設定値
	struct button* btn;
};

extern void buttonDetect(struct button_config* params);

#endif
