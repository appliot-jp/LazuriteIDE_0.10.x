/*****************************************************************************
    sys_uart.h

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
#ifndef _SYS_UART_H_
#define _SYS_UART_H_



/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
void sys_uart_init(void);
void uart_send_sync(unsigned char *data, unsigned int size);
void uart_set_port(void);
void uart_clear_port(void);


#endif	/* _SYS_UART_H_ */
