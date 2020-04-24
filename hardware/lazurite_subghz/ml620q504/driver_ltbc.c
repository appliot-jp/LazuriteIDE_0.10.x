/* FILE NAME: driver_ltbc.c
 *
 * Copyright (c) 2020  Lapis Semiconductor Co.,Ltd.
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

#ifdef SUBGHZ_OTA
	#pragma SEGCODE "OTA_SEGCODE"
	#pragma SEGINIT "OTA_SEGINIT"
	#pragma SEGNOINIT "OTA_SEGNOINIT"
	#pragma SEGCONST "OTA_SEGCONST"
#endif
/* --- Common Header --- */
#include "common.h"

/* --- Platfrom Header --- */
#include "driver_ltbc.h"
#include "driver_irq.h"
#include "mcu.h"
#include "rdwr_reg.h"

#define LTBC_SLOT_NUM	( 8 )
static int (*fn_p[LTBC_SLOT_NUM])(unsigned char count) = { NULL };
static unsigned char expires[LTBC_SLOT_NUM] = { 0x00 };
static unsigned char slot_usage = 0x00;

static void ltbc_isr(void)
{
	int i,ret;
	unsigned char count = LTBR;
	if (slot_usage != 0) {
		for (i=0; i<LTBC_SLOT_NUM; i++) {
			if (((1 << i) & slot_usage) && (fn_p[i] != NULL) && (count == expires[i])) {
				ret = fn_p[i](count);
				if (ret == 0) {
					ltbc_detach_handler((unsigned char)i);		// detach handler by myself
				} else if ((ret > 0) && (ret <= 256)) {
					expires[i] = (unsigned char)((count+ret+1)&0xfe);		// set next expire count
				}
			}
		}
	}
	return;
}

static void ltbc_enable_interrupt(void)
{
	IE7 = 0;				// disable ie of LTBC
	IRQ7 = 0;				// clear irq of LTBC
	irq_sethandler((unsigned char)IRQ_NO_LTBC0INT,ltbc_isr);	// attach IRQ handler
	IE7 = 1;				// enable ie of ELTBC0
	return;
}

static void ltbc_disable_interrupt(void)
{
	IE7 = 0;				// disable ie of LTBC
	IRQ7 = 0;				// clear irq of LTBC
	irq_sethandler((unsigned char)IRQ_NO_LTBC0INT,NULL);		// detatch IRQ handler
	return;
}

void ltbc_init(void)
{
	dis_interrupts(DI_LTBC);
	IE7 = 0;				// clear ie of LTBC
	IRQ7 = 0;				// clear irq of LTBC
	LTBADJ = 0;				// freq adjustment 0
	LTBINT = 0x630;			// use default
	enb_interrupts(DI_LTBC);
	return;
}

unsigned char ltbc_get_count(void)
{
	unsigned char c0,c1;

	c0 = LTBR;
	while (1) {
		c1 = LTBR;
		if (c0 == c1) break;
		else c0 = c1;
	};
	return c0;
}

void ltbc_attach_handler(unsigned char num, unsigned char expire, int (*func)(unsigned char count))
{
	if ((num < LTBC_SLOT_NUM) && (func != NULL)) {
		dis_interrupts(DI_LTBC);
		if (slot_usage == 0) ltbc_enable_interrupt();
		expires[num] = (unsigned char)((expire+1)&0xfe);	// odd number
		fn_p[num] = func;
		slot_usage |= (unsigned char)(1 << num);
		enb_interrupts(DI_LTBC);
	}
	return;
}

void ltbc_detach_handler(unsigned char num)
{
	if (num < LTBC_SLOT_NUM) {
		dis_interrupts(DI_LTBC);
		if (slot_usage != 0) {
			fn_p[num] = NULL;
			slot_usage &= (unsigned char)(~(1 << num));
			if (slot_usage == 0) ltbc_disable_interrupt();
		}
		enb_interrupts(DI_LTBC);
	}
	return;
}
