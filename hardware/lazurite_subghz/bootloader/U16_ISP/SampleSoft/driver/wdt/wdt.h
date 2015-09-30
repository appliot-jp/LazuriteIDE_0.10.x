/*****************************************************************************
 wdt.h

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
 * @file    wdt.h
 *
 * This file is API definition for 'WDT(Watch Dog Timer)' drivers.
 *
 */
#ifndef _WDT_H_
#define _WDT_H_

#include "rdwr_reg.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*--- bit field of registers ---*/
/* WDTMOD */
#define WDTMOD_WDT0         ( 0x01 )        /**< WDTMOD Register WDT0 bit     */
#define WDTMOD_WDT1         ( 0x02 )        /**< WDTMOD Register WDT1 bit     */
#define WDTMOD_HLTEN        ( 0x80 )        /**< WDTMOD Register HLTEN bit    */

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize WDT
 *
 * @param[in]   wdtmod  Setting value of WDTMOD register
 * @return      None
 */
#define         wdt_init( wdtmod )          write_reg8( WDTMOD, wdtmod )
/**
 * Enable WDT counting during "HALT mode" or "DEEP-HALT mode"
 *
 * @param       -
 * @return      None
 */
#define         wdt_enaHaltCount()          set_bit( HLTEN )
/**
 * Disable WDT counting during "HALT mode" or "DEEP-HALT mode"
 *
 * @param       -
 * @return      None
 */
#define         wdt_disHaltCount()          clear_bit( HLTEN )

void            wdt_clear( void );

#endif /*_WDT_H_*/

