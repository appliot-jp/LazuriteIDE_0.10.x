/*****************************************************************************
    remap.c

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
#include "flash.h"
#include "remap.h"


/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
extern void ResetPeri(void);
//extern void test(void);

/*******************************************************************************
    Routine Name:   Remap
    Form:           void Remap( unsigned long remap_add )
    Parameters:     remap_add 
    Return value:   void
    Description:    Remap with resetting peripheral
******************************************************************************/
void Remap( unsigned long remap_add )
{
	ResetPeri();

//	test();

	flash_executeRemap(remap_add);
}



/*******************************************************************************
    Routine Name:   ResetPeriBlock
    Form:           void ResetPeriBlock( void )
    Parameters:     none
    Return value:   none
    Description:    Reset peripheral with BLKCON
******************************************************************************/
void ResetPeriBlock( void )
{
	write_reg8(BLKCON0, 0xFF);
	write_reg8(BLKCON0, 0);

#if !(defined(_ML620Q15X))
	write_reg8(BLKCON1, 0xFF);
	write_reg8(BLKCON1, 0);
#endif

	write_reg8(BLKCON2, 0xFF);
	write_reg8(BLKCON2, 0);

	write_reg8(BLKCON3, 0xFF);
	write_reg8(BLKCON3, 0);

	write_reg8(BLKCON4, 0xFF);
	write_reg8(BLKCON4, 0);

#if !(defined(_ML620Q15X))
	write_reg8(BLKCON5, 0xFF);
 #ifdef _ML620Q40X_50X
	write_reg8(BLKCON5, 0x04);
 #else
	write_reg8(BLKCON5, 0);
 #endif
#endif

#if !(defined(_ML620Q40X_50X))
	write_reg8(BLKCON6, 0xFF);
	write_reg8(BLKCON6, 0);

	write_reg8(BLKCON7, 0xFF);
	write_reg8(BLKCON7, 0);
#endif
}
