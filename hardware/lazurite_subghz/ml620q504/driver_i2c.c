/* FILE NAME: driver_i2c.c
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

#ifdef SUBGHZ_OTA
	#pragma SEGCODE "OTA_SEGCODE"
	#pragma SEGINIT "OTA_SEGINIT"
	#pragma SEGNOINIT "OTA_SEGNOINIT"
	#pragma SEGCONST "OTA_SEGCONST"
#endif
#include <stdlib.h>
#include <stddef.h>
#include "common.h"
#include "mcu.h"
#include "rdwr_reg.h"
#include "binary.h"
#include "driver_irq.h"
#include "driver_i2c.h"
//********************************************************************************
//   local definitions
//********************************************************************************
typedef struct {
//	BOOLEAN	flag;			// flag of i2c isr
	UCHAR	*rx_buffer;		// buffer of RX
	UINT16	rx_index;		// buffer pointer of RX
	UINT16	rx_length;		// length to read
	UCHAR	*tx_buffer;		// buffer of TX
	UINT16	tx_index;		// buffer pointer of TX
	UINT16	tx_length;		// length to write
	CHAR   	status;			// current status
	BOOLEAN	bStop;			// stop code (false = no, true = required)
	UCHAR	err_code;		//
} _I2C_PARAM;

//********************************************************************************
//   global parameters
//********************************************************************************
static _I2C_PARAM	I2C[2] ={{false},{false}};
//********************************************************************************
//   local parameters
//********************************************************************************

//********************************************************************************
//   local function definitions
//********************************************************************************
void i2c_isr(UCHAR ch);
void i2c1_isr(void);
void i2c0_isr(void);
BOOLEAN i2c_check_read_last_data(UCHAR ch);
unsigned char i2c_update_err_code(UCHAR ch, unsigned short index);


//********************************************************************************
//   functions
//********************************************************************************

// initialization
void i2c_init(UCHAR ch)
{
	if(ch>=2) return;
	
	// reset common flag
	I2C[ch].status = I2C_MODE_IDLE;
	I2C[ch].err_code = 0;
	
	// Initializing hardware
	if(ch == 0)
	{
		clear_bit(DI2C0);
		P40D=1;P40MD1 = 0; P40MD0 = 1; P40C1 = 1; P40C0 = 0;	// port setting
		P41D=1;P41MD1 = 0; P41MD0 = 1; P41C1 = 1; P41C0 = 0;	// port setting
		I2C0MOD = 0x0203;									// OSCLK=16M, 400kbps, I2C start
		irq_sethandler(IRQ_NO_I2C0INT,i2c0_isr);		// set interrupt handler
		EI2C0 = 1;
	}
	else if(ch == 1)
	{
		clear_bit(DI2C1);
		P35D=1,P35MD1 = 0;	P35MD0 = 1;P35C1 = 1; P35C0 = 0;	// port setting
		P34D=1,P34MD1 = 0;	P34MD0 = 1;P34C1 = 1; P34C0 = 0;	// port setting
		I2C1MOD = 0x0203;									// OSCLK=16M, 400kbps, I2C start
		irq_sethandler(IRQ_NO_I2C1INT,i2c1_isr);		// set interrupt handler
		EI2C1 = 1;
	}
	return;
}

// reset parameter
void i2c_begin(UCHAR ch)
{
	// check I2C channel number
	if(ch>=2) return;

	// initializing parameters
	I2C[ch].tx_index = 0;
	I2C[ch].tx_length = 0;
	I2C[ch].rx_index = 0;
	I2C[ch].rx_length = 0;
	I2C[ch].status = I2C_MODE_READY;
	return;
}

void i2c_set_rx_buf(UCHAR ch,UCHAR *rxbufp, UINT16 length)
{
	I2C[ch].rx_buffer = rxbufp;
	I2C[ch].rx_length = length;
	I2C[ch].rx_index = 0;
	return;
}

void i2c_set_tx_buf(UCHAR ch,UCHAR *txbufp, UINT16 length)
{
	I2C[ch].tx_buffer = txbufp;
	I2C[ch].tx_length = length;
	I2C[ch].tx_index = 0;
	return;
}

void i2c_set_address(UCHAR ch, UCHAR address)
{
	if(ch == 0)
	{
		write_reg8(I2C0SA,(address << 1));		// set address to upper 7bit, LSB is RD/RW bit
	}
	else if(ch == 1)
	{
		write_reg8(I2C1SA,(address << 1));		// set address to upper 7bit, LSB is RD/RW bit
	}
	return;
}

CHAR i2c_get_status(UCHAR ch)
{
	return I2C[ch].status;
}

void i2c_send_stopbit(UCHAR ch)
{
	if(ch == 0)
	{
		write_reg8(I2C0CON0, 0x02);
	}
	else if(ch == 1)
	{
		write_reg8(I2C1CON0, 0x02);
	}
	return;
}

void i2c_start(UCHAR ch, BOOLEAN write, BOOLEAN cont)
{
	UCHAR *reg_SA;
	UCHAR *reg_CON;
	UCHAR start_code = 0;

	if(ch>=2) return;
	I2C[ch].err_code = 0;

// set register pointer
	switch(ch)
	{
	case 0:			// I2C0
		reg_SA = &I2C0SA;
		reg_CON = &I2C0CON0;
		break;
	case 1:			// I2C1
		reg_SA = &I2C1SA;
		reg_CON = &I2C1CON0;
		break;
	default:		// error
		return;
	}
// set access mode
	if(write == TRUE)
	{
		*reg_SA &= 0xFE;					// set LSB of I2C address  to 0
		I2C[ch].status = I2C_MODE_TX;		// set status to write
	}
	else // read sequence
	{
		*reg_SA |= 0x01;					// set LSB of I2C address to 1
		I2C[ch].status = I2C_MODE_RX_ADR;	// set status to read
	}
	
// set enable/disenable stop bit
	I2C[ch].bStop = cont;
	
// start or restart
	if((*reg_CON&0x01) == 0)				// check start or restart
	{
		start_code |= 0x01;
	}
	else
	{
		start_code |= 0x05;					// restart
	}
	
	*reg_CON = start_code;					// start
	
	return;
}


// I2C handler
// 15.09.20 Naotaka Saito correct return value of endTransmission
void i2c_isr(UCHAR ch)
{
	UCHAR *reg_CON;
	UCHAR *reg_TD;
	UCHAR *reg_RD;
	UCHAR startcode = 0;
	UCHAR i2c_result = 0;
	
//	if(	I2C[ch].flag == false) return;
	
//	I2C[ch].flag = false;
// Get register pointer of I2C
	switch(ch)
	{
	case 0:
		reg_CON = &I2C0CON0;
		reg_TD  = &I2C0TD;
		reg_RD  = &I2C0RD;
		break;
	case 1:
		reg_CON = &I2C1CON0;
		reg_TD  = &I2C1TD;
		reg_RD  = &I2C1RD;
		break;
	default:
		return;
	}
// check error status. Currently no operation	
	
	switch(I2C[ch].status)
	{
	// ****************** TRANSMITTING  *****************
	case I2C_MODE_TX:											// transfer following data
		I2C[ch].err_code = i2c_update_err_code(ch,I2C[ch].tx_index);
		if((I2C[ch].tx_index < I2C[ch].tx_length)&&(I2C[ch].err_code == I2C_RESULT_OK))
		{
			*reg_TD = I2C[ch].tx_buffer[I2C[ch].tx_index];		// set next data to tx register
			I2C[ch].tx_index++;
			startcode = 0x01;									// start transmitting
		}
		else													// tx_buffer full
		{
			if(I2C[ch].bStop == true)
			{
				i2c_send_stopbit(ch);
				I2C[ch].status = I2C_MODE_STOPBIT;
			}
			else
			{
				I2C[ch].status = I2C_MODE_READY;
			}
		}
		break;
	// ****************** RECIEVING *****************
	case I2C_MODE_RX_ADR:										// In case of sending I2C address, skip to send data to buffer
		I2C[ch].err_code = i2c_update_err_code(ch,I2C[ch].tx_index);
		if((I2C[ch].err_code != I2C_RESULT_OK)&&(I2C[ch].bStop != true))					// error process
		{
				I2C[ch].status = I2C_MODE_READY;
//			if(I2C[ch].bStop == true)
//			{
//				i2c_send_stopbit(ch);
//				I2C[ch].status = I2C_MODE_STOPBIT;
//			}
//			else
//			{
//				I2C[ch].status = I2C_MODE_READY;
//			}
			break;
		}
		I2C[ch].status = I2C_MODE_RX;
		startcode = 0x01;										// start transmitting
		if(i2c_check_read_last_data(ch) == true)
		{
			startcode |= 0x80;									// if next data is last, ACK is set to HIGH
			I2C[ch].status = I2C_MODE_RX_END;
		}
		break;
	case I2C_MODE_RX:											// reciever
		I2C[ch].rx_buffer[I2C[ch].rx_index] = *reg_RD; 			// set next data to rx register
		I2C[ch].rx_index++;
		startcode = 0x01;										// start transmitting
		if(i2c_check_read_last_data(ch) == true)
		{
			startcode |= 0x80;									// if next data is last, ACK is set to HIGH
			I2C[ch].status = I2C_MODE_RX_END;
		}
		break;
	case I2C_MODE_RX_END:
		I2C[ch].rx_buffer[I2C[ch].rx_index] = *reg_RD; 			// set next data to rx register
		I2C[ch].rx_index++;
		if(I2C[ch].bStop == true)
		{
			i2c_send_stopbit(ch);
			I2C[ch].status = I2C_MODE_STOPBIT;
		}
		else
		{
			I2C[ch].status = I2C_MODE_READY;
		}
		break;
	// ****************** STOP CODE *****************
	case I2C_MODE_STOPBIT:
		I2C[ch].status = I2C_MODE_READY;
		break;
	case I2C_MODE_READY:
		break;
	// ****************** ERROR *****************
	default:
		while(1){}
		break;
	}
	if(startcode != 0) *reg_CON = startcode;
	
	return;
}
// I2C0 interrupt handler
void i2c0_isr(void)
{
	i2c_isr(0);
}

// I2C1 interrupt handler
void i2c1_isr(void)
{
	i2c_isr(1);
}

BOOLEAN i2c_check_read_last_data(UCHAR ch)
{
	BOOLEAN ret;
	if((I2C[ch].rx_length - I2C[ch].rx_index) > 1)
		ret = false;											// ACK = LOW
	else
		ret = true;												// ACK = HIGH
	return ret;
}

unsigned char i2c_update_err_code(UCHAR ch, unsigned short index)
{
	unsigned char err_code = I2C_RESULT_OK;
	if(ch == 0)
	{
		if(I20ER != 0)	err_code = I2C_RESULT_ERROR;
		if(I20ACR != 0)
		{
			err_code = (index ? I2C_RESULT_DNACK :I2C_RESULT_ANACK);
		}
	}
	else if(ch == 1)
	{
		if(I21ER != 0)	err_code = I2C_RESULT_ERROR;
		if(I21ACR != 0)
		{
			err_code = (index ?I2C_RESULT_DNACK :I2C_RESULT_ANACK);
		}
	}
	
	return err_code;
}

UINT16 i2c_read_amount(UCHAR ch)
{
	return(I2C[ch].rx_index);
}

UINT8 i2c_get_err_status(UCHAR ch)
{
	return I2C[ch].err_code;
}

void i2c_force_stop(UCHAR ch)
{
	i2c_send_stopbit(ch);
	I2C[ch].status = I2C_MODE_STOPBIT;
}

void i2c_close(UCHAR ch)
{
	switch(ch)
	{
	case 0:
		P40MD1 = 0; P40MD0 = 0; P40C1 = 0; P40C0 = 0;	// port setting
		P41MD1 = 0; P41MD0 = 0; P41C1 = 0; P41C0 = 0;	// port setting
		I2C0MODL &= ~0x01;							// b1:400kbps,b0:I2C enable
		EI2C0 = 0;		QI2C0 = 0;					// disenable I2C interrupt
		I2C[ch].status = I2C_MODE_DIS;
		set_bit(DI2C0);								// disenable I2C0
		break;
	case 1:
		P35MD1 = 0;	P35MD0 = 0;P35C1 = 0; P35C0 = 0;	// port setting
		P34MD1 = 0;	P34MD0 = 0;P34C1 = 0; P34C0 = 0;	// port setting
		I2C1MODL &= ~0x01;								// I2C stop
		EI2C1 = 0;		QI2C1 = 0;					// disenable I2C interrupt
		I2C[ch].status = I2C_MODE_DIS;
		set_bit(DI2C1);								// disenable I2C1
		break;
	default:
		break;
	}
	return;
}

UINT16 i2c_write_amount(UCHAR ch)
{
	return(I2C[ch].tx_index);
}
