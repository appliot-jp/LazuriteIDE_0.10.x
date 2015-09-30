/* FILE NAME: rcAdc.h
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

#ifndef _RCADC_H_
#define _RCADC_H_

#include "rdwr_reg.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* RADMOD */
#define RADMOD_OM0              ( 0x0001 )                                      /**< RADMOD Register OM0 bit      */
#define RADMOD_OM1              ( 0x0002 )                                      /**< RADMOD Register OM1 bit      */
#define RADMOD_OM2              ( 0x0004 )                                      /**< RADMOD Register OM2 bit      */
#define RADMOD_OM3              ( 0x0008 )                                      /**< RADMOD Register OM3 bit      */
#define RADMOD_RADI             ( 0x0010 )                                      /**< RADMOD Register RADI bit     */
#define RADMOD_RACK0            ( 0x0020 )                                      /**< RADMOD Register RACK0 bit    */
#define RADMOD_RACK1            ( 0x0040 )                                      /**< RADMOD Register RACK1 bit    */
#define RADMOD_RACK2            ( 0x0080 )                                      /**< RADMOD Register RACK2 bit    */
#define RADMOD_RAMD0            ( 0x0100 )                                      /**< RADMOD Register RAMD0 bit    */

/*=== API parameters value ===*/
/* Parameters for rcAdc_execute() */
#define RCADC_OFF               ( 0 )
#define RCADC_RUN               ( 1 )

/* Parameters for rcAdc_init() */
/* mode */
#define RCADC_MODE0             ( 0 )                                           /**< Mode  : IN0 pin external clock input mode   */
#define RCADC_MODE1             ( RADMOD_OM0 )                                  /**< Mode  : RS0-CS0 oscillation mode            */
#define RCADC_MODE2             ( RADMOD_OM1 )                                  /**< Mode  : RT0-CS0 oscillation mode            */
#define RCADC_MODE3             ( RADMOD_OM1 | RADMOD_OM0 )                     /**< Mode  : RT0_1-CS0 oscillation mode          */
#define RCADC_MODE4             ( RADMOD_OM2 )                                  /**< Mode  : RS0-CT0 oscillation mode            */
#define RCADC_MODE5             ( RADMOD_OM2 | RADMOD_OM0 )                     /**< Mode  : RS1-CS1 oscillation mode            */
#define RCADC_MODE6             ( RADMOD_OM2 | RADMOD_OM1 )                     /**< Mode  : RT1-CS1 oscillation mode            */
#define RCADC_MODE7             ( RADMOD_OM2 | RADMOD_OM1 | RADMOD_OM0 )        /**< Mode  : IN1 pin external clock input mode   */
/* clock */
#define RCADC_CLK_LS            ( 0 )                                           /**< Clock : LSCLK                               */
#define RCADC_CLK_HS            ( RADMOD_RACK0 )                                /**< Clock : HSCLK                               */
#define RCADC_CLK_HS2           ( RADMOD_RACK1 )                                /**< Clock : 1/2HSCLK                            */
#define RCADC_CLK_HS4           ( RADMOD_RACK1 | RADMOD_RACK0 )                 /**< Clock : 1/4HSCLK                            */
#define RCADC_CLK_HS8           ( RADMOD_RACK2 )                                /**< Clock : 1/8HSCLK                            */
#define RCADC_CLK_HS16          ( RADMOD_RACK2 | RADMOD_RACK0 )                 /**< Clock : 1/16HSCLK                           */
#define RCADC_CLK_HS32          ( RADMOD_RACK2 | RADMOD_RACK1 )                 /**< Clock : 1/32HSCLK                           */
#define RCADC_CLK_HS64          ( RADMOD_RACK2 | RADMOD_RACK1 | RADMOD_RACK0 )  /**< Clock : 1/64HSCLK                           */
/* interrupt */
#define RCADC_INT_A             ( 0 )                                           /**< Interrupt : Counter A overflow              */
#define RCADC_INT_B             ( RADMOD_RADI )                                 /**< Interrupt : Counter B overflow              */


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Start/Stop RC-ADC
 *
 * @param       run     RCADC_OFF : Stop
 *                      RCADC_RUN : Start
 * @return      None
 */
#define         rcAdc_execute( run )        write_reg16( RADCON, run )

/**
 * Getting A/D conversion result (counter A)
 *
 * @param       -
 * @return      A/D conversion result (counter A)
 */
#define         rcAdc_getResultA()          ( ( (unsigned long)read_reg8( RADCA0L ) ) | ( ((unsigned long)read_reg8( RADCA0H ) <<  8) ) | ( ((unsigned long)read_reg8( RADCA1L ) << 16) ) )


/**
 * Getting A/D conversion result (counter B)
 *
 * @param       -
 * @return      A/D conversion result (counter B)
 */
#define         rcAdc_getResultB()          ( ( (unsigned long)read_reg8( RADCB0L ) ) | ( ((unsigned long)read_reg8( RADCB0H ) <<  8) ) | ( ((unsigned long)read_reg8( RADCB1L ) << 16) ) )

void            rcAdc_init( unsigned short radmod, unsigned long radc );

#endif /*_RCADC_H_*/

