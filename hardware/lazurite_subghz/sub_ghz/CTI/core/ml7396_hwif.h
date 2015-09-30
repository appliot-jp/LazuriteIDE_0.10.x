/* ml7396_hwif.h - ML7396ドライバハードウェアインターフェイス ヘッダファイル
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

#ifndef _INCLUDE_ML7396_HWIF_H
#define _INCLUDE_ML7396_HWIF_H


#include <stdint.h>


/** ハードウェアインタフェイス関数
 *
 * 戻り値: 0以上=正常終了, 0未満=異常終了
 */

/* ML7396モジュールの周辺デバイス初期化
 *  SPIバスデータ通信の準備
 *  SINTN割り込み制御の準備
 *  経過時間タイマのクリアと開始
 *  タイムアウト割り込み制御の準備
 *  RESET端子によるハードウェアリセット
 */
extern int ml7396_hwif_init(void);

/* ML7396モジュールのレジスタアクセス
 *  SPIバスの読み書き
 *
 * wdata[]: 書き込むデータ配列
 * size: 読み書きするデータ数
 * rdata[]: 読み出したデータを収納する配列
 */
extern int ml7396_hwif_spi_transfer(const uint8_t *wdata, uint8_t *rdata, uint8_t size);

/* ML7396モジュールの割り込みハンドラ登録
 *  SINTN端子が'L'レベルで発生する割り込みのハンドラ
 *
 * func: 割り込みハンドラ関数
 */
extern int ml7396_hwif_sint_handler(void (*func)(void));

/* ML7396モジュールの割り込み許可/遅延解除
 */
extern int ml7396_hwif_sint_ei(void);

/* ML7396モジュールの割り込み禁止/遅延
 *  再度許可されるかクリアされるまで遅延
 */
extern int ml7396_hwif_sint_di(void);

/* タイムアウト割り込みカウント開始
 *  指定時間経過後割り込みを発生させる
 *
 * msec: タイムアウト割り込みを発生させるまでの時間 (msec単位で指定)
 */
extern int ml7396_hwif_timer_start(uint16_t msec);

/* タイムアウト割り込みカウント停止/割り込みフラグクリア
 */
extern int ml7396_hwif_timer_stop(void);

/* タイムアウト割り込みハンドラ登録
 *
 * func: 割り込みハンドラ関数
 */
extern int ml7396_hwif_timer_handler(void (*func)(void));

/* タイムアウト割り込み許可/遅延解除
 */
extern int ml7396_hwif_timer_ei(void);

/* タイムアウト割り込み禁止/遅延
 *  再度許可されるかクリアされるまで遅延
 */
extern int ml7396_hwif_timer_di(void);

/* 経過時間タイマの取得
 *  周辺デバイス初期化からの経過時間を取得
 *
 * *msec: 取得した経過時間 (msec単位)
 */
extern int ml7396_hwif_timer_tick(uint32_t *msec);

/* ML7396のレジスタ設定
 *   チャネル設定
 *   帯域幅設定
 *   通信レイト設定
 *   送信出力設定
 *   その他デバイス特有の設定
 *     最初のクロック安定待ち待と最後のキャリブレーション実行は不要
 */
extern int ml7396_hwif_regset(void *data);


#endif  /* #ifndef _INCLUDE_ML7396_HWIF_H */
