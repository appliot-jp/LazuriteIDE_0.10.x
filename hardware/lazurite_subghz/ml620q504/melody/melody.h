/* FILE NAME: melody.h
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

#ifndef _MELODY_H_
#define _MELODY_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/*=== bit field of registers ===*/
/* MD0TMP */
#define MD0TMP_M0TM0        ( 0x01 )                                                     /**< MD0TMP Register M0TM0 bit */
#define MD0TMP_M0TM1        ( 0x02 )                                                     /**< MD0TMP Register M0TM1 bit */
#define MD0TMP_M0TM2        ( 0x04 )                                                     /**< MD0TMP Register M0TM2 bit */
#define MD0TMP_M0TM3        ( 0x08 )                                                     /**< MD0TMP Register M0TM3 bit */

/* MD0TL */
#define MD0TL_M0TN0         ( 0x0001 )                                                   /**< MD0TL Register M0TN0 bit  */
#define MD0TL_M0TN1         ( 0x0002 )                                                   /**< MD0TL Register M0TN1 bit  */
#define MD0TL_M0TN2         ( 0x0004 )                                                   /**< MD0TL Register M0TN2 bit  */
#define MD0TL_M0TN3         ( 0x0008 )                                                   /**< MD0TL Register M0TN3 bit  */
#define MD0TL_M0TN4         ( 0x0010 )                                                   /**< MD0TL Register M0TN4 bit  */
#define MD0TL_M0TN5         ( 0x0020 )                                                   /**< MD0TL Register M0TN5 bit  */
#define MD0TL_M0TN6         ( 0x0040 )                                                   /**< MD0TL Register M0TN6 bit  */
#define MD0TL_M0LN0         ( 0x0100 )                                                   /**< MD0TL Register M0LN0 bit  */
#define MD0TL_M0LN1         ( 0x0200 )                                                   /**< MD0TL Register M0LN1 bit  */
#define MD0TL_M0LN2         ( 0x0400 )                                                   /**< MD0TL Register M0LN2 bit  */
#define MD0TL_M0LN3         ( 0x0800 )                                                   /**< MD0TL Register M0LN3 bit  */
#define MD0TL_M0LN4         ( 0x1000 )                                                   /**< MD0TL Register M0LN4 bit  */
#define MD0TL_M0LN5         ( 0x2000 )                                                   /**< MD0TL Register M0LN5 bit  */

/*=== API Parameter value. ===*/
/* Parameters for melody_start() */
/* tempo */
#define MELODY_TEMPO_480    ( MD0TMP_M0TM0 )                                              /**< melody tempo : 480  */
#define MELODY_TEMPO_320    ( MD0TMP_M0TM1 )                                              /**< melody tempo : 320  */
#define MELODY_TEMPO_240    ( MD0TMP_M0TM1 | MD0TMP_M0TM0 )                               /**< melody tempo : 240  */
#define MELODY_TEMPO_192    ( MD0TMP_M0TM2 )                                              /**< melody tempo : 192  */
#define MELODY_TEMPO_160    ( MD0TMP_M0TM2 | MD0TMP_M0TM0 )                               /**< melody tempo : 160  */
#define MELODY_TEMPO_137    ( MD0TMP_M0TM2 | MD0TMP_M0TM1 )                               /**< melody tempo : 137  */
#define MELODY_TEMPO_120    ( MD0TMP_M0TM2 | MD0TMP_M0TM1 | MD0TMP_M0TM0 )                /**< melody tempo : 120  */
#define MELODY_TEMPO_107    ( MD0TMP_M0TM3 )                                              /**< melody tempo : 107  */
#define MELODY_TEMPO_96     ( MD0TMP_M0TM3 | MD0TMP_M0TM0 )                               /**< melody tempo : 96   */
#define MELODY_TEMPO_87     ( MD0TMP_M0TM3 | MD0TMP_M0TM1 )                               /**< melody tempo : 87   */
#define MELODY_TEMPO_80     ( MD0TMP_M0TM3 | MD0TMP_M0TM1 | MD0TMP_M0TM0 )                /**< melody tempo : 80   */
#define MELODY_TEMPO_74     ( MD0TMP_M0TM3 | MD0TMP_M0TM2 )                               /**< melody tempo : 74   */
#define MELODY_TEMPO_69     ( MD0TMP_M0TM3 | MD0TMP_M0TM2 | MD0TMP_M0TM0 )                /**< melody tempo : 69   */
#define MELODY_TEMPO_64     ( MD0TMP_M0TM3 | MD0TMP_M0TM2 | MD0TMP_M0TM1 )                /**< melody tempo : 64   */
#define MELODY_TEMPO_60     ( MD0TMP_M0TM3 | MD0TMP_M0TM2 | MD0TMP_M0TM1 | MD0TMP_M0TM0 ) /**< melody tempo : 60   */

/* Parameters for buzzer_start() */
/* buzzer type */
#define BUZZER_TYPE0        ( 0 )                                                         /**< buzzer type 0 : Discontinuous sounds 1 */
#define BUZZER_TYPE1        ( MD0TMP_M0TM0 )                                              /**< buzzer type 1 : Discontinuous sounds 2 */
#define BUZZER_TYPE2        ( MD0TMP_M0TM1 )                                              /**< buzzer type 2 : Single sound           */
#define BUZZER_TYPE3        ( MD0TMP_M0TM1 | MD0TMP_M0TM0 )                               /**< buzzer type 3 : Continuous sound       */

/* buzzer frequency */
#define BUZZER_FREQ_4096HZ  ( 0 )                                                         /**< buzzer output frequency : 4.096kHz     */
#define BUZZER_FREQ_2048HZ  ( MD0TL_M0TN0 )                                               /**< buzzer output frequency : 2.048kHz     */
#define BUZZER_FREQ_1024HZ  ( MD0TL_M0TN1 )                                               /**< buzzer output frequency : 1.024kHz     */
#define BUZZER_FREQ_683HZ   ( MD0TL_M0TN1 | MD0TL_M0TN0 )                                 /**< buzzer output frequency : 683kHz       */
#define BUZZER_FREQ_512HZ   ( MD0TL_M0TN2 )                                               /**< buzzer output frequency : 512Hz        */
#define BUZZER_FREQ_410HZ   ( MD0TL_M0TN2 | MD0TL_M0TN0 )                                 /**< buzzer output frequency : 410Hz        */
#define BUZZER_FREQ_341HZ   ( MD0TL_M0TN2 | MD0TL_M0TN1 )                                 /**< buzzer output frequency : 341Hz        */
#define BUZZER_FREQ_293HZ   ( MD0TL_M0TN2 | MD0TL_M0TN1 | MD0TL_M0TN0 )                   /**< buzzer output frequency : 293Hz        */

/* buzzer duty */
#define BUZZER_DUTY_1_8     ( MD0TL_M0LN0 )                                               /**< buzzer duty :  1/8DUTY   */
#define BUZZER_DUTY_2_8     ( MD0TL_M0LN1 )                                               /**< buzzer duty :  1/8DUTY   */
#define BUZZER_DUTY_3_8     ( MD0TL_M0LN1 | MD0TL_M0LN0 )                                 /**< buzzer duty :  1/8DUTY   */
#define BUZZER_DUTY_4_8     ( MD0TL_M0LN2 )                                               /**< buzzer duty :  2/8DUTY   */
#define BUZZER_DUTY_5_8     ( MD0TL_M0LN2 | MD0TL_M0LN0 )                                 /**< buzzer duty :  2/8DUTY   */
#define BUZZER_DUTY_6_8     ( MD0TL_M0LN2 | MD0TL_M0LN1 )                                 /**< buzzer duty :  3/8DUTY   */
#define BUZZER_DUTY_7_8     ( MD0TL_M0LN2 | MD0TL_M0LN1 | MD0TL_M0LN0 )                   /**< buzzer duty :  3/8DUTY   */
#define BUZZER_DUTY_8_8     ( MD0TL_M0LN3 )                                               /**< buzzer duty :  4/8DUTY   */
#define BUZZER_DUTY_9_8     ( MD0TL_M0LN3 | MD0TL_M0LN0 )                                 /**< buzzer duty :  4/8DUTY   */
#define BUZZER_DUTY_10_8    ( MD0TL_M0LN3 | MD0TL_M0LN1 )                                 /**< buzzer duty :  5/8DUTY   */
#define BUZZER_DUTY_11_8    ( MD0TL_M0LN3 | MD0TL_M0LN1 |MD0TL_M0LN0 )                    /**< buzzer duty :  5/8DUTY   */
#define BUZZER_DUTY_12_8    ( MD0TL_M0LN3 | MD0TL_M0LN2 )                                 /**< buzzer duty :  6/8DUTY   */
#define BUZZER_DUTY_13_8    ( MD0TL_M0LN3 | MD0TL_M0LN2 | MD0TL_M0LN0 )                   /**< buzzer duty :  6/8DUTY   */
#define BUZZER_DUTY_14_8    ( MD0TL_M0LN3 | MD0TL_M0LN2 | MD0TL_M0LN1 )                   /**< buzzer duty :  7/8DUTY   */
#define BUZZER_DUTY_15_8    ( MD0TL_M0LN3 | MD0TL_M0LN2 | MD0TL_M0LN1 | MD0TL_M0LN0 )     /**< buzzer duty :  7/8DUTY   */
#define BUZZER_DUTY_1_16    ( MD0TL_M0LN0 )                                               /**< buzzer duty :  1/16DUTY  */
#define BUZZER_DUTY_2_16    ( MD0TL_M0LN1 )                                               /**< buzzer duty :  1/16DUTY  */
#define BUZZER_DUTY_3_16    ( MD0TL_M0LN1 | MD0TL_M0LN0 )                                 /**< buzzer duty :  1/16DUTY  */
#define BUZZER_DUTY_4_16    ( MD0TL_M0LN2 )                                               /**< buzzer duty :  2/16DUTY  */
#define BUZZER_DUTY_5_16    ( MD0TL_M0LN2 | MD0TL_M0LN0 )                                 /**< buzzer duty :  2/16DUTY  */
#define BUZZER_DUTY_6_16    ( MD0TL_M0LN2 | MD0TL_M0LN1 )                                 /**< buzzer duty :  3/16DUTY  */
#define BUZZER_DUTY_7_16    ( MD0TL_M0LN2 | MD0TL_M0LN1 | MD0TL_M0LN0 )                   /**< buzzer duty :  3/16DUTY  */
#define BUZZER_DUTY_8_16    ( MD0TL_M0LN3 )                                               /**< buzzer duty :  4/16DUTY  */
#define BUZZER_DUTY_9_16    ( MD0TL_M0LN3 | MD0TL_M0LN0 )                                 /**< buzzer duty :  4/16DUTY  */
#define BUZZER_DUTY_10_16   ( MD0TL_M0LN3 | MD0TL_M0LN1 )                                 /**< buzzer duty :  5/16DUTY  */
#define BUZZER_DUTY_11_16   ( MD0TL_M0LN3 | MD0TL_M0LN1 |MD0TL_M0LN0 )                    /**< buzzer duty :  5/16DUTY  */
#define BUZZER_DUTY_12_16   ( MD0TL_M0LN3 | MD0TL_M0LN2 )                                 /**< buzzer duty :  6/16DUTY  */
#define BUZZER_DUTY_13_16   ( MD0TL_M0LN3 | MD0TL_M0LN2 | MD0TL_M0LN0 )                   /**< buzzer duty :  6/16DUTY  */
#define BUZZER_DUTY_14_16   ( MD0TL_M0LN3 | MD0TL_M0LN2 | MD0TL_M0LN1 )                   /**< buzzer duty :  7/16DUTY  */
#define BUZZER_DUTY_15_16   ( MD0TL_M0LN3 | MD0TL_M0LN2 | MD0TL_M0LN1 | MD0TL_M0LN0 )     /**< buzzer duty :  7/16DUTY  */

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Stop melody
 *
 * @param       -
 * @return      None
 */
#define         melody_stop()                 clear_bit( M0RUN )


/**
 * Checking melody output
 *
 * @param       -
 * @retval      0   : Melody is stop
 * @retval      1   : Melody is output
 */
#define         melody_checkOutput()         ( (unsigned char)get_bit( M0RUN ) )

/**
 * Stop buzzer
 *
 * @param       -
 * @return      None
 */
#define         buzzer_stop()                 melody_stop()

void            melody_start( unsigned char mdntmp, unsigned short *data, unsigned int size );
void            melody_continue( void );
void            buzzer_start( unsigned char mdntmp, unsigned short md0tl );

#endif /*_MELODY_H_*/
