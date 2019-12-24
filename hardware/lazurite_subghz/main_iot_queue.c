
/* FILE NAME: main_iot_queue.c
 * The MIT License (MIT)
 *
 * Copyright (c) 2019  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "lazurite.h"
#include <limits.h>

/* --------------------------------------------------------------------------------
 * MACRO switch
 * -------------------------------------------------------------------------------- */
#define DEBUG // uncomment, if debuggging
#define USE_LED // uncomment, if using LED for debugging
#define LIB_DEBUG // uncomment, if use libdebug
//#define BREAK_MODE // uncomment, if use BREAK_MODE of libdebug

#include "..\..\libraries\libdebug\libdebug.h"
#if defined(LIB_DEBUG) && !defined(DEBUG)
	#error Missing DEBUG macro.
#endif

/* --------------------------------------------------------------------------------
 * Please note that this sample program needs OTA library and the below AES key
 * must be changed apporpriately.
 * -------------------------------------------------------------------------------- */
#pragma SEGCONST "OTA_USER_PARAM"
const uint8_t ota_aes_key[OTA_AES_KEY_SIZE] = {
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
#pragma SEGCONST

#define ORANGE_LED				( 25 )
#define BLUE_LED				( 26 )
#define SUBGHZ_CH				( 36 )
#define DEFAULT_SLEEP_INTERVAL	( 5*1000ul )
#define MAX_BUF_SIZE			( 240 )
#define NO_SLEEP ( 0 )

typedef enum {
	STATE_TRIG_ACTIVATE = 0,
	STATE_WAIT_ACTIVATE,
	STATE_INIT_SENSOR,
	STATE_SEND_QUEUE,
	STATE_TRIG_RECONNECT,
	STATE_WAIT_RECONNECT,
	STATE_TRIG_UPD_PARAM,
	STATE_WAIT_UPD_PARAM,
	STATE_TRIG_FW_UPD,
	STATE_WAIT_FW_UPD
} MAIN_IOT_STATE;

typedef struct {
	MAIN_IOT_STATE func_mode;
	uint8_t sensor_type;
	bool enable_sense;
	uint32_t last_sense_time; // last timestamp of getting sensor data
	uint32_t sense_interval; // paramater gotten from device config via EACK
	uint32_t sleep_time; // control sleep interval in loop()
	uint16_t gateway_panid;
	uint16_t gateway_addr;
	uint16_t my_short_addr;
} MAIN_IOT_PARAM;

typedef struct {
	uint16_t panid;
	uint16_t addr;
	uint8_t *str;
	bool rx_on; // true:rxEnable, false:rxDisable
	uint32_t tx_time; // last timestamp of tx ok
	uint8_t fail; // to count up tx fail for triggering
	uint8_t retry; // to count up retry for waiting
	uint32_t backoff_time; // timestamp which back-off send is permitted
	bool set_backoff_time; // request to calculate back-off interval
} TX_PARAM;

/* --------------------------------------------------------------------------------
 * Prototype
 * -------------------------------------------------------------------------------- */
static SUBGHZ_MSG func_trxOrTxOnly(TX_PARAM *ptx);
static MAIN_IOT_STATE func_trigActivate(void);
static MAIN_IOT_STATE func_waitActivate(void);
static MAIN_IOT_STATE func_initSensor(void);
static MAIN_IOT_STATE func_sendQueue(void);
static MAIN_IOT_STATE func_trigReconnect(void);
static MAIN_IOT_STATE func_waitReconnect(void);
static MAIN_IOT_STATE func_trigUpdParam(void);
static MAIN_IOT_STATE func_waitUpdParam(void);
static MAIN_IOT_STATE func_trigFwUpd(void);
static MAIN_IOT_STATE func_waitFwUpd(void);

/* --------------------------------------------------------------------------------
 * Global variable
 * -------------------------------------------------------------------------------- */
bool waitEventFlag = false;
bool useInterruptFlag = false;
static uint8_t rx_buf[MAX_BUF_SIZE];
static uint8_t tx_buf[MAX_BUF_SIZE];
static TX_PARAM tx_param = {0xffff,0xffff,"",false,0,0,0,0,false};
MAIN_IOT_PARAM mip = {STATE_TRIG_ACTIVATE,0,false,0,DEFAULT_SLEEP_INTERVAL,NO_SLEEP,0xffff,0xffff,0xffff};

static MAIN_IOT_STATE (*functions[])(void) = {
	func_trigActivate,
	func_waitActivate,
	func_initSensor,
	func_sendQueue,
	func_trigReconnect,
	func_waitReconnect,
	func_trigUpdParam,
	func_waitUpdParam,
	func_trigFwUpd,
	func_waitFwUpd
};

OTA_PARAM ota_param = {
	0,						// hw type
	0,						// version
	"",		// must match with file name
	SUBGHZ_CH,
	0xffff,
	SUBGHZ_100KBPS,
	SUBGHZ_PWR_20MW,
	0xffff
};

const uint8_t vls_val[][8] = {
	"0",		//	"invalid",
	"0",		//	"invalid",
	"1.8",		//	"< 1.898V"
	"1.95",		//	"1.898 ~ 2.000V"
	"2.05",		//	"2.000 ~ 2.093V"
	"2.15",		//	"2.093 ~ 2.196V",
	"2.25",		//	"2.196 ~ 2.309V",
	"2.35",		//	"2.309 ~ 2.409V",
	"2.5",		//	"2.409 ~ 2.605V",
	"2.7",		//	"2.605 ~ 2.800V",
	"2.9",		//	"2.800 ~ 3.068V",
	"3.2",		//	"3.068 ~ 3.394V",
	"3.6",		//	"3.394 ~ 3.797V",
	"4.0",		//	"3.797 ~ 4.226V",
	"4.4",		//	"4.226 ~ 4.667V",
	"4.7"		//	"> 4.667V"
};

/* --------------------------------------------------------------------------------
 * Queue related functions
 * -------------------------------------------------------------------------------- */

#define MAX_QUEUE_LEN		( 32 )
#define QUEUE_ERR_EMPTY		( -1 )
#define QUEUE_ERR_FULL		( -2 )
#define QUEUE_ERR_PARAM		( -3 )

typedef struct {
	union  {
		int8_t int8_val;
		uint8_t uint8_val;
		int16_t int16_val;
		uint16_t uint16_val;
		int32_t int32_val;
		uint32_t uint32_val;
		float float_val;
		double double_val;
	} data;
	uint8_t type;
	uint8_t digit;
	int id;
	uint8_t state;
	uint8_t vls_level;
	int reason;
	uint32_t time;
} QUEUE_DATA;

#define QUEUE_DATA_SIZE		( sizeof(QUEUE_DATA) )

struct queue_t {
	QUEUE_DATA data[MAX_QUEUE_LEN];
	int head; // current index for reading queue
	int tail; // current index for writing queue
} queue;

#define queue_next(n) ( (n+1) % MAX_QUEUE_LEN ) // returns the next index of head or tail
#define queue_length() ( queue.tail < queue.head ? MAX_QUEUE_LEN + queue.tail - queue.head : queue.tail - queue.head )

static void queue_init() {
	queue.head = 0;
	queue.tail = 0;
	return;
}

/*
 * queue_write - write data to queue
 *   input: address of QUEUE_DATA
 *   output: 0 - success, or QUEUE_ERR_FULL
 */
static int queue_write(QUEUE_DATA *buf) {
	int next_tail;

	next_tail = queue_next(queue.tail);
	// queue is full if next index of tail equals current head
	if (next_tail == queue.head) return QUEUE_ERR_FULL;
	// write data
	queue.data[queue.tail] = *buf;
	// update tail
	queue.tail = next_tail;
#ifdef DEBUG
	Serial.print("head: ");
	Serial.print_long((long)queue.head,DEC);
	Serial.print(",tail: ");
	Serial.print_long((long)queue.tail,DEC);
	Serial.print(",len: ");
	Serial.println_long((long)queue_length(),DEC);
#endif
	return 0;
}

/*
 * queue_read - read data from the specified index of head
 *   input: index of head,
 *          address of QUEUE_DATA
 *   output: 0 - success, or QUEUE_ERR_EMPTY, QUEUE_ERR_PARAM
 */
static int queue_peek(int index, QUEUE_DATA **buf) {
	// queue is empty if current head equals current head
	if (queue.head == queue.tail) return QUEUE_ERR_EMPTY;
	// index is out of range
	if (queue.head <= queue.tail) {
		if ((index < queue.head) || (queue.tail <= index)) {
			return QUEUE_ERR_PARAM;
		}
	} else {
		if ((queue.tail <= index) && (index < queue.head)) {
			return QUEUE_ERR_PARAM;
		}
	}
	// address data
	*buf = &queue.data[index];
	return 0;
}

/*
 * queue_dequeue - release specified number of queue
 *   input: number of queue (> 0)
 *   output: 0 - success, or QUEUE_ERR_EMPTY, QUEUE_ERR_PARAM
 */
static int queue_dequeue(uint8_t num) {
	// queue is empty if current head equals current head
	if (queue.head == queue.tail) return QUEUE_ERR_EMPTY;
	// num is zero or more than queue length
	if ((num == 0) || (num > queue_length())) return QUEUE_ERR_PARAM;
	// update head
	queue.head = queue_next(queue.head+num-1);
#ifdef DEBUG
	Serial.print("head: ");
	Serial.print_long((long)queue.head,DEC);
	Serial.print(",tail: ");
	Serial.print_long((long)queue.tail,DEC);
	Serial.print(",len: ");
	Serial.println_long((long)queue_length(),DEC);
#endif
	return 0;
}

/* --------------------------------------------------------------------------------
 * Sensor related task
 * -------------------------------------------------------------------------------- */

#define MAX_INTERVAL			( 3600*1000ul )
#define KEEP_ALIVE_INTERVAL		( 1800*1000ul )
#define SENSOR_TYPE_V1			( 1 )
#define SENSOR_TYPE_V2			( 2 )
#define STATE_TO_OFF			( 0 )
#define STATE_TO_ON				( 1 )
#define EACK_NORMAL				( 0x00 )
#define EACK_FORCIBLE_FLAG		( 0x01 )
#define EACK_UPD_PARAM			( 0x02 )
#define EACK_ACTIVATION			( 0x03 )
#define EACK_FW_UPD				( 0xF0 )
#define EACK_ERR_FLAG			( 0x01 )
#define EACK_ERR_INTERVAL		( 0x02 )
#define EACK_ERR_SIZE			( 0x04 )
#define PARSE_ERR_UNDEF_FORMAT ( -1 )
#define PARSE_ERR_UNDEF_HEADER ( -2 )
#define PAYLOAD_HEADER_SIZE ( 3 )
#define PAYLOAD_PARAM_SIZE ( 5 )
#define PAYLOAD_SINGLE_LEN ( PAYLOAD_HEADER_SIZE + PAYLOAD_PARAM_SIZE )

__packed typedef struct {
	uint8_t eack_flag;
	uint16_t sleep_interval_sec;	// unit sec
} EACK_DATA;

static void SensorState_construct(void);
static void SensorState_initState(void);
static void SensorState_offStable(SensorState* p_this);
static void SensorState_offUnstable(SensorState* p_this);
static void SensorState_onStable(SensorState* p_this);
static void SensorState_onUnstable(SensorState* p_this);
static void SensorState_validateNextState(SensorState* p_this);

SensorState Sensor[MAX_SENSOR_NUM];

static double sensor_getDoubleData(SENSOR_VAL *val) {
	switch(val->type) {
		case INT8_VAL:
			return val->data.int8_val;
			break;
		case UINT8_VAL:
			return val->data.uint8_val;
			break;
		case INT16_VAL:
			return val->data.int16_val;
			break;
		case UINT16_VAL:
			return val->data.uint16_val;
			break;
		case INT32_VAL:
			return val->data.int32_val;
			break;
		case UINT32_VAL:
			return val->data.uint32_val;
			break;
		case FLOAT_VAL:
			return val->data.float_val;
			break;
		case DOUBLE_VAL:
			return val->data.double_val;
			break;
		default:
			break;
	}
	return 0;
}

/*
 * sensor_parsePayload - parse payload data
 *   input: address of mac.payload
 *   output: 1 - success, threshold parameters is changed (must be initialized)
 *           0 - success, threshold parameters is not changed
 *         < 0 - error
 */
static int sensor_parsePayload(uint8_t *payload) {
	int i,num;
	uint8_t *p[PAYLOAD_HEADER_SIZE+PAYLOAD_PARAM_SIZE*MAX_SENSOR_NUM+1];
	double thrs_on_val,thrs_off_val;
	uint32_t thrs_on_interval,thrs_off_interval;
	SensorState *ssp;
	bool changed=false;
	int ret=0;

	for (i=0;;i++) {
		if (i == 0) {
			p[i] = strtok(payload, ",");
		} else {
			p[i] = strtok(NULL, ",");
		}
		if (p[i] == NULL) break;
	}
	num = (i - PAYLOAD_HEADER_SIZE) / PAYLOAD_PARAM_SIZE;

	// payload(single sensor) :
	// "'activate' or 'debug',(gw_panid),(gw_shortaddr),
	//  (id),(thrs_on_val),(thrs_on_interval[sec]),(thrs_off_val),(thrs_off_interval[sec]),
	//
	// payload(multi sensor) :
	// "'activate' or 'debug',(gw_panid),(gw_shortaddr),
	//  (id),(thrs_on_val),(thrs_on_interval[sec]),(thrs_off_val),(thrs_off_interval[sec]),
	//   ...
	//  (id),(thrs_on_val),(thrs_on_interval[sec]),(thrs_off_val),(thrs_off_interval[sec])"

	if (i == PAYLOAD_SINGLE_LEN) {
		BREAK("single sensor");
		mip.sensor_type = SENSOR_TYPE_V1;
	} else if (((i - PAYLOAD_HEADER_SIZE) % PAYLOAD_PARAM_SIZE == 0) && (num <= MAX_SENSOR_NUM)) {
		BREAKL("multi sensor: ",(long)num,DEC);
		mip.sensor_type = SENSOR_TYPE_V2;
	} else {
		ret = PARSE_ERR_UNDEF_FORMAT; // undefined payload format
	}
	if (strncmp(p[0],"activate",8) == 0) {
		mip.gateway_panid = (uint16_t)strtoul(p[1],NULL,0);
		mip.gateway_addr = (uint16_t)strtoul(p[2],NULL,0);
		mip.my_short_addr = (uint16_t)strtoul(p[3],NULL,0);
		for (i=0; i<num; i++) {
			ssp = &Sensor[i];
			ssp->id = (uint16_t)strtoul(p[3+i*PAYLOAD_PARAM_SIZE],NULL,0);
			thrs_on_val = strtod(p[4+i*PAYLOAD_PARAM_SIZE],NULL);
			thrs_on_interval = strtoul(p[5+i*PAYLOAD_PARAM_SIZE],NULL,0) * 1000ul;
			thrs_off_val = strtod(p[6+i*PAYLOAD_PARAM_SIZE],NULL);
			thrs_off_interval = strtoul(p[7+i*PAYLOAD_PARAM_SIZE],NULL,0) * 1000ul;
			if (ssp->thrs_on_val != thrs_on_val) changed = true;
			if (ssp->thrs_on_interval != thrs_on_interval) changed = true;
			if (ssp->thrs_off_val != thrs_off_val) changed = true;
			if (ssp->thrs_off_interval != thrs_off_interval) changed = true;
			ssp->thrs_on_val = thrs_on_val;
			ssp->thrs_on_interval = thrs_on_interval;
			ssp->thrs_off_val = thrs_off_val;
			ssp->thrs_off_interval = thrs_off_interval;
		}
		BREAK(p[0]);
		BREAKL("panid: ",(long)mip.gateway_panid,HEX);
		BREAKL("addr: ",(long)mip.gateway_addr,HEX);
		BREAKL("my_short_addr: ",(long)mip.my_short_addr,HEX);
		for (i=0; i<MAX_SENSOR_NUM; i++) {
			ssp = &Sensor[i];
			BREAKL("id: ",(long)ssp->id,DEC);
			BREAKD("thrs_on_val: ",ssp->thrs_on_val,2);
			BREAKL("thrs_on_interval: ",(long)ssp->thrs_on_interval,DEC);
			BREAKD("thrs_off_val: ",ssp->thrs_off_val,2);
			BREAKL("thrs_off_interval: ",(long)ssp->thrs_off_interval,DEC);
		}
	} else {
		ret = PARSE_ERR_UNDEF_HEADER;	// string pattern unmatched
	}
#ifdef DEBUG
	delay(1000); // wait for message dump
#endif
	if (changed == true) ret = 1;
	return ret;
}

static int sensor_saveQueue(SensorState *p_this) {
	QUEUE_DATA buf;

	if ((p_this->next_state == SENSOR_STATE_ON_STABLE) || (p_this->next_state == SENSOR_STATE_ON_UNSTABLE)) {
		buf.state = STATE_TO_ON;
	} else {
		buf.state = STATE_TO_OFF;
	}
	buf.vls_level = p_this->vls_level;
	buf.time = mip.last_sense_time;
	buf.id = p_this->id;
	buf.reason = p_this->reason;
	switch(p_this->sensor_val.type) {
		case INT8_VAL:
			buf.data.int8_val = p_this->sensor_val.data.int8_val;
			break;
		case UINT8_VAL:
			buf.data.uint8_val = p_this->sensor_val.data.uint8_val;
			break;
		case INT16_VAL:
			buf.data.int16_val = p_this->sensor_val.data.int16_val;
			break;
		case UINT16_VAL:
			buf.data.uint16_val = p_this->sensor_val.data.uint16_val;
			break;
		case INT32_VAL:
			buf.data.int32_val = p_this->sensor_val.data.int32_val;
			break;
		case UINT32_VAL:
			buf.data.uint32_val = p_this->sensor_val.data.uint32_val;
			break;
		case FLOAT_VAL:
			buf.data.float_val = p_this->sensor_val.data.float_val;
			break;
		case DOUBLE_VAL:
			buf.data.double_val = p_this->sensor_val.data.double_val;
			break;
		default:
			break;
	}
	buf.type = p_this->sensor_val.type;
	buf.digit = p_this->sensor_val.digit;
	return queue_write(&buf);
}

static void sensor_operJudge(SensorState *p_this) {
	switch(p_this->sensor_val.type) {
		case INT8_VAL:
			p_this->sensor_comp_val = p_this->sensor_val.data.int8_val;
			break;
		case UINT8_VAL:
			p_this->sensor_comp_val = p_this->sensor_val.data.uint8_val;
			break;
		case INT16_VAL:
			p_this->sensor_comp_val = p_this->sensor_val.data.int16_val;
			break;
		case UINT16_VAL:
			p_this->sensor_comp_val = p_this->sensor_val.data.uint16_val;
			break;
		case INT32_VAL:
			p_this->sensor_comp_val = p_this->sensor_val.data.int32_val;
			break;
		case UINT32_VAL:
			p_this->sensor_comp_val = p_this->sensor_val.data.uint32_val;
			break;
		case FLOAT_VAL:
			p_this->sensor_comp_val = p_this->sensor_val.data.float_val;
			break;
		case DOUBLE_VAL:
			p_this->sensor_comp_val = p_this->sensor_val.data.double_val;
			break;
		default:
			break;
	}
#if 0//#ifdef DEBUG
	Serial.print("id: ");
	Serial.print_long((long)p_this->id,DEC);
	Serial.print(", state: ");
	Serial.print_long((long)p_this->next_state,DEC);
	Serial.print(" -> ");
#endif
	SensorState_validateNextState(p_this);
#if 0//#ifdef DEBUG
	Serial.println_long((long)p_this->next_state,DEC);
#endif
}

static uint32_t sensor_checkEack(MAIN_IOT_STATE *mode) {
	EACK_DATA *eack_data;
	int eack_size,i,eack_error=0;
	uint32_t tmp,sleep_time=NO_SLEEP;
	uint8_t *p;

	SubGHz.getEnhanceAck(&p,&eack_size);
	eack_data = (EACK_DATA *)p;
	/* no problem : Warning : W4027 : Cast operation may lead to odd boundary access */
	BREAKL("eack_size: ",(long)eack_size,DEC);
	if (eack_size == sizeof(EACK_DATA)) {
#ifdef DEBUG
		Serial.print("eack_data: ");
		for (i=0; i<sizeof(EACK_DATA); i++,p++) {
			Serial.print_long((long)*p,HEX);
			Serial.print(" ");
		}
		Serial.println("");
#endif
		switch (eack_data->eack_flag) {
			case EACK_NORMAL:
				break;
			case EACK_FORCIBLE_FLAG:
				for (i=0; i<MAX_SENSOR_NUM; i++) Sensor[i].save_request = true;
				break;
			case EACK_UPD_PARAM:
				*mode = STATE_TRIG_UPD_PARAM;
				break;
			case EACK_ACTIVATION:
				*mode = STATE_TRIG_ACTIVATE;
				break;
			case EACK_FW_UPD:
				*mode = STATE_TRIG_FW_UPD;
				break;
			default:
				eack_error |= EACK_ERR_FLAG;
				break;
		}
		tmp = eack_data->sleep_interval_sec * 1000ul;
		if (tmp > MAX_INTERVAL) {
			eack_error |= EACK_ERR_INTERVAL;
		} else if (tmp > KEEP_ALIVE_INTERVAL) {
			sleep_time = KEEP_ALIVE_INTERVAL;
		} else {
			sleep_time = tmp;
		}
	} else {
		eack_error |= EACK_ERR_SIZE;
	}
	if (eack_error) {
		Print.init(tx_buf,sizeof(tx_buf));
		Print.p("error, invalid EACK");
		if (eack_error & EACK_ERR_FLAG) {
			Print.p(" [flag]");
		}
		if (eack_error & EACK_ERR_INTERVAL) {
			Print.p(" [interval]");
		}
		if (eack_error & EACK_ERR_SIZE) {
			Print.p(" [size]");
		}
		BREAK(tx_buf);
		tx_param.panid = mip.gateway_panid;
		tx_param.addr = mip.gateway_addr;
		tx_param.str = tx_buf;
		tx_param.rx_on = false;
		func_trxOrTxOnly(&tx_param);
		*mode = STATE_TRIG_ACTIVATE;
	}
	return sleep_time;
}

static uint8_t sensor_restoreQueue(void) {
	QUEUE_DATA *buf;
	uint32_t now,diff;
	uint8_t tmp[100],n;
	uint16_t head_local;
	int len;

	if (mip.sensor_type == SENSOR_TYPE_V1) {
		Print.init(tx_buf,sizeof(tx_buf));
		len = 1;
	} else if (mip.sensor_type == SENSOR_TYPE_V2) {
		Print.init(tx_buf,sizeof(tx_buf)); // clear memory
		Print.init(tmp,sizeof(tmp));
		len = queue_length();
		Print.p("v2");
	} else {
		Print.init(tx_buf,sizeof(tx_buf));
		Print.p("unknown sensor type");
		return 0;
	}
	now = millis();
	head_local = queue.head;
	for (n=0; n<len; n++) {
		queue_peek(head_local,&buf);
		if (mip.sensor_type == SENSOR_TYPE_V2) { // for SENSOR_TYPE_V2
			Print.p(",");
			Print.l((long)buf->id,DEC);
			Print.p(",");
		}
		buf->state == STATE_TO_ON ? Print.p("on,") : Print.p("off,");
		switch(buf->type) {
			case INT8_VAL:
				Print.l((long)buf->data.int8_val,DEC);
				break;
			case UINT8_VAL:
				Print.l((long)buf->data.uint8_val,DEC);
				break;
			case INT16_VAL:
				Print.l((long)buf->data.int16_val,DEC);
				break;
			case UINT16_VAL:
				Print.l((long)buf->data.uint16_val,DEC);
				break;
			case INT32_VAL:
				Print.l((long)buf->data.int32_val,DEC);
				break;
			case UINT32_VAL:
				Print.l((long)buf->data.uint32_val,DEC);
				break;
			case FLOAT_VAL:
				Print.d((double)buf->data.float_val,buf->digit);
				break;
			case DOUBLE_VAL:
				Print.d(buf->data.double_val,buf->digit);
				break;
			default:
				break;
		}
		Print.p(",");
		Print.p(vls_val[buf->vls_level]);
		Print.p(",");
		if ((buf->state == STATE_TO_OFF) && (buf->reason != INVALID_REASON)) {
			Print.l((long)buf->reason,DEC);
		}
		diff = now - buf->time;
		Print.p(",");
		if (diff > 10) {
			Print.l(diff,DEC);
		}
		if (mip.sensor_type == SENSOR_TYPE_V2) { // for SENSOR_TYPE_V2
			if ((strlen(tx_buf)+Print.len()) <= MAX_BUF_SIZE) {
				strncat(tx_buf,tmp,Print.len());
				Print.init(tmp,sizeof(tmp)); // clear memory
				head_local = queue_next(head_local);
			} else {
				break;
			}
		}
	}
	return n;
}

static void sensor_main(void) {
	SensorState *ssp;
	int i;

	sensor_meas(Sensor);
	mip.last_sense_time = millis();
	for (i=0; i<MAX_SENSOR_NUM; i++) {
		ssp = &Sensor[i];
		if (ssp->id != INVALID_ID) {
			// operation change judgement
			sensor_operJudge(ssp);
			// keep alive condition judgement
			if (mip.last_sense_time - ssp->last_save_time >= KEEP_ALIVE_INTERVAL) {
				BREAK("keep alive");
				ssp->save_request = true;
			}
			// save to queue
			if (ssp->save_request) {
				ssp->save_request = false;
				ssp->vls_level = voltage_check(VLS_3_068);
				if (sensor_saveQueue(ssp) != 0) {
					BREAK("saveQueue error");
				} else {
					ssp->last_save_time = mip.last_sense_time;
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
 * Sensor state machine
 * -------------------------------------------------------------------------------- */

static void SensorState_construct(void) {
	int i;
	SensorState *ssp;

	for (i=0; i<MAX_SENSOR_NUM; i++) {
		ssp = &Sensor[i];
		ssp->id = INVALID_ID;
		ssp->thrs_on_val = 0.1;
		ssp->thrs_off_val = 0.1;
		ssp->thrs_on_interval = 0;
		ssp->thrs_off_interval = 0;
		ssp->thrs_on_start = 0;
		ssp->thrs_off_start = 0;
		ssp->reason = INVALID_REASON;
		ssp->next_state = SENSOR_STATE_OFF_STABLE;
		ssp->vls_level = 0;
		ssp->last_save_time = 0;
		ssp->save_request = false;
	}
}

static void SensorState_initState(void) {
	SensorState *ssp;
	int i;

	sensor_meas(Sensor);
	mip.last_sense_time = millis();
	for (i=0; i<MAX_SENSOR_NUM; i++) {
		ssp = &Sensor[i];
		if (ssp->id != INVALID_ID) {
			ssp->sensor_comp_val = sensor_getDoubleData(&ssp->sensor_val);
			ssp->thrs_on_start = 0;
			ssp->thrs_off_start = 0;
			ssp->vls_level = voltage_check(VLS_3_068);
			ssp->last_save_time = mip.last_sense_time;
			if (ssp->sensor_comp_val >= ssp->thrs_off_val) {
				ssp->next_state = SENSOR_STATE_ON_STABLE;
			} else {
				ssp->next_state = SENSOR_STATE_OFF_STABLE;
			}
			sensor_saveQueue(ssp);
		}
	}
}

static void SensorState_validateNextState(SensorState* p_this) {
	switch (p_this->next_state) {
		case SENSOR_STATE_OFF_STABLE:
			SensorState_offStable(p_this);
			break;
		case SENSOR_STATE_OFF_UNSTABLE:
			SensorState_offUnstable(p_this);
			break;
		case SENSOR_STATE_ON_STABLE:
			SensorState_onStable(p_this);
			break;
		case SENSOR_STATE_ON_UNSTABLE:
			SensorState_onUnstable(p_this);
			break;
		default:
			break;
	}
}

static void SensorState_offStable(SensorState* p_this) {
	p_this->next_state = SENSOR_STATE_OFF_STABLE;

	if (p_this->sensor_comp_val > p_this->thrs_on_val) {
		if (p_this->thrs_on_interval != 0) {
			p_this->thrs_on_start = mip.last_sense_time;
			p_this->next_state = SENSOR_STATE_OFF_UNSTABLE;
		} else {
			p_this->save_request = true;
			p_this->next_state = SENSOR_STATE_ON_STABLE;
		}
	}
}

static void SensorState_offUnstable(SensorState* p_this) {
	p_this->next_state = SENSOR_STATE_OFF_UNSTABLE;

	if (p_this->sensor_comp_val <= p_this->thrs_on_val) {
		p_this->next_state = SENSOR_STATE_OFF_STABLE;
	} else {
		if ((mip.last_sense_time - p_this->thrs_on_start) > p_this->thrs_on_interval) {
			p_this->save_request = true;
			p_this->next_state = SENSOR_STATE_ON_STABLE;
		}
	}
}

static void SensorState_onStable(SensorState* p_this) {
	p_this->next_state = SENSOR_STATE_ON_STABLE;

	if (p_this->sensor_comp_val < p_this->thrs_off_val) {
		if (p_this->thrs_off_interval != 0) {
			p_this->thrs_off_start = mip.last_sense_time;
			p_this->next_state = SENSOR_STATE_ON_UNSTABLE;
		} else {
			p_this->save_request = true;
			p_this->next_state = SENSOR_STATE_OFF_STABLE;
		}
	}
}

static void SensorState_onUnstable(SensorState* p_this) {
	p_this->next_state = SENSOR_STATE_ON_UNSTABLE;

	if (p_this->sensor_comp_val >= p_this->thrs_off_val) {
		p_this->next_state = SENSOR_STATE_ON_STABLE;
	} else {
		if ((mip.last_sense_time - p_this->thrs_off_start) > p_this->thrs_off_interval) {
			p_this->save_request = true;
			p_this->next_state = SENSOR_STATE_OFF_STABLE;
		}
	}
}

/* --------------------------------------------------------------------------------
 * State control task
 * -------------------------------------------------------------------------------- */
#define RX_INTERVAL ( 2*1000ul )
#define MAX_TX_COUNT ( 1 )
#define TX_INTERVAL ( rand()&1000 )
#define RETRY_INTERVAL ( 10*1000ul+ (rand()&500) )
#define MAX_ACTIVATE_RETRY ( 14 )
#define ACTIVATE_RETRY_INTERVAL ( 1800*1000ul )
#define MAX_UPD_PARAM_RETRY ( 3 )
#define MAX_BACKOFF_COUNT ( 8 )
#define MIN_BACKOFF_INTERVAL ( 1000ul ) // actual max interval is 2^8*1000=256 s

static uint8_t activate_str[50];
static uint8_t update_str[] = "update";
const uint16_t pow_arr[MAX_BACKOFF_COUNT+1] = {1,2,4,8,16,32,64,128,256};

static SUBGHZ_MSG func_trxOrTxOnly(TX_PARAM *ptx) {
	SUBGHZ_MSG msg;

	SubGHz.begin(SUBGHZ_CH,ptx->panid,SUBGHZ_100KBPS,SUBGHZ_PWR_20MW);
	if (ptx->rx_on == true) SubGHz.rxEnable(NULL);
	else SubGHz.rxDisable();
#ifdef USE_LED
	digitalWrite(BLUE_LED,LOW);
#endif
	msg = SubGHz.send(ptx->panid,ptx->addr,ptx->str,strlen(ptx->str),NULL);
#ifdef USE_LED
	digitalWrite(BLUE_LED,HIGH);
#endif
	if (ptx->rx_on != true) SubGHz.close();
	ptx->tx_time = millis();
	return msg;
}

static MAIN_IOT_STATE func_trigActivate(void) {
	MAIN_IOT_STATE mode = STATE_TRIG_ACTIVATE;
	SUBGHZ_MSG msg;

	tx_param.panid = 0xffff;
	tx_param.addr = 0xffff;
	tx_param.str = activate_str;
	tx_param.rx_on = true;
	msg = func_trxOrTxOnly(&tx_param);
	if (msg == SUBGHZ_OK) {
		mode = STATE_WAIT_ACTIVATE;
		BREAK("waiting...");
		tx_param.fail = 0;
		mip.sleep_time = NO_SLEEP;
	} else {
		SubGHz.close();
		BREAK("tx fail");
		if (tx_param.fail >= MAX_TX_COUNT) {
			tx_param.fail = 0;
			mip.sleep_time = DEFAULT_SLEEP_INTERVAL;
		} else {
			tx_param.fail++;
			mip.sleep_time = TX_INTERVAL;
		}
	}
	return mode;
}

static MAIN_IOT_STATE func_waitActivate(void) {
	MAIN_IOT_STATE mode = STATE_WAIT_ACTIVATE;
	SUBGHZ_MAC_PARAM mac;
	int rx_len;

	rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
	if (rx_len > 0) { // receive
		rx_buf[rx_len] = 0;
		SubGHz.decMac(&mac,rx_buf,rx_len);
		if (sensor_parsePayload(mac.payload) >= 0) { // parse ok
			mode = STATE_INIT_SENSOR;
			if (mip.my_short_addr != 0xffff) SubGHz.setMyAddress(mip.my_short_addr);
			if (sensor_activate() == true) {
				mip.sleep_time = DEFAULT_SLEEP_INTERVAL;
			} else {
				mip.sleep_time = NO_SLEEP;
			}
			tx_param.retry = 0; // clear
		}
	} else if (millis() - tx_param.tx_time > RX_INTERVAL) { // timeout
		mode = STATE_TRIG_ACTIVATE;
		SubGHz.close();
		tx_param.retry++;
		BREAKL("tx_param.retry: ",(long)tx_param.retry,DEC);
		if (tx_param.retry <= MAX_ACTIVATE_RETRY) {
			mip.sleep_time = RETRY_INTERVAL;
		} else {
			tx_param.retry = 0; // clear
			mip.sleep_time = ACTIVATE_RETRY_INTERVAL;
		}
	}
	return mode;
}

static MAIN_IOT_STATE func_initSensor(void) {
	SensorState_initState();
	mip.enable_sense = true;
	mip.sleep_time = NO_SLEEP;
	return STATE_SEND_QUEUE;
}

static MAIN_IOT_STATE func_sendQueue(void) {
	MAIN_IOT_STATE mode = STATE_SEND_QUEUE;
	SUBGHZ_MSG msg;
	uint8_t num=0;

	if (queue_length() == 0) {
		mip.sleep_time = mip.sense_interval;
	} else {
		num = sensor_restoreQueue();
		BREAK(tx_buf);
		tx_param.panid = mip.gateway_panid;
		tx_param.addr = mip.gateway_addr;
		tx_param.str = tx_buf;
		tx_param.rx_on = false;
		msg = func_trxOrTxOnly(&tx_param);
		if (msg == SUBGHZ_OK) {
			mip.sense_interval = sensor_checkEack(&mode);
			if (mode != STATE_SEND_QUEUE) {
				sensor_deactivate();
				mip.enable_sense = false;
			}
			queue_dequeue(num);
		} else { // fail
			mode = STATE_TRIG_RECONNECT;
			tx_param.set_backoff_time = true;
		}
		mip.sleep_time = NO_SLEEP;
	}
	return mode;
}

/*
 * compareTimestamp - comparer between base_time and target_time
 *   input: base_time - base timestamp
 *          target_time - target timestamp to be compared
 *  output: true - base_time < target_time
 *          false - base_time >= target_time
 */
static bool compareTimestamp(uint32_t base_time, uint32_t target_time) {
	bool ret;
	if (base_time >= target_time) {
		if (base_time - target_time > ULONG_MAX/2) ret = true;
		else ret = false;
	} else {
		if (target_time - base_time > ULONG_MAX/2) ret = false;
		else ret = true;
	}
	return ret;
}

static MAIN_IOT_STATE func_trigReconnect(void) {
	MAIN_IOT_STATE mode = STATE_TRIG_RECONNECT;
	SUBGHZ_MSG msg;
	static uint32_t backoff_interval=0;
	uint32_t now=millis(),sense_time;
	double tmp;

	sense_time = mip.last_sense_time + mip.sense_interval;
	// set backoff timestamp to reconnect
	if (tx_param.set_backoff_time == true) {
		tx_param.set_backoff_time = false;
		/* tmp = pow(2.0,(double)tx_param.retry) * MIN_BACKOFF_INTERVAL; */
		tmp = pow_arr[tx_param.retry] * MIN_BACKOFF_INTERVAL;
		tmp += tmp * rand() / ((double)RAND_MAX * 20); // +0~5%
		backoff_interval = (uint32_t)tmp;
		tx_param.backoff_time = now + backoff_interval;
		BREAKL("backoff_interval: ",backoff_interval,DEC);
	}
	if (compareTimestamp(tx_param.backoff_time,sense_time)) { // tx_param.backoff_time < sense_time
		if (compareTimestamp(tx_param.backoff_time,now)) { // tx_param.backoff_time < now
			mip.sleep_time = NO_SLEEP;
		} else { // tx_param.backoff_time >= now
			mip.sleep_time = tx_param.backoff_time - now;
		}
	} else { // tx_param.backoff_time >= sense_time
		if (compareTimestamp(sense_time,now)) { // sense_time < now
			mip.sleep_time = NO_SLEEP;
		} else { // sense_time >= now
			mip.sleep_time = sense_time - now;
		}
	}
	// try to reconnect if the backoff time has been gone over
	if (compareTimestamp(tx_param.backoff_time,now)) {
		tx_param.panid = 0xffff;
		tx_param.addr = 0xffff;
		tx_param.str = activate_str;
		tx_param.rx_on = true;
		msg = func_trxOrTxOnly(&tx_param);
		if (msg == SUBGHZ_OK) {
			mode = STATE_WAIT_RECONNECT;
			BREAK("waiting...");
			mip.sleep_time = NO_SLEEP;
		} else { // fail
			tx_param.retry++;
			tx_param.set_backoff_time = true;
		}
	}
	return mode;
}

static MAIN_IOT_STATE func_waitReconnect(void) {
	MAIN_IOT_STATE mode = STATE_WAIT_RECONNECT;
	SUBGHZ_MAC_PARAM mac;
	int rx_len,ret;

	rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
	if (rx_len > 0) { // receive
		rx_buf[rx_len] = 0;
		SubGHz.decMac(&mac,rx_buf,rx_len);
		ret = sensor_parsePayload(mac.payload);
		if (ret < 0) {
			BREAKL("parse error: ",(long)ret,DEC);
		} else {
			if (ret == 1) mode = STATE_INIT_SENSOR;
			else mode = STATE_SEND_QUEUE;
			if (mip.my_short_addr != 0xffff) SubGHz.setMyAddress(mip.my_short_addr);
			tx_param.retry = 0; // clear
			tx_param.backoff_time = 0; // clear
		}
	} else if (millis() - tx_param.tx_time > RX_INTERVAL) { // timeout
		BREAK("timeout");
		mode = STATE_TRIG_RECONNECT;
		SubGHz.close();
		tx_param.set_backoff_time = true;
		if (tx_param.retry < MAX_BACKOFF_COUNT) {
			tx_param.retry++;
		} else {
			tx_param.retry = MAX_BACKOFF_COUNT;
		}
	}
	return mode;
}

static MAIN_IOT_STATE func_trigUpdParam(void) {
	MAIN_IOT_STATE mode = STATE_TRIG_UPD_PARAM;
	SUBGHZ_MSG msg;

	tx_param.panid = mip.gateway_panid;
	tx_param.addr = mip.gateway_addr;
	tx_param.str = update_str;
	tx_param.rx_on = true;
	msg = func_trxOrTxOnly(&tx_param);
	if (msg == SUBGHZ_OK) {
		mode = STATE_WAIT_UPD_PARAM;
		BREAK("waiting...");
		tx_param.fail = 0;
		mip.sleep_time = NO_SLEEP;
	} else {
		SubGHz.close();
		BREAK("tx fail");
		if (tx_param.fail >= MAX_TX_COUNT) {
			tx_param.fail = 0;
			mip.sleep_time = DEFAULT_SLEEP_INTERVAL;
		} else {
			tx_param.fail++;
			mip.sleep_time = TX_INTERVAL;
		}
	}
	return mode;
}

static MAIN_IOT_STATE func_waitUpdParam(void) {
	MAIN_IOT_STATE mode = STATE_WAIT_UPD_PARAM;
	SUBGHZ_MAC_PARAM mac;
	int rx_len;

	rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
	if (rx_len > 0) { // receive
		rx_buf[rx_len] = 0;
		SubGHz.decMac(&mac,rx_buf,rx_len);
		if (sensor_parsePayload(mac.payload) >= 0) { // parse ok
			mode = STATE_INIT_SENSOR;
			if (mip.my_short_addr != 0xffff) SubGHz.setMyAddress(mip.my_short_addr);
			if (sensor_activate() == true) {
				mip.sleep_time = DEFAULT_SLEEP_INTERVAL;
			} else {
				mip.sleep_time = NO_SLEEP;
			}
			tx_param.retry = 0; // clear
		}
	} else if (millis() - tx_param.tx_time > RX_INTERVAL) { // timeout
		SubGHz.close();
		tx_param.retry++;
		BREAKL("tx_param.retry: ",(long)tx_param.retry,DEC);
		if (tx_param.retry <= MAX_UPD_PARAM_RETRY) {
			mode = STATE_TRIG_UPD_PARAM;
			mip.sleep_time = RETRY_INTERVAL;
		} else {
			mode = STATE_TRIG_ACTIVATE;
			tx_param.retry = 0; // clear
			sensor_deactivate();
			mip.enable_sense = false;
			mip.sleep_time = NO_SLEEP;
		}
	}
	return mode;
}

static MAIN_IOT_STATE func_trigFwUpd(void) {
	MAIN_IOT_STATE mode = STATE_TRIG_FW_UPD;
	// TBD
	return mode;
}

static MAIN_IOT_STATE func_waitFwUpd(void) {
	MAIN_IOT_STATE mode = STATE_WAIT_FW_UPD;
	// TBD
	return mode;
}

/* --------------------------------------------------------------------------------
 * Setup and loop
 * -------------------------------------------------------------------------------- */

void setup() {
	// put your setup code here, to run once:
	uint16_t addr16;
	volatile char* filename;
	char* pathname;
	volatile char* tmp;
	volatile char* cpVersion;

	digitalWrite(ORANGE_LED,HIGH);
	pinMode(ORANGE_LED,OUTPUT);
	pinMode(BLUE_LED,OUTPUT);
	digitalWrite(BLUE_LED,HIGH);

	Serial.begin(115200);
	SubGHz.init();
	addr16 = SubGHz.getMyAddress();
	sleep(1000);
	if (addr16 >= 0x1000) {
		Serial.print("0x");
	} else if (addr16 >= 0x0100) {
		Serial.print("0x0");
	} else if (addr16 >= 0x0010) {
		Serial.print("0x00");
	} else {
		Serial.print("0x000");
	}
	Serial.println_long((long)addr16,HEX);
	sleep(1000);
	srand(addr16);
	pathname = sensor_init();
	filename = strtok(pathname,"\\");
	do {
		tmp = strtok(NULL,"\\");
		if(tmp == NULL) {
			tmp=strtok(filename,".");
			tmp=strtok(filename,"_");
			cpVersion=strtok(NULL,"_");
			strncpy(ota_param.name,filename, OTA_PRGM_NAME_SIZE);
			ota_param.ver = (uint8_t)atoi(cpVersion);
			if(strlen(filename) > 11) {
				Serial.println("filename is too long");
				while(1){
					digitalWrite(BLUE_LED,LOW);
					digitalWrite(ORANGE_LED,LOW);
					sleep(500);
					digitalWrite(BLUE_LED,HIGH);
					digitalWrite(ORANGE_LED,HIGH);
					sleep(500);
				}
			}
			break;
		} else {
			filename = tmp;
		}
	} while(tmp != NULL);
	Print.init(activate_str,50);
	Print.p("factory-iot,");
	Print.p(ota_param.name);
	Print.p(",");
	Print.l((long)ota_param.ver,DEC);
#ifndef DEBUG
	Serial.end();
#endif
	queue_init();
	SensorState_construct();
}

void loop() {
	// put your main code here, to run repeatedly:
	uint32_t remain_time=0;

	// switching tasks
	mip.func_mode = functions[mip.func_mode]();
	// sleep or wait event
	if (mip.sleep_time != NO_SLEEP) {
		//BREAKL("mip.sleep_time: ",mip.sleep_time,DEC);
		remain_time = wait_event_timeout(&waitEventFlag,mip.sleep_time);
	}
	// always running task
	if (mip.enable_sense == true) { // sensor is enabled
		// case 1. int flag false, sense_time over
		if (useInterruptFlag == false) {
			if (millis() - mip.last_sense_time < mip.sense_interval) return;
		} else {
			// case 2. int flag true, int event exist
			if ((remain_time != 0) || (waitEventFlag == true)) {
				waitEventFlag = false;
				// call sensor_main()
			} else {
				return;
			}
		}
		sensor_main();
		//BREAKL("sensor_main: ",mip.last_sense_time,DEC);
	}
}
