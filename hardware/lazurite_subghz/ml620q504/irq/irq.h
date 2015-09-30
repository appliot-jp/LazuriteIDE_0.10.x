/* FILE NAME: irq.h
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

#ifndef _IRQ_H_
#define _IRQ_H_

#include "rdwr_reg.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit number of registers ===*/
/* ILCn */
#define ILCn_LnEXI0_BIT_NO              ( 0 )                   /**< ILCn Register L0EXI0 bit No.  */
#define ILCn_LnEXI1_BIT_NO              ( 2 )                   /**< ILCn Register L0EXI1 bit No.  */
#define ILCn_LnEXI2_BIT_NO              ( 4 )                   /**< ILCn Register L0EXI2 bit No.  */
#define ILCn_LnEXI3_BIT_NO              ( 6 )                   /**< ILCn Register L0EXI3 bit No.  */
#define ILCn_LnEXI4_BIT_NO              ( 0 )                   /**< ILCn Register L0EXI4 bit No.  */
#define ILCn_LnEXI5_BIT_NO              ( 2 )                   /**< ILCn Register L0EXI5 bit No.  */
#define ILCn_LnEXI6_BIT_NO              ( 4 )                   /**< ILCn Register L0EXI6 bit No.  */
#define ILCn_LnEXI7_BIT_NO              ( 6 )                   /**< ILCn Register L0EXI7 bit No.  */
#define ILCn_LnSIO0_BIT_NO              ( 0 )                   /**< ILCn Register L0SIO0 bit No.  */
#define ILCn_LnSIOF0_BIT_NO             ( 2 )                   /**< ILCn Register L0SIOF0 bit No. */
#define ILCn_LnI2C0_BIT_NO              ( 4 )                   /**< ILCn Register L0I2C0 bit No.  */
#define ILCn_LnI2C1_BIT_NO              ( 6 )                   /**< ILCn Register L0I2C1 bit No.  */
#define ILCn_LnUA0_BIT_NO               ( 0 )                   /**< ILCn Register L0UA0 bit No.   */
#define ILCn_LnUA1_BIT_NO               ( 2 )                   /**< ILCn Register L0UA1 bit No.   */
#define ILCn_LnUAF0_BIT_NO              ( 4 )                   /**< ILCn Register L0UAF0 bit No.  */
#define ILCn_LnLOSC_BIT_NO              ( 2 )                   /**< ILCn Register L0LOSC bit No.  */
#define ILCn_LnVLS_BIT_NO               ( 4 )                   /**< ILCn Register L0VLS bit No.   */
#define ILCn_LnMD0_BIT_NO               ( 6 )                   /**< ILCn Register L0MD0 bit No.   */
#define ILCn_LnSAD_BIT_NO               ( 0 )                   /**< ILCn Register L0SAD bit No.   */
#define ILCn_LnRAD_BIT_NO               ( 2 )                   /**< ILCn Register L0RAD bit No.   */
#define ILCn_LnCMP0_BIT_NO              ( 0 )                   /**< ILCn Register L0CMP0 bit No.  */
#define ILCn_LnCMP1_BIT_NO              ( 2 )                   /**< ILCn Register L0CMP1 bit No.  */
#define ILCn_LnTM0_BIT_NO               ( 0 )                   /**< ILCn Register L0TM0 bit No.   */
#define ILCn_LnTM1_BIT_NO               ( 2 )                   /**< ILCn Register L0TM1 bit No.   */
#define ILCn_LnTM2_BIT_NO               ( 4 )                   /**< ILCn Register L0TM2 bit No.   */
#define ILCn_LnTM3_BIT_NO               ( 6 )                   /**< ILCn Register L0TM3 bit No.   */
#define ILCn_LnTM4_BIT_NO               ( 0 )                   /**< ILCn Register L0TM4 bit No.   */
#define ILCn_LnTM5_BIT_NO               ( 2 )                   /**< ILCn Register L0TM5 bit No.   */
#define ILCn_LnTM6_BIT_NO               ( 4 )                   /**< ILCn Register L0TM6 bit No.   */
#define ILCn_LnTM7_BIT_NO               ( 6 )                   /**< ILCn Register L0TM7 bit No.   */
#define ILCn_LnFTM0_BIT_NO              ( 0 )                   /**< ILCn Register L0FTM0 bit No.  */
#define ILCn_LnFTM1_BIT_NO              ( 2 )                   /**< ILCn Register L0FTM1 bit No.  */
#define ILCn_LnFTM2_BIT_NO              ( 4 )                   /**< ILCn Register L0FTM2 bit No.  */
#define ILCn_LnFTM3_BIT_NO              ( 6 )                   /**< ILCn Register L0FTM3 bit No.  */
#define ILCn_LnLTBC0_BIT_NO             ( 0 )                   /**< ILCn Register L0LTBC0 bit No. */
#define ILCn_LnLTBC1_BIT_NO             ( 2 )                   /**< ILCn Register L0LTBC1 bit No. */
#define ILCn_LnLTBC2_BIT_NO             ( 4 )                   /**< ILCn Register L0LTBC2 bit No. */

/* EXICONn */
#define EXI0_EDGE_BIT_NO                ( 0 )                   /**< EXICONn Register (EXI0 edge configuration) bit No.  */
#define EXI1_EDGE_BIT_NO                ( 1 )                   /**< EXICONn Register (EXI1 edge configuration) bit No.  */
#define EXI2_EDGE_BIT_NO                ( 2 )                   /**< EXICONn Register (EXI2 edge configuration) bit No.  */
#define EXI3_EDGE_BIT_NO                ( 3 )                   /**< EXICONn Register (EXI3 edge configuration) bit No.  */
#define EXI4_EDGE_BIT_NO                ( 4 )                   /**< EXICONn Register (EXI4 edge configuration) bit No.  */
#define EXI5_EDGE_BIT_NO                ( 5 )                   /**< EXICONn Register (EXI5 edge configuration) bit No.  */
#define EXI6_EDGE_BIT_NO                ( 6 )                   /**< EXICONn Register (EXI6 edge configuration) bit No.  */
#define EXI7_EDGE_BIT_NO                ( 7 )                   /**< EXICONn Register (EXI7 edge configuration) bit No.  */

#define EXI0_SAMPLE_BIT_NO              ( 0 )                   /**< EXICONn Register (EXI0 sampling configuration) bit No.  */
#define EXI1_SAMPLE_BIT_NO              ( 1 )                   /**< EXICONn Register (EXI1 sampling configuration) bit No.  */
#define EXI2_SAMPLE_BIT_NO              ( 2 )                   /**< EXICONn Register (EXI2 sampling configuration) bit No.  */
#define EXI3_SAMPLE_BIT_NO              ( 3 )                   /**< EXICONn Register (EXI3 sampling configuration) bit No.  */
#define EXI4_SAMPLE_BIT_NO              ( 4 )                   /**< EXICONn Register (EXI4 sampling configuration) bit No.  */
#define EXI5_SAMPLE_BIT_NO              ( 5 )                   /**< EXICONn Register (EXI5 sampling configuration) bit No.  */
#define EXI6_SAMPLE_BIT_NO              ( 6 )                   /**< EXICONn Register (EXI6 sampling configuration) bit No.  */
#define EXI7_SAMPLE_BIT_NO              ( 7 )                   /**< EXICONn Register (EXI7 sampling configuration) bit No.  */

#define EXI0_FILTER_BIT_NO              ( 0 )                   /**< EXICONn Register (EXI0 filtering configuration) bit No.  */
#define EXI1_FILTER_BIT_NO              ( 1 )                   /**< EXICONn Register (EXI1 filtering configuration) bit No.  */
#define EXI2_FILTER_BIT_NO              ( 2 )                   /**< EXICONn Register (EXI2 filtering configuration) bit No.  */
#define EXI3_FILTER_BIT_NO              ( 3 )                   /**< EXICONn Register (EXI3 filtering configuration) bit No.  */
#define EXI4_FILTER_BIT_NO              ( 4 )                   /**< EXICONn Register (EXI4 filtering configuration) bit No.  */
#define EXI5_FILTER_BIT_NO              ( 5 )                   /**< EXICONn Register (EXI5 filtering configuration) bit No.  */
#define EXI6_FILTER_BIT_NO              ( 6 )                   /**< EXICONn Register (EXI6 filtering configuration) bit No.  */
#define EXI7_FILTER_BIT_NO              ( 7 )                   /**< EXICONn Register (EXI7 filtering configuration) bit No.  */

/*=== API parameters value ===*/
/* external interrupt edge */
#define EXIn_EDGE_DISABLE               ( 0 )                   /**< [EXIn]Interrupt disabled             */
#define EXIn_EDGE_FALLING               ( 1 )                   /**< [EXIn]Falling-edge interrupt mode    */
#define EXIn_EDGE_RISING                ( 2 )                   /**< [EXIn]Rising-edge interrupt mode     */
#define EXIn_EDGE_BOTH                  ( 3 )                   /**< [EXIn]Both-edge interrupt mode       */

/* external interrupt sampling mode */
#define EXIn_SAMPLING_DIS               ( 0 )                   /**< [EXIn]Without sampling               */
#define EXIn_SAMPLING_ENA               ( 1 )                   /**< [EXIn]With sampling                  */

/* external interrupt filter mode */
#define EXIn_FILTER_DIS                 ( 0 )                   /**< [EXIn]Without noise filter           */
#define EXIn_FILTER_ENA                 ( 1 )                   /**< [EXIn]With noise filter              */

/* external interrupt port */
#define EXIn_PORT_SEL_P00               ( 0x00 | 0x00 )         /**< [EXIn]Select Port (P00)              */
#define EXIn_PORT_SEL_P01               ( 0x00 | 0x01 )         /**< [EXIn]Select Port (P01)              */
#define EXIn_PORT_SEL_P02               ( 0x00 | 0x02 )         /**< [EXIn]Select Port (P02)              */
#define EXIn_PORT_SEL_P03               ( 0x00 | 0x03 )         /**< [EXIn]Select Port (P03)              */
#define EXIn_PORT_SEL_P04               ( 0x00 | 0x04 )         /**< [EXIn]Select Port (P04)              */
#define EXIn_PORT_SEL_P05               ( 0x00 | 0x05 )         /**< [EXIn]Select Port (P05)              */
#define EXIn_PORT_SEL_PXT0              ( 0x10 | 0x00 )         /**< [EXIn]Select Port (PXT0)             */
#define EXIn_PORT_SEL_PXT1              ( 0x10 | 0x01 )         /**< [EXIn]Select Port (PXT1)             */
#define EXIn_PORT_SEL_P20               ( 0x20 | 0x00 )         /**< [EXIn]Select Port (P20)              */
#define EXIn_PORT_SEL_P21               ( 0x20 | 0x01 )         /**< [EXIn]Select Port (P21)              */
#define EXIn_PORT_SEL_P22               ( 0x20 | 0x02 )         /**< [EXIn]Select Port (P22)              */
#define EXIn_PORT_SEL_P23               ( 0x20 | 0x03 )         /**< [EXIn]Select Port (P23)              */
#define EXIn_PORT_SEL_P30               ( 0x30 | 0x00 )         /**< [EXIn]Select Port (P30)              */
#define EXIn_PORT_SEL_P31               ( 0x30 | 0x01 )         /**< [EXIn]Select Port (P31)              */
#define EXIn_PORT_SEL_P32               ( 0x30 | 0x02 )         /**< [EXIn]Select Port (P32)              */
#define EXIn_PORT_SEL_P33               ( 0x30 | 0x03 )         /**< [EXIn]Select Port (P33)              */
#define EXIn_PORT_SEL_P34               ( 0x30 | 0x04 )         /**< [EXIn]Select Port (P34)              */
#define EXIn_PORT_SEL_P35               ( 0x30 | 0x05 )         /**< [EXIn]Select Port (P35)              */
#define EXIn_PORT_SEL_P36               ( 0x30 | 0x06 )         /**< [EXIn]Select Port (P36)              */
#define EXIn_PORT_SEL_P37               ( 0x30 | 0x07 )         /**< [EXIn]Select Port (P37)              */
#define EXIn_PORT_SEL_P40               ( 0x40 | 0x00 )         /**< [EXIn]Select Port (P40)              */
#define EXIn_PORT_SEL_P41               ( 0x40 | 0x01 )         /**< [EXIn]Select Port (P41)              */
#define EXIn_PORT_SEL_P42               ( 0x40 | 0x02 )         /**< [EXIn]Select Port (P42)              */
#define EXIn_PORT_SEL_P43               ( 0x40 | 0x03 )         /**< [EXIn]Select Port (P43)              */
#define EXIn_PORT_SEL_P44               ( 0x40 | 0x04 )         /**< [EXIn]Select Port (P44)              */
#define EXIn_PORT_SEL_P45               ( 0x40 | 0x05 )         /**< [EXIn]Select Port (P45)              */
#define EXIn_PORT_SEL_P46               ( 0x40 | 0x06 )         /**< [EXIn]Select Port (P46)              */
#define EXIn_PORT_SEL_P47               ( 0x40 | 0x07 )         /**< [EXIn]Select Port (P47)              */
#define EXIn_PORT_SEL_P50               ( 0x50 | 0x00 )         /**< [EXIn]Select Port (P50)              */
#define EXIn_PORT_SEL_P51               ( 0x50 | 0x01 )         /**< [EXIn]Select Port (P51)              */
#define EXIn_PORT_SEL_P52               ( 0x50 | 0x02 )         /**< [EXIn]Select Port (P52)              */
#define EXIn_PORT_SEL_P53               ( 0x50 | 0x03 )         /**< [EXIn]Select Port (P53)              */
#define EXIn_PORT_SEL_P54               ( 0x50 | 0x04 )         /**< [EXIn]Select Port (P54)              */
#define EXIn_PORT_SEL_P55               ( 0x50 | 0x05 )         /**< [EXIn]Select Port (P55)              */
#define EXIn_PORT_SEL_P56               ( 0x50 | 0x06 )         /**< [EXIn]Select Port (P56)              */
#define EXIn_PORT_SEL_P57               ( 0x50 | 0x07 )         /**< [EXIn]Select Port (P57)              */

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/*--- Enables interrupts ---*/
#define         irq_ext0_ena()          set_bit( EEXI0 )        /**< Enables EXI0INT  */
#define         irq_ext1_ena()          set_bit( EEXI1 )        /**< Enables EXI1INT  */
#define         irq_ext2_ena()          set_bit( EEXI2 )        /**< Enables EXI2INT  */
#define         irq_ext3_ena()          set_bit( EEXI3 )        /**< Enables EXI3INT  */
#define         irq_ext4_ena()          set_bit( EEXI4 )        /**< Enables EXI4INT  */
#define         irq_ext5_ena()          set_bit( EEXI5 )        /**< Enables EXI5INT  */
#define         irq_ext6_ena()          set_bit( EEXI6 )        /**< Enables EXI6INT  */
#define         irq_ext7_ena()          set_bit( EEXI7 )        /**< Enables EXI7INT  */
#define         irq_sio0_ena()          set_bit( ESIO0 )        /**< Enables SIO0INT  */
#define         irq_siof0_ena()         set_bit( ESIOF0 )       /**< Enables SIOF0INT */
#define         irq_i2c0_ena()          set_bit( EI2C0 )        /**< Enables I2C0INT  */
#define         irq_i2c1_ena()          set_bit( EI2C1 )        /**< Enables I2C1INT  */
#define         irq_ua0_ena()           set_bit( EUA0 )         /**< Enables UA0INT   */
#define         irq_ua1_ena()           set_bit( EUA1 )         /**< Enables UA1INT   */
#define         irq_uaf0_ena()          set_bit( EUAF0 )        /**< Enables UAF0INT  */
#define         irq_losc_ena()          set_bit( ELOSC )        /**< Enables LOSCINT  */
#define         irq_vls_ena()           set_bit( EVLS )         /**< Enables VLSINT   */
#define         irq_md0_ena()           set_bit( EMD0 )         /**< Enables MD0INT   */
#define         irq_sad_ena()           set_bit( ESAD )         /**< Enables SADINT   */
#define         irq_rad_ena()           set_bit( ERAD )         /**< Enables RADINT   */
#define         irq_cmp0_ena()          set_bit( ECMP0 )        /**< Enables CMP0INT  */
#define         irq_cmp1_ena()          set_bit( ECMP1 )        /**< Enables CMP1INT  */
#define         irq_tm0_ena()           set_bit( ETM0 )         /**< Enables TM0INT   */
#define         irq_tm1_ena()           set_bit( ETM1 )         /**< Enables TM1INT   */
#define         irq_tm2_ena()           set_bit( ETM2 )         /**< Enables TM2INT   */
#define         irq_tm3_ena()           set_bit( ETM3 )         /**< Enables TM3INT   */
#define         irq_tm4_ena()           set_bit( ETM4 )         /**< Enables TM4INT   */
#define         irq_tm5_ena()           set_bit( ETM5 )         /**< Enables TM5INT   */
#define         irq_tm6_ena()           set_bit( ETM6 )         /**< Enables TM6INT   */
#define         irq_tm7_ena()           set_bit( ETM7 )         /**< Enables TM7INT   */
#define         irq_ftm0_ena()          set_bit( EFTM0 )        /**< Enables FTM0INT  */
#define         irq_ftm1_ena()          set_bit( EFTM1 )        /**< Enables FTM1INT  */
#define         irq_ftm2_ena()          set_bit( EFTM2 )        /**< Enables FTM2INT  */
#define         irq_ftm3_ena()          set_bit( EFTM3 )        /**< Enables FTM3INT  */
#define         irq_tbc0_ena()          set_bit( ELTBC0 )       /**< Enables LTBC0INT */
#define         irq_tbc1_ena()          set_bit( ELTBC1 )       /**< Enables LTBC1INT */
#define         irq_tbc2_ena()          set_bit( ELTBC2 )       /**< Enables LTBC2INT */

/*--- Disables interrupts ---*/
#define         irq_ext0_dis()          clear_bit( EEXI0 )        /**< Disables EXI0INT */
#define         irq_ext1_dis()          clear_bit( EEXI1 )        /**< Disables EXI1INT */
#define         irq_ext2_dis()          clear_bit( EEXI2 )        /**< Disables EXI2INT */
#define         irq_ext3_dis()          clear_bit( EEXI3 )        /**< Disables EXI3INT */
#define         irq_ext4_dis()          clear_bit( EEXI4 )        /**< Disables EXI4INT */
#define         irq_ext5_dis()          clear_bit( EEXI5 )        /**< Disables EXI5INT */
#define         irq_ext6_dis()          clear_bit( EEXI6 )        /**< Disables EXI6INT */
#define         irq_ext7_dis()          clear_bit( EEXI7 )        /**< Disables EXI7INT */
#define         irq_sio0_dis()          clear_bit( ESIO0 )        /**< Disables SIO0INT */
#define         irq_siof0_dis()         clear_bit( ESIOF0 )       /**< Disables SIOF0INT*/
#define         irq_i2c0_dis()          clear_bit( EI2C0 )        /**< Disables I2C0INT */
#define         irq_i2c1_dis()          clear_bit( EI2C1 )        /**< Disables I2C1INT */
#define         irq_ua0_dis()           clear_bit( EUA0 )         /**< Disables UA0INT  */
#define         irq_ua1_dis()           clear_bit( EUA1 )         /**< Disables UA1INT  */
#define         irq_uaf0_dis()          clear_bit( EUAF0 )        /**< Disables UAF0INT */
#define         irq_losc_dis()          clear_bit( ELOSC )        /**< Disables LOSCINT */
#define         irq_vls_dis()           clear_bit( EVLS )         /**< Disables VLSINT  */
#define         irq_md0_dis()           clear_bit( EMD0 )         /**< Disables MD0INT  */
#define         irq_sad_dis()           clear_bit( ESAD )         /**< Disables SADINT  */
#define         irq_rad_dis()           clear_bit( ERAD )         /**< Disables RADINT  */
#define         irq_cmp0_dis()          clear_bit( ECMP0 )        /**< Disables CMP0INT */
#define         irq_cmp1_dis()          clear_bit( ECMP1 )        /**< Disables CMP1INT */
#define         irq_tm0_dis()           clear_bit( ETM0 )         /**< Disables TM0INT  */
#define         irq_tm1_dis()           clear_bit( ETM1 )         /**< Disables TM1INT  */
#define         irq_tm2_dis()           clear_bit( ETM2 )         /**< Disables TM2INT  */
#define         irq_tm3_dis()           clear_bit( ETM3 )         /**< Disables TM3INT  */
#define         irq_tm4_dis()           clear_bit( ETM4 )         /**< Disables TM4INT  */
#define         irq_tm5_dis()           clear_bit( ETM5 )         /**< Disables TM5INT  */
#define         irq_tm6_dis()           clear_bit( ETM6 )         /**< Disables TM6INT  */
#define         irq_tm7_dis()           clear_bit( ETM7 )         /**< Disables TM7INT  */
#define         irq_ftm0_dis()          clear_bit( EFTM0 )        /**< Disables FTM0INT */
#define         irq_ftm1_dis()          clear_bit( EFTM1 )        /**< Disables FTM1INT */
#define         irq_ftm2_dis()          clear_bit( EFTM2 )        /**< Disables FTM2INT */
#define         irq_ftm3_dis()          clear_bit( EFTM3 )        /**< Disables FTM3INT */
#define         irq_tbc0_dis()          clear_bit( ELTBC0 )       /**< Disables LTBC0INT*/
#define         irq_tbc1_dis()          clear_bit( ELTBC1 )       /**< Disables LTBC1INT*/
#define         irq_tbc2_dis()          clear_bit( ELTBC2 )       /**< Disables LTBC2INT*/

/*--- Check IRQ ---*/
#define         irq_wdt_checkIRQ()      get_bit( QWDT )           /**< Check WDTINT     */
#define         irq_ext0_checkIRQ()     get_bit( QEXI0 )          /**< Check EXI0INT    */
#define         irq_ext1_checkIRQ()     get_bit( QEXI1 )          /**< Check EXI1INT    */
#define         irq_ext2_checkIRQ()     get_bit( QEXI2 )          /**< Check EXI2INT    */
#define         irq_ext3_checkIRQ()     get_bit( QEXI3 )          /**< Check EXI3INT    */
#define         irq_ext4_checkIRQ()     get_bit( QEXI4 )          /**< Check EXI4INT    */
#define         irq_ext5_checkIRQ()     get_bit( QEXI5 )          /**< Check EXI5INT    */
#define         irq_ext6_checkIRQ()     get_bit( QEXI6 )          /**< Check EXI6INT    */
#define         irq_ext7_checkIRQ()     get_bit( QEXI7 )          /**< Check EXI7INT    */
#define         irq_sio0_checkIRQ()     get_bit( QSIO0 )          /**< Check SIO0INT    */
#define         irq_siof0_checkIRQ()    get_bit( QSIOF0 )         /**< Check SIOF0INT   */
#define         irq_i2c0_checkIRQ()     get_bit( QI2C0 )          /**< Check I2C0INT    */
#define         irq_i2c1_checkIRQ()     get_bit( QI2C1 )          /**< Check I2C1INT    */
#define         irq_ua0_checkIRQ()      get_bit( QUA0 )           /**< Check UA0INT     */
#define         irq_ua1_checkIRQ()      get_bit( QUA1 )           /**< Check UA1INT     */
#define         irq_uaf0_checkIRQ()     get_bit( QUAF0 )          /**< Check UAF0INT    */
#define         irq_losc_checkIRQ()     get_bit( QLOSC )          /**< Check LOSCINT    */
#define         irq_vls_checkIRQ()      get_bit( QVLS )           /**< Check VLSINT     */
#define         irq_md0_checkIRQ()      get_bit( QMD0 )           /**< Check MD0INT     */
#define         irq_sad_checkIRQ()      get_bit( QSAD )           /**< Check SADINT     */
#define         irq_rad_checkIRQ()      get_bit( QRAD )           /**< Check RADINT     */
#define         irq_cmp0_checkIRQ()     get_bit( QCMP0 )          /**< Check CMP0INT    */
#define         irq_cmp1_checkIRQ()     get_bit( QCMP1 )          /**< Check CMP1INT    */
#define         irq_tm0_checkIRQ()      get_bit( QTM0 )           /**< Check TM0INT     */
#define         irq_tm1_checkIRQ()      get_bit( QTM1 )           /**< Check TM1INT     */
#define         irq_tm2_checkIRQ()      get_bit( QTM2 )           /**< Check TM2INT     */
#define         irq_tm3_checkIRQ()      get_bit( QTM3 )           /**< Check TM3INT     */
#define         irq_tm4_checkIRQ()      get_bit( QTM4 )           /**< Check TM4INT     */
#define         irq_tm5_checkIRQ()      get_bit( QTM5 )           /**< Check TM5INT     */
#define         irq_tm6_checkIRQ()      get_bit( QTM6 )           /**< Check TM6INT     */
#define         irq_tm7_checkIRQ()      get_bit( QTM7 )           /**< Check TM7INT     */
#define         irq_ftm0_checkIRQ()     get_bit( QFTM0 )          /**< Check FTM0INT    */
#define         irq_ftm1_checkIRQ()     get_bit( QFTM1 )          /**< Check FTM1INT    */
#define         irq_ftm2_checkIRQ()     get_bit( QFTM2 )          /**< Check FTM2INT    */
#define         irq_ftm3_checkIRQ()     get_bit( QFTM3 )          /**< Check FTM3INT    */
#define         irq_tbc0_checkIRQ()     get_bit( QLTBC0 )         /**< Check LTBC0INT   */
#define         irq_tbc1_checkIRQ()     get_bit( QLTBC1 )         /**< Check LTBC1INT   */
#define         irq_tbc2_checkIRQ()     get_bit( QLTBC2 )         /**< Check LTBC2INT   */

/*--- Clear IRQ ---*/
#define         irq_wdt_clearIRQ()      clear_bit( QWDT )         /**< Clear WDTINT     */
#define         irq_ext0_clearIRQ()     clear_bit( QEXI0 )        /**< Clear EXI0INT    */
#define         irq_ext1_clearIRQ()     clear_bit( QEXI1 )        /**< Clear EXI1INT    */
#define         irq_ext2_clearIRQ()     clear_bit( QEXI2 )        /**< Clear EXI2INT    */
#define         irq_ext3_clearIRQ()     clear_bit( QEXI3 )        /**< Clear EXI3INT    */
#define         irq_ext4_clearIRQ()     clear_bit( QEXI4 )        /**< Clear EXI4INT    */
#define         irq_ext5_clearIRQ()     clear_bit( QEXI5 )        /**< Clear EXI5INT    */
#define         irq_ext6_clearIRQ()     clear_bit( QEXI6 )        /**< Clear EXI6INT    */
#define         irq_ext7_clearIRQ()     clear_bit( QEXI7 )        /**< Clear EXI7INT    */
#define         irq_sio0_clearIRQ()     clear_bit( QSIO0 )        /**< Clear SIO0INT    */
#define         irq_siof0_clearIRQ()    clear_bit( QSIOF0 )       /**< Clear SIOF0INT   */
#define         irq_i2c0_clearIRQ()     clear_bit( QI2C0 )        /**< Clear I2C0INT    */
#define         irq_i2c1_clearIRQ()     clear_bit( QI2C1 )        /**< Clear I2C1INT    */
#define         irq_ua0_clearIRQ()      clear_bit( QUA0 )         /**< Clear UA0INT     */
#define         irq_ua1_clearIRQ()      clear_bit( QUA1 )         /**< Clear UA1INT     */
#define         irq_uaf0_clearIRQ()     clear_bit( QUAF0 )        /**< Clear UAF0INT    */
#define         irq_losc_clearIRQ()     clear_bit( QLOSC )        /**< Clear LOSCINT    */
#define         irq_vls_clearIRQ()      clear_bit( QVLS )         /**< Clear VLSINT     */
#define         irq_md0_clearIRQ()      clear_bit( QMD0 )         /**< Clear MD0INT     */
#define         irq_sad_clearIRQ()      clear_bit( QSAD )         /**< Clear SADINT     */
#define         irq_rad_clearIRQ()      clear_bit( QRAD )         /**< Clear RADINT     */
#define         irq_cmp0_clearIRQ()     clear_bit( QCMP0 )        /**< Clear CMP0INT    */
#define         irq_cmp1_clearIRQ()     clear_bit( QCMP1 )        /**< Clear CMP1INT    */
#define         irq_tm0_clearIRQ()      clear_bit( QTM0 )         /**< Clear TM0INT     */
#define         irq_tm1_clearIRQ()      clear_bit( QTM1 )         /**< Clear TM1INT     */
#define         irq_tm2_clearIRQ()      clear_bit( QTM2 )         /**< Clear TM2INT     */
#define         irq_tm3_clearIRQ()      clear_bit( QTM3 )         /**< Clear TM3INT     */
#define         irq_tm4_clearIRQ()      clear_bit( QTM4 )         /**< Clear TM4INT     */
#define         irq_tm5_clearIRQ()      clear_bit( QTM5 )         /**< Clear TM5INT     */
#define         irq_tm6_clearIRQ()      clear_bit( QTM6 )         /**< Clear TM6INT     */
#define         irq_tm7_clearIRQ()      clear_bit( QTM7 )         /**< Clear TM7INT     */
#define         irq_ftm0_clearIRQ()     clear_bit( QFTM0 )        /**< Clear FTM0INT    */
#define         irq_ftm1_clearIRQ()     clear_bit( QFTM1 )        /**< Clear FTM1INT    */
#define         irq_ftm2_clearIRQ()     clear_bit( QFTM2 )        /**< Clear FTM2INT    */
#define         irq_ftm3_clearIRQ()     clear_bit( QFTM3 )        /**< Clear FTM3INT    */
#define         irq_tbc0_clearIRQ()     clear_bit( QLTBC0 )       /**< Clear LTBC0INT   */
#define         irq_tbc1_clearIRQ()     clear_bit( QLTBC1 )       /**< Clear LTBC1INT   */
#define         irq_tbc2_clearIRQ()     clear_bit( QLTBC2 )       /**< Clear LTBC2INT   */

/*--- Control interrupts priority level ---*/
/**
 * Enables interrupts priority level control
 *
 * @param   -
 * @return  None
 */
#define         irq_enaLevelCtrl()      set_bit( ILE )
/**
 * Disables interrupts priority level control
 *
 * @param   -
 * @return  None
 */
#define         irq_disLevelCtrl()      clear_bit( ILE )
/**
 * Get executing interrupts priority level
 *
 * @param   -
 * @return  The state of executing interrupts priority level
 *          (The value of CILL register)
 */
#define         irq_getLevelState()     read_reg8( CILL )

/**
 * Clear executing interrupts priority level
 *
 * @param   -
 * @return  None
 * @note    If interrupts priority level control is enabled,
 *          You must call this macro in end of ALL interrupts handler.
 */
#define         irq_clearLevelState()   write_reg8( CILL, 0 )


/*--- Setting interrupts priority level ---*/
#define         irq_common_setLevel( symbol, bit, level )       do {                                                                    \
                                                                    /* Setting register */                                              \
                                                                    clear_reg8( (symbol), (unsigned char)(0x3 << (bit)) );              \
                                                                    set_reg8( (symbol), (unsigned char)(((level) & 0x3) << (bit)) );    \
                                                                } while(0)
/**
 * Setting interrupt priority level (EXI0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after EXI0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ext0_setLevel( level )                      irq_common_setLevel( ILC1L, ILCn_LnEXI0_BIT_NO, level )

/**
 * Setting interrupt priority level (EXI1INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after EXI1INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ext1_setLevel( level )                      irq_common_setLevel( ILC1L, ILCn_LnEXI1_BIT_NO, level )

/**
 * Setting interrupt priority level (EXI2INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after EXI2INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ext2_setLevel( level )                      irq_common_setLevel( ILC1L, ILCn_LnEXI2_BIT_NO, level )

/**
 * Setting interrupt priority level (EXI3INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after EXI3INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ext3_setLevel( level )                      irq_common_setLevel( ILC1L, ILCn_LnEXI3_BIT_NO, level )

/**
 * Setting interrupt priority level (EXI4INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after EXI4INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ext4_setLevel( level )                      irq_common_setLevel( ILC1H, ILCn_LnEXI4_BIT_NO, level )

/**
 * Setting interrupt priority level (EXI5INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after EXI5INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ext5_setLevel( level )                      irq_common_setLevel( ILC1H, ILCn_LnEXI5_BIT_NO, level )

/**
 * Setting interrupt priority level (EXI6INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after EXI6INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ext6_setLevel( level )                      irq_common_setLevel( ILC1H, ILCn_LnEXI6_BIT_NO, level )

/**
 * Setting interrupt priority level (EXI7INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after EXI7INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ext7_setLevel( level )                      irq_common_setLevel( ILC1H, ILCn_LnEXI7_BIT_NO, level )


/**
 * Setting interrupt priority level (SIO0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after SIO0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_sio0_setLevel( level )                      irq_common_setLevel( ILC2L, ILCn_LnSIO0_BIT_NO, level )


/**
 * Setting interrupt priority level (SIOF0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after SIOF0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_siof0_setLevel( level )                     irq_common_setLevel( ILC2L, ILCn_LnSIOF0_BIT_NO, level )


/**
 * Setting interrupt priority level (I2C0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after I2C0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_i2c0_setLevel( level )                      irq_common_setLevel( ILC2L, ILCn_LnI2C0_BIT_NO, level )


/**
 * Setting interrupt priority level (I2C1INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after I2C1INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_i2c1_setLevel( level )                      irq_common_setLevel( ILC2L, ILCn_LnI2C1_BIT_NO, level )

/**
 * Setting interrupt priority level (UA0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after UA0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ua0_setLevel( level )                       irq_common_setLevel( ILC2H, ILCn_LnUA0_BIT_NO, level )

/**
 * Setting interrupt priority level (UA1INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after UA1INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ua1_setLevel( level )                       irq_common_setLevel( ILC2H, ILCn_LnUA1_BIT_NO, level )

/**
 * Setting interrupt priority level (UAF0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after UAF0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_uaf0_setLevel( level )                      irq_common_setLevel( ILC2H, ILCn_LnUAF0_BIT_NO, level )

/**
 * Setting interrupt priority level (LOSCINT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after LOSCINT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_losc_setLevel( level )                      irq_common_setLevel( ILC3H, ILCn_LnLOSC_BIT_NO, level )

/**
 * Setting interrupt priority level (VLSINT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after VLSINT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_vls_setLevel( level )                       irq_common_setLevel( ILC3H, ILCn_LnVLS_BIT_NO, level )

/**
 * Setting interrupt priority level (MD0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after MD0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_md0_setLevel( level )                       irq_common_setLevel( ILC3H, ILCn_LnMD0_BIT_NO, level )

/**
 * Setting interrupt priority level (SADINT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after SADINT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_sad_setLevel( level )                       irq_common_setLevel( ILC4L, ILCn_LnSAD_BIT_NO, level )

/**
 * Setting interrupt priority level (RADINT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after RADINT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_rad_setLevel( level )                       irq_common_setLevel( ILC4L, ILCn_LnRAD_BIT_NO, level )

/**
 * Setting interrupt priority level (CMP0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after CMP0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_cmp0_setLevel( level )                      irq_common_setLevel( ILC4H, ILCn_LnCMP0_BIT_NO, level )

/**
 * Setting interrupt priority level (CMP1INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after CMP1INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_cmp1_setLevel( level )                      irq_common_setLevel( ILC4H, ILCn_LnCMP1_BIT_NO, level )

/**
 * Setting interrupt priority level (TM0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after TM0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tm0_setLevel( level )                       irq_common_setLevel( ILC5L, ILCn_LnTM0_BIT_NO, level )

/**
 * Setting interrupt priority level (TM1INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after TM1INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tm1_setLevel( level )                       irq_common_setLevel( ILC5L, ILCn_LnTM1_BIT_NO, level )

/**
 * Setting interrupt priority level (TM2INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after TM2INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tm2_setLevel( level )                       irq_common_setLevel( ILC5L, ILCn_LnTM2_BIT_NO, level )

/**
 * Setting interrupt priority level (TM3INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after TM3INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tm3_setLevel( level )                       irq_common_setLevel( ILC5L, ILCn_LnTM3_BIT_NO, level )

/**
 * Setting interrupt priority level (TM4INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after TM4INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tm4_setLevel( level )                       irq_common_setLevel( ILC5H, ILCn_LnTM4_BIT_NO, level )

/**
 * Setting interrupt priority level (TM5INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after TM5INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tm5_setLevel( level )                       irq_common_setLevel( ILC5H, ILCn_LnTM5_BIT_NO, level )

/**
 * Setting interrupt priority level (TM6INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after TM6INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tm6_setLevel( level )                       irq_common_setLevel( ILC5H, ILCn_LnTM6_BIT_NO, level )

/**
 * Setting interrupt priority level (TM7INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after TM7INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tm7_setLevel( level )                       irq_common_setLevel( ILC5H, ILCn_LnTM7_BIT_NO, level )

/**
 * Setting interrupt priority level (FTM0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after FTM0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ftm0_setLevel( level )                      irq_common_setLevel( ILC6L, ILCn_LnFTM0_BIT_NO, level )

/**
 * Setting interrupt priority level (FTM1INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after FTM1INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ftm1_setLevel( level )                      irq_common_setLevel( ILC6L, ILCn_LnFTM1_BIT_NO, level )

/**
 * Setting interrupt priority level (FTM2INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after FTM2INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ftm2_setLevel( level )                      irq_common_setLevel( ILC6L, ILCn_LnFTM2_BIT_NO, level )

/**
 * Setting interrupt priority level (FTM3INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after FTM3INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_ftm3_setLevel( level )                      irq_common_setLevel( ILC6L, ILCn_LnFTM3_BIT_NO, level )

/**
 * Setting interrupt priority level (LTBC0INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after LTBC0INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tbc0_setLevel( level )                      irq_common_setLevel( ILC7L, ILCn_LnLTBC0_BIT_NO, level )

/**
 * Setting interrupt priority level (LTBC1INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after LTBC1INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tbc1_setLevel( level )                      irq_common_setLevel( ILC7L, ILCn_LnLTBC1_BIT_NO, level )

/**
 * Setting interrupt priority level (LTBC2INT)
 *
 * @param[in]   level   Setting value of interrupt priority level <br>
 *                      (from 0 to 3)
 * @return      None
 *
 * @note    Don't call this macro, after LTBC2INT enabled.
 * @note    Don't call this macro, in the state that disabled the interrupt priority level control.
 */
#define         irq_tbc2_setLevel( level )                      irq_common_setLevel( ILC7L, ILCn_LnLTBC2_BIT_NO, level )


/*--- Setting external interrupt parameters ---*/
void            irq_ext0_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port );
void            irq_ext1_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port );
void            irq_ext2_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port );
void            irq_ext3_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port );
void            irq_ext4_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port );
void            irq_ext5_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port );
void            irq_ext6_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port );
void            irq_ext7_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port );

#endif /*_IRQ_H_*/





