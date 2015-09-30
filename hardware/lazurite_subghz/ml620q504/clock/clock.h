
/* FILE NAME:  clock.h
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

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "rdwr_reg.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* FCON0 */
#define FCON0_SYSCO         ( 0x01 )        /**< FCON0 Register SYSCO bit     */
#define FCON0_SYSC1         ( 0x02 )        /**< FCON0 Register SYSC1 bit     */
#define FCON0_SYSC2         ( 0x04 )        /**< FCON0 Register SYSC2 bit     */
#define FCON0_OSCM0         ( 0x08 )        /**< FCON0 Register OSCM0 bit     */
#define FCON0_OSCM1         ( 0x10 )        /**< FCON0 Register OSCM1 bit     */
#define FCON0_OUTC0         ( 0x20 )        /**< FCON0 Register OUTC0 bit     */
#define FCON0_OUTC1         ( 0x40 )        /**< FCON0 Register OUTC1 bit     */
#define FCON0_OUTC2         ( 0x80 )        /**< FCON0 Register OUTC2 bit     */

/* FCON2 */
#define FCON2_XTMO          ( 0x01 )        /**< FCON2 Register XTMO bit      */
#define FCON2_XTM1          ( 0x02 )        /**< FCON2 Register XTM1 bit      */

/* FSTAT */
#define FSTAT_HOSCS         ( 0x02 )        /**< FSTAT Register HOSCS bit     */
#define FSTAT_LOSCS         ( 0x04 )        /**< FSTAT Register LOSCS bit     */

/*=== API parameters value ===*/
/* HSCLK frequency                                          */
/*     Initial configuration parameters for clk_setHsclk()  */
#define CLK_SYSC_OSCLK          (                                          0 )      /**< HSCLK frequency : OSCLK        */
#define CLK_SYSC_OSCLK_DIV2     (            0 |            0 |  FCON0_SYSCO )      /**< HSCLK frequency : 1/2OSCLK     */
#define CLK_SYSC_OSCLK_DIV4     (            0 |  FCON0_SYSC1 |            0 )      /**< HSCLK frequency : 1/4OSCLK     */
#define CLK_SYSC_OSCLK_DIV8     (            0 |  FCON0_SYSC1 |  FCON0_SYSCO )      /**< HSCLK frequency : 1/8OSCLK     */
#define CLK_SYSC_OSCLK_DIV16    (  FCON0_SYSC2 |            0 |            0 )      /**< HSCLK frequency : 1/16OSCLK    */
#define CLK_SYSC_OSCLK_DIV32    (  FCON0_SYSC2 |            0 |  FCON0_SYSCO )      /**< HSCLK frequency : 1/32OSCLK    */

/* HSCLK oscillation mode                                   */
/*     Initial configuration parameters for clk_setHsclk()  */
#define CLK_OSCM_CRYSTAL        (  FCON0_OSCM0 )                                    /**< HSCLK oscillation mode : Crystal/ceramic oscillation   */
#define CLK_OSCM_RC             (  FCON0_OSCM1 )                                    /**< HSCLK oscillation mode : RC oscillation                */
#define CLK_OSCM_EXTCLK         (  FCON0_OSCM1 |  FCON0_OSCM0 )                     /**< HSCLK oscillation mode : External clock input          */

/* OUTCLK frequency                                         */
/*     Initial configuration parameters for clk_setHsclk()  */
#define CLK_OUTC_OSCLK          (                                          0 )      /**< OUTCLK frequency : OSCLK        */
#define CLK_OUTC_OSCLK_DIV2     (            0 |            0 |  FCON0_OUTC0 )      /**< OUTCLK frequency : 1/2OSCLK     */
#define CLK_OUTC_OSCLK_DIV4     (            0 |  FCON0_OUTC1 |            0 )      /**< OUTCLK frequency : 1/4OSCLK     */
#define CLK_OUTC_OSCLK_DIV8     (            0 |  FCON0_OUTC1 |  FCON0_OUTC0 )      /**< OUTCLK frequency : 1/8OSCLK     */
#define CLK_OUTC_OSCLK_DIV16    (  FCON0_OUTC2 |            0 |            0 )      /**< OUTCLK frequency : 1/16OSCLK    */
#define CLK_OUTC_OSCLK_DIV32    (  FCON0_OUTC2 |            0 |  FCON0_OUTC0 )      /**< OUTCLK frequency : 1/32OSCLK    */

/* LSCLK oscillation mode                                   */
/*     Initial configuration parameters for clk_setLsclk()  */
#define CLK_XTM_CRYSTAL         ( FCON2_XTMO )                                      /**< LSCLK oscillation mode : Crystal/ceramic oscillation   */
#define CLK_XTM_RC              ( FCON2_XTM1 )                                      /**< LSCLK oscillation mode : RC oscillation                */
#define CLK_XTM_EXTCLK          ( FCON2_XTM1 | FCON2_XTMO )                         /**< LSCLK oscillation mode : External clock input          */

/* System clock mode                                        */
/*     Initial configuration parameters for clk_setSysclk() */
#define CLK_SYSCLK_LSCLK        ( 0 )                                               /**< System clock : LSCLK         */
#define CLK_SYSCLK_HSCLK        ( 1 )                                               /**< System clock : HSCLK         */

/* LSCLK crystal oscillation */
/*     Initial configuration parameters for clk_setHsclk()  */
#define CLK_LOSCON_DIS          ( 0 )                                               /**< Disable LSCLK crystal/ceramic oscillation, if LSCLK is RC oscillation. */
#define CLK_LOSCON_ENA          ( 1 )                                               /**< Enable LSCLK crystal/ceramic oscillation, if LSCLK is RC oscillation.  */

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Enable high-speed oscillation (oscillation start)
 *
 * @param   -
 * @return  None
 */
#define         clk_enaHsclk()          set_bit( ENOSC )

/**
 * Disable high-speed oscillation (oscillation stop)
 *
 * @param   -
 * @return  None
 */
#define         clk_disHsclk()          clear_bit( ENOSC )

/**
 * Getting status of high-speed oscillation
 *
 * @param   -
 * @return  Status of high-speed oscillation
 *          (The value of HOSST register)
 */
#define         clk_checkHsOscStable()  get_bit( HOSST )

/**
 * Getting status of low-speed oscillation
 *
 * @param   -
 * @return  Status of low-speed oscillation
 *          (The value of LOSST register)
 */
#define         clk_checkLsOscStable()  get_bit( LOSST )

/**
 * Getting status of clock generation circuit
 *
 * @param   -
 * @return  Status of clock generation circuit
 *          (The value of FSTAT register)
 */
#define         clk_getClkStatus()      read_reg8( FSTAT )

/**
 * Setting HSCLK configration
 *
 * @param[in]   fcon0   Configration of high-speed oscillation.                                                                   <br>
 *                      (Setting value of FCON0 register)                                                                         <br>
 *                      Specify the logical sum of the following items.
 * @arg                     HSCLK frequency           : CLK_SYSC_OSCLK, or CLK_SYSC_OSCLK_DIV2, or (...), or CLK_SYSC_OSCLK_DIV32
 * @arg                     HSCLK oscillation mode    : CLK_OSCM_CRYSTAL, or CLK_OSCM_RC, or CLK_OSCM_EXTCLK
 * @arg                     OUTCLK frequency          : CLK_OUTC_OSCLK, or CLK_OUTC_OSCLK_DIV2, or (...), or CLK_OUTC_OSCLK_DIV32
 * @arg                     LSCLK crystal oscillation : CLK_LOSCON_DIS, or CLK_LOSCON_EN
 *
 * @param[in]   loscon  Configration of low-speed oscillation.(1)                                                                 <br>
 *                          CLK_LOSCON_DIS           : Disable low-speed crystal oscillation if RC oscillation enable.            <br>
 *                          CLK_LOSCON_ENA           : Enable low-speed crystal oscillation if RC oscillation enable.             <br>
 *
 * @return      None
 * @note        This macro is invalid to set the bit of ENOSC and SYSCLK in FCON1 register.                                       <br>
 *              Please use clk_enaHsclk() and clk_setSysclk().
 */
#define         clk_setHsclk( fcon0, loscon )  do {                                                                                                                \
                                                        write_reg8( FCON0, (fcon0) );                           /* set configration of HSCLK */                         \
                                                        write_bit( LOSCON, (unsigned char)((loscon) & 0x1) );   /* set LSCLK crystal oscillation */                     \
                                                    } while(0)

/**
 * Setting LSCLK configration
 *
 * Configration of low-speed oscillation.                                       <br>
 * (A parameter is setting value of FCON2 register.)
 *
 * @param[in]   fcon2   CLK_XTM_CRYSTAL : Crystal/ceramic oscillation           <br>
 *                      CLK_XTM_RC      : RC oscillation                        <br>
 *                      CLK_XTM_EXTCLK  : External clock input
 * @return      None
 */
#define         clk_setLsclk( fcon2 )   write_reg8( FCON2, (unsigned char)(fcon2) )

/**
 * Setting system clock
 * 
 * @note    --- How to switching HSCLK ---
 * 
 *          (1)Disables HSCLK (Setting ENOSC=0)
 *              clk_disHsclk();
 *          (2)Configration of HSCLK (Setting FCON0/FCON1 registers)
 *              clk_setHsclk(...);
 *          (3)Enables HSCLK (Setting ENOSC=1)
 *              clk_enaHsclk();
 *          (4)Waiting stables high-speed oscillation
 *              ex.
 *                  while( clk_checkHsOscStable() == 0 ) {}
 *          (5)Setting SYSCLK=1
 *              clk_setSysclk( CLK_SYSCLK_HSCLK );
 * 
 * @see     MCU Users manual 'Description of Operation' in chapter 'Clock Generation Circuit'.
 * 
 * @param[in]   sysclk  CLK_SYSCLK_LSCLK : LSCLK(low-speed oscillation)         <br>
 *                      CLK_SYSCLK_HSCLK : HSCLK(high-speed oscillation)
 * @return      None
 */
#define         clk_setSysclk( sysclk ) write_bit( SYSCLK, (unsigned char)((sysclk) & 0x1) )

#endif /*_CLOCK_H_*/

