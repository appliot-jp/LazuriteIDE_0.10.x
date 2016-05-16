/* FILE NAME: driver_uart.c
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

#include "common.h"
#include "rdwr_reg.h"
#include "mcu.h"
#include "driver_irq.h"
#include "driver_uart.h"
#include "wdt.h"
#include "lp_manage.h"

//********************************************************************************
//   global parameters
//********************************************************************************

//********************************************************************************
//   local definitions
//********************************************************************************
#define GPIO_DIR_OFFSET		1
#define GPIO_C0_OFFSET		2
#define GPIO_C1_OFFSET		3
#define GPIO_MD0_OFFSET		4
#define GPIO_MD1_OFFSET		5

#define UAF0LSRL_UF0RFE		0x80
#define UAF0LSRL_UF0TEMT	0x40
#define UAF0LSRL_UF0THRE	0x20
#define UAF0LSRL_UF0BI		0x10
#define UAF0LSRL_UF0FER		0x08
#define UAF0LSRL_UF0PER		0x04
#define UAF0LSRL_UF0OER		0x02
#define UAF0LSRL_UF0DR		0x01

//********************************************************************************
//   local parameters
//********************************************************************************
typedef struct {
	UCHAR max_length;
	UCHAR *buf;
	UCHAR wr_p;
	UCHAR rd_p;
	UCHAR length;
} FIFO_CTRL;

static UCHAR uart_rx_buf[UART_BUFFER_LENGTH];
static UCHAR uart_tx_buf[UART_BUFFER_LENGTH];
static UCHAR uartf_rx_buf[UART_BUFFER_LENGTH];
static UCHAR uartf_tx_buf[UART_BUFFER_LENGTH];

static char uart_tx_flag;							// true = send, false = not send
static char uartf_tx_flag;							// true = send, false = not send
char uart_tx_sending=false;
char uartf_tx_sending=false;

static FIFO_CTRL uart_tx_fifo = {
	UART_BUFFER_LENGTH,
	uart_tx_buf,
	0,
	0
};
static FIFO_CTRL uart_rx_fifo = {
	UART_BUFFER_LENGTH,
	uart_rx_buf,
	0,
	0
};

static FIFO_CTRL uartf_tx_fifo = {
	UART_BUFFER_LENGTH,
	uartf_tx_buf,
	0,
	0
};
static FIFO_CTRL uartf_rx_fifo = {
	UART_BUFFER_LENGTH,
	uartf_rx_buf,
	0,
	0
};

const char* uartf_gpio_adr[] =
{
	&P4D,
	&P2D,
	&P3D
};
const char uartf_gpio_rx_bitmask[] =
{
	(1<<4),
	(1<<0),
	(1<<4)
};
const char uartf_gpio_tx_bitmask[] =
{
	(1<<5),
	(1<<1),
	(1<<5)
};


//********************************************************************************
//   local function definitions
//********************************************************************************

static void uart_tx_isr(void);
static void uart_rx_isr(void);
static void uartf_isr(void);
static volatile size_t uart_fifo_in(FIFO_CTRL* fifo_p, UCHAR data);
static volatile int uart_fifo_out(FIFO_CTRL* fifo_p);
static volatile int uart_fifo_out_peek(FIFO_CTRL* fifo_p);
static volatile size_t uart_fifo_available(FIFO_CTRL* fifo_p);
static void uart_fifo_init(FIFO_CTRL* fifo_p);

//********************************************************************************
//   local functions
//********************************************************************************

// ----------------------------------------------
// function  uartf_gpio_init(unsigned char n)
// (m) = arduino pin number
//       RX        TX
// n=0   P44(4)    P45(5)
// n=1   P20(A0)   P21(A1)
// n=2   P34(A4)   P35(A5)
// ----------------------------------------------
void uartf_gpio_init(unsigned char n)
{
	unsigned char* port;
	unsigned char bit;
	if(n>3) return;
	port = uartf_gpio_adr[n];
	bit = uartf_gpio_rx_bitmask[n];
	*(port+GPIO_DIR_OFFSET) |= bit;		//DIR = 1
	*(port+GPIO_C0_OFFSET)  |= bit;		//C0  = 1
	*(port+GPIO_C1_OFFSET)  |= bit;		//C1  = 1
	*(port+GPIO_MD0_OFFSET) |= bit;		//MD0 = 1
	*(port+GPIO_MD1_OFFSET) |= bit;		//MD1 = 1

	bit = uartf_gpio_tx_bitmask[n];
	*(port+GPIO_DIR_OFFSET) &= ~bit;	//DIR = 0
	*(port+GPIO_C0_OFFSET)  |= bit;		//C0  = 1
	*(port+GPIO_C1_OFFSET)  |= bit;		//C1  = 1
	*(port+GPIO_MD0_OFFSET) |= bit;		//MD0 = 1
	*(port+GPIO_MD1_OFFSET) |= bit;		//MD1 = 1

}

void uartf_gpio_end(unsigned char n)
{
	volatile unsigned char* port;
	volatile unsigned char bit;
	if(n>3) return;

	port = uartf_gpio_adr[n];
	bit = uartf_gpio_rx_bitmask[n];
	*(port+GPIO_DIR_OFFSET)&= ~bit;		//DIR = 0
	*(port+GPIO_C0_OFFSET) &= ~bit;		//C0  = 0
	*(port+GPIO_C1_OFFSET) &= ~bit;		//C1  = 0
	*(port+GPIO_MD0_OFFSET)&= ~bit;		//MD0 = 0
	*(port+GPIO_MD1_OFFSET)&= ~bit;		//MD1 = 0
	
	 bit = uartf_gpio_tx_bitmask[n];
	*(port+GPIO_DIR_OFFSET)&= ~bit;		//DIR = 0
	*(port+GPIO_C0_OFFSET) &= ~bit;		//C0  = 0
	*(port+GPIO_C1_OFFSET) &= ~bit;		//C1  = 0
	*(port+GPIO_MD0_OFFSET)&= ~bit;		//MD0 = 0
	*(port+GPIO_MD1_OFFSET)&= ~bit;		//MD1 = 0

}


// Enable UART
// baud = baud rate(Hz)
void uart_begin(UINT32 baud)
{
	set_bit(P30DIR);					// setting GPIO of RXD
	clear_bit(P30C1);
	clear_bit(P30C0);
	set_bit(P30MD1);
	set_bit(P30MD0);
	clear_bit(P31DIR);					// setting GPIO of TXD
	set_bit(P31C1);
	set_bit(P31C0);
	set_bit(P31MD1);
	set_bit(P31MD0);
	//set BLKCON
	clear_bit(DUA0);					// enable UART0
	clear_bit(DUA1);					// enable UART0
	
//#ifdef    EXTCLK							// BUG620Q504
	baud = 16000000/baud-1;					// calculate baud rate
	write_reg16(UA0BRT, (UINT16)baud);		// set baud rate
	write_reg8(UA0MOD1, 0x08);				// parity bit = non, stop bit = 1, flow = non
	write_reg8(UA0MOD0, 0x04);				// base clock = OSCLK(16MHz)
//#else  // EXTCLK
//	baud = 32768/1200-1;					// calculate baud rate
//	write_reg16(UA0BRT, (UINT16)baud);		// set baud rate
//	write_reg8(UA0MOD1, 0x08);				// parity bit = non, stop bit = 1, flow = non
//	write_reg8(UA0MOD0, 00);				// base clock = OSCLK(16MHz)
//#endif // EXTCLK

	// setting enable interrupt
	clear_bit(QUA0);						// clear irq
	clear_bit(QUA1);						// clear irq
	set_bit(EUA0);						// disenable interrupt of UART RX 
	set_bit(EUA1);						// disenable interrupt of UART TX
	irq_sethandler(IRQ_NO_UA0INT, uart_rx_isr);		// set interrupt handler of rx
	irq_sethandler(IRQ_NO_UA1INT, uart_tx_isr);		// set interrupt handler of tx

	// buffer reset
	uart_fifo_init(&uart_tx_fifo);
	uart_fifo_init(&uart_rx_fifo);

	// start
	set_bit(U0EN);	// UART Enable
	uart_tx_flag = false;
	uart_tx_sending = false;

	return;
}

void uartf_begin(UINT32 baud)
{

	clear_bit(DUAF0);	// UART Enable

	baud = 16000000*12/(baud*13*16);
	
	write_reg16(UAF0MOD,0x63);
	write_reg16(UAF0MOD,0x03);
	
	write_reg8(UAF0CAJ,0x0D);

	set_bit(UF0DLAB);
	write_reg16(UAF0BUF, (UINT16)baud);
	clear_bit(UF0DLAB);
	
	write_reg8(UAF0IER,0x07);						// all interrupts are enabled.
	
	clear_bit(QUAF0);								// clear irq
	set_bit(EUAF0);									// enable interrupt of UART TX
	irq_sethandler(IRQ_NO_UAF0INT, uartf_isr);		// set interrupt handler of rx

	// buffer reset
	uart_fifo_init(&uartf_tx_fifo);
	uart_fifo_init(&uartf_rx_fifo);
	// start
	uartf_tx_flag = false;
	return;
}

// Disenable UART
void uart_end(void)
{
  // wait for transmission of outgoing data
	clear_bit(EUA0);						// disenable interrupt of UART RX 
	clear_bit(EUA1);						// disenable interrupt of UART TX
	clear_bit(QUA0);						// clear irq
	clear_bit(QUA1);						// clear irq
	clear_bit(U0EN);						// rx disenable
//	clear_bit(U1EN);						// tx disenable
	set_bit(DUA0);							// enable UART0
	set_bit(DUA1);							// enable UART0
	return;
}

void uartf_end(void)
{
  // wait for transmission of outgoing data
	clear_bit(EUAF0);						// disenable interrupt of UART RX 
	clear_bit(QUAF0);						// clear irq
	set_bit(DUAF0);							// enable UART0
	return;
}

// return available bytes of fifo for rx
int uart_rx_available(void)
{
	return uart_rx_fifo.length;
}

int uartf_rx_available(void)
{
	return uartf_rx_fifo.length;
}

// return available bytes of fifo for tx
int uart_tx_available(void)
{
	return (uart_tx_fifo.max_length-uart_tx_fifo.length);
}

int uartf_tx_available(void)
{
	return (uartf_tx_fifo.max_length-uartf_tx_fifo.length);
}

// binary write(one byte)
// char data = data to be sent
// return = transfered data size(shoud be 1)
size_t uart_tx_write(char data)
{
	size_t res;
	
//	__DI();										// disenable interrupt
	dis_interrupts(DI_UART);
	res = uart_fifo_in(&uart_tx_fifo,data);
	
	if(res == 1)
	{
		if(uart_tx_flag == false)
		{
			data = (char)uart_fifo_out(&uart_tx_fifo);
			write_reg8(UA1BUF,data);
			uart_tx_flag = true;
			uart_tx_sending = true;
		}
	}
//	__EI();										// enable interrupt
	enb_interrupts(DI_UART);
	return res;
}

size_t uartf_tx_write(char data)
{
	size_t res;
	
//	__DI();										// disenable interrupt
	dis_interrupts(DI_UARTF);
	res = uart_fifo_in(&uartf_tx_fifo,data);
	
	if(res == 1)
	{
		if(uartf_tx_flag == false)
		{
			data = (char)uart_fifo_out(&uartf_tx_fifo);
			write_reg8(UAF0BUF,data);
			set_bit(UF0IRQ);
			uartf_tx_flag = true;
			uartf_tx_sending = true;
		}
	}
//	__EI();										// enable interrupt
	enb_interrupts(DI_UARTF);
	return res;
}


// binary read
// return = rx data. when the data is minus, fifo is empty.
int uart_rx_read(void)
{
	int res;
//	__DI();								// disenable interrupt
	dis_interrupts(DI_UART);
	res = uart_fifo_out(&uart_rx_fifo);		// get data
//	__EI();								// enable interrupt
	enb_interrupts(DI_UART);

	return res;
}

int uartf_rx_read(void)
{
	int res;
//	__DI();								// disenable interrupt
	dis_interrupts(DI_UARTF);
	res = uart_fifo_out(&uartf_rx_fifo);		// get data
//	__EI();								// enable interrupt
	enb_interrupts(DI_UARTF);
	return res;
}

void uart_flush(void)
{
	uart_fifo_init(&uart_rx_fifo);
	return;
}

void uartf_flush(void)
{
	uart_fifo_init(&uartf_rx_fifo);
	return;
}

int uart_peek(void)
{
	return uart_fifo_out_peek(&uart_rx_fifo);
}

int uartf_peek(void)
{
	return uart_fifo_out_peek(&uartf_rx_fifo);
}

// interrupt handler for tx
void uart_tx_isr(void)
{
	int data;
	if(uart_tx_flag == false)
	{
		uart_tx_sending = false;
	}
	data = uart_fifo_out(&uart_tx_fifo);
	if(data < 0)
	{
		uart_tx_flag = false;
		return;
	}
	write_reg8(UA1BUF, (char)data);
	return;
}

// interrupt handler for rx
void uart_rx_isr(void)
{
	char data;
	data = read_reg8(UA0BUF);
	uart_fifo_in(&uart_rx_fifo, data);
	return;
}

void uartf_isr(void)
{
	int data;
	volatile unsigned short status;
	status = read_reg16(UAF0IIR);
	switch(status&0x000E)
	{
	case (3<<1):							// ERROR
		status = read_reg16(UAF0LSR);
		break;
	case (2<<1):							// READ
	case (6<<1):							// READ
		data = read_reg8(UAF0BUF);
		uart_fifo_in(&uartf_rx_fifo, (unsigned char)data);
		break;
	case (1<<1):							// SEND
		data = uart_fifo_out(&uartf_tx_fifo);
		if(uartf_tx_flag == false)
		{
			uart_tx_sending = false;
		}
		if(data < 0)
		{
			uartf_tx_flag = false;
		}
		else
		{
			write_reg8(UAF0BUF, (char)data);
		}
		break;
	default:
		while(1){}
		break;
	}
	/*
	status = read_reg8(UAF0LSR);
	if((status & UAF0LSRL_UF0RFE) != 0)			// data with error is in FIFO
	{
	}
	
	if((status & UAF0LSRL_UF0TEMT) != 0)		// tx buf empty
	{
	}
	if((status & UAF0LSRL_UF0THRE) != 0)		// prepaired next tx data
	{
		data = uart_fifo_out(&uartf_tx_fifo);
		if(data < 0)
		{
			uartf_tx_flag = false;
		}
		else
		{
			write_reg8(UAF0BUF, (char)data);
		}
	}
	if((status & UAF0LSRL_UF0BI) != 0)			// break interrupt
	{
	}
	if((status & UAF0LSRL_UF0FER) != 0)			// framing error
	{
	}
	if((status & UAF0LSRL_UF0PER) != 0)			// parity error
	{
	}
	if((status & UAF0LSRL_UF0OER) != 0)			// over run error
	{
	}
	if((status & UAF0LSRL_UF0DR) != 0)			// data is in read buffer
	{
		data = read_reg8(UAF0BUF);
		uart_fifo_in(&uartf_rx_fifo, (unsigned char)data);
	}
	*/
	set_bit(UF0IRQ);
	return;
}

//******************************************************************************
//   FIFO CONTROL for UART
//******************************************************************************

volatile size_t uart_fifo_in(FIFO_CTRL* fifo_p, UCHAR data)
{
	if ( fifo_p->length >= fifo_p->max_length) return 0;	// fifo is full
	fifo_p->buf[fifo_p->wr_p] = data;						// input data to fifo
	fifo_p->length++;										// increment written length
	fifo_p->wr_p++;											// increment written pointer.
	if( fifo_p->wr_p >= fifo_p->max_length )				// if writtten pointer reach maximum length, it is reset.
	{
		fifo_p->wr_p = 0;
	}
	return 1;
}

// output data from fifo.
volatile int uart_fifo_out(FIFO_CTRL* fifo_p)
{
	UCHAR tmp = fifo_p->rd_p;				// stack read pointer.
	if ( fifo_p->length == 0) return -1;	// if no data, return -1
	fifo_p->length--;						// decrement written length
	fifo_p->rd_p++;							// increment read pointer
	if( fifo_p->rd_p >= fifo_p->max_length )	// if read pointer reach to maximum, it is reset.
	{
		fifo_p->rd_p = 0;
	}
	return fifo_p->buf[tmp];
}

volatile int uart_fifo_out_peek(FIFO_CTRL* fifo_p)
{
	if ( fifo_p->length == 0) return -1;	// if no data, return -1
	return fifo_p->buf[fifo_p->rd_p];
}

/*
// return available byte of fifo
volatile size_t uart_fifo_available(FIFO_CTRL* fifo_p)
{
	return (fifo_p->max_length - fifo_p->length);
}
*/

// initializing fifo
void uart_fifo_init(FIFO_CTRL* fifo_p)
{
	fifo_p->wr_p = 0;
	fifo_p->rd_p = 0;
	fifo_p->length = 0;
	return;
}
