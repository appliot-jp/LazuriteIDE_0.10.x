/* endian.h - エンディアン変換ヘッダファイル
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

#ifndef _INCLUDE_ENDIAN_H
#define _INCLUDE_ENDIAN_H


#ifdef LAZURITE_IDE
#include <stdint.h>
#endif


/** CPUのエンディアンへ変換
 */

/* ビッグエンディアン(ネットワークバイトオーダー)の2バイトデータを変換 */
#define n2u16(_n16) \
    ((uint16_t)((uint8_t *)(_n16))[0] << 8 | \
     (uint16_t)((uint8_t *)(_n16))[1] << 0 )

/* リトルエンディアン(VAXバイトオーダー)の2バイトデータを変換 */
#define v2u16(_v16) \
    ((uint16_t)((uint8_t *)(_v16))[0] << 0 | \
     (uint16_t)((uint8_t *)(_v16))[1] << 8 )


/** CPUのエンディアンから変換して変数に代入
 */

/* ビッグエンディアン(ネットワークバイトオーダー)の2バイトデータに変換 */
#define u2n16_set(_u16, _n16) \
    (((uint8_t *)(_n16))[0] = (uint8_t)((uint16_t)(_u16) >> 8), \
     ((uint8_t *)(_n16))[1] = (uint8_t)((uint16_t)(_u16) >> 0) )

/* リトルエンディアン(VAXバイトオーダー)の2バイトデータに変換 */
#define u2v16_set(_u16, _v16) \
    (((uint8_t *)(_v16))[0] = (uint8_t)((uint16_t)(_u16) >> 0), \
     ((uint8_t *)(_v16))[1] = (uint8_t)((uint16_t)(_u16) >> 8) )


#endif  /* #ifndef _INCLUDE_ENDIAN_H */
