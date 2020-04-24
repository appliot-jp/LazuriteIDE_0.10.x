/* FILE NAME: serial.c
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

#include "common.h"
#include "rdwr_reg.h"
#include "driver_irq.h"
#include "driver_uart.h"
#include "serial.h"
#include "wdt.h"
#include "lp_manage.h"
#include "print.h"
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
static const char crlf[] = "\r\n";

//********************************************************************************
//   local function definitions
//********************************************************************************
static void _hardware_serial11_begin(unsigned long baud);
static void _hardware_serial11_end(void);
static void _hardware_serial12_begin(unsigned long baud);
static void _hardware_serial12_end(void);
static void _hardware_serial13_begin(unsigned long baud);
static void _hardware_serial13_end(void);
static void _hardware_serial_print(char* data);
static void _hardware_serial_println(char* data);
static void _hardware_serial_print_long(long data, UCHAR fmt);
static void _hardware_serial_println_long(long data, UCHAR fmt);
static void _hardware_serial_print_double(double data, UCHAR digit);
static void _hardware_serial_println_double(double data, UCHAR digit);
static size_t _hardware_serial_tx_write(char* data, size_t quantity);

static void _hardware_serial21_begin(unsigned long baud);
static void _hardware_serial22_begin(unsigned long baud);
static void _hardware_serial23_begin(unsigned long baud);
static void _hardware_serial21_end(void);
static void _hardware_serial22_end(void);
static void _hardware_serial23_end(void);
static void _hardware_serial2_print(char* data);
static void _hardware_serial2_println(char* data);
static void _hardware_serial2_print_long(long data, UCHAR fmt);
static void _hardware_serial2_println_long(long data, UCHAR fmt);
static void _hardware_serial2_print_double(double data, UCHAR digit);
static void _hardware_serial2_println_double(double data, UCHAR digit);
static size_t _hardware_serial2_tx_write(char* data, size_t quantity);

const HardwareSerial Serial = {
	_hardware_serial12_begin,
	_hardware_serial12_end,
	uart_rx_available,
	uart_rx_read,
	uart_peek,
	uart_flush,
	_hardware_serial_print,
	_hardware_serial_println,
	_hardware_serial_print_long,
	_hardware_serial_println_long,
	_hardware_serial_print_double,
	_hardware_serial_println_double,
	_hardware_serial_tx_write,
	uart_tx_write,
	uart_tx_available
};
const HardwareSerial Serial1 = {
	_hardware_serial21_begin,
	_hardware_serial21_end,
	uartf_rx_available,
	uartf_rx_read,
	uartf_peek,
	uartf_flush,
	_hardware_serial2_print,
	_hardware_serial2_println,
	_hardware_serial2_print_long,
	_hardware_serial2_println_long,
	_hardware_serial2_print_double,
	_hardware_serial2_println_double,
	_hardware_serial2_tx_write,
	uartf_tx_write,
	uartf_tx_available
};
const HardwareSerial Serial2 = {
	_hardware_serial22_begin,
	_hardware_serial22_end,
	uartf_rx_available,
	uartf_rx_read,
	uartf_peek,
	uartf_flush,
	_hardware_serial2_print,
	_hardware_serial2_println,
	_hardware_serial2_print_long,
	_hardware_serial2_println_long,
	_hardware_serial2_print_double,
	_hardware_serial2_println_double,
	_hardware_serial2_tx_write,
	uartf_tx_write,
	uartf_tx_available
};
const HardwareSerial Serial3 = {
	_hardware_serial23_begin,
	_hardware_serial23_end,
	uartf_rx_available,
	uartf_rx_read,
	uartf_peek,
	uartf_flush,
	_hardware_serial2_print,
	_hardware_serial2_println,
	_hardware_serial2_print_long,
	_hardware_serial2_println_long,
	_hardware_serial2_print_double,
	_hardware_serial2_println_double,
	_hardware_serial2_tx_write,
	uartf_tx_write,
	uartf_tx_available
};

// local function

//********************************************************************************
//   local functions
//********************************************************************************
void _hardware_serial11_begin(unsigned long baud)
{
	uart_gpio_init(0);
	uart_begin(baud,NULL,NULL);
}
void _hardware_serial11_end(void)
{
	uart_gpio_end(0);
	uart_end();
}
void _hardware_serial12_begin(unsigned long baud)
{
	uart_gpio_init(1);
	uart_begin(baud,NULL,NULL);
}
void _hardware_serial12_end(void)
{
	uart_gpio_end(1);
	uart_end();
}
void _hardware_serial13_begin(unsigned long baud)
{
	uart_gpio_init(2);
	uart_begin(baud,NULL,NULL);
}
void _hardware_serial13_end(void)
{
	uart_gpio_end(2);
	uart_end();
}

void _hardware_serial21_begin(unsigned long baud)
{
	uart_gpio_init(3);
	uartf_begin(baud);
}
void _hardware_serial22_begin(unsigned long baud)
{
	uart_gpio_init(4);
	uartf_begin(baud);
}
void _hardware_serial23_begin(unsigned long baud)
{
	uart_gpio_init(5);
	uartf_begin(baud);
}
void _hardware_serial21_end(void)
{
	uart_gpio_end(3);
	uartf_end();
}
void _hardware_serial22_end(void)
{
	uart_gpio_end(4);
	uartf_end();
}
void _hardware_serial23_end(void)
{
	uart_gpio_end(5);
	uartf_end();
}
// binary write
// char *data = data buffer pointer
//   maximum data size is defined in fifo process
// size_t quantity = data size to be written
// return = transfered data size
size_t _hardware_serial_tx_write(char* data, size_t quantity)
{
	size_t n;
	
	for(n=0;n<quantity;n++)
	{
		if(uart_tx_write(data[n]) != 1)
		{
			break;
		}
	}
	
	return n;
}

size_t _hardware_serial2_tx_write(char* data, size_t quantity)
{
	size_t n;
	
	for(n=0;n<quantity;n++)
	{
		if(uartf_tx_write(data[n]) != 1)
		{
			break;
		}
	}
	
	return n;
}

// print long data attaching CR and LF.
void _hardware_serial_println_long(long data, UCHAR fmt)
{
	char tmp_c[33];							// 33 is maximum length of charactor
	printNumber(tmp_c, data,fmt);			// convert long data to charactor
	_hardware_serial_println(tmp_c);		// send data though uart
	return;
}

void _hardware_serial2_println_long(long data, UCHAR fmt)
{
	char tmp_c[33];							// 33 is maximum length of charactor
	printNumber(tmp_c, data,fmt);			// convert long data to charactor
	_hardware_serial2_println(tmp_c);		// send data though uart
	return;
}

// print long data without CR and LF.
void _hardware_serial_print_long(long data, UCHAR fmt)
{
	char tmp_c[33];
	printNumber(tmp_c,data,fmt);
	_hardware_serial_print(tmp_c);
}

void _hardware_serial2_print_long(long data, UCHAR fmt)
{
	char tmp_c[33];
	printNumber(tmp_c,data,fmt);
	_hardware_serial2_print(tmp_c);
}



// print float data attaching CR and LF.
void _hardware_serial_println_double(double data, UCHAR digit)
{
	char tmp_c[33];
	printFloat(tmp_c, data,digit);
	_hardware_serial_println(tmp_c);
}
void _hardware_serial2_println_double(double data, UCHAR digit)
{
	char tmp_c[33];
	printFloat(tmp_c, data,digit);
	_hardware_serial2_println(tmp_c);
}

// print float data without CR and LF.
void _hardware_serial_print_double(double data, UCHAR digit)
{
	char tmp_c[33];
//	unsigned char n;
	printFloat(tmp_c,data,digit);
//	for(n = 0; n < 33; n++)
//	{
//		if(tmp_c[n] == NULL) break;
//	}
	_hardware_serial_print(tmp_c);
}
void _hardware_serial2_print_double(double data, UCHAR digit)
{
	char tmp_c[33];
//	unsigned char n;
	printFloat(tmp_c,data,digit);
//	for(n = 0; n < 33; n++)
//	{
//		if(tmp_c[n] == NULL) break;
//	}
	_hardware_serial2_print(tmp_c);
}

// print data without CR and LF.
void _hardware_serial_print(char* data)
{
	int n = 0;
	if(data != NULL) {
		while(data[n] != NULL)
		{
			if(uart_tx_write(data[n]) == 1)
			{
				n++;
			} else {
				if(getMIE() == 0) {
					di_wait();
				} else {
					lp_setHaltMode();
				}
			}
		}
	}
	return;
}

void _hardware_serial2_print(char* data)
{
	int n = 0;

	if(data != NULL) {
		while(data[n] != NULL)
		{
			if(uartf_tx_write(data[n]) == 1)
			{
				n++;
			} else {
				if(getMIE() == 0) {
					di_wait();
				} else {
					lp_setHaltMode();
				}
			}
		}
	}
	return;
}

// print data with CR and LF.
void _hardware_serial_println(char* data)
{
	_hardware_serial_print(data);
	_hardware_serial_print(crlf);
}
void _hardware_serial2_println(char* data)
{
	_hardware_serial2_print(data);
	_hardware_serial2_print(crlf);
}

