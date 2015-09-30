/*****************************************************************************
 irq.h

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
    2014.03.31 ver.1.00

******************************************************************************/
/**
 * @file    irq.h
 *
 * This file is API definition for interrupt drivers.
 *
 */
#ifndef _IRQ_H_
#define _IRQ_H_

#include "rdwr_reg.h"


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/*--- Enables interrupts ---*/
#define         irq_ua0_ena()           set_bit( EUA0 )         /**< Enables UA0INT   */
#define         irq_ua1_ena()           set_bit( EUA1 )         /**< Enables UA1INT   */
#define         irq_tbc0_ena()          set_bit( ELTBC0 )       /**< Enables LTBC0INT */
#define         irq_tbc1_ena()          set_bit( ELTBC1 )       /**< Enables LTBC1INT */
#define         irq_tbc2_ena()          set_bit( ELTBC2 )       /**< Enables LTBC2INT */

/*--- Disables interrupts ---*/
#define         irq_ua0_dis()           clear_bit( EUA0 )         /**< Disables UA0INT  */
#define         irq_ua1_dis()           clear_bit( EUA1 )         /**< Disables UA1INT  */
#define         irq_tbc0_dis()          clear_bit( ELTBC0 )       /**< Disables LTBC0INT*/
#define         irq_tbc1_dis()          clear_bit( ELTBC1 )       /**< Disables LTBC1INT*/
#define         irq_tbc2_dis()          clear_bit( ELTBC2 )       /**< Disables LTBC2INT*/

/*--- Check IRQ ---*/
#define         irq_wdt_checkIRQ()      get_bit( QWDT )           /**< Check WDTINT     */
#define         irq_ua0_checkIRQ()      get_bit( QUA0 )           /**< Check UA0INT     */
#define         irq_ua1_checkIRQ()      get_bit( QUA1 )           /**< Check UA1INT     */
#define         irq_tbc0_checkIRQ()     get_bit( QLTBC0 )         /**< Check LTBC0INT   */
#define         irq_tbc1_checkIRQ()     get_bit( QLTBC1 )         /**< Check LTBC1INT   */
#define         irq_tbc2_checkIRQ()     get_bit( QLTBC2 )         /**< Check LTBC2INT   */

/*--- Clear IRQ ---*/
#define         irq_wdt_clearIRQ()      clear_bit( QWDT )         /**< Clear WDTINT     */
#define         irq_ua0_clearIRQ()      clear_bit( QUA0 )         /**< Clear UA0INT     */
#define         irq_ua1_clearIRQ()      clear_bit( QUA1 )         /**< Clear UA1INT     */
#define         irq_tbc0_clearIRQ()     clear_bit( QLTBC0 )       /**< Clear LTBC0INT   */
#define         irq_tbc1_clearIRQ()     clear_bit( QLTBC1 )       /**< Clear LTBC1INT   */
#define         irq_tbc2_clearIRQ()     clear_bit( QLTBC2 )       /**< Clear LTBC2INT   */

#endif /*_IRQ_H_*/
