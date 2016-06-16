#include "random.h"
#include <linux/kernel.h>
static const int8_t a[] = { 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 9, 9,
                               11, 11, 11, 11, 11, 11, 11, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 15, 15, 15, 15
};

static const int8_t b[] = { 1, 1, 5, 7, 7, 11, 15, 15, 5, 5, 5, 7, 7, 1, 1, 5, 11, 11, 13, 3, 7, 11, 7, 9, 11, 11, 7, 11, 15,
                                1, 3, 9, 9, 15, 9, 9, 7, 13, 1, 3, 5, 7, 11, 11, 11, 1, 3, 3, 3, 5, 7, 7, 7, 9, 1, 7, 1, 1, 5, 7
};

static const int8_t c[] = { 14, 15, 2, 4, 11, 3, 6, 7, 1, 13, 15, 13, 15, 12, 15, 11, 1, 11, 9, 7, 1, 11, 14, 8, 6, 11, 13, 5,
                                 1, 11, 4, 8, 13, 1, 5, 7, 13, 3, 7, 13, 3, 1, 3, 4, 5, 3, 13, 11, 12, 2, 2, 6, 9, 7, 1, 5, 1, 3, 2, 2
};

static uint16_t seeds;
static const uint8_t param_num = 10;
uint16_t rand(void)
{
	seeds ^= (seeds << a[param_num]);
	seeds ^= (seeds >> b[param_num]);
	return seeds ^= (seeds << c[param_num]);
}

void srand(uint16_t data)
{
	if(data ==0) data =1;
	seeds = data;
	return;
}

