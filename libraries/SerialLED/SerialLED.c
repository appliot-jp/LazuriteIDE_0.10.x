
#include "lazurite.h"
#include "SerialLED.h"
#include "driver_pin_assignment.h"
#include "wdt.h"

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
	short i;
	unsigned char tmp;
	unsigned char *port;
	unsigned char bit;
	unsigned char port_data0,port_data1;
	
	port = ml620504f_pin_to_port[digital_pin_to_port[led_pin]];
	bit = ml620504f_pin_to_bit[digital_pin_to_port[led_pin]];
	
	port_data0 = *port&~bit;			// prepare port data = 0
	port_data1 = *port|bit;				// prepare port data = 1
	
	*port = port_data0;					// initializing port
	wdt_clear();
	
	noInterrupts();
	i=data_length;
	tmp = *data;
	{
start:
		*port = port_data1;
		if((tmp&0x80)==0)
		{
			__asm("nop");				// 0-0
			__asm("nop");
			__asm("nop");
			*port = port_data0;
			data++;
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
		}
		else
		{
			__asm("nop");				// 0-1
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			data++;
			*port = port_data0;
		}
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		tmp <<=1;
		*port = port_data1;
		if((tmp&0x80)==0)
		{
			__asm("nop");				// 1-0
			__asm("nop");
			__asm("nop");
			*port = port_data0;
			i--;
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
		}
		else
		{
			__asm("nop");				// 1-1
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			i--;
			*port = port_data0;
		}
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		tmp <<=1;
		*port = port_data1;
		if((tmp&0x80)==0)
		{
			__asm("nop");				// 2-0
			__asm("nop");
			__asm("nop");
			*port = port_data0;
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
		}
		else
		{
			__asm("nop");				// 2-1
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			*port = port_data0;
		}
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		tmp <<=1;
		*port = port_data1;
		if((tmp&0x80)==0)
		{
			__asm("nop");				// 3-0
			__asm("nop");
			__asm("nop");
			*port = port_data0;
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
		}
		else
		{
			__asm("nop");				// 3-1
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			*port = port_data0;
		}
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		tmp <<=1;
		*port = port_data1;
		if((tmp&0x80)==0)
		{
			__asm("nop");				// 4-0
			__asm("nop");
			__asm("nop");
			*port = port_data0;
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
		}
		else
		{
			__asm("nop");				// 4-1
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			*port = port_data0;
		}
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		tmp <<=1;
		*port = port_data1;
		if((tmp&0x80)==0)
		{
			__asm("nop");				// 5-0
			__asm("nop");
			__asm("nop");
			*port = port_data0;
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
		}
		else
		{
			__asm("nop");				// 5-1
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			*port = port_data0;
		}
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		tmp <<=1;
		*port = port_data1;
		if((tmp&0x80)==0)
		{
			__asm("nop");				// 6-0
			__asm("nop");
			__asm("nop");
			*port = port_data0;
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
		}
		else
		{
			__asm("nop");				// 6-1
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			*port = port_data0;
		}
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		tmp <<=1;
		*port = port_data1;
		if((tmp&0x80)==0)
		{
			tmp = *data;				// 7-0
			__asm("nop");
			*port = port_data0;
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
		}
		else
		{
			tmp = *data;				// 7-1
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			__asm("nop");
			*port = port_data0;
		}
		__asm("nop");
		__asm("nop");
		if(i!=0) goto start;
	}
	interrupts();
	delayMicroseconds(100);
}

const t_SERIAL_LED led =
{
	led_ctrl_init,
	led_ctrl_write,
};
