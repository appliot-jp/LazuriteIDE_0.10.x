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


#ifndef _HAL_LAZURITE_H_
#define _HAL_LAZURITE_H_


#include <stdint.h>


/* タイマー割り込みコントロール関数 HAL_TIMER_enableInterrupt() と
 * HAL_TIMER_disableInterrupt() が定義されている場合はコメントアウト
 */
#define ML7396_HWIF_NOTHAVE_TIMER_DI

/* 戻り値定義
 *  0以上=正常終了, 0未満=異常終了
 */
#define HAL_STATUS_OK             0  /* 正常終了 */
#define HAL_STATUS_ERROR_PARAM   -1  /* パラメータ異常 */
#define HAL_STATUS_ERROR_STATE   -2  /* ステート異常 */
#define HAL_STATUS_ERROR_TIMEOUT -3  /* タイムアウト */

/* pin引数定義
 */
/*
#define HAL_GPIO_CSB    19
#define HAL_GPIO_RESETN 35
#define HAL_GPIO_DMON   30
#define HAL_GPIO_SINTN  15
#define BP3596A_SINTN_IRQNUM	3
*/
#define HAL_GPIO_CSB    6
#define HAL_GPIO_RESETN 7
#define HAL_GPIO_DMON   5
#define HAL_GPIO_SINTN  27
#define BP3596A_SINTN_IRQNUM	3

#endif  //_HAL_LAZURIE_H_
