/* FILE NAME: mk71251.h
 *
 * Copyright (c) 2018  Lapis Semiconductor Co.,Ltd.
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

#ifndef _MK71251_H_
#define _MK71251_H_
#include <common.h>

#define BLE_RX_BUF_SIZE		(128)
#define BLE_TX_BUF_SIZE		(128)

typedef enum {
	NOT_START = 0,
	DEEPSLEEP,
	ADVERTISE,
	CONNECT,
	SERIAL_CONNECT
} BLE_STATUS;

struct BLE_LED_PARAM {
	uint8_t cycle;
	uint8_t pin;
	uint8_t on_time;
	uint8_t off_time;
};
struct BLE_LED_CONFIG {
	struct BLE_LED_PARAM sleep;
	struct BLE_LED_PARAM advertise;
	struct BLE_LED_PARAM connect;
	struct BLE_LED_PARAM serial_connect;
};

typedef struct {
	void (*begin)(void (*callback)(int mode));
	void (*end)(void);
	int (*available)(void);
	int (*read)(void);
	int (*peek)(void);
	void (*flush)(void);
	volatile int (*print)(char* data);
	volatile int (*println)(char* data);
	volatile int (*print_long)(long data, UCHAR fmt);
	volatile int (*println_long)(long data, UCHAR fmt);
	volatile int (*print_double)(double data, UCHAR digit);
	volatile int (*println_double)(double data, UCHAR digit);
	volatile int (*write)(char* data, int quantity);
	volatile int (*write_byte)(char data);
	int (*tx_available)(void);								// add 
	volatile int (*getStatus)(void);
	volatile int (*advertise)(bool on);
	int (*setTxFlowCtrl)(bool on);
	void (*setLedConfig)(struct BLE_LED_CONFIG *conf);
	void (*getLedConfig)(struct BLE_LED_CONFIG *conf);
	int (*setPairingMode)(char mode);
	int (*getPairingMode)(char *mode);
	int (*setTxPower)(char pwr);
	int (*getTxPower)(char *pwr);
	int (*setAdvertiseInterval)(uint16_t fast, uint16_t slow, uint16_t time);
	int (*getAdvertiseInterval)(uint16_t *fast, uint16_t *slow, uint16_t *time);
	int (*setConnectInterval)(uint16_t fast, uint16_t slow, uint16_t time);
	int (*getConnectInterval)(uint16_t *fast, uint16_t *slow, uint16_t *time);
	int (*setAdvertiseData)(uint8_t *data, uint8_t len);
	int (*getAdvertiseData)(uint8_t *data, uint8_t *len);
	int (*setPasskey)(uint32_t key);
	int (*getPasskey)(uint32_t *key);
} BLUETOOTH;

extern const BLUETOOTH BLE;

#endif
