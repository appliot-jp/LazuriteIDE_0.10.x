#ifndef _SERIAL_LED_H_
#define _SERIAL_LED_H_

#include "lazurite.h"

#define BLUE	0x00,0x00,0xff
#define BLACK	0x00,0x00,0x00
#define RED		0x00,0xff,0x00
#define GREEN	0xff,0x00,0x00
#define WHITE	0xff,0xff,0xff


typedef struct {
	void (*init)(unsigned char pin, unsigned short len);
	void (*write)(unsigned char *data);
}t_SERIAL_LED;

extern const t_SERIAL_LED led;


#endif // _SERIAL_LED_H_

