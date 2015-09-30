
#include "lazurite.h"
#include "SerialLED.h"
#include "driver_pin_assignment.h"


static unsigned char led_pin;
static short data_length=0;
static void led_ctrl_init(unsigned char pin, unsigned short len)
{
	led_pin = pin;
	pinMode(led_pin, OUTPUT);
	data_length = len * 3;
	digitalWrite(led_pin,LOW);
}

static void led_ctrl_write(unsigned char *data)
{
#pragma ASM
	push	lr
	push	xr8
	push	xr4
	mov	er8,	er0

;;	port = ml620504f_pin_to_port[digital_pin_to_port[led_pin]];
	l	r0,	NEAR _led_pin
	mov	r1,	#00h
	l	r2,	NEAR _digital_pin_to_port[er0]
	mov	r3,	#00h
	mov	er0,	er2
	add	er0,	er2
	l	er10,	NEAR _ml620504f_pin_to_port[er0]

;;	port_data0 = *port&~bit;			// prepare port data = 0
	l	r4,	[er10]
	mov	er6,	er2
	l	r2,	NEAR _ml620504f_pin_to_bit[er2]
	xor	r2,	#0ffh
	and	r2,	r4
	mov	r4,	r2
	mov	r5,	r2	;; _lport_data0$8

;;	port_data1 = *port|bit;				// prepare port data = 1
	l	r2,	[er10]
	l	r3,	NEAR _ml620504f_pin_to_bit[er6]
	or	r2,	r3

;;	*port = port_data0;					// initializing port
	st	r4,	[er10]

;;	i=data_length;
	l	er6,	NEAR _data_length

;;	tmp = *data;
	l	r0,	[er8]
	mov	r3,	r0	;; _ltmp$2

;;start:
_$L2 :

;;	*port = port_data1;
	st	r2,	[er10]

;;	if((tmp&0x80)==0)
	tb	r3.7
	bne	_$L3

;;		__asm("nop");				// 0-0
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		*port = port_data0;
	st	r5,	[er10]

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;	else
	bal	_$L5
_$L3 :

;;		__asm("nop");				// 0-1
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		*port = port_data0;
	st	r5,	[er10]

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;	}
_$L5 :
	add	er8,	#1 

;;	tmp <<=1;
	sll	r3,	#01h

;;	*port = port_data1;
	st	r2,	[er10]

;;	if((tmp&0x80)==0)
	tb	r3.7
	bne	_$L6
CBLOCK 196 5 73

;;		__asm("nop");				// 1-0
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		*port = port_data0;
	st	r5,	[er10]

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;	else
	bal	_$L8
_$L6 :

;;		__asm("nop");				// 1-1
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		*port = port_data0;
	st	r5,	[er10]

;;		__asm("nop");
nop

;;		__asm("nop");
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0066H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0067H 0003H 000FH
nop
CBLOCKEND 196 6 104

;;	}
CLINEA 0000H 0000H 0068H 0002H 0002H
_$L8 :
	add	er6,	#-1

;;	tmp <<=1;
CLINEA 0000H 0001H 0069H 0002H 000AH
	sll	r3,	#01h

;;	*port = port_data1;
CLINEA 0000H 0001H 006AH 0002H 0014H
	st	r2,	[er10]

;;	if((tmp&0x80)==0)
CLINEA 0000H 0001H 006BH 0002H 0012H
	tb	r3.7
	bne	_$L9
CBLOCK 196 7 108

;;		__asm("nop");				// 2-0
CLINEA 0000H 0001H 006DH 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 006EH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 006FH 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 0070H 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 0071H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0072H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0073H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0074H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0075H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0076H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0077H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0078H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0079H 0003H 000FH
nop
CBLOCKEND 196 7 122

;;	else
CLINEA 0000H 0001H 007BH 0002H 0005H
	bal	_$L11
_$L9 :
CBLOCK 196 8 124

;;		__asm("nop");				// 2-1
CLINEA 0000H 0001H 007DH 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 007EH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 007FH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0080H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0081H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0082H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0083H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0084H 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 0085H 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 0086H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0087H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0088H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0089H 0003H 000FH
nop
CBLOCKEND 196 8 138

;;	}
CLINEA 0000H 0000H 008AH 0002H 0002H
_$L11 :

;;	tmp <<=1;
CLINEA 0000H 0001H 008BH 0002H 000AH
	sll	r3,	#01h

;;	*port = port_data1;
CLINEA 0000H 0001H 008CH 0002H 0014H
	st	r2,	[er10]

;;	if((tmp&0x80)==0)
CLINEA 0000H 0001H 008DH 0002H 0012H
	tb	r3.7
	bne	_$L12
CBLOCK 196 9 142

;;		__asm("nop");				// 3-0
CLINEA 0000H 0001H 008FH 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0090H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0091H 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 0092H 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 0093H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0094H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0095H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0096H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0097H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0098H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0099H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 009AH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 009BH 0003H 000FH
nop
CBLOCKEND 196 9 156

;;	else
CLINEA 0000H 0001H 009DH 0002H 0005H
	bal	_$L14
_$L12 :
CBLOCK 196 10 158

;;		__asm("nop");				// 3-1
CLINEA 0000H 0001H 009FH 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00A0H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00A1H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00A2H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00A3H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00A4H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00A5H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00A6H 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 00A7H 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 00A8H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00A9H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00AAH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00ABH 0003H 000FH
nop
CBLOCKEND 196 10 172

;;	}
CLINEA 0000H 0000H 00ACH 0002H 0002H
_$L14 :

;;	tmp <<=1;
CLINEA 0000H 0001H 00ADH 0002H 000AH
	sll	r3,	#01h

;;	*port = port_data1;
CLINEA 0000H 0001H 00AEH 0002H 0014H
	st	r2,	[er10]

;;	if((tmp&0x80)==0)
CLINEA 0000H 0001H 00AFH 0002H 0012H
	tb	r3.7
	bne	_$L15
CBLOCK 196 11 176

;;		__asm("nop");				// 4-0
CLINEA 0000H 0001H 00B1H 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00B2H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00B3H 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 00B4H 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 00B5H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00B6H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00B7H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00B8H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00B9H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00BAH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00BBH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00BCH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00BDH 0003H 000FH
nop
CBLOCKEND 196 11 190

;;	else
CLINEA 0000H 0001H 00BFH 0002H 0005H
	bal	_$L17
_$L15 :
CBLOCK 196 12 192

;;		__asm("nop");				// 4-1
CLINEA 0000H 0001H 00C1H 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00C2H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00C3H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00C4H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00C5H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00C6H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00C7H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00C8H 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 00C9H 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 00CAH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00CBH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00CCH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00CDH 0003H 000FH
nop
CBLOCKEND 196 12 206

;;	}
CLINEA 0000H 0000H 00CEH 0002H 0002H
_$L17 :

;;	tmp <<=1;
CLINEA 0000H 0001H 00CFH 0002H 000AH
	sll	r3,	#01h

;;	*port = port_data1;
CLINEA 0000H 0001H 00D0H 0002H 0014H
	st	r2,	[er10]

;;	if((tmp&0x80)==0)
CLINEA 0000H 0001H 00D1H 0002H 0012H
	tb	r3.7
	bne	_$L18
CBLOCK 196 13 210

;;		__asm("nop");				// 5-0
CLINEA 0000H 0001H 00D3H 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00D4H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00D5H 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 00D6H 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 00D7H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00D8H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00D9H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00DAH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00DBH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00DCH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00DDH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00DEH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00DFH 0003H 000FH
nop
CBLOCKEND 196 13 224

;;	else
CLINEA 0000H 0001H 00E1H 0002H 0005H
	bal	_$L20
_$L18 :
CBLOCK 196 14 226

;;		__asm("nop");				// 5-1
CLINEA 0000H 0001H 00E3H 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00E4H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00E5H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00E6H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00E7H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00E8H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00E9H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00EAH 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 00EBH 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 00ECH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00EDH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00EEH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00EFH 0003H 000FH
nop
CBLOCKEND 196 14 240

;;	}
CLINEA 0000H 0000H 00F0H 0002H 0002H
_$L20 :

;;	tmp <<=1;
CLINEA 0000H 0001H 00F1H 0002H 000AH
	sll	r3,	#01h

;;	*port = port_data1;
CLINEA 0000H 0001H 00F2H 0002H 0014H
	st	r2,	[er10]

;;	if((tmp&0x80)==0)
CLINEA 0000H 0001H 00F3H 0002H 0012H
	tb	r3.7
	bne	_$L21
CBLOCK 196 15 244

;;		__asm("nop");				// 6-0
CLINEA 0000H 0001H 00F5H 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00F6H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00F7H 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 00F8H 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 00F9H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00FAH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00FBH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00FCH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00FDH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00FEH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 00FFH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0100H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0101H 0003H 000FH
nop
CBLOCKEND 196 15 258

;;	else
CLINEA 0000H 0001H 0103H 0002H 0005H
	bal	_$L23
_$L21 :
CBLOCK 196 16 260

;;		__asm("nop");				// 6-1
CLINEA 0000H 0001H 0105H 0003H 0019H
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0106H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0107H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0108H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0109H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 010AH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 010BH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 010CH 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 010DH 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 010EH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 010FH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0110H 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0111H 0003H 000FH
nop
CBLOCKEND 196 16 274

;;	}
CLINEA 0000H 0000H 0112H 0002H 0002H
_$L23 :

;;	tmp <<=1;
CLINEA 0000H 0001H 0113H 0002H 000AH
	sll	r3,	#01h

;;		__asm("nop");
CLINEA 0000H 0001H 0114H 0003H 000FH
nop

;;	*port = port_data1;
CLINEA 0000H 0001H 0115H 0002H 0014H
	st	r2,	[er10]

;;	if((tmp&0x80)==0)
CLINEA 0000H 0001H 0116H 0002H 0012H
	tb	r3.7
	bne	_$L24
CBLOCK 196 17 279

;;		tmp = *data;				// 7-0
CLINEA 0000H 0001H 0118H 0003H 0018H
	l	r0,	[er8]
	mov	r3,	r0	;; _ltmp$2

;;		__asm("nop");
CLINEA 0000H 0001H 0119H 0003H 000FH
nop

;;		*port = port_data0;
CLINEA 0000H 0001H 011AH 0003H 0015H
	st	r5,	[er10]

;;		__asm("nop");
CLINEA 0000H 0001H 011BH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 011CH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 011DH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 011EH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 011FH 0003H 000FH
nop

;;		__asm("nop");
CLINEA 0000H 0001H 0120H 0003H 000FH
nop
CBLOCKEND 196 17 289

;;	else
CLINEA 0000H 0001H 0122H 0002H 0005H
	bal	_$L26
_$L24 :
CBLOCK 196 18 291

;;		tmp = *data;				// 7-1
CLINEA 0000H 0001H 0124H 0003H 0018H
	l	r0,	[er8]
	mov	r3,	r0	;; _ltmp$2

;;		*port = port_data0;
CLINEA 0000H 0001H 0125H 0003H 0015H
	st	r5,	[er10]
CBLOCKEND 196 18 294

;;	}
CLINEA 0000H 0000H 0126H 0002H 0002H
_$L26 :

;;	if(i!=0) goto start;
CLINEA 0000H 0001H 0128H 0002H 0015H
	mov	er6,	er6
	beq	_$M2
	b	_$L2
_$M2 :
#pragma ENDASM
	delayMicroseconds(100);
}

const t_SERIAL_LED led =
{
	led_ctrl_init,
	led_ctrl_write,
};
