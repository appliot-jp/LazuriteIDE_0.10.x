/*****************************************************************************
    isp.c

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
#include "main.h"
#include "flash.h"
#include "irq_i.h"
#include "uart0_i.h"
#include "tbc.h"
#include "wdt.h"
#include "xmodem.h"
#include "isp.h"
#include "led.h"
#include "sys_uart.h"
#include "sys_clock.h"
#include "remap.h"
#include <string.h>

/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
const char msgError[] =      { '\r', '\n', 'E', 'r', 'r', 'o', 'r', '\r', '\n' };


/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
static void _intNullHdr( void );
static void _intUA0INT( void );
static void _intUA1INT( void );
static void _intTBC128Hz( void );

void memory_dump(void);
int memory_dump_c_to_a(char* buf, unsigned char data);

/*############################################################################*/
/*#                             Vector Table                                 #*/
/*############################################################################*/
#include "vector_isp.h"


/*******************************************************************************
    Routine Name:   initPeri
    Form:           void initPeri( void )
    Parameters:     void
    Return value:   void
    Description:    initialize peripheral.
******************************************************************************/
void initPeri( void )
{
	/* System clock */
	sys_setHSclk();

	/* WDT */
	wdt_init(WDT_TIMEOUT);

	/* TBC */
	tbc_setInt0(TBC_LTBINT_128);	/* call after sys_setHSclk() */

	/* LED */
#ifdef USE_LED
	led_init();
#endif

	/* UART */
	sys_uart_init();
}



/*******************************************************************************
    Routine Name:   ResetPeri 
    Form:           void ResetPeri(void)
    Parameters:     void
    Return value:   void
    Description:    Reset Peripheral
******************************************************************************/
void ResetPeri(void)
{
	/* Disable UART and TBC(128Hz) Interrupt */
	irq_ua0_dis();
	irq_ua1_dis();
	irq_tbc0_dis();

	/* Clear UART and TBC(128Hz) Interrupt Request */
	irq_ua0_clearIRQ();
	irq_ua1_clearIRQ();
	irq_tbc0_clearIRQ();

	/* Set initial value Flash Register */
	write_reg16(FLASHA, 0x0000);
	write_reg16(FLASHD, 0x0000);
	write_reg8(FLASHSEG, 0x00);

#ifdef USE_LED
	led_reset();
#endif

	uart_clear_port();

	/* Reset Peripheral with BLKCON */
	ResetPeriBlock();

	/* Set initial value CLOCK */
	sys_reset_clock();

	write_reg16(LTBINT, 0x0630);
}


/*******************************************************************************
	Routine Name:	error_proc
	Form:			void error_proc( void )
	Parameters:		void
	Return value:	void
	Description:	error proc
******************************************************************************/
extern unsigned char XmodemBuf[];

void error_proc( void )
{
	irq_ua0_dis();

	/* Wait XMODEM sending data */
	while(get_bit(EUA1) == 1){}

	/* Sending error message */
	uart_send_sync((unsigned char*)msgError, sizeof(msgError));

//#ifdef USE_LED
	/* ERROR LED ON */
	led_on();
//#endif
	
//	memory_dump();
	
		/* endless loop */
	for(;;){
		wdt_clear();
	}
}


/*******************************************************************************
	Routine Name:	write_verify
	Form:			int write_verify(unsigned long *WriteAddr, 
													unsigned char *WriteData)
	Parameters:		unsigned long *WriteAddr
					unsigned char *WriteData
	Return value:	int	0=OK,-1=NG
	Description:	write & verify proc
******************************************************************************/
int write_verify(unsigned long *WriteAddr, unsigned char *WriteData)
{
	unsigned short write_cnt;
	unsigned long write_add;
	unsigned short write_data;

#ifndef USE_DATA_FALSH	/* for debug */
	if(*WriteAddr > (FLASH_SIZE-XMODEM_DATA_SIZE)){
		return NG;
	}

	if((*WriteAddr >= ISP_AREA_START_ADDR) && (*WriteAddr <= TEST_AREA_END_ADDR)){
		*WriteAddr += XMODEM_DATA_SIZE;
		return OK;
	}
#endif

	/* Enable Flash Self Programming */
	flash_controlSelfProg(FSELFPROG_EN);

	/* Erase Sector or Block */
	if((*WriteAddr >= (unsigned long)FLASH_MIN_SECTOR_ERASE_ADRS) 
				&& (*WriteAddr <= TEST_AREA_END_ADDR)){
		if((*WriteAddr % FLASH_SECTOR_SIZE) == 0){
			/* Erase Flash Sector */
			flash_eraseSector((unsigned short __far *)*WriteAddr);
		}
	}
	else{
		if((*WriteAddr % FLASH_BLOCK_SIZE) == 0){
			/* Erase Flash Block */
			flash_eraseBlock((unsigned short __far *)*WriteAddr);
		}
	}

	write_add = *WriteAddr + FLASH_SEG8_ADDR;

	for(write_cnt = 0; write_cnt < XMODEM_DATA_SIZE; write_cnt+=2,*WriteAddr+=2){
		/* Write to Flash */
		write_data = (unsigned short)*(WriteData+write_cnt) & 0x00FF;
		write_data |= (unsigned short)*(WriteData+write_cnt+1) << 8;
		flash_writeWord((unsigned short __far *)*WriteAddr,write_data);

		/* verify check */
		if((*(unsigned short __far *)(write_add+write_cnt)) != write_data){
			/* Disable Flash Self Programming */
			flash_controlSelfProg(FSELFPROG_DIS);
			return NG;
		}
	}

	/* Disable Flash Self Programming */
	flash_controlSelfProg(FSELFPROG_DIS);

	return OK;
}


/*******************************************************************************
	Routine Name:	_intNullHdr
	Form:			static void _intNullHdr( void )
	Parameters:		void
	Return value:	void
	Description:	NULL Handler.
******************************************************************************/
static void _intNullHdr( void )
{
	return;
}


/*******************************************************************************
    Routine Name:   _intUA0INT
    Form:           void _intUA0INT( void )
    Parameters:     void
    Return value:   void
    Description:    UART handler.
******************************************************************************/
static void _intUA0INT( void )
{
	uart0_continueRead();
}


/*******************************************************************************
    Routine Name:   _intUA1INT
    Form:           void _intUA1INT( void )
    Parameters:     void
    Return value:   void
    Description:    UART handler.
******************************************************************************/
static void _intUA1INT( void )
{
	uart0_continueWrite();
}


/*******************************************************************************
	Routine Name:	_intTBC128Hz
	Form:			static void _intTBC128Hz( void )
	Parameters:		void
	Return value:	void
	Description:	TBC 128Hz handler.
******************************************************************************/
static void _intTBC128Hz( void )
{
	Xmodem_CountTimeOut();
}

void memory_dump(void)
{
	int i;	// line
	int j;	// letter position
	int k;  // out buffer pointer
	int l;  // input buffer pointer
	char message[16];
	unsigned char error_buf[64];
	
	for(i=0;i<64;i++)
	{
		error_buf[i] = 0;
	}
	
	memset(message,0,sizeof(message));
	strcpy(message,"MEM DUMP\r\n");
	uart_send_sync(message,sizeof(message));
	
	
	// SOH , BLOCK NUM , Invert of BLOCK NUM
	k = 0;
	memset(error_buf,0,sizeof(error_buf));
	for(j=0;j<3;j++)
	{
		k += memory_dump_c_to_a(error_buf+k,XmodemBuf[j]);
	}
	error_buf[k++] = '\r';
	error_buf[k++] = '\n';
	uart_send_sync(error_buf,sizeof(error_buf));
	
	
	// 128 BYTE DATA
	for(i=0;i<8;i++)
	{
		k = 0;
		memset(error_buf,0,sizeof(error_buf));
		
		for(j=0;j<16;j++)
		{
			k += memory_dump_c_to_a(error_buf+k,XmodemBuf[i*16+j+3]);
		}
		error_buf[k++] = '\r';
		error_buf[k++] = '\n';
		uart_send_sync(error_buf,sizeof(error_buf));
	}
	
	// CRC
	k = 0;
	memset(error_buf,0,sizeof(error_buf));
	for(j=131;j<133;j++)
	{
		k += memory_dump_c_to_a(error_buf+k,XmodemBuf[j]);
	}
	error_buf[k++] = '\r';
	error_buf[k++] = '\n';
	uart_send_sync(error_buf,sizeof(error_buf));
}


int memory_dump_c_to_a(char* buf, unsigned char data)
{
	unsigned char tmp;
	tmp = ((data >> 4) & 0x0f);
	buf[0] = (tmp <= 9)?('0'+tmp):('a'+tmp-10);
	tmp = data & 0x0f;
	buf[1] = (tmp <= 9)?('0'+tmp):('a'+tmp-10);
	buf[2] = ' ';
	return 3;
}
