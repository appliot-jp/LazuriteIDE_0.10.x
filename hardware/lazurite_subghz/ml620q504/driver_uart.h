/* FILE NAME: driver_uart.h
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

#ifndef _DRIVER_UART_H_
#define _DRIVER_UART_H_

#include "common.h"
#include "mcu.h"


//********************************************************************************
//   global definitions
//********************************************************************************
#define UART_BUFFER_LENGTH 128
//********************************************************************************
//   global parameters
//********************************************************************************
extern char uart_tx_sending;							// true = send, false = not send
extern char uartf_tx_sending;							// true = send, false = not send


//********************************************************************************
//   extern function definitions
//********************************************************************************
extern void uartf_gpio_init(unsigned char n);
extern void uartf_gpio_end(unsigned char n);
extern void uart_begin(UINT32 baud);
extern void uartf_begin(UINT32 baud);
extern void uart_end(void);
extern void uartf_end(void);
extern int uart_rx_available(void);
extern int uartf_rx_available(void);
extern int uart_tx_available(void);
extern int uartf_tx_available(void);
extern size_t uart_tx_write(char data);
extern size_t uartf_tx_write(char data);
extern int uart_rx_read(void);
extern int uartf_rx_read(void);
extern void uart_flush(void);
extern void uartf_flush(void);
extern int uart_peak(void);
extern int uartf_peak(void);

#endif // _DRIVER_UART_H_

