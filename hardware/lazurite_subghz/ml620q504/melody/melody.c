/* FILE NAME: melody.c
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


#include "mcu.h"
#include "rdwr_reg.h"
#include "melody.h"

/*############################################################################*/
/*#                                   Type                                   #*/
/*############################################################################*/
/*=== control parameter for MELODY ===*/
typedef struct {
	unsigned short*	data;		/* pointer to melody data	*/
	unsigned int	size;		/* size of output data		*/
} tMelodyCtrlParam;

/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
/*=== control parameter for MELODY ===*/
static tMelodyCtrlParam _gsCtrlParam = { (void *)0 , 0 };

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
/**
 * Start melody
 *
 * @param[in]   mdntmp      Configuration of melody tempo                              <br>
 *                          (Setting value of MD0TMP register)                         <br>
 *                          Set the value of MELODY_TEMPO_480 from MELODY_TEMPO_60.    <br>
 * @param[in]   *data       pointer to area where the melody data is stored
 * @param[in]   size        number of a note code
 * @return      None
 */
void melody_start( unsigned char mdntmp, unsigned short *data, unsigned int size )
{
	/* melody output is stopped. */
	clear_bit( M0RUN );
	
	/* melody scale code and tone length is set. */
	write_reg16( MD0TL, *data );
	
	_gsCtrlParam.data = ++data;
	_gsCtrlParam.size = --size;
	
	/* melody tempo is set. */
	write_reg8( MD0TMP, mdntmp );
	
	/* melody mode is selected. */
	clear_bit( BZMD );
	
	/* melody output is start. */
	set_bit( M0RUN );
}

/**
 * Process to continue melody output
 *
 * @param        -
 * @return      None
 */
void melody_continue( void )
{
	if( _gsCtrlParam.size == 0 ){
		if( read_reg8( MD0TON ) == 0x00){
			/* melody output is stopped. */
			clear_bit( M0RUN );
		}
		else{
			/* rest data is set. */
			write_reg16( MD0TL, 0x0000 );
		}
	}
	else{
		/* next melody data is set. */
		write_reg16( MD0TL, *_gsCtrlParam.data );
		_gsCtrlParam.data++;
		_gsCtrlParam.size--;
	}
}

/**
 * Start buzzer
 *
 * @param[in]   mdntmp      Configuration of buzzzer type                       <br>
 *                          (Setting value of MD0TMP register)                  <br>
 *                          Set this value of BUZZER_TYPE0 from BUZZER_TYPE3.   <br>
 *
 * @param[in]  md0tl        Configuration of buzer frequency and duty           <br>
 *                          (Setting value of MD0TL register)                   <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Buzzer frequency                               : BUZZER_FREQ_4096HZ, or BUZZER_FREQ_2048HZ, or ( ... ), or BUZZER_FREQ_293HZ
 * @arg                         Buzzer duty
 *                               [When buzer frequency is 4.096MHz]            : BUZZER_DUTY_1_8, or BUZZER_DUTY_2_8, or ( ... ), or BUZZER_DUTY_15_8     <br>
 *                               [When buzer frequency is other than 4.096MHz] : BUZZER_DUTY_1_16, or BUZZER_DUTY_2_16, or ( ... ), or BUZZER_DUTY_15_16
 *
 * @return      None
 */
void buzzer_start( unsigned char mdntmp, unsigned short md0tl )
{
	/* buzzer output is stopped. */
	clear_bit( M0RUN );
	
	/* buzzer mode is selected. */
	set_bit( BZMD );
	
	/* buzzer type is set. */
	write_reg8( MD0TMP, mdntmp );
	
	/* buzzer frequency and duty is set. */
	write_reg16( MD0TL, md0tl );
	
	/* buzzer output is start. */
	set_bit( M0RUN );
}

