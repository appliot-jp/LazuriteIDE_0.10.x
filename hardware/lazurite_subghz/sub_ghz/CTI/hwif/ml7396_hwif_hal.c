/* ml7396_hwif_hal.c - ML7396ハードウェアインターフェース
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


#ifdef LAZURITE_IDE
#include <stddef.h>
#include <stdint.h>
#include "hal.h"
#include "../core/ml7396_hwif.h"
#include "../core/ml7396_reg.h"
#include "../core/endian.h"
#else
#include <linux/stddef.h>
#include "hal.h"
#include "../core/ml7396_hwif.h"
#include "../core/ml7396_reg.h"
#include "../core/endian.h"
#endif

static volatile struct {
    struct {                    /* タイマ関係パラメータ */
        void (*handler)(void);    /* タイマの割り込みハンドラ */
#ifdef ML7396_HWIF_NOTHAVE_TIMER_DI
        enum {                    /* 割り込み制御 */
            Disable=0,              /* 許可 */
            Enable                  /* 禁止 */
        } active;
        uint8_t call_count;       /* 割り込み禁止中のハンドラ呼び出し回数 */
#endif  /* #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */
    } timer;
} hwif = {
    {
        NULL  /* timer.handler */
#ifdef ML7396_HWIF_NOTHAVE_TIMER_DI
      , Disable,  /* timer.acvive */
        0         /* timer.call_count */
#endif  /* #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */
    }
};

#ifdef ML7396_HWIF_NOTHAVE_TIMER_DI
static void timer_handler(void) {
    switch (hwif.timer.active) {
    case Enable:
        if (hwif.timer.handler != NULL)
            hwif.timer.handler();
        break;
    case Disable:
        ++hwif.timer.call_count;
        break;
    }
}
#endif  /* #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */

// 2016.6.8 Eiichi Saito: SubGHz API common
//static void idle(void) {
//     // 1) 最適化で消えない事
//     // 2) アイドル期間中に実行したい短い処理が在ればここに追加
//}

int ml7396_hwif_init(void) {
    int status = -1;
    uint32_t wait_t;

    hwif.timer.handler = NULL;
#ifdef ML7396_HWIF_NOTHAVE_TIMER_DI
    hwif.timer.active = Disable;
    hwif.timer.call_count = 0;
#endif  /* #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */
// 2016.6.8 Eiichi Saito: SubGHz API common
    status = HAL_init(0x50,8);
	if(status != 0) return status;
//  HAL_SPI_setup();
//  HAL_GPIO_setup();
    HAL_TIMER_setup();
//  HAL_I2C_setup();
    ml7396_hwif_timer_tick(&wait_t);  /* 時間待ち起点 - (A) */
    status = 0;
    return status;
}

int ml7396_hwif_spi_transfer(const uint8_t *wdata, uint8_t *rdata, uint8_t size) {

    int status = -1;
// 2016.6.8 Eiichi Saito: SubGHz API common
    uint8_t wsize, rsize;

    if(wdata[0]&REG_ADR_WRITE_BIT) {
            wsize = size;
            rsize = 0;
    }else{
            wsize = 1;
            rsize = size - wsize;
    }

    HAL_SPI_transfer(wdata, wsize, rdata, rsize);
//  HAL_SPI_transfer(wdata, rdata, size);
    status = 0;
    return status;
}

int ml7396_hwif_sint_handler(void (*func)(void)) {
    int status = -1;

    HAL_GPIO_setInterrupt(func);
    status = 0;
    return status;
}

int ml7396_hwif_sint_ei(void) {
    int status = -1;

    HAL_GPIO_enableInterrupt();
    status = 0;
    return status;
}

int ml7396_hwif_sint_di(void) {
    int status = -1;

    HAL_GPIO_disableInterrupt();
    status = 0;
    return status;
}

int ml7396_hwif_timer_start(uint16_t msec) {
    int status = -1;

#ifdef ML7396_HWIF_NOTHAVE_TIMER_DI
    HAL_TIMER_start(msec, timer_handler);
#else  /* #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */
    HAL_TIMER_start(msec, hwif.timer.handler);
#endif  /* #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */
    status = 0;
    return status;
}

int ml7396_hwif_timer_stop(void) {
    int status = -1;

    HAL_TIMER_stop();
    status = 0;
    return status;
}

int ml7396_hwif_timer_handler(void (*func)(void)) {
    int status = -1;

    hwif.timer.handler = func;
    status = 0;
    return status;
}

int ml7396_hwif_timer_ei(void) {
    int status = -1;

#ifdef ML7396_HWIF_NOTHAVE_TIMER_DI
    switch (hwif.timer.active) {
    case Disable:
        hwif.timer.active = Enable;
        if (hwif.timer.call_count) {
            hwif.timer.call_count = 0;
            if (hwif.timer.handler != NULL)
                hwif.timer.handler();
        }
        break;
    case Enable:
        break;
    }
#else  /* #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */
    HAL_TIMER_enableInterrupt();
#endif  /* #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */
    status = 0;
    return status;
}

int ml7396_hwif_timer_di(void) {
    int status = -1;

#ifdef ML7396_HWIF_NOTHAVE_TIMER_DI
    switch (hwif.timer.active) {
    case Disable:
        break;
    case Enable:
        hwif.timer.active = Disable;
        break;
    }
#else  /** #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */
    HAL_TIMER_disableInterrupt();
#endif  /* #ifdef ML7396_HWIF_NOTHAVE_TIMER_DI */
    status = 0;
    return status;
}

int ml7396_hwif_timer_tick(uint32_t *msec) {
    int status = -1;

    HAL_TIMER_getTick(msec);
    status = 0;
    return status;
}

static int eeprom_read(uint8_t addr, uint8_t *data, uint8_t size) {
    int status = -1;

    // 2016.6.8 Eiichi Saito: SubGHz API common
    // HAL_I2C_read(0x50, addr, data, size);
    HAL_I2C_read(addr, data, size);
    status = 0;
    return status;
}


/* レジスタ設定引数
 */
typedef struct {
    uint8_t channel;
    uint8_t rate;
    uint8_t txPower;
    uint8_t device_id; // 2015.09.08 Eiichi Saito
    uint16_t address;
} Setup;

// 2015.09.08 Eiichi Saito
#define DEIVE_ID_ROHM  0x90
#define DEIVE_ID_LAPIS 0xD0

/* 浮動小数点数値をQフォーマット固定小数点数値に変換
 *
 * n: 浮動小数点数値 (固定値である事 そうでないと浮動小数点演算ライブラリがリンクされてしまう)
 * q: 小数点位置
 * type: 戻り値の型
 * 戻り値: 固定小数点数値
 */
#define FIXQ(n, q, type) ((type)((n) * ((type)1 << (q)) + .5))

/* 固定小数点数値の整数部を取得
 *
 * n: 固定小数点数値
 * q: 小数点位置
 * 戻り値: 整数部
 */
#define INTQ(n, q) ((n) >> (q))

/* レジスタ設定定数
 */
typedef struct {
    uint8_t rate;                /* レート設定 */
// 2015.05.19 Eiichi Saito
//  uint32_t freq[29];           /* 周波数設定 */
    uint32_t freq[38];           /* 周波数設定 */
    uint32_t chspc;              /* 帯域幅設定 */
    uint32_t iffreq;             /* IF周波数設定 */
    uint32_t iffreq_cca;         /* IF周波数設定(CCA) */
    uint16_t coef, ref;          /* BPF設定 */
    uint16_t coef_cca, ref_cca;  /* BPF設定(CCA) */
    uint16_t fdev;               /* GFSK周波数偏位設定 */
    uint8_t reg1;                /* 隠しレジスタ設定 */
    uint8_t div;                 /* ダイバーシティサーチ設定 */
} REGSET;
/* 設定定数 (浮動小数点演算はCPUでは処理せず全てコンパイラに任せる) */
static const REGSET regset_50kbps = {
    0x10,                               /* rate */
    {                                   /* freq[] (帯域幅200kHz) */
        // 2015.05.19 Eiichi Saito added to channel from 24 to 33
        FIXQ(920.6, 20, uint32_t),        /* Channel=24 */
        FIXQ(920.8, 20, uint32_t),        /* Channel=25 */
        FIXQ(921.0, 20, uint32_t),        /* Channel=26 */
        FIXQ(921.2, 20, uint32_t),        /* Channel=27 */
        FIXQ(921.4, 20, uint32_t),        /* Channel=28 */
        FIXQ(921.6, 20, uint32_t),        /* Channel=29 */
        FIXQ(921.8, 20, uint32_t),        /* Channel=30 */
        FIXQ(922.0, 20, uint32_t),        /* Channel=31 */
        FIXQ(922.2, 20, uint32_t),        /* Channel=32 */
        FIXQ(922.4, 20, uint32_t),        /* Channel=33 */
        FIXQ(922.6, 20, uint32_t),        /* Channel=34 */
        FIXQ(922.8, 20, uint32_t),        /* Channel=35 */
        FIXQ(923.0, 20, uint32_t),        /* Channel=36 */
        FIXQ(923.2, 20, uint32_t),        /* Channel=37 */
        FIXQ(923.4, 20, uint32_t),        /* Channel=38 */
        FIXQ(923.6, 20, uint32_t),        /* Channel=39 */
        FIXQ(923.8, 20, uint32_t),        /* Channel=40 */
        FIXQ(924.0, 20, uint32_t),        /* Channel=41 */
        FIXQ(924.2, 20, uint32_t),        /* Channel=42 */
        FIXQ(924.4, 20, uint32_t),        /* Channel=43 */
        FIXQ(924.6, 20, uint32_t),        /* Channel=44 */
        FIXQ(924.8, 20, uint32_t),        /* Channel=45 */
        FIXQ(925.0, 20, uint32_t),        /* Channel=46 */
        FIXQ(925.2, 20, uint32_t),        /* Channel=47 */
        FIXQ(925.4, 20, uint32_t),        /* Channel=48 */
        FIXQ(925.6, 20, uint32_t),        /* Channel=49 */
        FIXQ(925.8, 20, uint32_t),        /* Channel=50 */
        FIXQ(926.0, 20, uint32_t),        /* Channel=51 */
        FIXQ(926.2, 20, uint32_t),        /* Channel=52 */
        FIXQ(926.4, 20, uint32_t),        /* Channel=53 */
        FIXQ(926.6, 20, uint32_t),        /* Channel=54 */
        FIXQ(926.8, 20, uint32_t),        /* Channel=55 */
        FIXQ(927.0, 20, uint32_t),        /* Channel=56 */
        FIXQ(927.2, 20, uint32_t),        /* Channel=57 */
        FIXQ(927.4, 20, uint32_t),        /* Channel=58 */
        FIXQ(927.6, 20, uint32_t),        /* Channel=59 */
        FIXQ(927.8, 20, uint32_t),        /* Channel=60 */
        FIXQ(928.0, 20, uint32_t)         /* Channel=61 */
    },
    FIXQ(0.200, 20, uint32_t),          /* chspc = 200kHz */
    FIXQ(0.250, 20, uint32_t),          /* iffreq = 250kHz */
    FIXQ(0.250, 20, uint32_t),          /* iffreq_cca = 250kHz */
    FIXQ(1.440, 14, uint16_t), 0x034b,  /* coef = 1.440, ref */
    FIXQ(1.440, 14, uint16_t), 0x034b,  /* coef_cca = 1.440, ref_cca */
    0x02d8,                             /* fdev */
    0x24,                               /* reg1 */
    0x11                                /* div = 0x11以上 */
};
static const REGSET regset_100kbps = {
    0x11,                               /* rate */
    {                                   /* freq[] (帯域幅400kHz) */
        // 2015.05.19 Eiichi Saito added to channel from 24 to 33
        FIXQ(920.7, 20, uint32_t),        /* Channel=24,25 */
        FIXQ(920.9, 20, uint32_t),        /* Channel=25,26 */
        FIXQ(921.1, 20, uint32_t),        /* Channel=26,27 */
        FIXQ(921.3, 20, uint32_t),        /* Channel=27,28 */
        FIXQ(921.5, 20, uint32_t),        /* Channel=28,29 */
        FIXQ(921.7, 20, uint32_t),        /* Channel=29,30 */
        FIXQ(921.9, 20, uint32_t),        /* Channel=30,31 */
        FIXQ(922.1, 20, uint32_t),        /* Channel=31,32 */
        FIXQ(922.3, 20, uint32_t),        /* Channel=32,33 2015.05.19 Eiichi Saito can not channel */
        FIXQ(922.5, 20, uint32_t),        /* Channel=33,34 */
        FIXQ(922.7, 20, uint32_t),        /* Channel=34,35 */
        FIXQ(922.9, 20, uint32_t),        /* Channel=35,36 */
        FIXQ(923.1, 20, uint32_t),        /* Channel=36,37 */
        FIXQ(923.3, 20, uint32_t),        /* Channel=37,38 */
        FIXQ(923.5, 20, uint32_t),        /* Channel=38,39 */
        FIXQ(923.7, 20, uint32_t),        /* Channel=39,40 */
        FIXQ(923.9, 20, uint32_t),        /* Channel=40,41 */
        FIXQ(924.1, 20, uint32_t),        /* Channel=41,42 */
        FIXQ(924.3, 20, uint32_t),        /* Channel=42,43 */
        FIXQ(924.5, 20, uint32_t),        /* Channel=43,44 */
        FIXQ(924.7, 20, uint32_t),        /* Channel=44,45 */
        FIXQ(924.9, 20, uint32_t),        /* Channel=45,46 */
        FIXQ(925.1, 20, uint32_t),        /* Channel=46,47 */
        FIXQ(925.3, 20, uint32_t),        /* Channel=47,48 */
        FIXQ(925.5, 20, uint32_t),        /* Channel=48,49 */
        FIXQ(925.7, 20, uint32_t),        /* Channel=49,50 */
        FIXQ(925.9, 20, uint32_t),        /* Channel=50,51 */
        FIXQ(926.1, 20, uint32_t),        /* Channel=51,52 */
        FIXQ(926.3, 20, uint32_t),        /* Channel=52,53 */
        FIXQ(926.5, 20, uint32_t),        /* Channel=53,54 */
        FIXQ(926.7, 20, uint32_t),        /* Channel=54,55 */
        FIXQ(926.9, 20, uint32_t),        /* Channel=55,56 */
        FIXQ(927.1, 20, uint32_t),        /* Channel=56,57 */
        FIXQ(927.3, 20, uint32_t),        /* Channel=57,58 */
        FIXQ(927.5, 20, uint32_t),        /* Channel=58,59 */
        FIXQ(927.7, 20, uint32_t),        /* Channel=59,60 */
        FIXQ(927.9, 20, uint32_t),        /* Channel=60,61 */
        FIXQ(927.9, 20, uint32_t)         /* Channel=60,61 (チャネル設定60と61は同等) */
    },
    FIXQ(0.400, 20, uint32_t),          /* chspc = 400kHz */
    FIXQ(0.180, 20, uint32_t),          /* iffreq = 180kHz */
    FIXQ(0.250, 20, uint32_t),          /* iffreq_cca = 250kHz */
    FIXQ(1.000, 14, uint16_t), 0x024a,  /* coef = 1.000, ref */
    FIXQ(0.480, 14, uint16_t), 0x0119,  /* coef_cca = 1.000, ref_cca */
    0x05b0,                             /* fdev */
    0x27,                               /* reg1 */
    0x16                                /* div = 0x16以上 */
};

/* BP3596用ML7396レジスタ設定
 */
int ml7396_hwif_regset(void *data) {
    int status = -1;
    Setup *setup = (Setup *)data;
    const REGSET *regset;
    uint8_t reg_data[4];

    /* 個体識別コード取得 */
    // 2015.09.08 Eiichi Saito
    eeprom_read(0x23, reg_data, 1), setup->device_id = reg_data[0];

    /* 固定値設定 */
    reg_data[0] = 0x0f, ml7396_regwrite(REG_ADR_CLK_SET,             reg_data, 1);
    reg_data[0] = 0x22, ml7396_regwrite(REG_ADR_RX_PR_LEN_SFD_LEN,   reg_data, 1);
    reg_data[0] = 0x00, ml7396_regwrite(REG_ADR_SYNC_CONDITION,      reg_data, 1);
#ifdef LAZURITE_MINI
    reg_data[0] = 0x02, ml7396_regwrite(REG_ADR_2DIV_CNTRL,          reg_data, 1);  /* 0x04 CRCエラー発生し難くなった */
#else
    // 2015.09.08 Eiichi Saito
    if (setup->device_id == DEIVE_ID_LAPIS)
        // 2015.07.28 Eiichi Saito : Set Fly ANT 
        reg_data[0] = 0x0A, ml7396_regwrite(REG_ADR_2DIV_CNTRL,          reg_data, 1);  /* 0x04 CRCエラー発生し難くなった */
    else
        reg_data[0] = 0x04, ml7396_regwrite(REG_ADR_2DIV_CNTRL,          reg_data, 1);  /* 0x04 CRCエラー発生し難くなった */
#endif
    reg_data[0] = 0x04, ml7396_regwrite(REG_ADR_SYNC_MODE,           reg_data, 1);
    reg_data[0] = 0x10, ml7396_regwrite(REG_ADR_RAMP_CNTRL,          reg_data, 1);
    reg_data[0] = 0x1e, ml7396_regwrite(REG_ADR_GAIN_MtoL,           reg_data, 1);
    reg_data[0] = 0x02, ml7396_regwrite(REG_ADR_GAIN_LtoM,           reg_data, 1);
    reg_data[0] = 0x9e, ml7396_regwrite(REG_ADR_GAIN_HtoM,           reg_data, 1);
    reg_data[0] = 0x02, ml7396_regwrite(REG_ADR_GAIN_MtoH,           reg_data, 1);
    reg_data[0] = 0x15, ml7396_regwrite(REG_ADR_RSSI_ADJ_M,          reg_data, 1);
    reg_data[0] = 0x2b, ml7396_regwrite(REG_ADR_RSSI_ADJ_L,          reg_data, 1);
    reg_data[0] = 0x22, ml7396_regwrite(REG_ADR_RSSI_STABLE_TIME,    reg_data, 1);  //
    reg_data[0] = 0xd4, ml7396_regwrite(REG_ADR_RSSI_VAL_ADJ,        reg_data, 1);
    reg_data[0] = 0x01, ml7396_regwrite(REG_ADR_AFC_CNTRL,           reg_data, 1);
    reg_data[0] = 0xaa, ml7396_regwrite(REG_ADR_PREAMBLE_SET,        reg_data, 1);
    reg_data[0] = 0x09, ml7396_regwrite(REG_ADR_SFD1_SET1,           reg_data, 1);
    reg_data[0] = 0x72, ml7396_regwrite(REG_ADR_SFD1_SET2,           reg_data, 1);
    reg_data[0] = 0xf6, ml7396_regwrite(REG_ADR_SFD1_SET3,           reg_data, 1);
    reg_data[0] = 0x72, ml7396_regwrite(REG_ADR_SFD1_SET4,           reg_data, 1);
    reg_data[0] = 0x5e, ml7396_regwrite(REG_ADR_SFD2_SET1,           reg_data, 1);
    reg_data[0] = 0x70, ml7396_regwrite(REG_ADR_SFD2_SET2,           reg_data, 1);
    reg_data[0] = 0xc6, ml7396_regwrite(REG_ADR_SFD2_SET3,           reg_data, 1);
    reg_data[0] = 0xb4, ml7396_regwrite(REG_ADR_SFD2_SET4,           reg_data, 1);
    reg_data[0] = 0xb6, ml7396_regwrite(REG_ADR_2DIV_GAIN_CNTRL,     reg_data, 1);
    reg_data[0] = 0x84, ml7396_regwrite(1,0x39,                      reg_data, 1);  /* 隠しレジスタ */
    reg_data[0] = 0x8f, ml7396_regwrite(REG_ADR_PLL_CTRL,            reg_data, 1);
    reg_data[0] = 0x32, ml7396_regwrite(REG_ADR_RX_ON_ADJ2,          reg_data, 1);
    reg_data[0] = 0x0f, ml7396_regwrite(REG_ADR_LNA_GAIN_ADJ_M,      reg_data, 1);
    reg_data[0] = 0x01, ml7396_regwrite(REG_ADR_LNA_GAIN_ADJ_L,      reg_data, 1);
    reg_data[0] = 0xff, ml7396_regwrite(REG_ADR_MIX_GAIN_ADJ_M,      reg_data, 1);
    reg_data[0] = 0xff, ml7396_regwrite(REG_ADR_MIX_GAIN_ADJ_L,      reg_data, 1);
    reg_data[0] = 0xb4, ml7396_regwrite(REG_ADR_TX_OFF_ADJ1,         reg_data, 1);
    reg_data[0] = 0x01, ml7396_regwrite(REG_ADR_RSSI_SLOPE_ADJ,      reg_data, 1);
    reg_data[0] = 0x04, ml7396_regwrite(REG_ADR_PA_ON_ADJ,           reg_data, 1);
    reg_data[0] = 0x0a, ml7396_regwrite(REG_ADR_RX_ON_ADJ,           reg_data, 1);
    reg_data[0] = 0x00, ml7396_regwrite(REG_ADR_RXD_ADJ,             reg_data, 1);
    reg_data[0] = 0x2c, ml7396_regwrite(2,0x2d,                      reg_data, 1);  /* 隠しレジスタ */
    reg_data[0] = 0xc0, ml7396_regwrite(2,0x2e,                      reg_data, 1);  /* 隠しレジスタ */
    reg_data[0] = 0x17, ml7396_regwrite(2,0x2f,                      reg_data, 1);  /* 隠しレジスタ */
    reg_data[0] = 0x17, ml7396_regwrite(2,0x30,                      reg_data, 1);  /* 隠しレジスタ */
    eeprom_read(0x2b, reg_data, 1), ml7396_regwrite(REG_ADR_PA_ADJ1, reg_data, 1);  /*  1mW粗調整 */
    eeprom_read(0x29, reg_data, 1), ml7396_regwrite(REG_ADR_PA_ADJ3, reg_data, 1);  /* 20mW粗調整 */
    switch (setup->txPower) {
    case  1:  /*  1mW */
        eeprom_read(0x2c, reg_data, 1), ml7396_regwrite(REG_ADR_PA_REG_FINE_ADJ, reg_data, 1);
        reg_data[0] = 0x01, ml7396_regwrite(REG_ADR_PA_CNTRL,                    reg_data, 1);
        break;
    case 20:  /* 20mW */
        eeprom_read(0x2a, reg_data, 1), ml7396_regwrite(REG_ADR_PA_REG_FINE_ADJ, reg_data, 1);
        reg_data[0] = 0x13, ml7396_regwrite(REG_ADR_PA_CNTRL,                    reg_data, 1);
        break;
    default:
        goto error;
    }
    reg_data[0] = 0x0f, ml7396_regwrite(REG_ADR_SW_OUT_RAMP_ADJ,     reg_data, 1);
    reg_data[0] = 0x08, ml7396_regwrite(REG_ADR_IQ_MAG_ADJ,          reg_data, 1);
    reg_data[0] = 0x20, ml7396_regwrite(REG_ADR_IQ_PHASE_ADJ,        reg_data, 1);
#ifdef LAZURITE_MINI
    reg_data[0] = 0x07, ml7396_regwrite(REG_ADR_PA_REG_ADJ1,         reg_data, 1);
    reg_data[0] = 0x07, ml7396_regwrite(REG_ADR_PA_REG_ADJ2,         reg_data, 1);
    reg_data[0] = 0x07, ml7396_regwrite(REG_ADR_PA_REG_ADJ3,         reg_data, 1);
    reg_data[0] = 0x30, ml7396_regwrite(REG_ADR_CCA_LEVEL,           reg_data, 1);
#else
    reg_data[0] = 0x06, ml7396_regwrite(REG_ADR_PA_REG_ADJ1,         reg_data, 1);
    reg_data[0] = 0x01, ml7396_regwrite(REG_ADR_PA_REG_ADJ2,         reg_data, 1);
    reg_data[0] = 0x01, ml7396_regwrite(REG_ADR_PA_REG_ADJ3,         reg_data, 1);
    reg_data[0] = 0x55, ml7396_regwrite(REG_ADR_CCA_LEVEL,           reg_data, 1);
#endif
    reg_data[0] = 0x04, ml7396_regwrite(REG_ADR_TX_PR_LEN,           reg_data, 1);  /* 0x04以上 */
    reg_data[0] = 0x1f, ml7396_regwrite(REG_ADR_RSSI_LPF_ADJ,        reg_data, 1);
    reg_data[0] = 0x44, ml7396_regwrite(REG_ADR_PLL_CP_ADJ,          reg_data, 1);
    eeprom_read(0x2d, reg_data, 1), ml7396_regwrite(REG_ADR_OSC_ADJ, reg_data, 1);  /* XA */
// 2015.07.28 Eiichi Saito : Packet loss correction
    if (setup->device_id == DEIVE_ID_LAPIS) {
        eeprom_read(0x80, reg_data, 1), ml7396_regwrite(REG_ADR_OSC_ADJ2, reg_data, 1);  /* XA */
    //  reg_data[0] = 0x06, ml7396_regwrite(REG_ADR_OSC_ADJ2, reg_data, 1);  /* XA */
    //  reg_data[0] = 0x58, ml7396_regwrite(REG_ADR_OSC_ADJ, reg_data, 1);  /* XA */
    }
#ifdef LAZURITE_MINI
    eeprom_read(0x81, reg_data, 1), ml7396_regwrite(REG_ADR_PA_REG_ADJ3, reg_data, 1);
    eeprom_read(0x82, reg_data, 1), ml7396_regwrite(REG_ADR_RF_CNTRL_SET, reg_data, 1);
#endif
    /* 可変値設定 */
    switch (setup->rate) {
    case  50:  /*  50kbps */
        regset = &regset_50kbps;
        break;
    case 100:  /* 100kbps */
        regset = &regset_100kbps;
        break;
    default:
        goto error;
    }
    /* レート設定 */
    reg_data[0] = regset->rate, ml7396_regwrite(REG_ADR_DATA_SET, reg_data, 1);
    {  /* 周波数設定 */
        uint32_t freq_ch0, freq_min;
        uint8_t n4, a;
        uint32_t f;

        // 2015.05.19 Eiichi Saito added to channel
//      if (setup->channel < 33 || setup->channel > 61)
        if (setup->channel < 24 || setup->channel > 61)
            goto error;
        if (setup->rate == 100 && setup->channel == 32)
            goto error;
        // 2015.07.30 Eiichi Saito for ARIB 5ms:idle_wait=on, 128us:idle_wait=off
        /* CCA IDLE WAIT時間設定 */
        if (setup->channel <= 32) {
            reg_data[0] = 0x01, ml7396_regwrite(REG_ADR_IDLE_WAIT_H, reg_data, 1);
            reg_data[0] = 0x18, ml7396_regwrite(REG_ADR_IDLE_WAIT_L, reg_data, 1);
        } else
        if (setup->channel <= 61 ) {
            reg_data[0] = 0x00, ml7396_regwrite(REG_ADR_IDLE_WAIT_H, reg_data, 1);
        // 2015.05.23 Eiichi Saito ARIB 5ms:idle_wait=on, 1.7us:idle_wait=off
            reg_data[0] = 0x64, ml7396_regwrite(REG_ADR_IDLE_WAIT_L, reg_data, 1);
        }

        // 2015.05.19 Eiichi Saito added to channel
//      freq_ch0 = regset->freq[setup->channel - 33];  /* チャネル番号の周波数を取得 */
        freq_ch0 = regset->freq[setup->channel - 24];  /* チャネル番号の周波数を取得 */
        freq_min = freq_ch0 - FIXQ(2.0, 20, uint32_t);  /* CH0周波数より2MHz低い値をキャリブレーション下限値とする (浮動小数点演算はCPUでは処理せず全てコンパイラに任せる) */
        freq_ch0 /= 36;
        freq_min /= 36;
        if (INTQ(freq_min, 20) != INTQ(freq_ch0, 20))  /* 36MHz 境界を跨ぐ設定は無効 */
            goto error;
        n4 = (INTQ(freq_ch0 >> 2, 20) & 0x0f) << 2;  /* nの4倍値 */
        a = (INTQ(freq_ch0, 20) - n4) & 0x03;
        f = (freq_ch0 - ((n4 + a) << 20)) & 0x0fffff;
        reg_data[0] = f >>  0 & 0xff, reg_data[1] = f >>  8 & 0xff, reg_data[2] = f >> 16 & 0x0f;
        reg_data[3] = n4 << 2 | a;
        ml7396_regwrite(REG_ADR_CH0_FL, reg_data, 4);  /* 特殊コマンド: bp.param[BP_PARAM_CH0_FL]の値を設定 */
        n4 = (INTQ(freq_min >> 2, 20) & 0x0f) << 2;  /* nの4倍値 */
        a = (INTQ(freq_min, 20) - n4) & 0x03;
        f = (freq_min - ((n4 + a) << 20)) & 0x0fffff;
        reg_data[0] = f >>  0 & 0xff, reg_data[1] = f >>  8 & 0xff, reg_data[2] = f >> 16 & 0x0f;
        reg_data[3] = n4 << 2 | a;  /* このデータは使われない */
        ml7396_regwrite(REG_ADR_VCO_CAL_MIN_FL, reg_data, 3);  /* 特殊コマンド: bp.param[BP_PARAM_MIN_FL]の値を設定 */
        reg_data[0] = 0x07, ml7396_regwrite(REG_ADR_VCO_CAL_MAX_N, reg_data, 1);  /* 帯域幅 400kHz */
    }
    {
        uint16_t n;

        /* 帯域幅設定 */
        n = regset->chspc / 36;
        reg_data[0] = n >>  0 & 0xff, reg_data[1] = n >>  8 & 0xff;
        ml7396_regwrite(REG_ADR_CH_SPACE_L, reg_data, 2);
        /* IF周波数設定 */
        n = regset->iffreq / 36;
        reg_data[0] = n >>  8 & 0xff, reg_data[1] = n >>  0 & 0xff;
        ml7396_regwrite(REG_ADR_IF_FREQ_H, reg_data, 2);
        ml7396_regwrite(REG_ADR_IF_FREQ_AFC_H, reg_data, 2);
        n = regset->iffreq_cca / 36;
        reg_data[0] = n >>  8 & 0xff, reg_data[1] = n >>  0 & 0xff;
        ml7396_regwrite(REG_ADR_IF_FREQ_CCA_H, reg_data, 2);
    }
    {  /* BPF設定 */
        uint8_t bpf;
        uint16_t n;

        ml7396_regread(REG_ADR_BPF_ADJ_OFFSET, reg_data, 1), bpf = reg_data[0];
        if (bpf & 0x80)
            n = regset->ref + (uint16_t)INTQ((uint32_t)(bpf & 0x7f) * regset->coef, 14);
        else
            n = regset->ref - (uint16_t)INTQ((uint32_t)(bpf & 0x7f) * regset->coef, 14);
        reg_data[0] = n >> 8 & 0xff, reg_data[1] = n >> 0 & 0xff;
        ml7396_regwrite(REG_ADR_BPF_ADJ_H,     reg_data, 2);  /* REG_ADR_BPF_ADJ_H と REG_ADR_BPF_ADJ_L に書き込む */
        ml7396_regwrite(REG_ADR_BPF_AFC_ADJ_H, reg_data, 2);  /* REG_ADR_BPF_AFC_ADJ_H と REG_ADR_BPF_AFC_ADJ_L に書き込む */
        if (bpf & 0x80)
            n = regset->ref_cca + (uint16_t)INTQ((uint32_t)(bpf & 0x7f) * regset->coef_cca, 14);
        else
            n = regset->ref_cca - (uint16_t)INTQ((uint32_t)(bpf & 0x7f) * regset->coef_cca, 14);
        reg_data[0] = n >> 8 & 0xff, reg_data[1] = n >> 0 & 0xff;
        ml7396_regwrite(REG_ADR_BPF_CCA_ADJ_H, reg_data, 2);  /* REG_ADR_BPF_CCA_ADJ_H と REG_ADR_BPF_CCA_ADJ_L に書き込む */
    }

    // 2015.05.21 Eiichi Saito
    reg_data[0] = 0x2c, ml7396_regwrite(REG_ADR_PRIVATE_BPF_CAP1,  reg_data, 1);
    reg_data[0] = 0xc0, ml7396_regwrite(REG_ADR_PRIVATE_BPF_CAP2,  reg_data, 1);
    reg_data[0] = 0x17, ml7396_regwrite(REG_ADR_PRIVATE_BPF_ADJ1,  reg_data, 1);
    reg_data[0] = 0x17, ml7396_regwrite(REG_ADR_PRIVATE_BPF_ADJ2,  reg_data, 1);

    /* GFSK周波数偏位設定 */
    reg_data[0] = regset->fdev >> 0 & 0xff, reg_data[1] = regset->fdev >> 8 & 0xff;
    reg_data[0] = 0xb0, ml7396_regwrite(REG_ADR_F_DEV_L, reg_data, 2);
    /* 隠しレジスタ設定 */
    reg_data[0] = regset->reg1, ml7396_regwrite(2,0x0e, reg_data, 1);
    /* ダイバーシティサーチ設定 */
    reg_data[0] = regset->div, ml7396_regwrite(REG_ADR_2DIV_SEARCH, reg_data, 1);

    /* 自機器アドレス取得 */
    eeprom_read(0x26, reg_data, 2), setup->address = n2u16(reg_data);

    status = 0;
error:
    return status;
}
