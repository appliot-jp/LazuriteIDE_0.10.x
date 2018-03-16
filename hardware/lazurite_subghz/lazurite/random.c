/* FILE NAME: random.c
 *
 * Copyright (c) 2018  LAPIS Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 */

#define RAND_MAX ( 32767 )

unsigned long seeds = 1;

int rand(void)
{	/* compute pseudo-random value */
	seeds = seeds * 1103515245 + 12345;
	return ((unsigned int)(seeds >> 16) & RAND_MAX);
}

void srand(unsigned int data)
{	/* alter the seeds */
	seeds = data;
}
