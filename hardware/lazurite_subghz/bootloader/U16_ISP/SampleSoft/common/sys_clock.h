/*****************************************************************************
    sys_clock.h

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
#ifndef _SYS_CLOCK_H_
#define _SYS_CLOCK_H_



/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
void sys_setHSclk( void );
void sys_reset_clock(void);



#endif /*_SYS_CLOCK_H_*/
