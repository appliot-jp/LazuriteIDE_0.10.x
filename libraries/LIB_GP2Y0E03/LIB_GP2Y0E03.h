
#ifndef _GP2Y0E03_H_
#define _GP2Y0E03_H_

#include "lazurite.h"

typedef struct {
	void (*init)(unsigned char i2c_addr);
	unsigned char (*measure)(void);
	void (*meas_mode)(unsigned char mode);
	void (*stby_mode)(bool mode);
} tGP2Y0E03;

extern tGP2Y0E03 gp2y0e03;

#endif // _GP2Y0E03_H_
