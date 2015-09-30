/* FILE NAME: pin_assignment.h
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


#ifndef _PIN_ASIGNMENT_H_
#define _PIN_ASIGNMENT_H_

//********************************************************************************
//   global definitions
//********************************************************************************

// analog pins are assgined as digital pins

#define SS				10
#define	A0				14
#define	A1				15
#define	A2				16
#define	A3				17
#define	A4				18
#define	A5				19
#define	FLASH_CSB		20
#define	NFC_RFDET		21
#define	NFC_IRQ			22
#define	SDA0			23
#define	SCL0			24
#define	LED_NFC			25
#define	LED_BLE			26
#define	MOSI0			28
#define	MISO0			29
#define	RF_CSB			30
#define	SCLK0			31
#define	RF_P0			32
#define	RF_P1			33
#define	RF_IRQ			34
#define	RF_RSTB			35

extern const unsigned char digital_pin_to_port[];
extern const unsigned char analog_pin_to_port[];
extern const unsigned char pin_to_pwm[];
extern const unsigned char pwm_to_ftm[];

#define MAX_PIN_NO	35
#define MAX_AIN_NO	5


#endif // _PIN_ASIGNMENT_H_

