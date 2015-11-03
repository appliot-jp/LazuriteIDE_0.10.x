/* bp3596.c - SDK API
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

#define Version 1,4  /* バージョン番号: Major, Miner */


#include <stdint.h>
#include <string.h>
#include "../core/ml7396.h"
#include "../core/endian.h"
#include "../core/ieee802154.h"
#include "bp3596.h"


/* 内部エラーコード
 */
#define BP3596_STATUS_UNKNOWN INT_MIN  /* システムエラー */

/* 送受信バッファサイズ
 */
#define RXBUFFER_CAPACITY ML7396_BUFFER_CAPACITY  /* 受信バッファ */
#define TXBUFFER_CAPACITY ML7396_BUFFER_CAPACITY  /* 送信バッファ */

/* APIグローバル変数
 */
static struct {
    uint16_t *addr;   /* 自機アドレスのポインタ */
    uint16_t *panID;  /* 自機が属しているPANIDのポインタ */
    struct {
        ML7396_Buffer buffer;
        void (*done)(const void *data, uint8_t rssi, int status);  /* コールバック関数 */
        struct {
            uint16_t dstpanid;
            uint16_t dstaddr0;
            uint16_t dstaddr1;
        } filter;
    } rx;
    struct {
        ML7396_Buffer buffer;
        void (*done)(uint8_t rssi, int status);  /* コールバック関数 */
    } tx;
} api = {
    NULL  /* 未初期化設定を addr==NUKLL で判定 */
};


/* 受信完了コールバック関数 */
static void rxdone(ML7396_Buffer *buffer) {
    if (api.rx.done)
        api.rx.done((void *)buffer->data, buffer->opt.common.ed, buffer->status);  /*@ ED値をそのままrssiとして返しているが変換が必要? */
}

/* 送信完了コールバック関数 */
static void txdone(ML7396_Buffer *buffer) {
    if (api.tx.done)
        api.tx.done(buffer->opt.common.ed, buffer->status);  /*@ ED値をそのままrssiとして返しているが変換が必要? */
}

/* 受信フィルタ関数 */
static int rxfilter(const ML7396_Header *header) {
    int filter = 0;

    if (api.rx.filter.dstpanid == 0 && api.rx.filter.dstaddr0 == 0 && api.rx.filter.dstaddr1 == 0 ||
        header->dstpanid == api.rx.filter.dstpanid ||
        header->dstaddr == api.rx.filter.dstaddr0 ||
        header->dstaddr == api.rx.filter.dstaddr1 )
        filter = !0;
    return filter;
}

/* 送信データ作成
 *
 * header構造体を解析して下記情報をdataのMACヘッダに埋め込む:
 *  フレームコントロールのPANID圧縮
 *  フレームコントロールのシーケンス番号圧縮
 *  フレームコントロールの受信アドレスモード
 *  フレームコントロールの送信アドレスモード
 *  シーケンス番号
 *  宛て先PANID
 *  宛て先アドレス
 *  送り元PANID
 *  送り元アドレス
 *
 *  data: 送信データバッファ
 *  size: 送信データバッファサイズ
 *  *header: 送信データに展開するヘッダ情報
 *  戻り値: ペイロードデータの先頭アドレス
 *          戻り値 - data = MACヘッダサイズ
 *          MACヘッダサイズ + ペイロードサイス = 送信データサイズ
 *
 */
static uint8_t *make_data(uint8_t *data, uint16_t size, ML7396_Header *header) {
    uint8_t *payload = NULL;
    struct {
        uint16_t dstaddrmode;
        uint16_t srcaddrmode;
        uint16_t panidcomps;
        uint16_t seqsuppress;
    } fc;

    /* IEEE 802.15.4e フレームのビーコンとデータ、ACK以外は未対応 */
    switch (header->fc & (IEEE802154_FC_IEEE802154_MASK|IEEE802154_FC_TYPE_MASK)) {
    case IEEE802154_FC_IEEE802154_E|IEEE802154_FC_TYPE_BEACON:
    case IEEE802154_FC_IEEE802154_E|IEEE802154_FC_TYPE_DATA:
    case IEEE802154_FC_IEEE802154_E|IEEE802154_FC_TYPE_ACK:
        break;
    default:
        goto error;
    }
    /* ヘッダ情報から宛て先/送り元のPANID/アドレスとシーケンス番号のフィールドサイズを取得 */
    fc.dstaddrmode = header->dstaddr == ML7396_HEADER_ADDRNONE ? IEEE802154_FC_DAMODE_NONE : IEEE802154_FC_DAMODE_SHORT;
    fc.srcaddrmode = header->srcaddr == ML7396_HEADER_ADDRNONE ? IEEE802154_FC_SAMODE_NONE : IEEE802154_FC_SAMODE_SHORT;
    switch (fc.dstaddrmode | fc.srcaddrmode) {  /* 規格上無効な組み合わせは未対応 */
    case IEEE802154_FC_DAMODE_NONE|IEEE802154_FC_SAMODE_NONE:
        if (header->dstpanid == ML7396_HEADER_PANIDNONE && header->srcpanid == ML7396_HEADER_PANIDNONE)
            fc.panidcomps = 0;
        else if (header->dstpanid != ML7396_HEADER_PANIDNONE && header->srcpanid == ML7396_HEADER_PANIDNONE)
            fc.panidcomps = IEEE802154_FC_PANID_COMPS;
        else
            goto error;
        break;
    case IEEE802154_FC_DAMODE_NONE|IEEE802154_FC_SAMODE_SHORT:
        if (header->dstpanid == ML7396_HEADER_PANIDNONE && header->srcpanid == ML7396_HEADER_PANIDNONE)
            fc.panidcomps = IEEE802154_FC_PANID_COMPS;
        else if (header->dstpanid == ML7396_HEADER_PANIDNONE && header->srcpanid != ML7396_HEADER_PANIDNONE)
            fc.panidcomps = 0;
        else
            goto error;
        break;
    case IEEE802154_FC_DAMODE_SHORT|IEEE802154_FC_SAMODE_NONE:
    case IEEE802154_FC_DAMODE_SHORT|IEEE802154_FC_SAMODE_SHORT:
        if (header->dstpanid == ML7396_HEADER_PANIDNONE && header->srcpanid == ML7396_HEADER_PANIDNONE)
            fc.panidcomps = IEEE802154_FC_PANID_COMPS;
        else if (header->dstpanid != ML7396_HEADER_PANIDNONE && header->srcpanid == ML7396_HEADER_PANIDNONE)
            fc.panidcomps = 0;
        else
            goto error;
        break;
    default:
        goto error;
    }
    fc.seqsuppress = header->seq == ML7396_HEADER_SEQNONE ? IEEE802154_FC_SEQ_SUPPRESS : 0;
    if (size < 2)
        goto error;
    /* MACヘッダのフィールドコントロールに宛て先/送り元のPANID/アドレスとシーケンス番号のフィールドサイズ情報を反映 */
    header->fc &= ~(IEEE802154_FC_PANID_COMPS|IEEE802154_FC_SEQ_SUPPRESS|IEEE802154_FC_DAMODE_MASK|IEEE802154_FC_SAMODE_MASK);
    header->fc |= fc.dstaddrmode | fc.srcaddrmode | fc.panidcomps | fc.seqsuppress;
    u2v16_set(header->fc, data), data += 2, size -= 2;
    /* MACヘッダに宛て先/送り元のPANID/アドレスとシーケンス番号を付加 */
    if (!fc.seqsuppress) {
        if (size < 1)
            goto error;
        *data++ = header->seq, --size;
    }
    if (header->dstpanid != ML7396_HEADER_PANIDNONE) {
        if (size < 2)
            goto error;
        u2v16_set(header->dstpanid, data), data += 2, size -= 2;
    }
    if (header->dstaddr != ML7396_HEADER_ADDRNONE) {
        if (size < 2)
            goto error;
        u2v16_set(header->dstaddr, data), data += 2, size -= 2;
    }
    if (header->srcpanid != ML7396_HEADER_PANIDNONE) {
        if (size < 2)
            goto error;
        u2v16_set(header->srcpanid, data), data += 2, size -= 2;
    }
    if (header->srcaddr != ML7396_HEADER_ADDRNONE) {
        if (size < 2)
            goto error;
        u2v16_set(header->srcaddr, data), data += 2, size -= 2;
    }
    /* ペイロードの先頭アドレスを返す */
    payload = data;
error:
    return payload;
}


int BP3596_reset(void) {
    int status = BP3596_STATUS_UNKNOWN;
    static uint8_t rxdata[RXBUFFER_CAPACITY], txdata[TXBUFFER_CAPACITY];  /* 送受信バッファ */

    ml7396_reset();
    memset(&api, 0, sizeof(api));  /* API共有データをクリア */
    /* 受信バッファ初期化 */
    api.rx.buffer.data = rxdata, api.rx.buffer.capacity = RXBUFFER_CAPACITY;
    api.rx.buffer.opt.rx.done = rxdone, api.rx.buffer.opt.rx.next = &api.rx.buffer;
#ifndef SNIFFER
	api.rx.buffer.opt.rx.filter = rxfilter;
#endif
    /* 送信バッファ初期化 */
    api.tx.buffer.data = txdata, api.tx.buffer.capacity = TXBUFFER_CAPACITY;
    api.tx.buffer.opt.tx.done = txdone, api.tx.buffer.opt.tx.next = NULL;
    /* 受信フィルタク初期化 */
    api.rx.filter.dstpanid = 0, api.rx.filter.dstaddr0 = 0, api.rx.filter.dstaddr1 = 0;
    status = BP3596_STATUS_OK;
    return status;
}

// 2015.10.26 Eiichi Saito   addition random backoff
int BP3596_setup(uint8_t channel, uint8_t rate, uint8_t txPower, uint8_t senseTime,
                 uint8_t txRetry, uint16_t txInterval, uint16_t ccaWait ) {
    int status = BP3596_STATUS_UNKNOWN;
    struct {
        uint8_t channel;
        uint8_t rate;
        uint8_t txPower;
        uint16_t address;
    } setup;

    setup.channel = channel;
    setup.rate = rate, setup.txPower = txPower;
    if (ml7396_setup(&setup) < 0) {
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    api.addr = ml7396_myaddr(), *api.addr = setup.address;
    api.panID = ml7396_mypanid(), *api.panID = 0;
    /* 送信バッファ設定 */
    api.tx.buffer.opt.tx.ack.wait = txInterval, api.tx.buffer.opt.tx.ack.retry = txRetry;
// 2015.10.26 Eiichi Saito   addition random backoff
//  api.tx.buffer.opt.tx.cca.wait = 100, api.tx.buffer.opt.tx.cca.retry = senseTime;
    api.tx.buffer.opt.tx.cca.wait = ccaWait , api.tx.buffer.opt.tx.cca.retry = senseTime;
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_getMyAddr(uint16_t *addr) {
    int status = BP3596_STATUS_UNKNOWN;

    if (addr == NULL) {
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    if (api.addr == NULL) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    *addr = *api.addr;
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_setMyPANID(uint16_t panID) {
    int status = BP3596_STATUS_UNKNOWN;

    if (api.addr == NULL) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    *api.panID = panID;
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_setFilter(uint16_t panID, uint16_t addr0, uint16_t addr1) {
    int status = BP3596_STATUS_UNKNOWN;

    api.rx.filter.dstpanid = panID;
    api.rx.filter.dstaddr0 = addr0;
    api.rx.filter.dstaddr1 = addr1;
    ml7396_setAddrFilter((uint8_t *)&api.rx.filter);
    status = BP3596_STATUS_OK;
    return status;
}

int BP3596_send(const void *data, uint16_t size,
                uint8_t addrType, uint16_t dstAddr, uint16_t dstPANID ) {
    int status = BP3596_STATUS_UNKNOWN;
    static uint8_t seq = 0;
    ML7396_Header header;
    uint8_t *payload;

    if (data == NULL || size == 0) {
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    if (api.addr == NULL) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    header.fc = IEEE802154_FC_IEEE802154_E|IEEE802154_FC_TYPE_DATA;
    header.seq = seq++;
    header.dstpanid = ML7396_HEADER_PANIDNONE;
    header.dstaddr  = ML7396_HEADER_ADDRNONE;
    header.srcpanid = ML7396_HEADER_PANIDNONE;
    header.srcaddr  = ML7396_HEADER_ADDRNONE;
    switch (addrType) {
    case 0:
        break;
    case 1:
        if (dstPANID == ML7396_HEADER_PANIDNONE) {
            status = BP3596_STATUS_ERROR_PARAM;
            goto error;
        }
        header.dstpanid = dstPANID;
        break;
    case 2:
        if (*api.addr == ML7396_HEADER_ADDRNONE ||
            *api.panID == ML7396_HEADER_PANIDNONE ) {
            status = BP3596_STATUS_ERROR_PARAM;
            goto error;
        }
        header.srcpanid = *api.panID;
        header.srcaddr  = *api.addr;
        break;
    case 3:
        if (*api.addr == ML7396_HEADER_ADDRNONE) {
            status = BP3596_STATUS_ERROR_PARAM;
            goto error;
        }
        header.srcaddr  = *api.addr;
        break;
    case 4:
        if (dstPANID == ML7396_HEADER_PANIDNONE ||
            dstAddr == ML7396_HEADER_ADDRNONE ) {
            status = BP3596_STATUS_ERROR_PARAM;
            goto error;
        }
        header.dstpanid = dstPANID;
        header.dstaddr  = dstAddr;
        break;
    case 5:
        if (dstAddr == ML7396_HEADER_ADDRNONE) {
            status = BP3596_STATUS_ERROR_PARAM;
            goto error;
        }
        header.dstaddr  = dstAddr;
        break;
    case 6:
        if (dstPANID == ML7396_HEADER_PANIDNONE ||
            dstAddr == ML7396_HEADER_ADDRNONE ||
            *api.addr == ML7396_HEADER_ADDRNONE ) {
            status = BP3596_STATUS_ERROR_PARAM;
            goto error;
        }
        if (dstAddr != 0xffff)
            header.fc |= IEEE802154_FC_ACKREQ;
        header.dstpanid = dstPANID;
        header.dstaddr  = dstAddr;
        header.srcaddr  = *api.addr;
        break;
    case 7:
        if (dstAddr == ML7396_HEADER_ADDRNONE ||
            *api.addr == ML7396_HEADER_ADDRNONE ) {
            status = BP3596_STATUS_ERROR_PARAM;
            goto error;
        }
        if (dstAddr != 0xffff)
            header.fc |= IEEE802154_FC_ACKREQ;
        header.dstaddr  = dstAddr;
        header.srcaddr  = *api.addr;
        break;
    default:
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    payload = make_data(api.tx.buffer.data, api.tx.buffer.capacity, &header);
    if (payload == NULL) {
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    api.tx.buffer.size = size + (payload - api.tx.buffer.data);
    if (api.tx.buffer.size > api.tx.buffer.capacity) {
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    memcpy(payload, data, size);
    if (ml7396_txstart(&api.tx.buffer) < 0) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_sendRaw(const void *data, uint16_t size) {
    int status = BP3596_STATUS_UNKNOWN;

    if (data == NULL || size == 0) {
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    if (api.addr == NULL) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    if (size > TXBUFFER_CAPACITY) {
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    memcpy(api.tx.buffer.data, data, size), api.tx.buffer.size = size;
    if (ml7396_txstart(&api.tx.buffer) < 0) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_setFuncSendComplete(void (*sendComplete)(uint8_t rssi, int status)) {
    int status = BP3596_STATUS_UNKNOWN;

    if (sendComplete == NULL) {
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    api.tx.done = sendComplete;
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_recvEnable(void) {
    int status = BP3596_STATUS_UNKNOWN;

    if (api.addr == NULL) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    if (ml7396_rxstart(&api.rx.buffer) < 0) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_recvDisable(void) {
    int status = BP3596_STATUS_UNKNOWN;

    if (api.addr == NULL) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    if (ml7396_rxstop() < 0) {  /* 処理内容は上の同じだがエラー判定のレイヤが違うので別にしておく */
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_setFuncRecvComplete(void (*recvComplete)(const void *data, uint8_t rssi, int status)) {
    int status = BP3596_STATUS_UNKNOWN;

    if (recvComplete == NULL) {
        status = BP3596_STATUS_ERROR_PARAM;
        goto error;
    }
    api.rx.done = recvComplete;
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_sleep(void) {
    int status = BP3596_STATUS_UNKNOWN;

    if (api.addr == NULL) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    if (ml7396_sleep() < 0) {  /* 処理内容は上の同じだがエラー判定のレイヤが違うので別にしておく */
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_wakeup(void) {
    int status = BP3596_STATUS_UNKNOWN;

    if (api.addr == NULL) {
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    if (ml7396_wakeup() < 0) {  /* 処理内容は上の同じだがエラー判定のレイヤが違うので別にしておく */
        status = BP3596_STATUS_ERROR_STATE;
        goto error;
    }
    status = BP3596_STATUS_OK;
error:
    return status;
}

int BP3596_getState(void) {
    int status = BP3596_STATUS_UNKNOWN;

    switch (ml7396_state()) {
    case ML7396_StateReset:
        status = BP3596_STATUS_STATE_RESET;
        break;
    case ML7396_StateIdle:
        status = ml7396_rxbuffer() != NULL ? BP3596_STATUS_STATE_RECV : BP3596_STATUS_STATE_IDLE;
        break;
    case ML7396_StateSendACK:
        status = BP3596_STATUS_STATE_RECV;
        break;
    case ML7396_StateSending:
        status = BP3596_STATUS_STATE_SEND;
        break;
    case ML7396_StateWaitACK:
        status = BP3596_STATUS_STATE_SEND;
        break;
    case ML7396_StateSleep:
        status = BP3596_STATUS_STATE_SLEEP;
        break;
    }
    return status;
}

static uint16_t ver2u16(uint8_t Maj, uint8_t Min) {
    return (uint16_t)Maj << 8 | (uint16_t)Min << 0;  /* Version を 16ビット数値に変換 */
}

int BP3596_getVersion(uint16_t *version) {
    int status = BP3596_STATUS_UNKNOWN;

    *version = ver2u16(Version);
    status = BP3596_STATUS_OK;
    return status;
}
