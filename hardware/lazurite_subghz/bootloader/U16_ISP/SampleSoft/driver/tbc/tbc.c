/*****************************************************************************
    tbc.c

    Copyright (C) 2014 LAPIS Semiconductor Co., Ltd.
    All rights reserved.

    This software is provided "as is" and any expressed or implied
    warranties, including, but not limited to, the implied warranties of
    merchantability and fitness for a particular purpose are disclaimed.
    LAPIS Semiconductor shall not be liable for any direct, indirect,
    consequential or incidental damages arising from using or modifying
    this software.
    You (customer) can modify and use this software in whole or part on
    your own responsibility, only for the purpose of developing the software
    for use with microcontroller manufactured by LAPIS Semiconductor.

    History
    2014.04.14  ver.1.00
******************************************************************************/
#include "mcu.h"
#include "rdwr_reg.h"
#include "irq_i.h"
#include "tbc.h"


/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define NOP()		__asm("nop\n")


/*******************************************************************************
    Routine Name:   tbc_setInt0
    Form:           void tbc_setInt0(unsigned char ltb)
    Parameters:     ltb ... value of LTBINT
    Return value:   none
    Description:    Sets tbc interrupt 0
******************************************************************************/
void tbc_setInt0(unsigned char ltb)
{
	unsigned short val;

	val = read_reg16(LTBINT) & 0xFFF0;
	val |= ltb;
	write_reg16(LTBINT, val);
}


/*******************************************************************************
    Routine Name:   tbc_wait15ms
    Form:           void tbc_wait15ms(void)
    Parameters:     none
    Return value:   none
    Description:    Wait 15ms with using TBC64Hz
                    Please call this function before TBC interrupts are disable.
******************************************************************************/
void tbc_wait15ms(void)
{
	tbc_setInt0(TBC_LTBINT_64);

	irq_tbc0_dis();
	irq_tbc1_dis();
	irq_tbc2_dis();

	write_reg8(LTBR, 0);
	NOP();
	irq_tbc0_clearIRQ();
	while(irq_tbc0_checkIRQ() == 0){}
}
