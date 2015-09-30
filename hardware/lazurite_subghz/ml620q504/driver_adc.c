/* FILE NAME: driver_adc.c
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
#include "driver_adc.h"
#include "driver_pin_assignment.h"
#include "mcu.h"
#include "rdwr_reg.h"

//********************************************************************************
//   global parameters
//********************************************************************************
const unsigned char ml620504f_ain_to_pin[] = 
{
	16,	//	AIN0	P34
	17,	//	AIN1	P35
	18,	//	AIN2	P36
	19,	//	AIN3	P37
	8,	//	AIN4	P20
	9,	//	AIN5	P21
	10,	//	AIN6	P22
	11,	//	AIN7	P23
	0,	//	AIN8	P00
	1,	//	AIN9	P01
	2,	//	AIN10	P02
	3,	//	AIN11	P03
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

//--------------------------------------------------------------------------------
//   int analogRead(uint8_t pin)
//   parameters:
//       pin = 0 ~ 11  0=AIN0, 1=AIN1, ... 11=AIN
//   return:
//       12bit ADC result
//--------------------------------------------------------------------------------

int drv_analogRead(unsigned char ain)
{
	volatile int res = -1;

	if(ain > ML620504F_MAX_AIN_NO) return res;
	
	clear_bit(DSAD);						// Power ON ADC
	// set clock
	write_reg8(SADCON0,0x22);				// 1/4 OSCLK
	
	drv_pinMode(ml620504f_ain_to_pin[ain],HIZ);			// set GPIO to analog read mode
	
	// set ADC settings
	write_reg16(SADTCH,0x0000);				// does not use as touch ADC
	write_reg16(SADEN,((UINT16)1)<<ain);	// set analog switch
	write_reg16(SADCVT,0x0378);				// set accuracy??
	
	set_bit(SARUN);							// start ADC
	
	while(get_bit(SARUN) == 0x01){}			// wait AD convertion
	
	res = *(&SADR0 + ain);					// get result
	
	set_bit(DSAD);							// Power down ADC
	
	return res;
}
