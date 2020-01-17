/* FILE NAME: driver_irq.c
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
	#pragma SEGINTR "OTA_SEGCODE"
#endif
#include "mcu.h"
#include "rdwr_reg.h"
#include "driver_irq.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define IRQ_SIZE 38								// Number of IRQ

/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
static void ( *_sIrqHdr[IRQ_SIZE] )( void );	// Function for interrupt
void _intNullHdr( void );
unsigned short di_flag = 0;

/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
static void s_handlerWDTINT( void );
static void s_handlerEXI0INT( void );
static void s_handlerEXI1INT( void );
static void s_handlerEXI2INT( void );
static void s_handlerEXI3INT( void );
static void s_handlerEXI4INT( void );
static void s_handlerEXI5INT( void );
static void s_handlerEXI6INT( void );
static void s_handlerEXI7INT( void );
static void s_handlerSIO0INT( void );
static void s_handlerSIOF0INT( void );
static void s_handlerI2C0INT( void );
static void s_handlerI2C1INT( void );
static void s_handlerUA0INT( void );
static void s_handlerUA1INT( void );
static void s_handlerUAF0INT( void );
static void s_handlerLOSCINT( void );
static void s_handlerVLSINT( void );
static void s_handlerMD0INT( void );
static void s_handlerSADINT( void );
static void s_handlerRADINT( void );
static void s_handlerCMP0INT( void );
static void s_handlerCMP1INT( void );
static void s_handlerTM0INT( void );
static void s_handlerTM1INT( void );
static void s_handlerTM2INT( void );
static void s_handlerTM3INT( void );
static void s_handlerTM4INT( void );
static void s_handlerTM5INT( void );
static void s_handlerTM6INT( void );
static void s_handlerTM7INT( void );
static void s_handlerFTM0INT( void );
static void s_handlerFTM1INT( void );
static void s_handlerFTM2INT( void );
static void s_handlerFTM3INT( void );
static void s_handlerLTBC0INT( void );
static void s_handlerLTBC1INT( void );
static void s_handlerLTBC2INT( void );

/*=== set Interrupt Vector ===*/
/* If enables multiple interrupts,              */
/* specify '2' in the INTERRUPT category field. */
#pragma INTERRUPT   s_handlerWDTINT     0x0008  1
#pragma INTERRUPT   s_handlerEXI0INT    0x0010  1
#pragma INTERRUPT   s_handlerEXI1INT    0x0012  1
#pragma INTERRUPT   s_handlerEXI2INT    0x0014  1
#pragma INTERRUPT   s_handlerEXI3INT    0x0016  1
#pragma INTERRUPT   s_handlerEXI4INT    0x0018  1
#pragma INTERRUPT   s_handlerEXI5INT    0x001A  1
#pragma INTERRUPT   s_handlerEXI6INT    0x001C  1
#pragma INTERRUPT   s_handlerEXI7INT    0x001E  1
#pragma INTERRUPT   s_handlerSIO0INT    0x0020  1
#pragma INTERRUPT   s_handlerSIOF0INT   0x0022  1
#pragma INTERRUPT   s_handlerI2C0INT    0x0024  1
#pragma INTERRUPT   s_handlerI2C1INT    0x0026  1
#pragma INTERRUPT   s_handlerUA0INT     0x0028  1
#pragma INTERRUPT   s_handlerUA1INT     0x002A  1
#pragma INTERRUPT   s_handlerUAF0INT    0x002C  1
#pragma INTERRUPT   s_handlerLOSCINT    0x003A  1
#pragma INTERRUPT   s_handlerVLSINT     0x003C  1
#pragma INTERRUPT   s_handlerMD0INT     0x003E  1
#pragma INTERRUPT   s_handlerSADINT     0x0040  1
#pragma INTERRUPT   s_handlerRADINT     0x0042  1
#pragma INTERRUPT   s_handlerCMP0INT    0x0048  1
#pragma INTERRUPT   s_handlerCMP1INT    0x004A  1
#pragma INTERRUPT   s_handlerTM0INT     0x0050  1
#pragma INTERRUPT   s_handlerTM1INT     0x0052  1
#pragma INTERRUPT   s_handlerTM2INT     0x0054  1
#pragma INTERRUPT   s_handlerTM3INT     0x0056  1
#pragma INTERRUPT   s_handlerTM4INT     0x0058  1
#pragma INTERRUPT   s_handlerTM5INT     0x005A  1
#pragma INTERRUPT   s_handlerTM6INT     0x005C  1
#pragma INTERRUPT   s_handlerTM7INT     0x005E  1
#pragma INTERRUPT   s_handlerFTM0INT    0x0060  1
#pragma INTERRUPT   s_handlerFTM1INT    0x0062  1
#pragma INTERRUPT   s_handlerFTM2INT    0x0064  1
#pragma INTERRUPT   s_handlerFTM3INT    0x0066  1
#pragma INTERRUPT   s_handlerLTBC0INT   0x0070  1
#pragma INTERRUPT   s_handlerLTBC1INT   0x0072  1
#pragma INTERRUPT   s_handlerLTBC2INT   0x0074  1

/*--- Handler table. ---*/
//static void _intNullHdr( void );



/*############################################################################*/
/*#                          Interrupt handler                               #*/
/*############################################################################*/

/*******************************************************************************
	Routine Name:	_intNullHdr
	Form:			static void _intNullHdr( void )
	Parameters:		void
	Return value:	void
	Description:	NULL Handler.
******************************************************************************/
void _intNullHdr( void )
{
	return;
}

static void s_handlerWDTINT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by WDTINT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_WDTINT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerEXI0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by EXI0INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_EXI0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerEXI1INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by EXI1INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_EXI1INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerEXI2INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by EXI2INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_EXI2INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerEXI3INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by EXI3INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_EXI3INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerEXI4INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by EXI4INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_EXI4INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerEXI5INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by EXI5INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_EXI5INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerEXI6INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by EXI6INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_EXI6INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerEXI7INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by EXI7INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_EXI7INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerSIO0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by SIO0INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_SIO0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerSIOF0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by SIOF0INT,       */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_SIOF0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerI2C0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by I2C0INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_I2C0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerI2C1INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by I2C1INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_I2C1INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerUA0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by UA0INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_UA0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerUA1INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by UA1INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_UA1INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerUAF0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by UAF0INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_UAF0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerLOSCINT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by LOSCINT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_LOSCINT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerVLSINT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by VLSINT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_VLSINT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerMD0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by MD0INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_MD0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerSADINT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by SADINT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_SADINT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerRADINT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by RADINT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_RADINT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerCMP0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by CMP0INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_CMP0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerCMP1INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by CMP1INT,        */
	/* add to your aplication's code this.                                    */
	_sIrqHdr[IRQ_NO_CMP1INT]();
	di_flag &= ~DI_INTERRUPT;
	di_flag |= DI_INTERRUPT;
}

static void s_handlerTM0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by TM0INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_TM0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerTM1INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by TM1INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_TM1INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerTM2INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by TM2INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_TM2INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerTM3INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by TM3INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_TM3INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerTM4INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by TM4INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_TM4INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerTM5INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by TM5INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_TM5INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerTM6INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by TM6INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_TM6INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerTM7INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by TM7INT,         */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_TM7INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerFTM0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by FTM0INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_FTM0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerFTM1INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by FTM1INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_FTM1INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerFTM2INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by FTM2INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_FTM2INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerFTM3INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by FTM3INT,        */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_FTM3INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerLTBC0INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by LTBC0INT,       */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_LTBC0INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerLTBC1INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by LTBC1INT,       */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_LTBC1INT]();
	di_flag &= ~DI_INTERRUPT;
}

static void s_handlerLTBC2INT( void )
{
	/*===ToDo.===*/
	/* If you need a aplication's interrupt handler called by LTBC2INT,       */
	/* add to your aplication's code this.                                    */
	di_flag |= DI_INTERRUPT;
	_sIrqHdr[IRQ_NO_LTBC2INT]();
	di_flag &= ~DI_INTERRUPT;
}

/*******************************************************************************
	Routine Name:	irq_init
	Form:			void irq_init( void )
	Parameters:		void
	Return value:	void
	Description:	irq module initialize.
******************************************************************************/
void irq_init( void )
{
	int		irqNo;
	write_reg16(IE01, 0x00);
	write_reg16(IE23, 0x00);
	write_reg16(IE45, 0x00);
	write_reg16(IE67, 0x00);
	write_reg16(IRQ01, 0x00);
	write_reg16(IRQ23, 0x00);
	write_reg16(IRQ45, 0x00);
	write_reg16(IRQ67, 0x00);
	clear_bit(ILE);
	
	/*=== handler setting. ===*/
	for( irqNo = 0; irqNo < IRQ_SIZE; irqNo++ ) {
		_sIrqHdr[irqNo] = _intNullHdr;
	}
}

/*******************************************************************************
	Routine Name:	irq_setHdr
	Form:			int irq_setHdr( unsigned char intNo, void (*func)( void ) )
	Parameters:		unsigned char intNo  : interrupt number
					void (*func)( void ) : call back function in event of interrupt
	Return value:	IRQ_R_OK / IRQ_R_ERR_INTNO
	Description:	set interrupt call back function
******************************************************************************/
int irq_sethandler( unsigned char intNo, void (*func)( void ) )
{
	/*=== parameter check. ===*/
	/*--- check [intNo]. ---*/
	if( intNo >= (unsigned char)IRQ_SIZE ) {
		return ( IRQ_R_ERR_INTNO );
	}
	/*=== Handler setting.  ===*/
	if( func != (void *)0 ) {
		_sIrqHdr[intNo] = func;
	}
	else {
		_sIrqHdr[intNo] = _intNullHdr;
	}
	return ( IRQ_R_OK );
}


void rst_interrupts(void)
{
	di_flag = 0;
	__EI();
}

unsigned char getMIE(void)
{
#asm
        mov     r0,     psw             ;PSWをリード
        
        and     r0,     #8              ;MIEのみにデータに加工する
        srl     r0,     #3              ;
#endasm
}

void enb_interrupts(unsigned short irq_ch)
{
	di_flag &= ~irq_ch;			// 割り込み禁止中フラグをリセット
		if(di_flag == 0) __EI();	// 何れからも割り込み禁止されていなければ割り込み許可
}


void dis_interrupts(unsigned short irq_ch)
{
		__DI();						// 割り込み禁止
		di_flag |= irq_ch;			// 割り込み禁止中フラグをセット
}

