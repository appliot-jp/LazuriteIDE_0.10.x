/*****************************************************************************
    sys_clock.c

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
    2014.04.21  ver.1.00
******************************************************************************/
#include "mcu.h"
#include "clock_i.h"
#include "tbc.h"


/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#if defined(_ML620Q15X)
 #define FCON0_DATA			0x08
 #define FCON0_INIT_DATA	0x33
#elif defined(_ML620Q40X_50X)
 #include "wdt.h"

#ifdef ML620Q504_OSC_RC
  #define FCON0_DATA			0x11
 #else
  #define FCON0_DATA			0x08
#endif
 
 #define FCON0_INIT_DATA	0x73
#else
 #define FCON0_DATA			0x08
 #define FCON0_INIT_DATA	0x3B
#endif


/*******************************************************************************
    Routine Name:   sys_setHSclk
    Form:           void sys_setHSclk( void )
    Parameters:     void
    Return value:   void
    Description:    Sets the system clock to HSCLK
******************************************************************************/
void sys_setHSclk(void)
{
	
	clk_disHsclk();                             // stop HSCLK oscillation
#ifdef    EXTCLK
	clk_setHsclk(0x18); /* set HSCLK */		// BUG620Q504
#else  // Not EXTCLK
	clk_setHsclk(0x10); /* set HSCLK */		// BUG620Q504
#endif // EXTCLK
	clk_enaHsclk();                             // start HSCLK oscillation

	clk_setSysclk( CLK_SYSCLK_HSCLK );          // Choose HSCLK
	
#if 0
	clk_setSysclk(CLK_SYSCLK_LSCLK);	/* Sets to LSCLK (for setting HSCLK) */
	clk_disHsclk();
	clk_setHsclk(FCON0_DATA);
	clk_enaHsclk();
#ifdef _ML620Q40X_50X
#ifndef ML620Q504_OSC_RC
	while( clk_checkHsOscStable() == 0 ) {
		wdt_clear();
	};
#else  // ML620Q504_OSC_RC
	tbc_wait15ms();
#endif // ML620Q504_OSC_RC
#else
	tbc_wait15ms();
#endif
	clk_setSysclk(CLK_SYSCLK_HSCLK);	/* Sets to HSCLK */
#endif //0
}


/*******************************************************************************
    Routine Name:   sys_reset_clock
    Form:           void sys_reset_clock( void )
    Parameters:     void
    Return value:   void
    Description:    Initializes the clock()
******************************************************************************/
void sys_reset_clock(void)
{
	clk_setSysclk(CLK_SYSCLK_LSCLK);
	clk_disHsclk();
	clk_setHsclk(FCON0_INIT_DATA);

	/* Set initial to HSCLK */
	clk_enaHsclk();
#ifdef _ML620Q40X_50X
	__asm("nop");
	__asm("nop");
#else
	tbc_wait15ms();
#endif
	clk_setSysclk(CLK_SYSCLK_HSCLK);	/* Sets to HSCLK */
}
