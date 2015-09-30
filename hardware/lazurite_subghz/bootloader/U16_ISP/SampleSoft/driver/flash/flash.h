/*****************************************************************************
 flash.h

 Copyright (C) 2014 LAPIS Semiconductor Co., LTD.
 All rights reserved.

 This software is provided "as is" and any expressed or implied
  warranties, including, but not limited to, the implied warranties of
  merchantability and fitness for a particular purpose are disclaimed.
 LAPIS SEMICONDUCTOR shall not be liable for any direct, indirect,
 consequential or incidental damages arising from using or modifying
 this software.
 You (customer) can modify and use this software in whole or part on
 your own responsibility, only for the purpose of developing the software
 for use with microcontroller manufactured by LAPIS SEMICONDUCTOR.

 History
    2014.03.31 ver.0.90

******************************************************************************/
/**
 * @file    flash.h
 *
 * This file is API definition for 'Flash Memory' drivers.
 *
 */

#ifndef _FLASH_H_
#define _FLASH_H_

#include "rdwr_reg.h"

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Control Flash Self Programming
 *
 * Enable or disable to erase and write protection to the flash memory.
 *
 * @param[in]   fselfFlag   Enable or disable to erase and write. <br>
 *                          (Setting value of FLASHSLF register)
 * @return      None
 */
#define         flash_controlSelfProg( fselfFlag )      write_reg8( FLASHSLF, (unsigned char)(fselfFlag) )

void            flash_eraseBlock( unsigned short __far *addr );
void            flash_eraseSector( unsigned short __far *addr );
void            flash_writeWord( unsigned short __far *addr, unsigned short data );
void            flash_executeRemap( unsigned long remapAddr );

#endif /*_FLASH_H_*/

