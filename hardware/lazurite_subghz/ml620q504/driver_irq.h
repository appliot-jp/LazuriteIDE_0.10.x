/* FILE NAME: driver_irq.h
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

#ifndef _DRIVER_IRQ_H_
#define _DRIVER_IRQ_H_

#include "irq.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define IRQ_R_OK				(  0 )
#define IRQ_R_ERR_INTNO			( -1 )

#define  IRQ_NO_WDTINT		( 0 )
#define  IRQ_NO_EXI0INT		( 1 )
#define  IRQ_NO_EXI1INT		( 2 )
#define  IRQ_NO_EXI2INT		( 3 )
#define  IRQ_NO_EXI3INT		( 4 )
#define  IRQ_NO_EXI4INT		( 5 )
#define  IRQ_NO_EXI5INT		( 6 )
#define  IRQ_NO_EXI6INT		( 7 )
#define  IRQ_NO_EXI7INT		( 8 )
#define  IRQ_NO_SIO0INT		( 9 )
#define  IRQ_NO_SIOF0INT	( 10 )
#define  IRQ_NO_I2C0INT		( 11 )
#define  IRQ_NO_I2C1INT		( 12 )
#define  IRQ_NO_UA0INT		( 13 )
#define  IRQ_NO_UA1INT		( 14 )
#define  IRQ_NO_UAF0INT		( 15 )
#define  IRQ_NO_LOSCINT		( 16 )
#define  IRQ_NO_VLSINT		( 17 )
#define  IRQ_NO_MD0INT		( 18 )
#define  IRQ_NO_SADINT		( 19 )
#define  IRQ_NO_RADINT		( 20 )
#define  IRQ_NO_CMP0INT		( 21 )
#define  IRQ_NO_CMP1INT		( 22 )
#define  IRQ_NO_TM0INT		( 23 )
#define  IRQ_NO_TM1INT		( 24 )
#define  IRQ_NO_TM2INT		( 25 )
#define  IRQ_NO_TM3INT		( 26 )
#define  IRQ_NO_TM4INT		( 27 )
#define  IRQ_NO_TM5INT		( 28 )
#define  IRQ_NO_TM6INT		( 29 )
#define  IRQ_NO_TM7INT		( 30 )
#define  IRQ_NO_FTM0INT		( 31 )
#define  IRQ_NO_FTM1INT		( 32 )
#define  IRQ_NO_FTM2INT		( 33 )
#define  IRQ_NO_FTM3INT		( 34 )
#define  IRQ_NO_LTBC0INT	( 35 )
#define  IRQ_NO_LTBC1INT	( 36 )
#define  IRQ_NO_LTBC2INT	( 37 )




/*############################################################################*/
/*#                                  Extern                                   #*/
/*############################################################################*/
//*******************************************************************************
//	Routine Name:	irq_init
//	Form:			void irq_init(void);
//	Parameters:		void
//	Return value:	void
//	Description:	initialization of interrupt.
//******************************************************************************/
extern void irq_init(void);

//*******************************************************************************
//	Routine Name:	irq_sethandler
//	Form:			int irq_sethandler( unsigned char intNo, void (*func)( void ) );
//	Parameters:		intNo: interrupt no, such as IRQ_NO_FTM0INT, IRQ_NO_FTM0INT and so on.
//					func: function pointer to be called when interrupt.
//	Return value:	void
//	Description:	initialization of interrupt.
//******************************************************************************/
extern int irq_sethandler( unsigned char intNo, void (*func)( void ) );
extern void _intNullHdr(void);

#define 	DI_GPIO			0x0001
#define 	DI_UART			0x0002
#define 	DI_UARTF		0x0004
#define 	DI_SUBGHZ		0x0008
#define 	DI_MILLIS		0x0010
#define		DI_WIRING_PULSE	0x0020
#define		DI_USER			0x0040
#define		DI_INTERRUPT	0x0080
#define		DI_DFLASH		0x0100
extern void enb_interrupts(unsigned short irq_ch);
extern void dis_interrupts(unsigned short irq_ch);
extern void rst_interrupts(void);

#endif /* _DRIVER_GPIO_H_ */
