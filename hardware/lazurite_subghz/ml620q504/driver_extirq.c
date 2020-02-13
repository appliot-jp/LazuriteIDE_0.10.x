/* FILE NAME: driver_extirq.c
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

#ifdef SUBGHZ_OTA
	#pragma SEGCODE "OTA_SEGCODE"
	#pragma SEGINIT "OTA_SEGINIT"
	#pragma SEGNOINIT "OTA_SEGNOINIT"
	#pragma SEGCONST "OTA_SEGCONST"
#endif
#include "common.h"
#include "lazurite.h"
#include "driver_pin_assignment.h"
#include "driver_extirq.h"
#include "driver_irq.h"
#include "driver_gpio.h"
#include "mcu.h"
#include "rdwr_reg.h"

//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   local definitions
//********************************************************************************
//********************************************************************************
//   local parameters
//********************************************************************************
void isr_ext_irq0(void);
void isr_ext_irq1(void);
void isr_ext_irq2(void);
void isr_ext_irq3(void);
void isr_ext_irq4(void);
void isr_ext_irq5(void);
void isr_ext_irq6(void);
void isr_ext_irq7(void);


EXT_IRQ_PARAM ext_irq_param[8];

const unsigned char ml620504f_exisel[ML620504F_MAX_PIN_NO+1] =
{
	0x00,	//	P00
	0x01,	//	P01
	0x02,	//	P02
	0x03,	//	P03
	0x04,	//	P04
	0x05,	//	P05
	0x10,	//	P10
	0x11,	//	P11
	0x20,	//	P20
	0x21,	//	P21
	0x22,	//	P22
	0x23,	//	P23
	0x30,	//	P30
	0x31,	//	P31
	0x32,	//	P32
	0x33,	//	P33
	0x34,	//	P34
	0x35,	//	P35
	0x36,	//	P36
	0x37,	//	P37
	0x40,	//	P40
	0x41,	//	P41
	0x42,	//	P42
	0x43,	//	P43
	0x44,	//	P44
	0x45,	//	P45
	0x46,	//	P46
	0x47,	//	P47
	0x50,	//	P50
	0x51,	//	P51
	0x52,	//	P52
	0x53,	//	P53
	0x54,	//	P54
	0x55,	//	P55
	0x56,	//	P56
	0x57,	//	P57
};

//********************************************************************************
//   local function declaration
//********************************************************************************
//********************************************************************************
//   local functions
//********************************************************************************

void isr_ext_irq(unsigned char irqnum)
{
	unsigned long start_time = 0;
	volatile uint8_t pinData;
	
	while(1)
	{
		ext_irq_param[irqnum].func();
		if(ext_irq_param[irqnum].mode != LOW) {
			break;
		} else {
			pinData = drv_digitalRead(ext_irq_param[irqnum].pin);
			if(pinData != LOW) {
				break;
			}
		}
		if(start_time == 0) {
			start_time = millis();
		} else {
			unsigned long now = millis();
			if((now - start_time)>1000) {
				__asm("brk\n");
			}
		}
	}
}

void isr_ext_irq0(void) {isr_ext_irq(0);}
void isr_ext_irq1(void) {isr_ext_irq(1);}
void isr_ext_irq2(void) {isr_ext_irq(2);}
void isr_ext_irq3(void) {isr_ext_irq(3);}
void isr_ext_irq4(void) {isr_ext_irq(4);}
void isr_ext_irq5(void) {isr_ext_irq(5);}
void isr_ext_irq6(void) {isr_ext_irq(6);}
void isr_ext_irq7(void) {isr_ext_irq(7);}

void drv_attachInterrupt(unsigned char pin,unsigned char irqnum, void (*func)(void), int mode,bool sampling, bool filter)
{
	unsigned char* exisel;
	// error check
	if(pin > ML620504F_MAX_PIN_NO) return;
	if(func == NULL) return;
	if(irqnum > ML620504F_MAX_EXTIRQ_NO) return;

	exisel = &EXI0SEL;

	IE1 &= ~num_to_bit[irqnum];			// disenable IRQ
	IRQ1 &= ~num_to_bit[irqnum];		// clear IRQ

	ext_irq_param[irqnum].pin = pin;
	ext_irq_param[irqnum].func = func;
	ext_irq_param[irqnum].mode = mode;


	// set interrupt handler
	switch(irqnum)
	{
		case 0:
			irq_sethandler(IRQ_NO_EXI0INT,isr_ext_irq0);	// set callback function
			break;
		case 1:
			irq_sethandler(IRQ_NO_EXI1INT,isr_ext_irq1);	// set callback function
			break;
		case 2:
			irq_sethandler(IRQ_NO_EXI2INT,isr_ext_irq2);	// set callback function
			break;
		case 3:
			irq_sethandler(IRQ_NO_EXI3INT,isr_ext_irq3);	// set callback function
			break;
		case 4:
			irq_sethandler(IRQ_NO_EXI4INT,isr_ext_irq4);	// set callback function
			break;
		case 5:
			irq_sethandler(IRQ_NO_EXI5INT,isr_ext_irq5);	// set callback function
			break;
		case 6:
			irq_sethandler(IRQ_NO_EXI6INT,isr_ext_irq6);	// set callback function
			break;
		case 7:
			irq_sethandler(IRQ_NO_EXI7INT,isr_ext_irq7);	// set callback function
			break;
		default:
			break;
	}

	*(exisel+irqnum) = ml620504f_exisel[pin];						// set port number for ext irq.

	(sampling) ? (EXICON2 |= num_to_bit[irqnum]) : (EXICON2 &= ~num_to_bit[irqnum]);	// seting of sampling
	(filter) ? (EXICON3 |= num_to_bit[irqnum]) : (EXICON3 &= ~num_to_bit[irqnum]);		// setting of filter

	switch(mode)
	{
		case LOW:		// FALLING EDGE
			EXICON0 |= num_to_bit[irqnum];
			EXICON1 &= ~num_to_bit[irqnum];
			if(drv_digitalRead(pin) == LOW)
			{
				IRQ1 |= num_to_bit[irqnum];							// if pin is already LOW, set trigger of interrupt;
			}
			break;
		case RISING:	// RISING EDGE
			EXICON0 &= ~num_to_bit[irqnum];
			EXICON1 |= num_to_bit[irqnum];
			break;
		case FALLING:	// FALLING EDGE
			EXICON0 |= num_to_bit[irqnum];
			EXICON1 &= ~num_to_bit[irqnum];
			break;
		case CHANGE:	// BOTH EDGE
			EXICON0 |= num_to_bit[irqnum];
			EXICON1 |= num_to_bit[irqnum];
			break;
		default:
			break;
	}

	IE1 |= num_to_bit[irqnum];			// disenable IRQ

}


void drv_detachInterrupt(unsigned char irqnum)
{
	IE1 &= ~num_to_bit[irqnum];			// disenable IRQ
	IRQ1 &= ~num_to_bit[irqnum];			// disenable IRQ
	EXICON0 &= ~num_to_bit[irqnum];		// clear extirq condition
	EXICON1 &= ~num_to_bit[irqnum];		// clear extirq condition
	irq_sethandler((unsigned char)(IRQ_NO_EXI0INT+irqnum),_intNullHdr);	// set callback function
}

