/* bp3596.h - SDK API ヘッダファイル
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

#ifndef _INCLUDE_BP3596_H
#define _INCLUDE_BP3596_H


#include <stdint.h>


/* 戻り値
 */
#define BP3596_STATUS_OK           0
#define BP3596_STATUS_ERROR_PARAM -1
#define BP3596_STATUS_ERROR_STATE -2

/* BP3596_setup() の引数
 */
#define BP3596_RATE_100KBPS 100
#define BP3596_RATE_50KBPS   50
#define BP3596_POWER_20MW    20
#define BP3596_POWER_1MW      1

/* コールバック関数の status 引数
 */
#define BP3596_STATUS_ERROR_INIT  -100  /* 未処理状態 */
#define BP3596_STATUS_ERROR_SIZE  -101  /* データサイズが異常 */
#define BP3596_STATUS_ERROR_STOP  -102  /* 処理が中断された */
#define BP3596_STATUS_ERROR_CRC   -103  /* CRCエラー */
#define BP3596_STATUS_ERROR_CCA   -104  /* CCAエラー */
#define BP3596_STATUS_ERROR_RETRY -105  /* リトライエラー */

/* 状態
 */
#define BP3596_STATUS_STATE_RESET 0
#define BP3596_STATUS_STATE_IDLE  1
#define BP3596_STATUS_STATE_RECV  2
#define BP3596_STATUS_STATE_SEND  3
#define BP3596_STATUS_STATE_SLEEP 4

/* 外部公開API関数
 */
extern int BP3596_reset(void);
// 2015.10.26 Eiichi Saito   addition random backoff
extern int BP3596_setup(uint8_t channel, uint8_t rate, uint8_t txPower, uint8_t senseTime,
                        uint8_t txRetry, uint16_t txInterval, uint16_t ccaWait );
extern int BP3596_getMyAddr(uint16_t *addr);
extern int BP3596_setMyPANID(uint16_t panID);
extern int BP3596_setFilter(uint16_t panID, uint16_t addr0, uint16_t addr1);
// 2016.03.14 tx send event
extern int BP3596_sendIdle(void);
extern int BP3596_send(const void *data, uint16_t size,
                       uint8_t addrType, uint16_t dstAddr, uint16_t dstPANID );
extern int BP3596_sendRaw(const void *data, uint16_t size);
extern int BP3596_setFuncSendComplete(void (*sendComplete)(uint8_t rssi, int status));
extern int BP3596_recvEnable(void);
extern int BP3596_recvDisable(void);
extern int BP3596_setFuncRecvComplete(void (*recvComplete)(const void *data, uint8_t rssi, int status));
extern int BP3596_sleep(void);
extern int BP3596_wakeup(void);
extern int BP3596_getState(void);
extern int BP3596_getVersion(uint16_t *version);


#endif  /* #ifndef _INCLUDE_BP3596_H */
