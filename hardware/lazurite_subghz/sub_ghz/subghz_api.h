/* subghz_api.h
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


#ifndef	_SUBGHZ_API_H_
#define _SUBGHZ_API_H_

#ifdef LAZURITE_IDE
#include "common.h"
#include "lazurite.h"
#endif
#include "CTI/api/bp3596.h"

typedef enum {
	SUBGHZ_OK = 0,
	SUBGHZ_RESET_FAIL,			// 1
	SUBGHZ_SETUP_FAIL,			// 2
	SUBGHZ_SLEEP_FAIL,			// 3
	SUBGHZ_WAKEUP_FAIL,			// 4
	SUBGHZ_MYADDR_FAIL,			// 5
	SUBGHZ_SETFIL_FAIL,			// 6
	SUBGHZ_TX_COMP_FAIL,		// 7
	SUBGHZ_TX_FAIL,				// 8
	SUBGHZ_TX_CCA_FAIL,			// 9
	SUBGHZ_TX_ACK_FAIL,			// 10
	SUBGHZ_RX_ENB_FAIL,			// 11
	SUBGHZ_RX_DIS_FAIL,			// 12
	SUBGHZ_RX_COMP_FAIL,		// 13
	SUBGHZ_PANID,				// 14
	SUBGHZ_ERR_ADDRTYPE,		// 15
	SUBGHZ_TTL_SEND_OVR,		// 16
	SUBGHZ_DUMMY				// --
} SUBGHZ_MSG;

// rate parameters
typedef enum {
	SUBGHZ_100KBPS = BP3596_RATE_100KBPS,
	SUBGHZ_50KBPS  = BP3596_RATE_50KBPS
} SUBGHZ_RATE;

// txPower parameters
typedef enum {
	SUBGHZ_PWR_20MW = BP3596_POWER_20MW,
	SUBGHZ_PWR_1MW = BP3596_POWER_1MW
} SUBGHZ_POWER;

typedef struct
{
	uint8_t rssi;
	int status;
} SUBGHZ_STATUS;

typedef struct
{
	uint8_t addrType;
	uint8_t senseTime;
	uint8_t txRetry;
	uint16_t txInterval;
	uint16_t myAddress;
	// 2015.10.26 Eiichi Saito   addition random backoff
	uint16_t ccaWait;
} SUBGHZ_PARAM;

typedef struct {
	uint8_t frame_type:3;
	uint8_t sec_enb:1;
	uint8_t pending:1;
	uint8_t ack_req:1;
	uint8_t panid_comp:1;
	uint8_t nop:1;
	uint8_t seq_comp:1;
	uint8_t ielist:1;
	uint8_t tx_addr_type:2;
	uint8_t frame_ver:2;
	uint8_t rx_addr_type:2;
} s_MAC_HEADER_BIT_ALIGNMENT;
typedef union {
	uint8_t data[2];
	uint16_t header;
	s_MAC_HEADER_BIT_ALIGNMENT alignment;
} u_MAC_HEADER;
typedef struct
{
	u_MAC_HEADER mac_header;
	uint8_t seq_num;
	uint8_t addr_type;
	uint16_t rx_panid;
	uint8_t  rx_addr[8];
	uint16_t tx_panid;
	uint8_t  tx_addr[8];
	uint8_t *raw;
	int16_t raw_len;
	uint8_t *payload;
	int16_t payload_len;
} SUBGHZ_MAC_PARAM;

typedef struct
{
	SUBGHZ_MSG (*init)(void);
	SUBGHZ_MSG (*remove)(void);
	SUBGHZ_MSG (*begin)(uint8_t ch, uint16_t panid, SUBGHZ_RATE rate, SUBGHZ_POWER txPower);
	SUBGHZ_MSG (*close)(void);
	SUBGHZ_MSG (*send)(uint16_t panid, uint16_t dstAddr, uint8_t *data, uint16_t len, void (*callback)(uint8_t rssi, int status));
	SUBGHZ_MSG (*rxEnable)(void (*callback)(const uint8_t *data, uint8_t rssi, int status));
	SUBGHZ_MSG (*rxDisable)(void);
	short (*readData)(uint8_t *data, uint16_t max_size);
	uint16_t (*getMyAddress)(void);
	void (*getStatus)(SUBGHZ_STATUS *tx, SUBGHZ_STATUS *rx);
	void (*msgOut)(SUBGHZ_MSG msg);
	SUBGHZ_MSG (*setSendMode)(SUBGHZ_PARAM *param);
	SUBGHZ_MSG (*getSendMode)(SUBGHZ_PARAM *param);
	void (*decMac)(SUBGHZ_MAC_PARAM *mac,uint8_t *raw,uint16_t raw_len);
} SubGHz_CTRL;

extern const SubGHz_CTRL SubGHz;

#endif	//_SUBGHZ_API_H_
