/* FILE NAME: ftimer3.h
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

#ifndef _FTIMER3_H_
#define _FTIMER3_H_

#include "rdwr_reg.h"
#include "ftimer_common.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Initialize FTimer
 *
 * @param[in]   ftnmod      Configuration FTimer mode                           <br>
 *                          (Setting value of FT3MOD register)                  <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Mode                                : FTM_MD_TIMER, or FTM_MD_CAPTURE, or FTM_MD_PWM1, or FTM_MD_PWM2
 * @arg                         Dead time disable/enable            : FTM_DTEN_DIS, or FTM_DTEN_ENA
 * @arg                         One-shout mode                                                                        <br>
 *                                  [for Timer/PWM]                 : FTM_OST_REROAD, or FTM_OST_ONESHOT              <br>
 *                                  [for Capture]                   : FTM_OST_CAP_AUTO, or FTM_OST_CAP_SINGLE
 * @arg                         Output signal when FTimer is stoped : FTM_STPO_LOW, or FTM_STPO_KEEP
 *
 * @param[in]   ftnclk      Configuration clock                                 <br>
 *                          (Setting value of FT3CLK register)                  <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Clock source                        : FTM_CK_LSCLK, or FTM_CK_OSCLK, or FTM_CK_HSCLK, or FTM_CK_EXTCLK, or FTM_CK_LCDCLK
 * @arg                         Division ratio                      : FTM_CKD_DIV1, or FTM_CKD_DIV2, ( ... ), or FTM_CKD_DIV32, or FTM_CKD_DIV64
 * @arg                         External clock source               : FTM_XCK_TMCKI0, or FTM_XCK_TMCKI1, ( ... ), or FTM_XCK_TMCKI6, or FTM_XCK_TMCKI7
 * @return      None
 *
 * @note        When using the One-shot mode / Single mode, set to Åg1Åh FTnIEP of FTnINTE register always.
 */
#define         ftimer3_init( ftnmod, ftnclk )                  do {                                                    \
                                                                    write_reg16( FT3MOD, (unsigned short)(ftnmod) );    \
                                                                    write_reg16( FT3CLK, (unsigned short)(ftnclk) );    \
                                                                } while (0)

/**
 * Setting trigger parameters
 *
 * @param[in]   ftntrg0     Trigger parameter (1)                               <br>
 *                          (Setting value of FT3TRG0 register)                 <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Counter start                         : FTM_ST0_DIS,  or FTM_ST0_ENA
 * @arg                         Counter stop                          : FTM_ST1_DIS,  or FTM_ST1_ENA
 * @arg                         Counter clear                         : FTM_EXCL_DIS, or FTM_EXCL_ENA
 * @arg                         Counter start when counting is stoped : FTM_CST_DIS,  or FTM_CST_ENA
 * @arg                         Trigger event source                  : FTM_STS_EXI0, or FTM_STS_EXI1, ( ... ), or FTM_STS_FTM2, or FTM_STS_FTM3
 *
 * @param[in]   ftntrg1     Trigger parameter (2)                               <br>
 *                          (Setting value of FT3TRG1 register)                 <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Trigger event edge                  : FTM_EDGE_RISE_RISE,  or FTM_EDGE_FALL_RISE,  or FTM_EDGE_RISE_FALL,  FTM_EDGE_FALL_FALL
 * @arg                         Emergency trigger source            : FTM_EMERGE_SRC_EXI0, or FTM_EMARGE_SRC_EXI4, or FTM_EMARGE_SRC_CMP0, FTM_EMARGE_SRC_CMP1
 * @return      None
 */
#define         ftimer3_setTriggerParam( ftntrg0, ftntrg1 )     do {                                                    \
                                                                    write_reg16( FT3TRG0, (unsigned short)(ftntrg0) );  \
                                                                    write_reg16( FT3TRG1, (unsigned short)(ftntrg1) );  \
                                                                } while (0)

/**
 * Setting interrupt
 *
 * @param[in]   ftninte     Disable/enable interupt                             <br>
 *                          (Setting value of FT3INTE register)                 <br>
 *                          Specify the logical sum of the following items.
 * @arg                         FTnIEP  : FTM_IEP_DIS,  or FTM_IEP_ENA
 * @arg                         FTnIEA  : FTM_IEA_DIS,  or FTM_IEA_ENA
 * @arg                         FTnIEB  : FTM_IEB_DIS,  or FTM_IEB_ENA
 * @arg                         FTnIETS : FTM_IETS_DIS, or FTM_IETS_ENA
 * @arg                         FTnIETR : FTM_IETR_DIS, or FTM_IETR_ENA
 *
 * @return      None
 */
#define         ftimer3_setInt( ftninte )       write_reg16( FT3INTE, (unsigned short)(ftninte) )

/**
 * Setting cycle counter
 *
 * @param[in]   ftnp        Cycle of timer counting                             <br>
 *                          (Setting value of FT3P register)                    <br>
 *                          Set this value form 1 to 0xFFFF.                    <br>
 * 
 * @return      None
 */
#define         ftimer3_setCycle( ftnp )        write_reg16( FT3P, (unsigned short)(ftnp) )

/**
 * Setting event counter A
 *
 * @param[in]   ftnea       Setting value of FT3EA register                     <br>
 *                              [for Timer]    : the count of interrupt trigger <br>
 *                              [for PWM]      : the duty of TMOUT output       <br>
 *                              [for Capture]  : invalid
 * 
 * @return      None
 */
#define         ftimer3_setEventCntA( ftnea )   write_reg16( FT3EA, (unsigned short)(ftnea) )

/**
 * Setting event counter B
 *
 * @param[in]   ftneb       Setting value of FT3EB register                         <br>
 *                              [for Timer]     : the count of interrupt trigger    <br>
 *                              [for PWM]       : the duty of TMOUT output          <br>
 *                              [for Capture]   : invalid
 * 
 * @return      None
 */
#define         ftimer3_setEventCntB( ftneb )   write_reg16( FT3EB, (unsigned short)(ftneb) )

/**
 * Setting DeadTime
 *
 * @param[in]   ftndt       Setting value of FT3DT register                                     <br>
 *                              [for Timer/PWM] : the clock of DeadTime ( 0 : Not-DeadTime )    <br>
 *                              [for Capture]   : invalid
 * @return      None
 */
#define         ftimer3_setDeadtime( ftndt )    write_reg16( FT3DT, (unsigned short)(ftndt) )

/**
 * Start FTimer
 *
 * @param[in]   ftncon0     Setting value of FT3CON0 register                                   <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Trigger disable/enable              : FTM_TGEN_DIS, or FTM_TGEN_ENA
 * @arg                         Emergency stop disable/enable       : FTM_EMGEN_DIS, or FTM_EMGEN_ENA
 * @arg                         Mask of output disable/enable       : FTM_SDN_DIS, or FTM_SDN_ENA
 * @return      None
 */
#define         ftimer3_start( ftncon0 )        write_reg8( FT3CON0, (unsigned char)( (ftncon0)|FTnCON0_FTnRUN ) )

/**
 * Stop FTimer
 *
 * @param       -
 * @return      None
 */
#define         ftimer3_stop()                  clear_bit( FT3RUN )

/**
 * Getting timer counter value
 *
 * @param       -
 * @return      The value of FTIMER counter     <br>
 *              (A value of FT3C register)      <br>
 */
#define         ftimer3_getCnt()                read_reg16( FT3C )

/**
 * Getting event (A) counter value
 *
 * @param       -
 * @return      The value of capture result     <br>
 *              (A value of FT3EA register)     <br>
 * @note        This macro is Capture mode only.
 */
#define         ftimer3_getEventCntA()          read_reg16( FT3EA )

/**
 * Getting event (B) counter value
 *
 * @param       -
 * @return      The value of capture result     <br>
 *              (A value of FT3EB register)     <br>
 * @note        This macro is Capture mode only.
 */
#define         ftimer3_getEventCntB()          read_reg16( FT3EB )

/**
 * Trig to update FTimer parameters
 *
 * @param       -
 * @return      None
 */
#define         ftimer3_trigUpdateDutyCycle()   set_bit( FT3UD )

/**
 * Checking to update FTimer parameters
 *
 * @param       -
 * @retval      0   status : Normal(able to update)
 * @retval      1   status : Busy(invalid updating)
 */
#define         ftimer3_checkEnaUpdate()        ( (unsigned char)get_bit( FT3UD ) )

/**
 * Getting FTimer status
 *
 * @param       -
 * @return      Status                          <br>
 *              (A value of FT3CON0 register)   <br>
 */
#define         ftimer3_getStatus()             read_reg8( FT3CON0 )

/**
 * Getting interrupt cause
 *
 * Get causes which FTimer interrupt is requested.
 *
 * @param       -
 * @return      Interrupt causes <br>
 *              (A value of FT3INTS register)
 */
#define         ftimer3_getIntCause()           read_reg16( FT3INTS )

/**
 * Clear interrupt cause
 *
 * @param[in]   ftnintc     Mask value to clear interupt cause                  <br>
 *                          (Setting value of FT3INTC register)                 <br>
 *                          Specify the logical sum of the following items.
 * @arg                         FTnICP  : FTnINTC_FTnICP
 * @arg                         FTnICA  : FTnINTC_FTnICA
 * @arg                         FTnICB  : FTnINTC_FTnICB
 * @arg                         FTnICTS : FTnINTC_FTnICTS
 * @arg                         FTnICTR : FTnINTC_FTnICTR
 * @arg                         FTnICES : FTnINTC_FTnICES
 *
 * @return      None
 */
#define         ftimer3_clearIntCause( ftnintc ) write_reg16( FT3INTC, (unsigned short)(ftnintc) )


#endif /*_FTIMER3_H_*/

