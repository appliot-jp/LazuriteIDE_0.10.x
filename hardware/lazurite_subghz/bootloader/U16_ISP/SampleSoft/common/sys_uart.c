/*****************************************************************************
    uart_sync.c

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
    2014.04.14  ver.1.00
******************************************************************************/
#include "mcu.h"
#include "rdwr_reg.h"
#include "irq_i.h"
#include "uart0_i.h"



/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
/* UART */
#if (defined(_ML620Q15X))
 #define UA0MOD0_DATA	0x14	/* receive mode, HSCLK, P42 */
 #define UA0MOD1_DATA	0x08	/* 8bit, non parity, 1stopbit, positive, LSB */
 #define UA0BRT_DATA	0x0046	/* 115200bps */
#elif defined(_ML620Q40X_50X)
 #define UA0MOD0_DATA	0x04
 #define UA0MOD1_DATA	0x08	/* 8bit, non parity, 1stopbit, positive, LSB */
 #ifndef ML620Q504_OSC_RC
  #define UA0BRT_DATA	0x008D	/* 115200bps */
 #else   // ML620Q504_OSC_RC
  #define UA0BRT_DATA   0x0089	/* 115200bps */
 #endif  // ML620Q504_OSC_RC
#endif



void uart_set_port(void)
{
#if defined(_ML620Q15X)
 #if 1	/* for reducing code size */
	write_reg16(P4MOD,0x080C);
	write_reg16(P4CON,0x0C0C);
	write_reg8(P4DIR,0x04);
 #else
	set_bit(P42MD0);		/* RXD */
	clear_bit(P42MD1);
	set_bit(P42C0);
	set_bit(P42C1);
	set_bit(P42DIR);

	set_bit(P43MD0);		/* TXD */
	set_bit(P43MD1);
	set_bit(P43C0);
	set_bit(P43C1);
	clear_bit(P43DIR);
 #endif
#elif defined(_ML620Q40X_50X)
 #if 1	/* for reducing code size */
  #ifdef ML620Q504_USE_P3
	write_reg16(P3MOD,0x0303);
	write_reg16(P3CON,0x0303);
	write_reg8(P3DIR,0x01);
  #else  // ML620Q504_USE_P3
	write_reg16(P4MOD,0x0303);
	write_reg16(P4CON,0x0303);
	write_reg8(P4DIR,0x01);
  #endif // ML620Q504_USE_P3
 #else
	set_bit(P40MD0);		/* RXD */
	set_bit(P40MD1);
	set_bit(P40C0);
	set_bit(P40C1);
	set_bit(P40DIR);

	set_bit(P41MD0);		/* TXD */
	set_bit(P41MD1);
	set_bit(P41C0);
	set_bit(P41C1);
	clear_bit(P41DIR);
 #endif
#endif
}


void uart_clear_port(void)
{
#if defined(_ML620Q15X)
 #if 1	/* for reducing code size */
	write_reg16(P4MOD,0x0000);
	write_reg16(P4CON,0x0000);
	write_reg8(P4DIR,0x00);
 #else
	clear_bit(P42MD0);		/* RXD */
	clear_bit(P42C0);
	clear_bit(P42C1);
	clear_bit(P42DIR);

	clear_bit(P43MD0);		/* TXD */
	clear_bit(P43MD1);
	clear_bit(P43C0);
	clear_bit(P43C1);
	/*clear_bit(P43DIR);*/
 #endif
#elif defined(_ML620Q40X_50X)
 #if 1	/* for reducing code size */
  #ifdef ML620Q504_USE_P3
	write_reg16(P3CON,0x0000);
	write_reg16(P3MOD,0x0000);
	write_reg8(P3DIR,0x00);
  #else // ML620Q504_USE_P3
	write_reg16(P4CON,0x0000);
	write_reg16(P4MOD,0x0000);
	write_reg8(P4DIR,0x00);
  #endif // ML620Q504_USE_P3
 #else
	clear_bit(P40MD0);		/* RXD */
	clear_bit(P40MD1);
	clear_bit(P40C0);
	clear_bit(P40C1);
	clear_bit(P40DIR);

	clear_bit(P41C0);
	clear_bit(P41C1);
	clear_bit(P41MD0);		/* TXD */
	clear_bit(P41MD1);
 #endif
#endif
}



/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
static volatile unsigned char flgUartFin=0;



/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
static void waitSendComplete( unsigned int size, unsigned char errStatus );



/*******************************************************************************
    Routine Name:   sys_uart_init
    Form:           void sys_uart_init(void)
    Parameters:     none
    Return value:   none
    Description:    
******************************************************************************/
void sys_uart_init(void)
{
	/* initialize UART */
	uart0_init(UA0MOD0_DATA, UA0MOD1_DATA, UA0BRT_DATA);

	/* UART Port */
	uart_set_port();

	irq_ua0_clearIRQ();
	irq_ua1_clearIRQ();
}



/*******************************************************************************
    Routine Name:   uart_send_sync
    Form:           void uart_send_sync(unsigned char *data, unsigned int size)
    Parameters:     data : Pointer to data
                    size : Size of transmission
    Return value:   none
    Description:    
******************************************************************************/
void uart_send_sync(unsigned char *data, unsigned int size)
{
	flgUartFin = 0;
	uart0_write(data, size, waitSendComplete);
	irq_ua1_ena();
	while ( flgUartFin==0 ){}
}



/*******************************************************************************
    Routine Name:   waitSendComplete
    Form:           static void waitSendComplete( unsigned int size, unsigned char errStatus )
    Parameters:     unsigned int size        : size of transmission
                    unsigned char errStatus  : error status
    Return value:   void
    Description:    UART transmission completion callback function.
******************************************************************************/
static void waitSendComplete( unsigned int size, unsigned char errStatus )
{
	flgUartFin = 1;
	irq_ua1_dis();
}
