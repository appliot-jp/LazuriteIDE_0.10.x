/* FILE NAME: digitalio.c
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


#include "common.h"
#include "lazurite.h"
#include "driver_gpio.h"
#include "pin_assignment.h"

//********************************************************************************
//   local definitions
//********************************************************************************
//********************************************************************************
//   local parameters
//********************************************************************************
const unsigned char digital_pin_to_port[MAX_PIN_NO+1]=
{
	12,	//	0			P30
	13,	//	1			P31
	28,	//	2			P50
	31,	//	3			P53
	22,	//	4			P42
	23,	//	5			P43
	14,	//	6			P32
	15,	//	7			P33
	35,	//	8			P57
	30,	//	9			P52
	19,	//	10			P37
	24,	//	11			P44
	25,	//	12			P45
	18,	//	13			P36
	8,	//	AIN0		P20		14
	9,	//	AIN1		P21		15
	10,	//	AIN2		P22		16
	3,	//	AIN3		P03		17
	16,	//	AIN4		P34		18
	17,	//	AIN5		P35		19
	11,	//	FLASH_CSB	P23		20
	32,	//	NFC_RFDET	P54		21
	33,	//	NFC_IRQ		P55		22
	20,	//	SDA0		P40		23
	21,	//	SCL0		P41		24
	29,	//	LED_NFC		P51		25
	34,	//	LED_BLE		P56		26
	26,	//	NC			P46		27
	0,	//	MOSI0		P00		28
	1,	//	MISO0		P01		29
	6,	//	RF_CSB		P10		30
	2,	//	SCLK0		P02		31
	5,	//	RF_P0		P05		32
	4,	//	RF_P1		P04		33
	27,	//	RF_IRQ		P47		34
	7,	//	RF_RSTB		P11		35
};
//********************************************************************************
//   local function definitions
//********************************************************************************
//********************************************************************************
//   functions
//********************************************************************************
volatile void pinMode(uint8_t pin, uint8_t mode)
{
	if(pin > MAX_PIN_NO) return;
	drv_pinMode(digital_pin_to_port[pin], mode);
	return;
}

int digitalRead(uint8_t pin)
{
	if(pin > MAX_PIN_NO) return(LOW);
	return drv_digitalRead(digital_pin_to_port[pin]);
}

void digitalWrite(uint8_t pin, uint8_t val)
{
	if(pin > MAX_PIN_NO) return;
	drv_digitalWrite(digital_pin_to_port[pin], val);
	return;
}


