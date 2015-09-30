/*****************************************************************************
	xmodem.c

	Copyright (C) 2011 LAPIS Semiconductor Co., Ltd.
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
    2011.02.07  ver.1.00
    2013.01.21	ver.1.10
******************************************************************************/
#ifndef _XMODEM_H_
#define _XMODEM_H_

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define INIT_STATE	0
#define RECV_STATE	1
#define RECV_END	2
#define RECV_EOT	3
#define EOT_END		4
#define TIMEOUT_ERR	5
#define RETRY_ERR	6
#define SEND_ERR	7

#define SOH		0x01
#define STX		0x02
#define EOT		0x04
#define ACK		0x06
#define NAK		0x15
#define CAN		0x18

#define XMODEM_DATA_SIZE	(128)
#define XMODEM_BLOCK_SIZE	(133)


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
void Xmodem_Init( unsigned char *RecvBuf );
void Xmodem_SendByte( unsigned char send_data );
unsigned char Xmodem_ReadStatus( void );
void Xmodem_CountTimeOut( void );


#endif /*_XMODEM_H_*/
