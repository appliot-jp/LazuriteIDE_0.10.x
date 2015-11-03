
#ifndef _LEDDOTMATRIX_H_
#define  _LEDDOTMATRIX_H_

#include "lazurite.h"

typedef struct {
	bool (*init)(void);
	void (*setMemory)(uint8_t *up, uint16_t up_size, uint8_t *lo, uint16_t lo_size);
	void (*setFlash)(uint8_t up_sector, uint16_t up_offset, uint16_t up_size, uint8_t lo_sector,uint16_t lo_offset, uint16_t lo_size);
	void (*shift)(int speed, bool up_shift, bool lo_shift);
} LED_DOT_MATRIX;

extern const LED_DOT_MATRIX LedDotMatrix;

#endif //_LEDDOTMATRIX_H_

