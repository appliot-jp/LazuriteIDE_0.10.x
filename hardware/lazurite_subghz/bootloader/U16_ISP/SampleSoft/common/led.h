/*****************************************************************************
    led.h

    Copyright (C) 2014 LAPIS Semiconductor Co., Ltd.
    All rights reserved.

    This software is provided "as is" and any expressed or implied
    warranties, including, but not limited to, the implied warranties of
    merchantability and fitness for a particular purpose are disclaimed.
    LAPIS Semiconductor shall not be liable for any direct, indirect,
    consequential or incidental damages arising from using or modifying
    this software.
    You (customer) can modify and use this software in whole or part on
    your own responsibility, only for the purpose of developing the software
    for use with microcontroller manufactured by LAPIS Semiconductor.

    History
    2014.04.21  ver.1.00
******************************************************************************/
#ifndef _LED_H_
#define _LED_H_

#include "rdwr_reg.h"


/*******************************************************************************
    Routine Name:   led_init
    Form:           void led_init( void )
    Parameters:     void
    Return value:   void
    Description:    Initilization of LED
******************************************************************************/
#if defined(_ML620Q15X)
 #define led_init()		do{								\
							write_reg16(P2CON, 0x0100);	\
							write_reg8(P2D, 0x01);		\
						}while(0)
#elif defined(_ML620Q40X_50X)
 #define led_init()		do{								\
							set_bit(P52C0);	\
							set_bit(P52C1);	\
							set_bit(P52D);	\
						}while(0)
#endif

/*******************************************************************************
    Routine Name:   led_on
    Form:           void led_on( void )
    Parameters:     void
    Return value:   void
    Description:    LED Lighting
******************************************************************************/
#if defined(_ML620Q15X)
 #define led_on()		clear_bit(P20D)
#elif defined(_ML620Q40X_50X)
// #define led_on()		clear_bit(P52D)
 #define led_on()		set_bit(P51D)
#endif

/*******************************************************************************
    Routine Name:   led_reset
    Form:           void led_reset( void )
    Parameters:     void
    Return value:   void
    Description:    LED reseting
******************************************************************************/
#if defined(_ML620Q15X)
 #define led_reset()		do{								\
							write_reg16(P2CON, 0x0000);		\
							write_reg8(P2D, 0x00);			\
						}while(0)
#elif defined(_ML620Q40X_50X)
 #define led_reset()		do{								\
							clear_bit(P52C0);	\
							clear_bit(P52C1);	\
							clear_bit(P52D);	\
						}while(0)
#endif


#endif	/* _LED_H_ */
