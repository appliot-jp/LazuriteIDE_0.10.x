/* FILE NAME: SerialLED.h
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


#ifndef _SERIAL_LED_H_
#define _SERIAL_LED_H_

#include "lazurite.h"

#define BLUE	0x00,0x00,0xff
#define BLACK	0x00,0x00,0x00
#define RED		0x00,0xff,0x00
#define GREEN	0xff,0x00,0x00
#define WHITE	0xff,0xff,0xff


typedef struct {
	void (*init)(unsigned char pin, unsigned short len);
	void (*write)(unsigned char *data);
}t_SERIAL_LED;

extern const t_SERIAL_LED led;


#endif // _SERIAL_LED_H_

