/* FILE NAME: lazurite_system.c
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
#include "mcu.h"
#include "lazurite.h"
#include "lazurite_system.h"
#include "wdt.h"
#include "driver_timer.h"
#include "driver_irq.h"
#include "driver_i2c.h"
#include "driver_uart.h"
#include "lp_manage.h"
#include "clock.h"
#include "rdwr_reg.h"

//********************************************************************************
//   global parameters
//********************************************************************************
//BOOL	delay_irq_mode;					//
//********************************************************************************
//   local definitions
//********************************************************************************

//********************************************************************************
//   local parameters
//********************************************************************************
static unsigned long sys_timer_ms;

static unsigned long delay_current_time_h;
static struct {
	unsigned long target_h;
	unsigned short target_l;
} delay_time;
//********************************************************************************
//   local function definitions
//********************************************************************************
void isr_sys_timer(void);
static void clk_block_ctrl_init(void);
void watch_dog_isr(void);
static void lazurite_gpio_init(void);
static void init_timer(void);
static void delay_isr(void);
static volatile bool delay_flag;
//static void ms_timer6_set(void);
//static void ms_timer6_start(void);
//static void ms_timer6_stop(void);
//static void ms_timer6_isr(void);
//static void ms_timer6_init(void);

//********************************************************************************
//   local functions
//********************************************************************************
void init(void)
{
    /* --- MIE Disable --- */
    __DI();
	
	/* Set oscillation mode */
	clk_setLsclk( CLK_XTM_RC );    /* crystal/ceramic oscillation */
	/* Wait stables crystal oscillation */
//	while( (clk_getClkStatus() & FSTAT_LOSCS) != 0 ) {}
	
	clk_disHsclk();                             // stop HSCLK oscillation
	clk_setHsclk( (unsigned char)(CLK_SYSC_OSCLK | CLK_OSCM_RC | CLK_OUTC_OSCLK | CLK_LOSCON_DIS), CLK_LOSCON_DIS ); /* set HSCLK */		// BUG620Q504
	clk_enaHsclk();                             // start HSCLK oscillation

	clk_setSysclk( CLK_SYSCLK_HSCLK );          // Choose HSCLK
	
	clk_block_ctrl_init();				/* Block Control            */
	
	// irq handler initialization
    irq_init();                         /* Interrupt                */
	
	// Watch dog timer
	irq_sethandler(IRQ_NO_WDTINT,watch_dog_isr);
	wdt_init( WDTMOD_WDT1 | WDTMOD_WDT0);
	wdt_disHaltCount();	
	
	// Initializing GPIO
	lazurite_gpio_init();
	
	// Initializing timer
	init_timer();
	
	/* MIE Enable */
	__EI();
	return;
}

void lazurite_gpio_init(void)
{
	// please see design note for details.
	write_reg8(P0D,0x00);
	write_reg8(P1D,0x00);
	write_reg8(P2D,0x00);
	write_reg8(P3D,0x00);
	write_reg8(P4D,0x00);
	write_reg8(P5D,0x00);
	write_reg8(P0DIR,0x00);
	write_reg16(P0CON,0x0000);
	write_reg16(P0MOD,0x0000);
	write_reg8(P1DIR,0x00);
	write_reg16(P1CON,0x0000);
	write_reg8(P2DIR,0x00);
	write_reg16(P2CON,0x0000);
	write_reg16(P2MOD,0x0000);
	write_reg8(P3DIR,0x01);
	write_reg16(P3CON,0x0202);
	write_reg16(P3MOD,0x0303);
	write_reg8(P4DIR,0x00);
	write_reg16(P4CON,0x0000);
	write_reg16(P4MOD,0x0000);
	write_reg8(P5DIR,0x00);
	write_reg16(P5CON,0x0000);
	write_reg16(P5MOD,0x0000);
}

void HALT_Until_Event(HALT_EVENT halt_event)
{
	BOOLEAN cont;
	cont = true;
	
	while(cont)
	{
		wdt_clear();
		lp_setHaltMode();
		// process during waiting
		i2c_isr(0);
		i2c_isr(1);
		switch(halt_event)
		{
		case HALT_I2C1_END:
			if(i2c_get_status(1) <= I2C_MODE_ERROR)
				cont = false;
			break;
		case HALT_I2C0_END:
			if(i2c_get_status(0) <= I2C_MODE_ERROR)
				cont = false;
			break;
		default:
			cont = false;
			break;
		}
	}
	return;
}

void delay_isr(void)
{
	if(delay_time.target_h == 0)
	{
		if((delay_time.target_l == 0) || (TM67C >= delay_time.target_l))
		{
			delay_flag = true;
			return;
		}
		T6OST = 1;
		TM67D = delay_time.target_l;
		
		delay_time.target_l = 0;
	}
	else
	{
		delay_time.target_h--;
	}
	return;
}

void delay_long(unsigned long ms)
{
	float f_tm_count;						// temporary to calcurate timer count
	
	// setting flag for delay
	// delay_flag = false:		during delay
	// delay_flag = true:		finish delay
	delay_flag = false;
	
	// calcurate delay timer count
	// timer count is expanded to 48bit (upper 32bit = software timer, lower 16bit = hardware timer)
	f_tm_count = (float)ms;
	f_tm_count = f_tm_count * 1.024f + 0.5;
	delay_time.target_l = (unsigned short)f_tm_count%65536;		// lower 16bit  <= ms / 1.024 + 0.5
	delay_time.target_h = (unsigned long)f_tm_count/65536;		// upper 24bit  <= ms / 1.024 + 0.5
//	delay_time.target_l = (unsigned short) ms&0x0000FFFF;		// lower 16bit  <= ms / 1.024 + 0.5
//	delay_time.target_h = (ms >> 16);
	// setup timer
	timer_16bit_set(6,0x68,0xFFFF,delay_isr);
	
	// correct timer parameter according to delay time
	delay_isr();

	timer_16bit_start(6);
	
	while(delay_flag == false)
	{
		lp_setHaltMode();
	}
	timer_16bit_stop(6);
	return;
}

void sleep_long(unsigned long ms)
{
	float f_tm_count;						// temporary to calcurate timer count
	
	// setting flag for delay
	// delay_flag = false:		during delay
	// delay_flag = true:		finish delay
	delay_flag = false;
	
	// calcurate delay timer count
	// timer count is expanded to 48bit (upper 32bit = software timer, lower 16bit = hardware timer)
	f_tm_count = (float)ms;
	f_tm_count = f_tm_count * 1.024f + 0.5;
	delay_time.target_l = (unsigned short)f_tm_count%65536;		// lower 16bit  <= ms / 1.024 + 0.5
	delay_time.target_h = (unsigned long)f_tm_count/65536;		// upper 24bit  <= ms / 1.024 + 0.5
	
	// setup timer
	timer_16bit_set(6,0x68,0xFFFF,delay_isr);
	
	// correct timer parameter according to delay time
	delay_isr();
	
	timer_16bit_start(6);
	
	while(delay_flag == false)
	{
		if((uart_tx_sending == true) || (uartf_tx_sending == true))
		{
			lp_setHaltMode();
		}
		else
		{
			lp_setDeepHaltMode();
		}
	}
	timer_16bit_stop(6);
	return;
}


// for quick access, API is not in use.
// delay interval is trimmed by NOP operation
void delay_microseconds(unsigned long us)
{
	if(us >= 2)
	{
		us -= 1;
		while(us > 0)
		{
			__asm("nop\n");
			__asm("nop\n");
			__asm("nop\n");
			__asm("nop\n");
			__asm("nop\n");
			us--;
		}
	}
	
	return;
}

unsigned long millis(void)
{
	unsigned long timer_data;
	unsigned long ms;
	
	__DI();
	timer_data = read_reg16(TM01C);
	ms = sys_timer_ms;
	if(QTM1 == 1)
	{
		ms+=2000;
		timer_data=0;
	}
	__EI();
	
	ms = ms + ((timer_data * 1000) >> 15);		// (timer_data / 32.768) = (timer_data * 1000) /32768
	
	return ms;
}

unsigned long micros(void)
{
	unsigned long us;
	unsigned long ms;
	unsigned long timer_data;
	
	__DI();
	
	timer_data = read_reg16(TM01C);
	ms = sys_timer_ms;
	if(QTM1 == 1)
	{
		ms+=2000;
		timer_data=0;
	}
	
	__EI();
	
	timer_data = (timer_data * 15625) >> 9;		// reduce fractions (1000000 / F_LSCLK)
	
	us = ms * 1000;
	us +=  timer_data;
	
	return us;
}


void isr_sys_timer(void)
{
	sys_timer_ms+=2000;
	return;
}

// 
//  init timer
//  16bit timer control for millis() and micros()
//  use timer0 and timer1

static void init_timer(void)
{
	sys_timer_ms = 0;
	timer_16bit_set(TM_MILLIS,0x40,0xFFFF,isr_sys_timer);
	timer_16bit_start(TM_MILLIS);
	return;
}

static void clk_block_ctrl_init(void)
{
	write_reg16(BLKCON01,0x0FFF);
	write_reg16(BLKCON23,0xC1DF);
	write_reg16(BLKCON45,0x0603);
	
}

void watch_dog_isr(void)
{
	wdt_clear();
}
