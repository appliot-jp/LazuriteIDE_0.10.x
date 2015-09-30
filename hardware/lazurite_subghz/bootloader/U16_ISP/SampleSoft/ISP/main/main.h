/*****************************************************************************
    main.h

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
#ifndef _MAIN_H_
#define _MAIN_H_


/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define OK				(0)
#define NG				(-1)

#define FSELFPROG_EN	1
#define FSELFPROG_DIS	0

#define FLASH_SEG1_ADDR		( 0x10000 )
#define FLASH_SEG8_ADDR		( 0x80000 )


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
void ResetPeri(void);


#endif /*_MAIN_H_*/
