#ifndef _SERIAL_LED_H_
#define _SERIAL_LED_H_

#include "lazurite.h"

typedef struct {
	void (*init)(unsigned char pin, unsigned short len);
	void (*write)(unsigned char *data);
}t_SERIAL_LED;

extern const t_SERIAL_LED led;


#endif // _SERIAL_LED_H_

