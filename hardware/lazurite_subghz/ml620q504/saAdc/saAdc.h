/* FILE NAME: saAdc.h
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

#ifndef _SAADC_H_
#define _SAADC_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* SADCON0 */
#define SADCON0_SALP        ( 0x01 )                     /**< SADCON0 Register SALP bit  */
#define SADCON0_SACK        ( 0x02 )                     /**< SADCON0 Register SACK bit  */
#define SADCON0_SATCM       ( 0x04 )                     /**< SADCON0 Register SATCM bit */
#define SADCON0_SACD0       ( 0x10 )                     /**< SADCON0 Register SACD0 bit */
#define SADCON0_SACD1       ( 0x20 )                     /**< SADCON0 Register SACD1 bit */

/* SADEN */
#define SADEN_SACH0         ( 0x0001 )                   /**< SADEN Register SACH0 bit   */
#define SADEN_SACH1         ( 0x0002 )                   /**< SADEN Register SACH1 bit   */
#define SADEN_SACH2         ( 0x0004 )                   /**< SADEN Register SACH2 bit   */
#define SADEN_SACH3         ( 0x0008 )                   /**< SADEN Register SACH3 bit   */
#define SADEN_SACH4         ( 0x0010 )                   /**< SADEN Register SACH4 bit   */
#define SADEN_SACH5         ( 0x0020 )                   /**< SADEN Register SACH5 bit   */
#define SADEN_SACH6         ( 0x0040 )                   /**< SADEN Register SACH6 bit   */
#define SADEN_SACH7         ( 0x0080 )                   /**< SADEN Register SACH7 bit   */
#define SADEN_SACH8         ( 0x0100 )                   /**< SADEN Register SACH8 bit   */
#define SADEN_SACH9         ( 0x0200 )                   /**< SADEN Register SACH9 bit   */
#define SADEN_SACHA         ( 0x0400 )                   /**< SADEN Register SACHA bit   */
#define SADEN_SACHB         ( 0x0800 )                   /**< SADEN Register SACHB bit   */

/* SADTCH */
#define SADTCH_SACH0        ( 0x0001 )                   /**< SADTCH Register SACH0 bit  */
#define SADTCH_SACH1        ( 0x0002 )                   /**< SADTCH Register SACH1 bit  */
#define SADTCH_SACH2        ( 0x0004 )                   /**< SADTCH Register SACH2 bit  */
#define SADTCH_SACH3        ( 0x0008 )                   /**< SADTCH Register SACH3 bit  */
#define SADTCH_SACH4        ( 0x0010 )                   /**< SADTCH Register SACH4 bit  */
#define SADTCH_SACH5        ( 0x0020 )                   /**< SADTCH Register SACH5 bit  */
#define SADTCH_SACH6        ( 0x0040 )                   /**< SADTCH Register SACH6 bit  */
#define SADTCH_SACH7        ( 0x0080 )                   /**< SADTCH Register SACH7 bit  */
#define SADTCH_SACH8        ( 0x0100 )                   /**< SADTCH Register SACH8 bit  */
#define SADTCH_SACH9        ( 0x0200 )                   /**< SADTCH Register SACH9 bit  */
#define SADTCH_SACHA        ( 0x0400 )                   /**< SADTCH Register SACHA bit  */
#define SADTCH_SACHB        ( 0x0800 )                   /**< SADTCH Register SACHB bit  */

/* SADTRG */
#define SADTRG_SAST0        ( 0x0001 )                   /**< SADTRG Register SAST0 bit  */
#define SADTRG_SASTS0       ( 0x0100 )                   /**< SADTRG Register SASTS0 bit */
#define SADTRG_SASTS1       ( 0x0200 )                   /**< SADTRG Register SASTS1 bit */
#define SADTRG_SASTS2       ( 0x0400 )                   /**< SADTRG Register SASTS2 bit */
#define SADTRG_SASTS3       ( 0x0800 )                   /**< SADTRG Register SASTS3 bit */
#define SADTRG_SASTS4       ( 0x1000 )                   /**< SADTRG Register SASTS4 bit */

/*=== API Parameters value. ===*/
/* Parameters for saAdc_execute() */
#define SAADC_OFF                ( 0 )
#define SAADC_RUN                ( 1 )

/* Parameters for saAdc_init() */
/* loop */
#define SAADC_ONESHOT            ( 0 )
#define SAADC_LP_CONTINUE        ( SADCON0_SALP )

/* clock */
#define SAADC_CK_LOW             ( 0 )
#define SAADC_CK_HIGH            ( SADCON0_SACK )

/* mode */
#define SAADC_MODE_NOT_TOUCH     ( 0 )
#define SAADC_MODE_TOUCH         ( SADCON0_SATCM )

/* division */
#define SAADC_DIV1               ( 0 )
#define SAADC_DIV2               ( SADCON0_SACD0 )
#define SAADC_DIV4               ( SADCON0_SACD1 )

/* sadtrg */
#define SAADC_TRG_NOT            ( 0 )
#define SAADC_TRG_TIMER0INT      ( SADTRG_SAST0 )
#define SAADC_TRG_TIMER1INT      ( SADTRG_SAST0 | SADTRG_SASTS0 )
#define SAADC_TRG_TIMER2INT      ( SADTRG_SAST0 | SADTRG_SASTS1 )
#define SAADC_TRG_TIMER3INT      ( SADTRG_SAST0 | SADTRG_SASTS1 | SADTRG_SASTS0 )
#define SAADC_TRG_TIMER4INT      ( SADTRG_SAST0 | SADTRG_SASTS2 )
#define SAADC_TRG_TIMER5INT      ( SADTRG_SAST0 | SADTRG_SASTS2 | SADTRG_SASTS0 )
#define SAADC_TRG_TIMER6INT      ( SADTRG_SAST0 | SADTRG_SASTS2 | SADTRG_SASTS1 )
#define SAADC_TRG_TIMER7INT      ( SADTRG_SAST0 | SADTRG_SASTS2 | SADTRG_SASTS1 | SADTRG_SASTS0 )
#define SAADC_TRG_FTIMER0INT     ( SADTRG_SAST0 | SADTRG_SASTS4 )
#define SAADC_TRG_FTIMER1INT     ( SADTRG_SAST0 | SADTRG_SASTS4 | SADTRG_SASTS0 )
#define SAADC_TRG_FTIMER2INT     ( SADTRG_SAST0 | SADTRG_SASTS4 | SADTRG_SASTS1 )
#define SAADC_TRG_FTIMER3INT     ( SADTRG_SAST0 | SADTRG_SASTS4 | SADTRG_SASTS1 | SADTRG_SASTS0 )

/* sadcvt */
#define SAADC_CVT_SACK_0_SATCM_0 ( 0x0001 )
#define SAADC_CVT_SACK_1_SATCM_0 ( 0x0378 )
#define SAADC_CVT_SACK_1_SATCM_1 ( 0xAB78 )

/* Parameters for saAdc_setConversionMode() */
#define SAADC_TOUCH_MODE_OFF     ( 0 )
#define SAADC_TOUCH_MODE_ON      ( 1 )

/*=== API return value ===*/
#define SAADC_R_OK               (  0  )  /**< Status of OK (success)                     */
#define SAADC_R_NOT_FIN          (  -1 )  /**< SA-ADC is running (failure)                */

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize SA-ADC
 *
 * @param[in]   sadcon0     Configuration of SA-ADC operation                                       <br>
 *                          (Setting value of SADCON0 register)                                     <br>
 *                          Specify the logical sum of the following items.
 * @arg                     loop       : SAADC_ONESHOT, or SAADC_LP_CONTINUE
 * @arg                     clock      : SAADC_CK_LOW, or SAADC_CK_HIGH
 * @arg                     mode       : SAADC_MODE_NOT_TOUCH, or SAADC_MODE_TOUCH
 * @arg                     division   : SAADC_DIV1, or SAADC_DIV2, or SAADC_DIV4
 *
 * @return      None
 */
#define         saAdc_init( sadcon0 )               do {                                                  \
                                                        clear_bit( SARUN );                               \
                                                        write_reg8( SADCON0, (unsigned char)(sadcon0) );    \
                                                    } while (0)

/**
 * Setting SA-ADC mode
 *
 * @param       mode        SAADC_TOUCH_MODE_OFF : Touch sensor mode disable                          <br>
 *                          SAADC_TOUCH_MODE_ON  : Touch sensor mode enable
 * @return      None
 */
#define         saAdc_setConversionMode( mode )     do {                                                   \
                                                        clear_bit( SARUN );                                \
                                                        write_bit( SATCM, (unsigned char)((mode) & 0x1) ); \
                                                    } while (0)

/**
 * Setting conversion channels
 *
 * @param[in]   saden       Configuration of the conversion channels                                  <br>
 *                          (Setting value of SADEN register)                                         <br>
 *                          Specify the logical sum of the item of SADEN_SACH0 form SADEN_SACHB.
 *
 * @return      None
 */
#define         saAdc_setEnableChannel( saden )     write_reg16( SADEN, (unsigned short)(saden) )

/**
 * Setting conversion channels in touch mode
 *
 * @param[in]   sadtch      Configuration of the conversion channels in touch mode                    <br>
 *                          (Setting value of SADEN register)                                         <br>
 *                          Specify the logical sum of the item of SADTCH_SACH0 form SADTCH_SACHB.
 *
 * @return      None
 */
#define         saAdc_setTouchChannel( sadtch )     write_reg16( SADTCH, (unsigned short)(sadtch) )

/**
 * Setting triger event
 *
 * @param[in]   sadtrg      Configuration of triger event                                             <br>
 *                          (Setting value of SADTRG register)                                        <br>
 *                          Set the value form SAADC_TRG_NOT to SAADC_TRG_FTIMER3INT.                 <br>
 *
 * @return      None
 * @note        Even if a trigger event occurs during A/D conversion (when SARUN is set to Åg1Åh),    <br>
 *              it is ignored and the running A/D conversion continues.                               <br>
 *              In addition, the control of SARUN by Consecutive A/D conversion and software isn't    <br>
 *              possible.
 */
#define         saAdc_setTrigger( sadtrg )          write_reg16( SADTRG, (unsigned short)(sadtrg) )

/**
 * Setting conversion time
 *
 * @param[in]   sadcvt      Configuration of conversion time                                          <br>
 *                          (Setting value of SADCVT register)                                        <br>
 *                          Set this value form SAADC_CVT_SACK_0_SATCM_0 to SAADC_CVT_SACK_1_SATCM_1. <br>
 *
 * @return      None
 */
#define         saAdc_setAccuracy( sadcvt )         write_reg16( SADCVT, (unsigned short)(sadcvt) )

/**
 * Start/Stop SA-ADC
 *
 * @param       run         SAADC_OFF : Stop
 *                          SAADC_RUN : Start
 * @return      None
 */
#define         saAdc_execute( run )                write_reg8( SADCON1, (unsigned char)(run) )

/**
 * Getting SA-ADC running status
 *
 * @param       -
 * @retval      0   : SA-ADC is stop
 * @retval      1   : SA-ADC is runnning
 */
#define         saAdc_checkRunning()                ( (unsigned char)get_bit( SARUN ) )

/**
 * Getting A/D conversion result0
 *
 * @param       -
 * @return      A/D conversion result0
 */
#define         saAdc_getResult0()                  read_reg16( SADR0 )

/**
 * Getting A/D conversion result1
 *
 * @param       -
 * @return      A/D conversion result1
 */
#define         saAdc_getResult1()                  read_reg16( SADR1 )

/**
 * Getting A/D conversion result2
 *
 * @param       -
 * @return      A/D conversion result2
 */
#define         saAdc_getResult2()                  read_reg16( SADR2 )

/**
 * Getting A/D conversion result3
 *
 * @param       -
 * @return      A/D conversion result3
 */
#define         saAdc_getResult3()                  read_reg16( SADR3 )

/**
 * Getting A/D conversion result4
 *
 * @param       -
 * @return      A/D conversion result4
 */
#define         saAdc_getResult4()                  read_reg16( SADR4 )

/**
 * Getting A/D conversion result5
 *
 * @param       -
 * @return      A/D conversion result5
 */
#define         saAdc_getResult5()                  read_reg16( SADR5 )

/**
 * Getting A/D conversion result6
 *
 * @param       -
 * @return      A/D conversion result6
 */
#define         saAdc_getResult6()                  read_reg16( SADR6 )

/**
 * Getting A/D conversion result7
 *
 * @param       -
 * @return      A/D conversion result7
 */
#define         saAdc_getResult7()                  read_reg16( SADR7 )

/**
 * Getting A/D conversion result8
 *
 * @param       -
 * @return      A/D conversion result8
 */
#define         saAdc_getResult8()                  read_reg16( SADR8 )

/**
 * Getting A/D conversion result9
 *
 * @param       -
 * @return      A/D conversion result9
 */
#define         saAdc_getResult9()                  read_reg16( SADR9 )

/**
 * Getting A/D conversion resultA
 *
 * @param       -
 * @return      A/D conversion resultA
 */
#define         saAdc_getResultA()                  read_reg16( SADRA )

/**
 * Getting A/D conversion resultB
 *
 * @param       -
 * @return      A/D conversion resultB
 */
#define         saAdc_getResultB()                  read_reg16( SADRB )

#endif /*_SAADC_H_*/
