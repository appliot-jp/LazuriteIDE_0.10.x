/*****************************************************************************
    main.c

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
#include "isp.h"


/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define REMAPADD_REG_VAL	(ISP_AREA_START_ADDR>>12)


/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
static void _intNullHdr( void );


/*############################################################################*/
/*#                             Vector Table                                 #*/
/*############################################################################*/
#include "vector_bootloader.h"



/*******************************************************************************
    Routine Name:   main
    Form:           int main( void )
    Parameters:     void
    Return value:   int
                        but not return...
    Description:    main
******************************************************************************/
int main( void )
{
	/* Set Remap Address */
	write_reg8(REMAPADD, REMAPADD_REG_VAL);

	/* Set ELEVEL 2 */
	__asm("mov r0,psw\n or r0,#2\n mov psw,r0\n");
	/* Software Reset */
	__asm("brk");

	/* endless loop */
	while(1){}
}


/*******************************************************************************
    Routine Name:   _intNullHdr
    Form:           static void _intNullHdr( void )
    Parameters:     void
    Return value:   void
    Description:    NULL Handler.
******************************************************************************/
static void _intNullHdr( void )
{
	return;
}
