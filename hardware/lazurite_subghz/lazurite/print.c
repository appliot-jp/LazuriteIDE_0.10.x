/* FILE NAME: print.c
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
#include "print.h"
#include "string.h"
#include "serial.h"

void print_init(char* x, size_t size);
int printBuf_ln(void);
int printBuf(char* data);
int printBuf_l(long data, unsigned char base);
int printBuf_d(double data, unsigned char digit);
int printBuf_f(float data, unsigned char digit);
int printBuf_status(void);
int printBuf_len(void);


static unsigned short print_buf_size = 0;
static unsigned short print_buf_len = 0;
static char* print_buf=NULL;

const t_print Print =
{
	print_init,
	printBuf,
	printBuf_l,
	printBuf_f,
	printBuf_d,
	printBuf_ln,
	printBuf_status,
	printBuf_len,
};

void print_init(char* x, size_t size)
{
	
	print_buf_size = size-1;
	print_buf_len = 0;
	print_buf = x;
	if((print_buf_size == 0) || (print_buf == NULL)) return;
	*print_buf = NULL;
}

int printBuf_ln(void)
{
	if((print_buf_size == 0) || (print_buf == NULL)) return;
	
	if(print_buf_size <= print_buf_len)
	{
		*(print_buf + print_buf_size - 1) = NULL;
		print_buf_len--;
	}
	return printBuf("\n");
}

int printBuf(char* data)
{
	if((print_buf_size == 0) || (print_buf == NULL)) return;

	print_buf_len = strlen(strncat(print_buf,data,print_buf_size-print_buf_len));
	
	return print_buf_size-print_buf_len;
}

int printBuf_l(long data, unsigned char base)
{
	char buf[33];
	printNumber(buf,data,base);
	return printBuf(buf);
}

int printBuf_d(double data, unsigned char digit)
{
	char buf[33];
	printFloat(buf,data,digit);
	return printBuf(buf);
}

int printBuf_f(float data, unsigned char digit)
{
	char buf[33];
	printFloat(buf,(double)data,digit);
	return printBuf(buf);
}

int printBuf_status(void){
	if(print_buf == NULL) return -1;
	
	return print_buf_size-print_buf_len;
}

int printBuf_len(void)
{
	return print_buf_len;
}

size_t printNumber(char * x, long data, unsigned char base)
{
	char buf[33];					// buffer for charactor  size = 4byte long + NULL code
 	unsigned char digit = 0;		// number of digit
	char sign = 0;					// sign
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
		c = (char)(m - base * n);
		buf[digit] = (char)(c < 10 ? c + '0' : c + 'A' - 10);
		digit++;
	} while(n);
	
	if((base == DEC) && (sign == 1))	// generate sign
	{
		buf[digit] = '-';
		digit++;
	}
	
	for(i=0;i<(size_t)digit;i++)				// reverse charactor
	{
		x[i] = buf[digit-i-1];
	}
	x[i] = NULL;						// add NULL code
	return i;
}

// convert data to charactor
// char *x = pointer of charactor data
// float data = data to be converted
// unsigned char digit = fractional point of 4 down, 5 up round process
// size_t retun value = length of charactor
size_t printFloat(char* x, double data, unsigned char digit)
{
	long m;
	double round;
	size_t n = 0;
	unsigned char i;
	
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
		x[n++] = '-';
	}
	// round up & down process
	round = 0.5;
	for(i=0;i<digit;++i)
	{
		round /= 10;
	}
	data += round;
	
	// convert to integer
	m=(long)data;						// convert integer
	
	// printing integer part
	n += printNumber(x+n,m,DEC);		// convert integer part to charactor
	
	if(digit == 0) return n;			// in case of integer
	
	/// add dicimal point
	x[n++] = '.';
	
	// print fractional part
	data -= m;
	for(i=0;i<digit;i++)
	{
		data *= 10;
		m = (long)data;
		if(m==0)
		{
			x[n++] = '0';
		}
	}

	if( m != 0 )
	{
		 n += printNumber(x+n,m,DEC);
	}
	else
	{
		x[n++] = NULL;
	}
	
	return n;
}
