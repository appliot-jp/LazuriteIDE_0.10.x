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

#ifdef SUBGHZ_OTA
	#pragma SEGCODE "OTA_SEGCODE"
	#pragma SEGINIT "OTA_SEGINIT"
	#pragma SEGNOINIT "OTA_SEGNOINIT"
#endif
#include "common.h"
#include "mcu.h"
#include "lazurite.h"
#include "lazurite_system.h"
#include "wdt.h"
#include "driver_timer.h"
#include "driver_irq.h"
#include "driver_i2c.h"
#include "driver_uart.h"
#include "driver_gpio.h"
#include "lp_manage.h"
#include "clock.h"
#include "rdwr_reg.h"

//#define MIE_TEST
//#define _DEBUG

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
static unsigned long sys_timer_count=0;

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
static void start_long_timer(unsigned long ms);
static void stop_long_timer(void);

#ifdef LAZURITE_BLE
	extern void ble_timer_func();
#endif

//********************************************************************************
//   local functions
//********************************************************************************
void init(void)
{
    /* --- MIE Disable --- */
    __DI();
	write_reg8(BLKCON01,0x0373);
	/* Set oscillation mode */
	clk_setLsclk( CLK_XTM_CRYSTAL );    /* crystal/ceramic oscillation */
	/* Wait stables crystal oscillation */
	while( (clk_getClkStatus() & FSTAT_LOSCS) != 0 ) {}
	
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
	rst_interrupts();
	__EI();
	return;
}

void lazurite_gpio_init(void)
{
	// please see design note for details.
	write_reg8(P0D,0x10);
	write_reg8(P1D,0x01);
	write_reg8(P2D,0x00);
	write_reg8(P3D,0x00);
	write_reg8(P4D,0x00);
	write_reg8(P5D,0x42);
	write_reg8(P0DIR,0x00);
	write_reg16(P0CON,0x3535);
	write_reg16(P0MOD,0x0500);
	write_reg8(P1DIR,0x10);
	write_reg16(P1CON,0x0303);
	write_reg8(P2DIR,0x00);
	write_reg16(P2CON,0x0000);
	write_reg16(P2MOD,0x0000);
	write_reg8(P3DIR,0x00);
	write_reg16(P3CON,0x0000);
	write_reg16(P3MOD,0x0000);
	write_reg8(P4DIR,0x80);
	write_reg16(P4CON,0x0303);
	write_reg16(P4MOD,0x0003);
	write_reg8(P5DIR,0x00);
	write_reg16(P5CON,0x0000);
	write_reg16(P5MOD,0x0000);

	
	#ifdef PWR_LED
	drv_pinMode(11,OUTPUT);			//PWR LED ON
	drv_digitalWrite(11,LOW);
	#endif
}

static uint16_t halt_event_flag = false;
static void halt_event_isr(void)
{
	halt_event_flag=true;
}
void HALT_Until_Event(HALT_EVENT halt_event,uint16_t timeout)
{
	BOOLEAN cont;
	cont = true;
	halt_event_flag = false;
	
	if(timeout)
	{
		timer_16bit_set(6,0xE8,timeout,halt_event_isr);
		timer_16bit_start(6);
	}
	while(cont)
	{
		CHAR status;
		lp_setHaltMode();
		// process during waiting
		i2c_isr(0);
		i2c_isr(1);
		switch(halt_event)
		{
		case HALT_I2C1_END:
			status=i2c_get_status(1);
			if((halt_event_flag)||((!timeout)&&(status == I2C_MODE_ERROR)))
			{
				i2c_force_stop(1);
				cont = false;
			} else if(status < I2C_MODE_ERROR)
			{
				cont = false;
			}
			break;
		case HALT_I2C0_END:
			status=i2c_get_status(0);
			if((halt_event_flag)||((!timeout)&&(status == I2C_MODE_ERROR)))
			{
				i2c_force_stop(0);
				cont = false;
			} else if(status < I2C_MODE_ERROR)
			{
				cont = false;
			}
			break;
		default:
			if(halt_event_flag) {
				cont = false;
			}
			break;
		}		
		wdt_clear();
	}
	if(timeout)
		timer_16bit_stop(6);
	return;
}

void delay_isr(void)
{
#ifdef MIE_TEST
	noInterrupts();
	interrupts();
#endif
	
	// check end of timer
	if(delay_time.target_h == 0)
	{
		delay_flag = true;
		return;
	}
	
	// decriment high part of timer
	delay_time.target_h--;
	
	// update timer data
	if(delay_time.target_h == 0)
	{
		if((delay_time.target_l == 0) || (TM67C >= delay_time.target_l))
		{
			delay_flag = true;
			return;
		}
		else
		{
			T6OST = 1;
			TM67D = delay_time.target_l;
			
			delay_time.target_l = 0;
		}
	}
	#ifdef _DEBUG
	Serial.print("TimerH:");
	Serial.print_long(delay_time.target_h,DEC);
	Serial.print("\tTimerL:");
	Serial.println_long(delay_time.target_l,DEC);
	#endif
	return;
}

static void start_long_timer(unsigned long ms)
{
	unsigned long tmp_target_l;
	
	delay_flag = false;
	tmp_target_l = ((ms % 64000)<<8)/250;
	delay_time.target_l = (unsigned short)tmp_target_l;
	delay_time.target_h = ms / 64000;
	
#ifdef _DEBUG
	Serial.print_long(delay_time.target_h,HEX);
	Serial.print("\t");
	Serial.println_long(delay_time.target_l,HEX);
#endif
	
	if(delay_time.target_h==0)
	{
		timer_16bit_set(6,0xE8,(unsigned short)tmp_target_l,delay_isr);
		delay_time.target_l = 0;
	}
	else
	{
		timer_16bit_set(6,0x68,0xFFFF,delay_isr);
	}
	
	// setup timer
	timer_16bit_start(6);
}

static void stop_long_timer(void)
{
	timer_16bit_stop(6);
}

void delay_long(unsigned long ms)
{
	if(subghz_api_status != 0) {
		uint32_t event_time;
		event_time = millis();
		while((millis() - event_time) < ms) {}
	} else {
		start_long_timer(ms);

		while(delay_flag == false)
		{
			lp_setHaltMode();
			wdt_clear();
		}
	}

	stop_long_timer();
	return;
}

void sleep_long(unsigned long ms)
{
	if(subghz_api_status != 0) {
		uint32_t event_time;
		event_time = millis();
		while((millis() - event_time) < ms) {}
	} else {
		start_long_timer(ms);
#ifdef PWR_LED
		drv_digitalWrite(11,HIGH);		// PWR LED OFF
#endif
		while(delay_flag == false)
		{
			if(getMIE() == 0) {
				digitalWrite(25,LOW);
				if(QTM1 == 1) {
					isr_sys_timer();
					QTM1 = 0;
				}
				if(QTM7 == 1) {
					delay_isr();
					QTM7 = 0;
				}
			}
#ifdef SUBGHZ
			if((uart_tx_sending == true) || (uartf_tx_sending == true) || (subghz_api_status != 0))
#else
				if((uart_tx_sending == true) || (uartf_tx_sending == true))
#endif
				{
					lp_setHaltMode();
					wdt_clear();
				}
				else
				{
					lp_setHaltHMode();
					wdt_clear();
				}
		}
		stop_long_timer();

#ifdef PWR_LED
		drv_digitalWrite(11,LOW);		// PWR LED ON
#endif
		digitalWrite(25,HIGH);
	} 
	return;
}

volatile unsigned long millis(void)
{
	unsigned long timer_l;
	unsigned long timer_h;
	unsigned long result;

	//	__DI();
	dis_interrupts(DI_MILLIS);
	timer_l = read_reg16(TM01C);
	timer_h = sys_timer_count;

	if(QTM1 == 1)
	{
		timer_h++;
		timer_l=0;
	}

	//	__EI();
	enb_interrupts(DI_MILLIS);

	timer_l = (timer_l * 1000) >> 15;
	timer_h = timer_h * 2000;

	result = timer_h + timer_l;

	return result;
}

static void (*millis_timer_func)(uint32_t sys_timer_count);
void isr_sys_timer(void)
{
	sys_timer_count++;
#ifdef LAZURITE_BLE
	ble_timer_func();
#endif
	if(millis_timer_func) millis_timer_func(sys_timer_count);
	return;
}

// 
//  init timer
//  16bit timer control for millis() and micros()
//  use timer0 and timer1

static void init_timer(void)
{
	sys_timer_count = 0;
	millis_timer_func=NULL;
	timer_16bit_set(TM_MILLIS,0x40,0xFFFF,isr_sys_timer);
	timer_16bit_start(TM_MILLIS);
	return;
}

void set_timer0_function(void (*func)(uint32_t sys_timer_count))
{
	millis_timer_func = func;
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
#ifndef	_WDT
	wdt_clear();
#endif
}

void wait_event(bool *flag)
{	
#ifdef PWR_LED
	drv_digitalWrite(11,HIGH);		// PWR LED OFF
#endif

	while(*flag == false)
	{
		if(getMIE() == 0) {
			digitalWrite(25,LOW);
			if(QTM1 == 1) {
				isr_sys_timer();
				QTM1 = 0;
			}
			if(QTM7 == 1) {
				delay_isr();
				QTM7 = 0;
			}
			digitalWrite(25,LOW);
		}
#ifdef SUBGHZ
		if((uart_tx_sending == true) || (uartf_tx_sending == true) || (subghz_api_status != 0))
#else
			if((uart_tx_sending == true) || (uartf_tx_sending == true))
#endif
			{
				lp_setHaltMode();
				wdt_clear();
			}
			else
			{
				lp_setHaltHMode();
				wdt_clear();
			}
	}
	*flag = false;
#ifdef PWR_LED
	drv_digitalWrite(11,LOW);		// PWR LED ON
#endif
	digitalWrite(25,HIGH);
}

volatile unsigned long micros(void)
{
	unsigned long timer_l;
	unsigned long timer_h;
	unsigned long result;

	//	__DI();
	dis_interrupts(DI_MILLIS);
	timer_l = read_reg16(TM01C);
	timer_h = sys_timer_count;

	if(QTM1 == 1)
	{
		timer_h++;
		timer_l=0;
	}

	//	__EI();
	enb_interrupts(DI_MILLIS);

	timer_l = (timer_l * 15625) >> 9;
	timer_h = timer_h * 2000000;

	result = timer_h + timer_l;

	return result;
}

// for quick access, API is not in use.
// delay interval is trimmed by NOP operation
volatile void delay_microseconds(unsigned long us)
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

#ifdef SUBGHZ_OTA
#pragma SEGCODE
#pragma SEGINIT
#pragma SEGNOINIT
#endif

void interrupts()
{
	enb_interrupts(DI_USER);
}

void noInterrupts()
{
	dis_interrupts(DI_USER);
}

uint32_t wait_event_timeout(uint8_t *flag,uint32_t time)
{	
	uint32_t result;

	start_long_timer(time);

#ifdef PWR_LED
	drv_digitalWrite(11,HIGH);		// PWR LED OFF
#endif
	result = millis();
	while((*flag == false) && (delay_flag == false))
	{
#ifdef SUBGHZ
		if((uart_tx_sending == true) || (uartf_tx_sending == true) || (subghz_api_status != 0))
#else
			if((uart_tx_sending == true) || (uartf_tx_sending == true))
#endif
			{
				lp_setHaltMode();
				wdt_clear();
			}
			else
			{
				lp_setHaltHMode();
				wdt_clear();
			}
	}
	stop_long_timer();

	if (*flag == false) {			// timeout
		result = 0;
	} else {						// remaining time
		result = time - (millis() - result);
	}

	*flag = false;

#ifdef PWR_LED
	drv_digitalWrite(11,LOW);		// PWR LED ON
#endif

	return result;
}

static boolean vls_oneshot_check(uint8_t level)
{
	clear_bit(ENVLS);				// VLS OFF
	write_reg16(VLSMOD, 0x700);		// reset : NO, interrupt : NO, sampling : YES
	write_reg8(VLSCONL, level);		// set threshold

	set_bit(ENVLS);					// VLS ON
	while (VLSRF == 0) wdt_clear();	// wait until check result becomes valid

	clear_bit(ENVLS);				// VLS OFF
	return VLSF;					// return 0 : if greator than, 1 : if less than
}

// starts checking from "level" assigned in argument.
// returns VLS_1_898 ~ VLS_4_667 : the exceeded threshold voltage,
//               VLS_UNDER_1_898 : under 1.898V,
//                             0 : if "level" assgined in argument is out of range.
uint8_t voltage_check(uint8_t level)
{
	uint8_t ret = 0;

	if ((level >= VLS_1_898) && (level <= VLS_4_667))	// check the range of level
	{
		clear_bit(DVLS);			// enable VLS
		while (vls_oneshot_check(level))
		{
			level--;
			if (level < VLS_1_898) break;
		}
		set_bit(DVLS);				// disable VLS
		ret = level;
	}
	return ret;
}
