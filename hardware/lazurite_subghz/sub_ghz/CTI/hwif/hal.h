/* hal.h - HALヘッダファイル
 *
 * Copyright (c) 2015  Communication Technology Inc.,
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


#ifndef _HAL_H_
#define _HAL_H_


#ifdef	LAZURITE_IDE
//#include <stdint.h>
#else	// LAZURITE_IDE
#include <linux/delay.h>
#endif	// LAZURITE_IDE


/* タイマー割り込みコントロール関数 HAL_TIMER_enableInterrupt() と
 * HAL_TIMER_disableInterrupt() が定義されている場合はコメントアウト
 */
#define ML7396_HWIF_NOTHAVE_TIMER_DI

/* return value
 * 
 */
#define HAL_STATUS_OK				0	// 
#define HAL_ERROR_PARAM		-1	//
#define HAL_ERROR_STATE		-2	// 
#define HAL_ERROR_TIMEOUT	-3	// 
#define HAL_ERROR_SPI		-4 		 // SPI error
#define HAL_ERROR_I2C		-5 		 // I2C errror
#define HAL_ERROR_IRQ		-5 		 // IRQ error
#define HAL_ERROR_THREAD	-6 		 // thread error

extern int HAL_init(uint8_t i2c_addr, uint8_t addr_bits);
extern int HAL_remove(void);
extern int HAL_GPIO_setValue(uint8_t pin, uint8_t value);
extern int HAL_GPIO_getValue(uint8_t pin, uint8_t *value);
extern int HAL_GPIO_setInterrupt(void (*func)(void));
extern int HAL_GPIO_enableInterrupt(void);
extern int HAL_GPIO_disableInterrupt(void);
extern int HAL_TIMER_setup(void);
extern int HAL_TIMER_start(uint16_t msec, void (*func)(void));
extern int HAL_TIMER_stop(void);
#ifndef ML7396_HWIF_NOTHAVE_TIMER_DI
extern int HAL_TIMER_enableInterrupt(void);
extern int HAL_TIMER_disableInterrupt(void);
#endif  /* #ifndef ML7396_HWIF_NOTHAVE_TIMER_DI */
extern int HAL_TIMER_getTick(uint32_t *tick);
extern int HAL_SPI_transfer(const uint8_t *wdata, uint16_t wsize,uint8_t *rdata, uint16_t rsize);
extern int HAL_I2C_read(uint16_t addr, uint8_t *data, uint8_t size);
extern void HAL_EX_enableInterrupt(void);
extern void HAL_EX_disableInterrupt(void);

#ifdef LAZURITE_IDE
#define HAL_delayMicroseconds(v) delay_microseconds((unsigned long)v)
#define HAL_millis() millis()
#define HAL_sleep(v) sleep(v)
#else
#define HAL_delayMicroseconds(v) udelay((unsigned long)v)
extern uint32_t HAL_millis(void);
extern void HAL_sleep(uint32_t time);
#endif

extern int EXT_SPI_transfer(const uint8_t *wdata, uint16_t wsize,uint8_t *rdata, uint16_t rsize);
extern int EXT_I2C_read(uint16_t addr, uint8_t *data, uint8_t size);
extern void EXT_tx_led_flash(uint32_t time);
extern void EXT_rx_led_flash(uint32_t time);
extern void EXT_set_tx_led(int value);
extern void EXT_set_rx_led(int value);

#endif  /* #ifndef _INCLUDE_HAL_H */
