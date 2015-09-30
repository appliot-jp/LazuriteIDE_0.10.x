/* FILE NAME: HardwareServo.h
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

#ifndef _HARDWARESERVO_H_
#define _HARDWARESERVO_H_

typedef struct {
	bool (*init)(unsigned char num, unsigned short period);
	bool (*attach)(unsigned char ch, unsigned char pin);
	bool (*detach)(unsigned char pin,unsigned char level);
	void (*start)(void);
	void (*stop)(void);
	void (*close)(void);
	void (*write)(unsigned char ch, unsigned short time);
	void (*dt)(unsigned char ch, unsigned short deadtime);
	void (*update)(void);
} HardwareServo;

extern const HardwareServo hsv;

#endif // _HARDWARESERVO_H_
