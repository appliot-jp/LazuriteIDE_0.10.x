/* FILE NAME: driver_gpio.c
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
#include "driver_gpio.h"
#include "driver_pin_assignment.h"
#include "driver_irq.h"
#include "mcu.h"
#include "lazurite.h"

//********************************************************************************
//   global parameters
//********************************************************************************
const unsigned char *ml620504f_pin_to_port[] =
{
	&P0D,	//	0	P00
	&P0D,	//	1	P01
	&P0D,	//	2	P02
	&P0D,	//	3	P03
	&P0D,	//	4	P04
	&P0D,	//	5	P05
	&P1D,	//	6	P10
	&P1D,	//	7	P11
	&P2D,	//	8	P20
	&P2D,	//	9	P21
	&P2D,	//	10	P22
	&P2D,	//	11	P23
	&P3D,	//	12	P30
	&P3D,	//	13	P31
	&P3D,	//	14	P32
	&P3D,	//	15	P33
	&P3D,	//	16	P34
	&P3D,	//	17	P35
	&P3D,	//	18	P36
	&P3D,	//	19	P37
	&P4D,	//	20	P40
	&P4D,	//	21	P41
	&P4D,	//	22	P42
	&P4D,	//	23	P43
	&P4D,	//	24	P44
	&P4D,	//	25	P45
	&P4D,	//	26	P46
	&P4D,	//	27	P47
	&P5D,	//	28	P50
	&P5D,	//	29	P51
	&P5D,	//	30	P52
	&P5D,	//	31	P53
	&P5D,	//	32	P54
	&P5D,	//	33	P55
	&P5D,	//	34	P56
	&P5D,	//	35	P57
};

const unsigned char ml620504f_pin_to_bit[] =
{
	0x01,	//	P00
	0x02,	//	P01
	0x04,	//	P02
	0x08,	//	P03
	0x10,	//	P04
	0x20,	//	P05
	0x01,	//	P10
	0x02,	//	P11
	0x01,	//	P20
	0x02,	//	P21
	0x04,	//	P22
	0x08,	//	P23
	0x01,	//	P30
	0x02,	//	P31
	0x04,	//	P32
	0x08,	//	P33
	0x10,	//	P34
	0x20,	//	P35
	0x40,	//	P36
	0x80,	//	P37
	0x01,	//	P40
	0x02,	//	P41
	0x04,	//	P42
	0x08,	//	P43
	0x10,	//	P44
	0x20,	//	P45
	0x40,	//	P46
	0x80,	//	P47
	0x01,	//	P50
	0x02,	//	P51
	0x04,	//	P52
	0x08,	//	P53
	0x10,	//	P54
	0x20,	//	P55
	0x40,	//	P56
	0x80,	//	P57
};

//********************************************************************************
//   local definitions
//********************************************************************************
//********************************************************************************
//   local parameters
//********************************************************************************
//********************************************************************************
//   local function declaration
//********************************************************************************
//********************************************************************************
//   local functions
//********************************************************************************

volatile void drv_pinMode(unsigned char pin, unsigned char mode)
{
	UCHAR *port;
	UCHAR bit;
	if(pin > ML620504F_MAX_PIN_NO) 	return;
	
	dis_interrupts(DI_GPIO);
	
	bit = ml620504f_pin_to_bit[pin];
	port = ml620504f_pin_to_port[pin];
	switch(mode)
	{
	case INPUT_PULLUP:
		*(port+1) |= bit;		// PnDIR  = 1
		*(port+2) &= ~bit;		// PnCON0 = 0
		*(port+3) |= bit;		// PnCON1 = 1
		*(port+4) &= ~bit;		// PnMOD0 = 0
		*(port+5) &= ~bit;		// PnMOD1 = 0
		break;
	case INPUT_PULLDOWN:
		*(port+1) |= bit;		// PnDIR  = 1
		*(port+2) |= bit;		// PnCON0 = 1
		*(port+3) &= ~bit;		// PnCON1 = 0
		*(port+4) &= ~bit;		// PnMOD0 = 0
		*(port+5) &= ~bit;		// PnMOD1 = 0
		break;
	case INPUT:
		*(port+1) |= bit;		// PnDIR  = 1
		*(port+2) &= ~bit;		// PnCON0 = 0
		*(port+3) &= ~bit;		// PnCON1 = 0
		*(port+4) &= ~bit;		// PnMOD0 = 0
		*(port+5) &= ~bit;		// PnMOD1 = 0
		break;
	case OUTPUT:
		*(port+1) &= ~bit;		// PnDIR  = 0
		*(port+2) |= bit;		// PnCON0 = 1
		*(port+3) |= bit;		// PnCON1 = 1
		*(port+4) &= ~bit;		// PnMOD0 = 0
		*(port+5) &= ~bit;		// PnMOD1 = 0
		break;
	case HIZ:
		*(port+1) &= ~bit;		// PnDIR  = 0
		*(port+2) &= ~bit;		// PnCON0 = 0
		*(port+3) &= ~bit;		// PnCON1 = 0
		*(port+4) &= ~bit;		// PnMOD0 = 0
		*(port+5) &= ~bit;		// PnMOD1 = 0
		break;
	case OPEN_DRAIN:
		*(port+1) &= ~bit;		// PnDIR  = 0
		*(port+2) &= ~bit;		// PnCON0 = 0
		*(port+3) |= bit;		// PnCON1 = 1
		*(port+4) &= ~bit;		// PnMOD0 = 0
		*(port+5) &= ~bit;		// PnMOD1 = 0
		break;
	default:
		break;
	}
	enb_interrupts(DI_GPIO);
}

int drv_digitalRead(unsigned char pin)
{
	UCHAR *port;
	UCHAR bit;
	int res;
	if(pin > ML620504F_MAX_PIN_NO) 	return(LOW);
	
//	dis_interrupts(DI_GPIO);
	
	bit = ml620504f_pin_to_bit[pin];
	port = ml620504f_pin_to_port[pin];
	res = ((*port)&bit)!=0 ? HIGH : LOW;
	
//	enb_interrupts(DI_GPIO);
	
	return(res);
	
}

void drv_digitalWrite(unsigned char pin, unsigned char val)
{
	UCHAR *port;
	UCHAR bit;
	
	if(pin > ML620504F_MAX_PIN_NO) 	return;
	
//	dis_interrupts(DI_GPIO);
	
	bit = ml620504f_pin_to_bit[pin];
	port = ml620504f_pin_to_port[pin];
	if(val == LOW)
	{
		*(port) &= ~bit;		
	}
	else
	{
		*(port) |= bit;		
	}
	
//	enb_interrupts(DI_GPIO);
	
	return;
}


