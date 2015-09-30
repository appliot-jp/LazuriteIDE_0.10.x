/* FILE NAME: flash.h
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

