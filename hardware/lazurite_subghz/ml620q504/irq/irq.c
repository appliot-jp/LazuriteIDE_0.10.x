/* FILE NAME: irq.c
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
#include "irq.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/

/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/

/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
static void s_irq_common_setMultiBit( unsigned char *symbol1, unsigned char *symbol2, unsigned char bit_no, unsigned char val );

/*############################################################################*/
/*#                              Subroutine                                  #*/
/*############################################################################*/
#pragma INLINE s_irq_common_setMultiBit
static void s_irq_common_setMultiBit( unsigned char *symbol1, unsigned char *symbol2, unsigned char bit_no, unsigned char val )
{
	unsigned char mask;

	mask = (unsigned char)(0x01 << bit_no);
	clear_reg8( *symbol1, mask );
	clear_reg8( *symbol2, mask );
	if( (val & 0x01) != 0 ){
		set_reg8( *symbol1, mask );    /* set low bit */
	}
	if( (val & 0x02) != 0 ){
		set_reg8( *symbol2, mask );    /* set high bit */
	}
}

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/


/**
 * Setting external interrupt parameters (EXI0INT)
 *
 * @param[in]   edge        EXIn_EDGE_DISABLE : interrupt disable               <br>
 *                          EXIn_EDGE_FALLING : falling edge                    <br>
 *                          EXIn_EDGE_RISING  : rising edge                     <br>
 *                          EXIn_EDGE_BOTH    : both edge
 *
 * @param[in]   sampling    EXIn_SAMPLING_DIS : whithout sampling               <br>
 *                          EXIn_SAMPLING_ENA : with sampling
 *
 * @param[in]   filter      EXIn_FILTER_DIS   : without filtering               <br>
 *                          EXIn_FILTER_ENA   : with filtering
 *
 * @param[in]   port        Select interrupt source
 * @arg                         EXIn_PORT_SEL_P00
 * @arg                         EXIn_PORT_SEL_P01
 * @arg                         (...)
 * @arg                         EXIn_PORT_SEL_P57
 * @return      None
 */
void irq_ext0_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port )
{
	/* Set edge */
	s_irq_common_setMultiBit( &EXICON0, &EXICON1, EXI0_EDGE_BIT_NO, edge );
	/* Set enable/disable sampling */
	write_bit( EXI0SM, (unsigned char)(sampling & 0x1) );
	/* Set enable/disable noise filter */
	write_bit( EXI0FL, (unsigned char)(filter & 0x1) );
	/* Select port */
	write_reg8( EXI0SEL, port );
}

/**
 * Setting external interrupt parameters (EXI1INT)
 *
 * @param[in]   edge        EXIn_EDGE_DISABLE : interrupt disable               <br>
 *                          EXIn_EDGE_FALLING : falling edge                    <br>
 *                          EXIn_EDGE_RISING  : rising edge                     <br>
 *                          EXIn_EDGE_BOTH    : both edge
 *
 * @param[in]   sampling    EXIn_SAMPLING_DIS : whithout sampling               <br>
 *                          EXIn_SAMPLING_ENA : with sampling
 *
 * @param[in]   filter      EXIn_FILTER_DIS   : without filtering               <br>
 *                          EXIn_FILTER_ENA   : with filtering
 *
 * @param[in]   port        Select interrupt source
 * @arg                         EXIn_PORT_SEL_P00
 * @arg                         EXIn_PORT_SEL_P01
 * @arg                         (...)
 * @arg                         EXIn_PORT_SEL_P57
 * @return      None
 */
void irq_ext1_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port )
{
	/* Set edge */
	s_irq_common_setMultiBit( &EXICON0, &EXICON1, EXI1_EDGE_BIT_NO, edge );
	/* Set enable/disable sampling */
	write_bit( EXI1SM, (unsigned char)(sampling & 0x1) );
	/* Set enable/disable noise filter */
	write_bit( EXI1FL, (unsigned char)(filter & 0x1) );
	/* Select port */
	write_reg8( EXI1SEL, port );
}

/**
 * Setting external interrupt parameters (EXI2INT)
 *
 * @param[in]   edge        EXIn_EDGE_DISABLE : interrupt disable               <br>
 *                          EXIn_EDGE_FALLING : falling edge                    <br>
 *                          EXIn_EDGE_RISING  : rising edge                     <br>
 *                          EXIn_EDGE_BOTH    : both edge
 *
 * @param[in]   sampling    EXIn_SAMPLING_DIS : whithout sampling               <br>
 *                          EXIn_SAMPLING_ENA : with sampling
 *
 * @param[in]   filter      EXIn_FILTER_DIS   : without filtering               <br>
 *                          EXIn_FILTER_ENA   : with filtering
 *
 * @param[in]   port        Select interrupt source
 * @arg                         EXIn_PORT_SEL_P00
 * @arg                         EXIn_PORT_SEL_P01
 * @arg                         (...)
 * @arg                         EXIn_PORT_SEL_P57
 * @return      None
 */
void irq_ext2_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port )
{
	/* Set edge */
	s_irq_common_setMultiBit( &EXICON0, &EXICON1, EXI2_EDGE_BIT_NO, edge );
	/* Set enable/disable sampling */
	write_bit( EXI2SM, (unsigned char)(sampling & 0x1) );
	/* Set enable/disable noise filter */
	write_bit( EXI2FL, (unsigned char)(filter & 0x1) );
	/* Select port */
	write_reg8( EXI2SEL, port );
}

/**
 * Setting external interrupt parameters (EXI3INT)
 *
 * @param[in]   edge        EXIn_EDGE_DISABLE : interrupt disable               <br>
 *                          EXIn_EDGE_FALLING : falling edge                    <br>
 *                          EXIn_EDGE_RISING  : rising edge                     <br>
 *                          EXIn_EDGE_BOTH    : both edge
 *
 * @param[in]   sampling    EXIn_SAMPLING_DIS : whithout sampling               <br>
 *                          EXIn_SAMPLING_ENA : with sampling
 *
 * @param[in]   filter      EXIn_FILTER_DIS   : without filtering               <br>
 *                          EXIn_FILTER_ENA   : with filtering
 *
 * @param[in]   port        Select interrupt source
 * @arg                         EXIn_PORT_SEL_P00
 * @arg                         EXIn_PORT_SEL_P01
 * @arg                         (...)
 * @arg                         EXIn_PORT_SEL_P57
 * @return      None
 */
void irq_ext3_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port )
{
	/* Set edge */
	s_irq_common_setMultiBit( &EXICON0, &EXICON1, EXI3_EDGE_BIT_NO, edge );
	/* Set enable/disable sampling */
	write_bit( EXI3SM, (unsigned char)(sampling & 0x1) );
	/* Set enable/disable noise filter */
	write_bit( EXI3FL, (unsigned char)(filter & 0x1) );
	/* Select port */
	write_reg8( EXI3SEL, port );
}

/**
 * Setting external interrupt parameters (EXI4INT)
 *
 * @param[in]   edge        EXIn_EDGE_DISABLE : interrupt disable               <br>
 *                          EXIn_EDGE_FALLING : falling edge                    <br>
 *                          EXIn_EDGE_RISING  : rising edge                     <br>
 *                          EXIn_EDGE_BOTH    : both edge
 *
 * @param[in]   sampling    EXIn_SAMPLING_DIS : whithout sampling               <br>
 *                          EXIn_SAMPLING_ENA : with sampling
 *
 * @param[in]   filter      EXIn_FILTER_DIS   : without filtering               <br>
 *                          EXIn_FILTER_ENA   : with filtering
 *
 * @param[in]   port        Select interrupt source
 * @arg                         EXIn_PORT_SEL_P00
 * @arg                         EXIn_PORT_SEL_P01
 * @arg                         (...)
 * @arg                         EXIn_PORT_SEL_P57
 * @return      None
 */
void irq_ext4_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port )
{
	/* Set edge */
	s_irq_common_setMultiBit( &EXICON0, &EXICON1, EXI4_EDGE_BIT_NO, edge );
	/* Set enable/disable sampling */
	write_bit( EXI4SM, (unsigned char)(sampling & 0x1) );
	/* Set enable/disable noise filter */
	write_bit( EXI4FL, (unsigned char)(filter & 0x1) );
	/* Select port */
	write_reg8( EXI4SEL, port );
}

/**
 * Setting external interrupt parameters (EXI5INT)
 *
 * @param[in]   edge        EXIn_EDGE_DISABLE : interrupt disable               <br>
 *                          EXIn_EDGE_FALLING : falling edge                    <br>
 *                          EXIn_EDGE_RISING  : rising edge                     <br>
 *                          EXIn_EDGE_BOTH    : both edge
 *
 * @param[in]   sampling    EXIn_SAMPLING_DIS : whithout sampling               <br>
 *                          EXIn_SAMPLING_ENA : with sampling
 *
 * @param[in]   filter      EXIn_FILTER_DIS   : without filtering               <br>
 *                          EXIn_FILTER_ENA   : with filtering
 *
 * @param[in]   port        Select interrupt source
 * @arg                         EXIn_PORT_SEL_P00
 * @arg                         EXIn_PORT_SEL_P01
 * @arg                         (...)
 * @arg                         EXIn_PORT_SEL_P57
 * @return      None
 */
void irq_ext5_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port )
{
	/* Set edge */
	s_irq_common_setMultiBit( &EXICON0, &EXICON1, EXI5_EDGE_BIT_NO, edge );
	/* Set enable/disable sampling */
	write_bit( EXI5SM, (unsigned char)(sampling & 0x1) );
	/* Set enable/disable noise filter */
	write_bit( EXI5FL, (unsigned char)(filter & 0x1) );
	/* Select port */
	write_reg8( EXI5SEL, port );
}

/**
 * Setting external interrupt parameters (EXI6INT)
 *
 * @param[in]   edge        EXIn_EDGE_DISABLE : interrupt disable               <br>
 *                          EXIn_EDGE_FALLING : falling edge                    <br>
 *                          EXIn_EDGE_RISING  : rising edge                     <br>
 *                          EXIn_EDGE_BOTH    : both edge
 *
 * @param[in]   sampling    EXIn_SAMPLING_DIS : whithout sampling               <br>
 *                          EXIn_SAMPLING_ENA : with sampling
 *
 * @param[in]   filter      EXIn_FILTER_DIS   : without filtering               <br>
 *                          EXIn_FILTER_ENA   : with filtering
 *
 * @param[in]   port        Select interrupt source
 * @arg                         EXIn_PORT_SEL_P00
 * @arg                         EXIn_PORT_SEL_P01
 * @arg                         (...)
 * @arg                         EXIn_PORT_SEL_P57
 * @return      None
 */
void irq_ext6_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port )
{
	/* Set edge */
	s_irq_common_setMultiBit( &EXICON0, &EXICON1, EXI6_EDGE_BIT_NO, edge );
	/* Set enable/disable sampling */
	write_bit( EXI6SM, (unsigned char)(sampling & 0x1) );
	/* Set enable/disable noise filter */
	write_bit( EXI6FL, (unsigned char)(filter & 0x1) );
	/* Select port */
	write_reg8( EXI6SEL, port );
}

/**
 * Setting external interrupt parameters (EXI7INT)
 *
 * @param[in]   edge        EXIn_EDGE_DISABLE : interrupt disable               <br>
 *                          EXIn_EDGE_FALLING : falling edge                    <br>
 *                          EXIn_EDGE_RISING  : rising edge                     <br>
 *                          EXIn_EDGE_BOTH    : both edge
 *
 * @param[in]   sampling    EXIn_SAMPLING_DIS : whithout sampling               <br>
 *                          EXIn_SAMPLING_ENA : with sampling
 *
 * @param[in]   filter      EXIn_FILTER_DIS   : without filtering               <br>
 *                          EXIn_FILTER_ENA   : with filtering
 *
 * @param[in]   port        Select interrupt source
 * @arg                         EXIn_PORT_SEL_P00
 * @arg                         EXIn_PORT_SEL_P01
 * @arg                         (...)
 * @arg                         EXIn_PORT_SEL_P57
 * @return      None
 */
void irq_ext7_init( unsigned char edge, unsigned char sampling, unsigned char filter, unsigned char port )
{
	/* Set edge */
	s_irq_common_setMultiBit( &EXICON0, &EXICON1, EXI7_EDGE_BIT_NO, edge );
	/* Set enable/disable sampling */
	write_bit( EXI7SM, (unsigned char)(sampling & 0x1) );
	/* Set enable/disable noise filter */
	write_bit( EXI7FL, (unsigned char)(filter & 0x1) );
	/* Select port */
	write_reg8( EXI7SEL, port );
}

