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
    2011.02.23  ver.1.00
    2013.01.21  ver.1.10
******************************************************************************/
#include "mcu.h"
#include "irq_i.h"
#include "uart0_i.h"
#include "crc.h"
#include "xmodem.h"

/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
static unsigned char XmodemStatus;
static unsigned char XmodemRetryCnt;
static unsigned char XmodemBlockNum;
static unsigned char XmodemSendData;
static unsigned short XmodemTimeoutCnt;
static unsigned short XmodemTimeoutRetryCnt;

static unsigned char *XmodemBuf;

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define TIMEOUT_CNT		1280	// 128Hz * 1280 = 10sec
#define RETRY_CNT		10


/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/
static int Xmodem_CheckBlockData( unsigned char *RecvData, unsigned char block_num );
static void Xmodem_StartTimeOut( void );
static void Xmodem_StopTimeOut( void );
static void Xmodem_SendByteComplete( unsigned int size, unsigned char errStat );
static void Xmodem_RecvBlockComplete( unsigned int size, unsigned char errStat );
static char Xmodem_RecvChar( unsigned char data, unsigned char errStat );
static void Xmodem_RetryProc( void );

/*******************************************************************************
	Routine Name:	Xmodem_Init
	Form:			void Xmodem_Init( unsigned char *RecvBuf )
	Parameters:		unsigned char *RecvBuf
	Return value:	void
	Description:	initialize Xmodem
******************************************************************************/
void Xmodem_Init( unsigned char *RecvBuf )
{
	XmodemBuf = (unsigned char *)RecvBuf;
	XmodemStatus = INIT_STATE;
	XmodemRetryCnt = 0;
	XmodemBlockNum = 1;
	XmodemSendData = 0;
	XmodemTimeoutCnt = 0;
	XmodemTimeoutRetryCnt = 0;
}


/*******************************************************************************
	Routine Name:	Xmodem_Check_BlockData
	Form:			static int Xmodem_Check_BlockData( unsigned char *RecvData, unsigned char BlockNum )
	Parameters:		unsigned char *RecvData
					unsigned char BlockNum
	Return value:	int OK=0,NG=-1
	Description:	Check Block Data Proc(Block Data, CRC and Block No)
******************************************************************************/
static int Xmodem_CheckBlockData( unsigned char *RecvData, unsigned char BlockNum )
{
	unsigned short crc;
	unsigned char loopcnt;
	unsigned short data;
	unsigned short tmp;

	/* check block data */
	if(RecvData[0] + RecvData[1] != 0xff){
		// error
		return -1;
	}
	/* check CRC */
	crc = 0;
	for(loopcnt = 0; loopcnt < XMODEM_DATA_SIZE; loopcnt++){
		data = (unsigned short)(RecvData[loopcnt+2]);
		crc = UpdCRC16(data, crc);
	}
	tmp = RecvData[loopcnt+2];
	tmp = tmp << 8;
	tmp |= RecvData[loopcnt+3];
	if(crc != tmp){
		/* CRC ERROR */
		return -1;
	}
	/* check block no */
	if(RecvData[0] != BlockNum){
		/* Block No ERROR */
		return -1;
	}
	return 0;
}


/*******************************************************************************
	Routine Name:	Xmodem_StartTimeOut
	Form:			static void Xmodem_StartTimeOut( void )
	Parameters:		void
	Return value:	void
	Description:	Start TimeOut Proc
******************************************************************************/
static void Xmodem_StartTimeOut( void )
{
	irq_tbc0_clearIRQ();
	irq_tbc0_ena();
	XmodemTimeoutCnt = 0;
}


/*******************************************************************************
	Routine Name:	Xmodem_StopTimeOut
	Form:			static void Xmodem_StopTimeOut( void )
	Parameters:		void
	Return value:	void
	Description:	Stop TimeOut Proc
******************************************************************************/
static void Xmodem_StopTimeOut( void )
{
	irq_tbc0_dis();
}


/*******************************************************************************
	Routine Name:	Xmodem_SendByteComplete
	Form:			static void Xmodem_SendByteComplete( unsigned int Size, unsigned char ErrStat )
	Parameters:		unsigned int Size
					unsigned char ErrStat
	Return value:	void
	Description:	Send Byte Complete Proc
******************************************************************************/
static void Xmodem_SendByteComplete( unsigned int Size, unsigned char ErrStat )
{
	irq_ua1_dis();

	switch(XmodemSendData){
		case 'C':
		case ACK:
		case NAK:
			if(XmodemStatus != RECV_EOT){
				uart0_read(XmodemBuf, XMODEM_BLOCK_SIZE, Xmodem_RecvChar, Xmodem_RecvBlockComplete);
				irq_ua0_ena();
				Xmodem_StartTimeOut();
			}
			else{
				XmodemStatus = EOT_END;
			}
			break;
		case CAN:
			XmodemStatus = SEND_ERR;
			break;
		default:
			irq_ua0_dis();
			XmodemStatus = SEND_ERR;
			Xmodem_SendByte(CAN);
			break;
	}
}

/*******************************************************************************
	Routine Name:	Xmodem_SendByte
	Form:			int Xmodem_SendByte( unsigned char send_data )
	Parameters:		unsigned char send_data
	Return value:	void
	Description:	Send Byte Proc
******************************************************************************/
void Xmodem_SendByte( unsigned char send_data )
{
	XmodemSendData = send_data;
	if(XmodemStatus != RECV_EOT){
		if((send_data == 'C') || (send_data == ACK)){
			XmodemStatus = RECV_STATE;
		}
	}
	uart0_write(&XmodemSendData, 1, Xmodem_SendByteComplete );
	irq_ua1_ena();
}


/*******************************************************************************
	Routine Name:	Xmodem_RecvBlockComplete
	Form:			static void Xmodem_RecvBlockComplete( unsigned int Size, unsigned char ErrStat )
	Parameters:		unsigned int Size
					unsigned char ErrStat
	Return value:	void
	Description:	Receive Block Complete Proc
******************************************************************************/
static void Xmodem_RecvBlockComplete( unsigned int Size, unsigned char ErrStat )
{
	Xmodem_StopTimeOut();

	if(ErrStat != 0){
		Xmodem_RetryProc();
	}
	else{
		if(Xmodem_CheckBlockData(XmodemBuf+1, XmodemBlockNum) != 0){
			XmodemRetryCnt++;
			if(XmodemRetryCnt > RETRY_CNT){
				XmodemStatus = RETRY_ERR;
				Xmodem_SendByte(CAN);
			}
			else
			{
				if(XmodemBlockNum == 1){
					Xmodem_SendByte('C');
				}
				else{
					// NAK send
					Xmodem_SendByte(NAK);
				}
			}
		}
		else{
			XmodemStatus = RECV_END;
			XmodemBlockNum++;
			XmodemRetryCnt = 0;
		}
	}
	irq_ua0_dis();
}


/*******************************************************************************
	Routine Name:	Xmodem_RecvChar
	Form:			static char Xmodem_RecvChar( unsigned int Size, unsigned char ErrStat )
	Parameters:		unsigned int Size
					unsigned char ErrStat
	Return value:	signed char return '0' only
	Description:	Receive Block Byte Complete Proc
******************************************************************************/
static char Xmodem_RecvChar( unsigned char Data, unsigned char ErrStat )
{
	XmodemTimeoutCnt = 0;
	XmodemTimeoutRetryCnt = 0;
	if(ErrStat != 0){
		Xmodem_RetryProc();
	}
	else{
		switch(XmodemBuf[0]){
			case EOT:
				// ACK send
				XmodemStatus = RECV_EOT;
				Xmodem_SendByte(ACK);
				XmodemRetryCnt = 0;
				break;
			case STX:
				irq_ua0_dis();
				Xmodem_SendByte(CAN);
				XmodemRetryCnt = 0;
				break;
			case SOH:
				XmodemRetryCnt = 0;
				break;
			default:
				Xmodem_SendByte(CAN);	/* irq_ua0_dis() is called in Xmodem_RetryProc() */
				Xmodem_RetryProc();
				break;
		}
	}
	return 0;
}


/*******************************************************************************
	Routine Name:	Xmodem_ReadStatus
	Form:			unsigned char Xmodem_ReadStatus( void )
	Parameters:		void
	Return value:	unsigned char
	Description:	Xmodem Read Status Proc
******************************************************************************/
unsigned char Xmodem_ReadStatus( void )
{
	return(XmodemStatus);
}


/*******************************************************************************
	Routine Name:	Xmodem_CountTimeOut
	Form:			void Xmodem_CountTimeOut( void )
	Parameters:		void
	Return value:	void
	Description:	Count TimeOut Proc(TBC 128Hz) 
******************************************************************************/
void Xmodem_CountTimeOut( void )
{
	XmodemTimeoutCnt++;
	if(XmodemTimeoutCnt > TIMEOUT_CNT){
		XmodemTimeoutRetryCnt++;
		irq_ua0_dis();

		if(XmodemTimeoutRetryCnt > RETRY_CNT){
			Xmodem_StopTimeOut();
			XmodemTimeoutCnt = TIMEOUT_CNT;
			XmodemStatus = TIMEOUT_ERR;
			Xmodem_SendByte(CAN);
		}
		else{
			Xmodem_SendByte(XmodemSendData);
		}
	}
}


/*******************************************************************************
	Routine Name:	Xmodem_RetryProc
	Form:			static void Xmodem_RetryProc( void )
	Parameters:		void
	Return value:	void
	Description:	Retry Proc
******************************************************************************/
static void Xmodem_RetryProc( void )
{
	irq_ua0_dis();

	XmodemRetryCnt++;
	if(XmodemRetryCnt > RETRY_CNT){
		XmodemStatus = RETRY_ERR;
		Xmodem_SendByte(CAN);
	}
	else{
		Xmodem_SendByte(NAK);
	}
}
