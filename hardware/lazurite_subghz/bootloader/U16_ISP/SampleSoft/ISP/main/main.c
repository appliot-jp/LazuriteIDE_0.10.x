/*****************************************************************************
    main.c

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
#include "main.h"
#include "xmodem.h"
#include "isp.h"
#include "remap.h"
#include "sys_uart.h"
#include "wdt.h"
#include <string.h>
#include "tbc.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/


/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
unsigned char XmodemBuf[XMODEM_BLOCK_SIZE];
const char msgIspProg[] =    { 'I', 'S', 'P', '\r', '\n' };


/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
void initPeri( void );
void error_proc( void );
int write_verify(unsigned long *WriteAddr, unsigned char *WriteData);
void end_of_update(void);
/*******************************************************************************
    Routine Name:   main
    Form:           int main( void )
    Parameters:     void
    Return value:   int
                        but not return...
    Description:    main
******************************************************************************/
int main( void )
{
	unsigned long write_addr;
	unsigned char status;

	//  LED ON, when enter to boot mode
	P51D = 1;			
	P56D = 1;			

	P56DIR = 0;
	P56C1 = 1;
	P56C0 = 1;
	P56MD0 = 0;
	P56MD1 = 0;
	P51DIR = 0;
	P51C1 = 1;
	P51C0 = 1;
	P51MD0 = 0;
	P51MD1 = 0;

	memset(XmodemBuf,0,sizeof(XmodemBuf));
	
	/* initialize variables */
#ifndef DEBUG
	write_addr = (unsigned long)USER_AREA_ADDR;
#else
	write_addr = (unsigned long)0x2000;
#endif
	/* initialize peripheral */
	initPeri();
	Xmodem_Init( XmodemBuf );
	__EI();
	//  LED ON, when enter to boot mode

	/* Display program */
	uart_send_sync((unsigned char*)msgIspProg, sizeof(msgIspProg));

	Xmodem_SendByte('C');

	P51D = 0;					// Orange LED ON
	
	/* main loop */	
	for(;;){
		/* clear WDT */
		wdt_clear();
		
		status = Xmodem_ReadStatus();
		
		switch(status){
			case RECV_END:
				P56D = 0;		// BLUE LED ON
				if(write_verify(&write_addr, XmodemBuf+3) != OK){
					error_proc();  /* endless loop in error_proc()*/
				}
				else{
					Xmodem_SendByte(ACK);
				}
				P56D = 1;		// BLUE LED OFF
				break;
			case EOT_END:	// original was remap.  change to infinity loop.
				end_of_update();
//				Remap((unsigned long)USER_AREA_ADDR);
				break;
			case TIMEOUT_ERR:
			case RETRY_ERR:
			case SEND_ERR:
#ifdef DEBUG
				XmodemBuf[2] = status;
#endif
				error_proc();  /* endless loop in error_proc()*/
				break;
			default:
				break;
		}
	}
}

void end_of_update(void)
{
	unsigned char n;
	P51D = 1;				// ORANGE LED OFF
	for(;;)
	{
		for(n=0;n<133;n++)	// about 2s cycle
		{
			wdt_clear();
			tbc_wait15ms();
		}
		P51D = ~P51D;		// INVERT ORANGE LED
	}
}
