/* FILE NAME: serial_back.c
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
#include "lazurite.h"
#include "rdwr_reg.h"
#include "digitalio.h"
#include "analogio.h"
#include "mcu.h"
#include "driver_irq.h"
#include "serial.h"
#include "wdt.h"
#include <string.h>

//********************************************************************************
//   global parameters
//********************************************************************************

//********************************************************************************
//   local definitions
//********************************************************************************
//#define UART_TX_
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

UCHAR uart_rxbuf[UART_BUFFER_LENGTH];
UCHAR uart_txbuf[UART_BUFFER_LENGTH];

FIFO_CTRL uart_tx_fifo = {
	UART_BUFFER_LENGTH,
	uart_txbuf,
	0,
	0
};
FIFO_CTRL uart_rx_fifo = {
	UART_BUFFER_LENGTH,
	uart_rxbuf,
	0,
	0
};

//********************************************************************************
//   local function definitions
//********************************************************************************
void _hardware_serial_begin(UINT32 baud);
void _hardware_serial_end(void);
int _hardware_serial_rx_available(void);
int _hardware_serial_tx_available(void);
int _hardware_serial_rx_read(void);
void _hardware_serial_flush(void);
int _hardware_serial_peak(void);
void _hardware_serial_print(char* data);
void _hardware_serial_println(char* data);
void _hardware_serial_print_long(long data, UCHAR fmt);
void _hardware_serial_println_long(long data, UCHAR fmt);
void _hardware_serial_print_double(double data, UCHAR digit);
void _hardware_serial_println_double(double data, UCHAR digit);
size_t _hardware_serial_tx_write(char* data, size_t quantity);
size_t _hardware_serial_tx_write_byte(char data);

const HardwareSerial Serial = {
	_hardware_serial_begin,
	_hardware_serial_end,
	_hardware_serial_rx_available,
	_hardware_serial_rx_read,
	_hardware_serial_peak,
	_hardware_serial_flush,
	_hardware_serial_print,
	_hardware_serial_println,
	_hardware_serial_print_long,
	_hardware_serial_println_long,
	_hardware_serial_print_double,
	_hardware_serial_println_double,
	_hardware_serial_tx_write,
	_hardware_serial_tx_write_byte,
	_hardware_serial_tx_available
};

// local function
void _hardware_serial_rx_isr(void);							// rx interrupt handler
void _hardware_serial_tx_isr(void);							// tx interrupt handler
size_t _printNumber(CHAR * c, long data, UCHAR fmt);		// convert number to charactor
size_t _printFloat(CHAR * c, double data, UCHAR digit);		// convert float to charactor
volatile int _fifo_out(FIFO_CTRL* fifo_p);					// fifo output
volatile size_t _fifo_in(FIFO_CTRL* fifo_p, UCHAR data);	// fifo input
volatile size_t _fifo_available(FIFO_CTRL* fifo_p);			// return available byte
void _fifo_init(FIFO_CTRL* fifo_p);							// fifo initialization
void _fifo_out_peek(FIFO_CTRL* fifo_p);							// fifo peak (read witouht pointer incrementation)

//********************************************************************************
//   local functions
//********************************************************************************

// Enable UART
// baud = baud rate(Hz)
void _hardware_serial_begin(UINT32 baud)
{
	CHAR vmon;
	// register setup
	pinMode(VMON, INPUT);					// check of USB connection
	vmon = (CHAR)digitalRead(VMON);
	pinMode(VMON, HIZ);
	if(vmon == LOW) return;					// if USB is not connected, quit UART initialization.
	
	set_bit(P30DIR);						// setting GPIO of RXD
	clear_bit(P30C1);
	clear_bit(P30C0);
	set_bit(P30MD1);
	set_bit(P30MD0);
	clear_bit(P31DIR);						// setting GPIO of TXD
	set_bit(P31C1);
	set_bit(P31C1);
	set_bit(P31MD1);
	set_bit(P31MD0);
	
	//set BLKCON
	clear_bit(DUA0);						// enable UART0
	clear_bit(DUA1);						// enable UART0

#ifdef    EXTCLK							// BUG620Q504
	baud = 16000000/baud-1;					// calculate baud rate
	write_reg16(UA0BRT, (UINT16)baud);		// set baud rate
	write_reg8(UA0MOD1, 0x08);				// parity bit = non, stop bit = 1, flow = non
	write_reg8(UA0MOD0, 0x04);				// base clock = OSCLK(16MHz)
#else  // EXTCLK
	baud = 32768/1200-1;					// calculate baud rate
	write_reg16(UA0BRT, (UINT16)baud);		// set baud rate
	write_reg8(UA0MOD1, 0x08);				// parity bit = non, stop bit = 1, flow = non
	write_reg8(UA0MOD0, 00);				// base clock = OSCLK(16MHz)
#endif // EXTCLK

	// setting enable interrupt
	clear_bit(QUA0);						// clear irq
	clear_bit(QUA1);						// clear irq
	set_bit(EUA0);						// disenable interrupt of UART RX 
	set_bit(EUA1);						// disenable interrupt of UART TX
	irq_sethandler(IRQ_NO_UA0INT, _hardware_serial_rx_isr);		// set interrupt handler of rx
	irq_sethandler(IRQ_NO_UA1INT, _hardware_serial_tx_isr);		// set interrupt handler of tx

	// buffer reset
	_fifo_init(&uart_tx_fifo);
	_fifo_init(&uart_rx_fifo);

	// start
	set_bit(U0EN);	// UART Enable

	return;
}

// Disenable UART
void _hardware_serial_end(void)
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

// return available bytes of fifo for rx
int _hardware_serial_rx_available(void)
{
	return _fifo_available(&uart_rx_fifo);
}

// return available bytes of fifo for tx
int _hardware_serial_tx_available(void)
{
	return _fifo_available(&uart_tx_fifo);
}

// binary write(one byte)
// char data = data to be sent
// return = transfered data size(shoud be 1)
size_t _hardware_serial_tx_write_byte(char data)
{
	size_t res;
	__DI();
	res = _fifo_in(&uart_tx_fifo,data);			// input data to fifo
	if((U1FUL == 0)&&(QUA1 == 0))				// send first byte of data, if uart is not available
	{
		data = (CHAR)_fifo_out(&uart_tx_fifo);	// get data from fifo
		write_reg8(UA1BUF,(UCHAR)data);
	}
	__EI();
	return res;
}

// binary write
// char *data = data buffer pointer
//   maximum data size is defined in fifo process
// size_t quantity = data size to be written
// return = transfered data size
size_t _hardware_serial_tx_write(char* data, size_t quantity)
{
	size_t n;
	int res;
	__DI();										// disenable interrupt for avoidance of conflict in fifo access 
	for(n=0;n<quantity;n++)						// send data to fifo
	{
		if(_fifo_in(&uart_tx_fifo, data[n]) == 0)
		{
			break;
		}
	}
	if((U1FUL == 0)&&(QUA1 == 0))				// send first byte of data
	{
		res = _fifo_out(&uart_tx_fifo);
		write_reg8(UA1BUF,(UCHAR)res);
	}
	__EI();										// enable interrupt
	return n;
}

// binary read
// return = rx data. when the data is minus, fifo is empty.
int _hardware_serial_rx_read(void)
{
	int res;
	__DI();								// disenable interrupt
	res = _fifo_out(&uart_rx_fifo);		// get data
	__EI();								// enable interrupt
	return res;
}

// interrupt handler for tx
void _hardware_serial_tx_isr(void)
{
	int data;
	data = _fifo_out(&uart_tx_fifo);
	if(data < 0) return;
	write_reg8(UA1BUF, (char)data);
	return;
}

// interrupt handler for rx
void _hardware_serial_rx_isr(void)
{
	char data;
	data = read_reg8(UA0BUF);
	_fifo_in(&uart_rx_fifo, data);
	return;
}


// print long data attaching CR and LF.
void _hardware_serial_println_long(long data, UCHAR fmt)
{
	char tmp_c[33];							// 33 is maximum length of charactor
	_printNumber(tmp_c, data,fmt);			// convert long data to charactor
	_hardware_serial_println(tmp_c);		// send data though uart
	return;
}

// print long data without CR and LF.
void _hardware_serial_print_long(long data, UCHAR fmt)
{
	char tmp_c[33];
	_printNumber(tmp_c,data,fmt);
	_hardware_serial_print(tmp_c);
}

// print float data attaching CR and LF.
void _hardware_serial_println_double(double data, UCHAR digit)
{
	char tmp_c[33];
	_printFloat(tmp_c, data,digit);
	_hardware_serial_println(tmp_c);
}

// print float data without CR and LF.
void _hardware_serial_print_double(double data, UCHAR digit)
{
	char tmp_c[33];
	unsigned char n;
	_printFloat(tmp_c,data,digit);
	for(n = 0; n < 33; n++)
	{
		if(tmp_c[n] == NULL) break;
	}
	_hardware_serial_print(tmp_c);
}

// print data without CR and LF.
void _hardware_serial_print(char* data)
{
	int n = 0;
	int res;
	char tx_flag = false;
	if(	_fifo_out_peak(&uart_tx_fifo) < 0)
	{
		tx_flag = true;
	}
	while(data[n] != NULL)
	{
		if(_fifo_in(&uart_tx_fifo,data[n]) == 1)
		{
			n++;
			if(tx_flag == true)
//			if((U1FUL == 0)&&(QUA1 == 0))				// send first byte of data
			{
				res = _fifo_out(&uart_tx_fifo);
				write_reg8(UA1BUF,res);
				tx_flag = false;
			}
		}
		else
		{
			lp_setHaltMode();
		lp_setHaltMode();
		}
		wdt_clear();
	}
	return;
}

// print data with CR and LF.
void _hardware_serial_println(char* data)
{
	_hardware_serial_print(data);
	_hardware_serial_print("\r\n");
/*	int n = 0;
	char c[] = "\r\n";
	int res;
	while(data[n] != NULL)
	{
		if(_fifo_in(&uart_tx_fifo,data[n]) == 1)		// waiting until data is not full.
		{
			n++;
			if((U1FUL == 0)&&(QUA1 == 0))				// send first byte of data
			{
				res = _fifo_out(&uart_tx_fifo);
				write_reg8(UA1BUF,res);
			}
		}
	}
	for(n = 0;n < sizeof(c); n++)					// send CR and LF
	{
		while(_fifo_in(&uart_tx_fifo,c[n]) == 0)
		{
			wdt_clear();
		}	// waiting until data is not full.
		if((U1FUL == 0)&&(QUA1 == 0))				// send first byte of data
		{
			res = _fifo_out(&uart_tx_fifo);
			write_reg8(UA1BUF,res);
		}
	}
	return;*/
}

void _hardware_serial_flush(void)
{
	_fifo_init(&uart_rx_fifo);
	return;
}
int _hardware_serial_peak(void)
{
	return _fifo_out_peak(&uart_rx_fifo);
}



// input data to fifo.
volatile size_t _fifo_in(FIFO_CTRL* fifo_p, UCHAR data)
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
volatile int _fifo_out(FIFO_CTRL* fifo_p)
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

volatile int _fifo_out_peak(FIFO_CTRL* fifo_p)
{
	if ( fifo_p->length == 0) return -1;	// if no data, return -1
	return fifo_p->buf[fifo_p->rd_p];
}

// return available byte of fifo
volatile size_t _fifo_available(FIFO_CTRL* fifo_p)
{
	return (fifo_p->max_length - fifo_p->length);
}

// initializing fifo
void _fifo_init(FIFO_CTRL* fifo_p)
{
	fifo_p->wr_p = 0;
	fifo_p->rd_p = 0;
	fifo_p->length = 0;
	return;
}
// convert data to charactor
// CHAR *x = pointer of charactor data
// long data = data to be converted
// UCHAR base = data format(unit) BIN, OCT, DEC, HEX
// size_t retun value = length of charactor
size_t _printNumber(CHAR * x, long data, UCHAR base)
{
	char buf[33];			// buffer for charactor  size = 4byte long + NULL code
 	char digit = 0;			// number of digit
	char sign = 0;			// sign
	unsigned long n;
	unsigned long m;
    char c;
	size_t i;
	if (base < 2) base = 10;			// if base is wrong, it is fixed to DEC
	n = (unsigned long)data;

	if((base == DEC) && (data < 0))		// process for minus
	{
		sign = 1;						// stack sign bit
		n = ~n;							// convert to absolute value
		n += 1;							// convert to absolute value
	}

	do {								// convert data to character from 1st digit
		m = n;
		n /= base;
		c = (CHAR)(m - base * n);
		buf[digit] = (CHAR)(c < 10 ? c + '0' : c + 'A' - 10);
		digit++;
	} while(n);
	
	if((base == DEC) && (sign == 1))	// generate sign
	{
		buf[digit] = '-';
		digit++;
	}
	
	for(i=0;i<digit;i++)				// reverse charactor
	{
		x[i] = buf[digit-i-1];
	}
	x[i] = NULL;						// add NULL code
	return i;
}

// convert data to charactor
// CHAR *x = pointer of charactor data
// float data = data to be converted
// UCHAR digit = fractional point of 4 down, 5 up round process
// size_t retun value = length of charactor
size_t _printFloat(CHAR* x, double data, UCHAR digit)
{
	long m;
	double round;
	char *y = x;
	UCHAR i;
	size_t m_digit;
	
	// check overflow
	if((data > 2147483647.5)||(data < -2147483647.5))
	{
		strcpy(x,"ovf");
		return sizeof("ovf");
	}
	
	// convert to absolute value
	if(data < 0)
	{
		data *= -1;
		*y = '-';
		y++;
	}
	// round up & down process
	round = 0.5;
	for(i=0;i<digit;++i)
	{
		round /= 10;
	}
	data += round;
	
	// convert to integer
	m=(long)data;							// convert integer
	
	// printing integer part
	m_digit = _printNumber(y,m,DEC);		// convert integer part to charactor
	
	if(digit == 0) return m_digit;
	
	/// add dicimal point
	y += m_digit;
	*y = '.';
	y++;
	
	// print fractional part
	data -= m;
	for(i=0;i<digit;i++)
	{
		data *= 10;
		m = (long)data;
		if(m==0)
		{
			*y = '0';
			y++;
		}
	}
	data -= m;
	
	return _printNumber(y,m,DEC);
}

void serial_logout_i(UCHAR* addr, UINT16 val)
{
	Serial.print_long((long)addr,HEX);					// test
	Serial.print("\t");									// test
	Serial.println_long((long)val,HEX);					// test
	return;
}

