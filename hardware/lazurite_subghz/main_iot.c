
/* FILE NAME: main_iot.c
 * The MIT License (MIT)
 *
 * Copyright (c) 2018  Lapis Semiconductor Co.,Ltd.
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
#include "wdt.h"

/* --------------------------------------------------------------------------------
 * MACRO switch
 * -------------------------------------------------------------------------------- */
//#define DEBUG // uncomment, if debuggging
//#define LIB_DEBUG // uncomment, if use libdebug
//#define BREAK_MODE // uncomment, if use BREAK_MODE of libdebug
#define USE_DEBUG_LED // uncomment, if use blue led for debugging

#include "..\..\libraries\libdebug\libdebug.h"
#if defined(LIB_DEBUG) && !defined(DEBUG)
	#error Missing DEBUG macro.
#endif

/* --------------------------------------------------------------------------------
 * Please note that this program needs OTA library and the below AES key
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
#define MAX_QUEUE_LEN			( 64 )
#define SENSOR_INIT_START		( 0 )
#define SENSOR_INIT_STARTING	( 1 )
#define SENSOR_INIT_DONE		( 2 )
#define SENSOR_METHOD_LEVEL		( 0 )
#define SENSOR_METHOD_PULSE		( 1 )

typedef enum {
	STATE_TRIG_ACTIVATE = 0,
	STATE_WAIT_ACTIVATE,
	STATE_SEND_REALTIME,
	STATE_SEND_QUEUE_DATA,
	STATE_TRIG_RECONNECT,
	STATE_WAIT_RECONNECT,
	STATE_TRIG_UPD_PARAM,
	STATE_WAIT_UPD_PARAM,
	STATE_TRIG_FW_UPD,
	STATE_WAIT_FW_UPD
} MAIN_IOT_STATE;

typedef struct {
	bool enable_sense;
	bool send_request;
	uint8_t sensor_type;
	uint8_t sensor_method;
	uint8_t sensor_init_state; // sensor initialized status (start/started/done)
	int sensor_num;
	uint16_t gateway_panid;
	uint16_t gateway_addr;
	uint16_t my_short_addr;
	uint32_t last_sense_time; // last timestamp of getting sensor data
	uint32_t sense_interval; // paramater gotten from device config via EACK
	uint32_t sleep_time; // control sleep interval in loop()
	MAIN_IOT_STATE func_mode;
} MAIN_IOT_PARAM;

typedef struct {
	uint16_t panid;
	uint16_t addr;		// short addr
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
static SUBGHZ_MSG subghzSend(TX_PARAM *ptx);

/* --------------------------------------------------------------------------------
 * Global variable
 * -------------------------------------------------------------------------------- */
bool waitEventFlag = false;
bool useInterruptFlag = false;
static uint8_t rx_buf[MAX_BUF_SIZE];
static uint8_t tx_buf[MAX_BUF_SIZE];
static TX_PARAM tx_param = {
	0xffff,	// uint16_t panid;
	0xffff,	// uint16_t addr;		// short addr
	"",		// uint8_t *str;
	false,	// bool rx_on; // true:rxEnable, false:rxDisable
	0,		// uint32_t tx_time; // last timestamp of tx ok
	0,		// uint8_t fail; // to count up tx fail for triggering
	0,		// uint8_t retry; // to count up retry for waiting
	0,		// uint32_t backoff_time; // timestamp which back-off send is permitted
	false	// bool set_backoff_time; // request to calculate back-off interval
};
MAIN_IOT_PARAM mip = {
	false,					// bool enable_sense;
	false,					// bool send_request;
	0,						// uint8_t sensor_type;
	SENSOR_METHOD_LEVEL,	// uint8_t sensor_method;
	SENSOR_INIT_START,		// uint8_t sensor_init_state; // sensor initialized status (start/started/done)
	0,						// int sensor_num;
	0xffff,					// uint16_t gateway_panid;
	0xffff,					// uint16_t gateway_addr;
	0xffff,					// uint16_t my_short_addr;
	0,						// uint32_t last_sense_time; // last timestamp of getting sensor data
	DEFAULT_SLEEP_INTERVAL,	// uint32_t sense_interval; // paramater gotten from device config via EACK
	DEFAULT_SLEEP_INTERVAL,	// uint32_t sleep_time; // control sleep interval in loop()
	STATE_TRIG_ACTIVATE		// MAIN_IOT_STATE func_mode;
};
OTA_PARAM ota_param = {
	0,					// uint8_t hw_type;
	0,					// uint8_t ver;
	"",					// uint8_t name[OTA_PRGM_NAME_SIZE];
	SUBGHZ_CH,			// uint8_t ch;
	0xffff,				// uint16_t panid;
	SUBGHZ_100KBPS,		// SUBGHZ_RATE rate;
	SUBGHZ_PWR_20MW,	// SUBGHZ_POWER txPower;
	0xffff				// uint16_t hostAddr;
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

#ifdef IOT_QUEUE
/* --------------------------------------------------------------------------------
 * Queue functions
 * -------------------------------------------------------------------------------- */
#define QUEUE_ERR_EMPTY		( -1 )
#define QUEUE_ERR_FULL		( -2 )
#define QUEUE_ERR_PARAM		( -3 )

typedef struct {
	int id;
	SENSOR_VAL sensor_val;
	int reason;
	SENSOR_STATE next_state;
	uint8_t vls_level;
	uint32_t time;
} QUEUE_DATA;

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
#endif

/* --------------------------------------------------------------------------------
 * Sensor functions
 * -------------------------------------------------------------------------------- */

#define MAX_INTERVAL			( 3600*1000ul )
#define KEEP_ALIVE_INTERVAL		( 1800*1000ul )
#define SENSOR_TYPE_V1			( 1 )
#define SENSOR_TYPE_V2			( 2 )
#define EACK_NORMAL				( 0x00 )
#define EACK_FORCIBLE_FLAG		( 0x01 )
#define EACK_UPD_PARAM			( 0x02 )
#define EACK_ACTIVATE			( 0x03 )
#define EACK_FW_UPD				( 0xF0 )
#define EACK_ERR_FLAG			( 0x01 )
#define EACK_ERR_INTERVAL		( 0x02 )
#define EACK_ERR_SIZE			( 0x04 )
#define PARSE_PARAM_NO_CHANGE	( 0 )
#define PARSE_PARAM_CHANGE		( 1 )
#define PARSE_ERR_UNDEF_FORMAT	( -1 )
#define PARSE_ERR_UNDEF_HEADER	( -2 )
#define PAYLOAD_HEADER_SIZE		( 3 )
#define PAYLOAD_PARAM_SIZE		( 5 )
#define PAYLOAD_SINGLE_LEN		( PAYLOAD_HEADER_SIZE + PAYLOAD_PARAM_SIZE )
#define PAYLOAD_SINGLE_LEN_PULSE	( PAYLOAD_HEADER_SIZE + PAYLOAD_PARAM_SIZE+1 )
#define NO_SLEEP 				( 0 )

__packed typedef struct {
	uint8_t eack_flag;
	uint16_t sleep_interval_sec;	// unit sec
} EACK_DATA;

static void SensorState_offStable(SensorState* p_this);
static void SensorState_offUnstable(SensorState* p_this);
static void SensorState_onStable(SensorState* p_this);
static void SensorState_onUnstable(SensorState* p_this);

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

static void sensor_construct(void) {
	int i;
	SensorState *ssp = &Sensor[0];

	for (i=0; i<MAX_SENSOR_NUM; i++,ssp++) {
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
		ssp->init_state = SENSOR_INIT_START;
	}
}

static void sensor_state_init(void) {
	int i;
	SensorState *ssp = &Sensor[0];

	for (i=0; i<MAX_SENSOR_NUM; i++,ssp++) {
		ssp->init_state = SENSOR_INIT_START;
	}
	mip.sensor_init_state = SENSOR_INIT_START;
}

/*
 * sensor_parsePayload - parse payload data
 *   input: address of mac.payload
 *   output: PARSE_PARAM_CHANGE - success, threshold parameters is changed (must be initialized)
 *           PARSE_PARAM_NO_CHANGE - success, threshold parameters is not changed
 *         < 0 - error
 */
static int sensor_parsePayload(uint8_t *payload) {
	uint8_t *p[PAYLOAD_HEADER_SIZE+PAYLOAD_PARAM_SIZE*MAX_SENSOR_NUM+1];
	double thrs_on_val,thrs_off_val;
	uint32_t thrs_on_interval,thrs_off_interval;
	SensorState *ssp = &Sensor[0];
	bool changed=false;
	int i,ret=PARSE_PARAM_NO_CHANGE;

	BREAKS("parsePayload: ",payload);
	for (i=0;;i++) {
		if (i == 0) {
			p[i] = strtok(payload, ",");
		} else {
			p[i] = strtok(NULL, ",");
		}
		if (p[i] == NULL) break;
	}
	mip.sensor_num = (i - PAYLOAD_HEADER_SIZE) / PAYLOAD_PARAM_SIZE;

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
		BREAK("single");
		mip.sensor_type = SENSOR_TYPE_V1;
	} else if (i == PAYLOAD_SINGLE_LEN_PULSE) {
		BREAK("single pulse");
		mip.sensor_type = SENSOR_TYPE_V1;
		mip.sensor_method = (uint8_t)strtoul(p[i-1],NULL,0);
		BREAKL("sensor_method: ",mip.sensor_method,DEC);
	} else if ((mip.sensor_num > 0) &&
	  ((i - PAYLOAD_HEADER_SIZE) % PAYLOAD_PARAM_SIZE == 0) &&
	  (mip.sensor_num <= MAX_SENSOR_NUM)) {
		BREAKL("multi: ",(long)mip.sensor_num,DEC);
		mip.sensor_type = SENSOR_TYPE_V2;
	} else {
		ret = PARSE_ERR_UNDEF_FORMAT; // undefined payload format
	}
	if (strncmp(p[0],"activate",8) == 0) {
		mip.gateway_panid = (uint16_t)strtoul(p[1],NULL,0);
		mip.gateway_addr = (uint16_t)strtoul(p[2],NULL,0);
		mip.my_short_addr = (uint16_t)strtoul(p[3],NULL,0);
		for (i=0; i<mip.sensor_num; i++,ssp++) {
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
		for (;i<MAX_SENSOR_NUM;i++,ssp++) {
			ssp->id = INVALID_ID;
		}
		BREAK(p[0]);
		BREAKL("panid: ",(long)mip.gateway_panid,HEX);
		BREAKL("addr: ",(long)mip.gateway_addr,HEX);
		BREAKL("my_short_addr: ",(long)mip.my_short_addr,HEX);
		ssp = &Sensor[0];
		for (i=0; i<MAX_SENSOR_NUM; i++,ssp++) {
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
	if (changed == true) ret = PARSE_PARAM_CHANGE;
	return ret;
}
#ifdef IOT_QUEUE
static int sensor_saveQueue(SensorState *p_this) {
	QUEUE_DATA buf;

	buf.next_state = p_this->next_state;
	buf.vls_level = p_this->vls_level;
	buf.time = mip.last_sense_time;
	buf.id = p_this->id;
	buf.reason = p_this->reason;
	buf.sensor_val = p_this->sensor_val;
#ifdef DEBUG
	Serial.print_long((long)buf.id,DEC);
	Serial.print(",");
	switch(buf.sensor_val.type) {
		case INT8_VAL:
			Serial.print_long((long)buf.sensor_val.data.int8_val,DEC);
			break;
		case UINT8_VAL:
			Serial.print_long((long)buf.sensor_val.data.uint8_val,DEC);
			break;
		case INT16_VAL:
			Serial.print_long((long)buf.sensor_val.data.int16_val,DEC);
			break;
		case UINT16_VAL:
			Serial.print_long((long)buf.sensor_val.data.uint16_val,DEC);
			break;
		case INT32_VAL:
			Serial.print_long((long)buf.sensor_val.data.int32_val,DEC);
			break;
		case UINT32_VAL:
			Serial.print_long((long)buf.sensor_val.data.uint32_val,DEC);
			break;
		case FLOAT_VAL:
			Serial.print_double((double)buf.sensor_val.data.float_val,buf.sensor_val.digit);
			break;
		case DOUBLE_VAL:
			Serial.print_double(buf.sensor_val.data.double_val,buf.sensor_val.digit);
			break;
		default:
			break;
	}
	Serial.print(",");
	Serial.print(vls_val[buf.vls_level]);
	Serial.print(",");
	Serial.print_long((long)buf.reason,DEC);
	Serial.print(",");
	Serial.println_long((long)buf.time,DEC);
#endif
	return queue_write(&buf);
}
#endif

static bool sensor_operJudge(SensorState *p_this) {
	bool ret = false;
	uint8_t init_state;

	p_this->sensor_comp_val = sensor_getDoubleData(&p_this->sensor_val);
	if (p_this->init_state == SENSOR_INIT_START) {
		p_this->init_state = SENSOR_INIT_STARTING;
		mip.sleep_time = mip.sense_interval;
		if (p_this->sensor_comp_val < p_this->thrs_off_val) {
			p_this->thrs_off_start = mip.last_sense_time;
			p_this->next_state = SENSOR_STATE_OFF_UNSTABLE;
		} else {
			p_this->thrs_on_start = mip.last_sense_time;
			p_this->next_state = SENSOR_STATE_ON_UNSTABLE;
		}
	}
#if 0//def DEBUG
	Serial.print("id: ");
	Serial.print_long((long)p_this->id,DEC);
	Serial.print(", state: ");
	Serial.print_long((long)p_this->next_state,DEC);
	Serial.print(" -> ");
#endif
	switch (p_this->next_state) {
		case SENSOR_STATE_OFF_STABLE:
			SensorState_offStable(p_this);
 			if (p_this->next_state == SENSOR_STATE_ON_STABLE) ret = true;
			break;
		case SENSOR_STATE_OFF_UNSTABLE:
			init_state = p_this->init_state;
			SensorState_offUnstable(p_this);
			if (p_this->next_state == SENSOR_STATE_OFF_STABLE) {
				if (init_state == SENSOR_INIT_STARTING) {
					p_this->init_state = SENSOR_INIT_DONE;
					ret = true;
				}
			} else if (p_this->next_state == SENSOR_STATE_ON_STABLE) {
				ret = true;
			}
			break;
		case SENSOR_STATE_ON_STABLE:
			SensorState_onStable(p_this);
 			if (p_this->next_state == SENSOR_STATE_OFF_STABLE) ret = true;
			break;
		case SENSOR_STATE_ON_UNSTABLE:
			init_state = p_this->init_state;
			SensorState_onUnstable(p_this);
			if (p_this->next_state == SENSOR_STATE_ON_STABLE) {
				if (init_state == SENSOR_INIT_STARTING) {
					p_this->init_state = SENSOR_INIT_DONE;
					ret = true;
				}
			} else if (p_this->next_state == SENSOR_STATE_OFF_STABLE) {
				ret = true;
			}
			break;
		default:
			break;
	}
#if 0//def DEBUG
	Serial.println_long((long)p_this->next_state,DEC);
#endif
	return ret;
}

static uint32_t sensor_checkEack(MAIN_IOT_STATE *mode) {
	SensorState *ssp = &Sensor[0];
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
				for (i=0; i<mip.sensor_num; i++,ssp++) {
					ssp->save_request = true;
				}
				break;
			case EACK_UPD_PARAM:
				*mode = STATE_TRIG_UPD_PARAM;
				break;
			case EACK_ACTIVATE:
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
		subghzSend(&tx_param);
		SubGHz.close();
		*mode = STATE_TRIG_ACTIVATE;
	}
	return sleep_time;
}

static uint8_t sensor_genPayload(void) {
#ifdef IOT_QUEUE
	QUEUE_DATA *ptr;
	uint16_t head_local;
	uint32_t now,diff;
#else
	SensorState *ptr;
#endif
	uint8_t tmp[100],n;
	int num;

	// V1 format :
	// 'on/off',(value),(voltage)
	// 'on/off',(value),(voltage),(reason)
	//
	// V2 format :
	// 'v2','id','on/off',(value),(voltage),[reason],[deltaT], ... ,'id', ...

	if (mip.sensor_type == SENSOR_TYPE_V1) {
		Print.init(tx_buf,sizeof(tx_buf));
		num = 1;
	} else if (mip.sensor_type == SENSOR_TYPE_V2) {
		Print.init(tx_buf,sizeof(tx_buf)); // clear memory
		Print.init(tmp,sizeof(tmp));
#ifdef IOT_QUEUE
		num = queue_length();
#else
		num = mip.sensor_num;
#endif
		Print.p("v2");
	} else {
		Print.init(tx_buf,sizeof(tx_buf));
		Print.p("unknown sensor type");
		return 0;
	}
#ifdef IOT_QUEUE
	now = millis();
	head_local = queue.head;
#else
	ptr = &Sensor[0];
#endif
	for (n=0; n<num; n++) {
#ifdef IOT_QUEUE
		if (queue_peek(head_local,&ptr) != 0) return n;
#else
		if (ptr->save_request == false) {
			ptr++;
			continue;
		}
#endif
		if (mip.sensor_type == SENSOR_TYPE_V2) { // for SENSOR_TYPE_V2
			Print.p(",");
			Print.l((long)ptr->id,DEC);
			Print.p(",");
		}
		if (ptr->next_state == SENSOR_STATE_ON_STABLE) {
			Print.p("on,");
		} else {
			Print.p("off,");
		}
		switch(ptr->sensor_val.type) {
			case INT8_VAL:
				Print.l((long)ptr->sensor_val.data.int8_val,DEC);
				break;
			case UINT8_VAL:
				Print.l((long)ptr->sensor_val.data.uint8_val,DEC);
				break;
			case INT16_VAL:
				Print.l((long)ptr->sensor_val.data.int16_val,DEC);
				break;
			case UINT16_VAL:
				Print.l((long)ptr->sensor_val.data.uint16_val,DEC);
				break;
			case INT32_VAL:
				Print.l((long)ptr->sensor_val.data.int32_val,DEC);
				break;
			case UINT32_VAL:
				Print.l((long)ptr->sensor_val.data.uint32_val,DEC);
				break;
			case FLOAT_VAL:
				Print.d((double)ptr->sensor_val.data.float_val,ptr->sensor_val.digit);
				break;
			case DOUBLE_VAL:
				Print.d(ptr->sensor_val.data.double_val,ptr->sensor_val.digit);
				break;
			default:
				break;
		}
		Print.p(",");
		Print.p(vls_val[ptr->vls_level]);
		Print.p(",");
		if (((ptr->next_state == SENSOR_STATE_OFF_STABLE)
					|| (ptr->next_state == SENSOR_STATE_OFF_UNSTABLE))
				&& (ptr->reason != INVALID_REASON)) {
			Print.l((long)ptr->reason,DEC);
		}
		Print.p(",");
#ifdef IOT_QUEUE
		diff = now - ptr->time;
		if (diff > 10) {
			Print.l(diff,DEC);
		}
#endif
		if (mip.sensor_type == SENSOR_TYPE_V2) { // for SENSOR_TYPE_V2
			if ((strlen(tx_buf)+Print.len()) <= MAX_BUF_SIZE) {
				strncat(tx_buf,tmp,Print.len());
				Print.init(tmp,sizeof(tmp)); // clear memory
#ifdef IOT_QUEUE
				head_local = queue_next(head_local);
#else
				ptr++;
#endif
			} else {
				break;
			}
		}
	}
	return n;
}

static void sensor_main(void) {
	SensorState *ssp = &Sensor[0];
	int i;
	bool init_done = true, change_to_stable = false, vol_check_done = false;
	uint8_t vls_level = 0;

	sensor_meas(Sensor);
	mip.last_sense_time = millis();
	for (i=0; i<mip.sensor_num; i++,ssp++) {
		change_to_stable = sensor_operJudge(ssp);
		if (mip.sensor_method == SENSOR_METHOD_PULSE) {
			// operation change judgement
			if ((change_to_stable == true)
				&& (ssp->next_state == SENSOR_STATE_OFF_STABLE)) {
				ssp->save_request = true;
			}
		} else {
			// operation change judgement
			if (change_to_stable == true) {
				ssp->save_request = true;
			// keep alive condition judgement
			} else if ((ssp->last_save_time != 0)
				&& (mip.last_sense_time-ssp->last_save_time >= KEEP_ALIVE_INTERVAL)
				&& ((ssp->next_state == SENSOR_STATE_OFF_STABLE)
					|| (ssp->next_state == SENSOR_STATE_ON_STABLE))) {
				BREAK("keep alive");
				ssp->save_request = true;
			}
		}
		if (ssp->save_request == true) {
			mip.send_request = true;
			if (vol_check_done == false) {
				vol_check_done = true;
				vls_level = voltage_check(VLS_3_068);
			}
			ssp->vls_level = vls_level;
#ifdef IOT_QUEUE
			// save to queue
			ssp->save_request = false;
			if (sensor_saveQueue(ssp) != 0) {
				BREAK("saveQueue error");
			} else {
				ssp->last_save_time = mip.last_sense_time;
			}
#endif
		}
		if (ssp->init_state != SENSOR_INIT_DONE) init_done = false;
	}
	if (init_done == true) mip.sensor_init_state = SENSOR_INIT_DONE;
}

/* --------------------------------------------------------------------------------
 * Sensor state functions
 * -------------------------------------------------------------------------------- */

static void SensorState_offStable(SensorState* p_this) {
	p_this->next_state = SENSOR_STATE_OFF_STABLE;

	if (p_this->sensor_comp_val > p_this->thrs_on_val) {
		if (p_this->thrs_on_interval != 0) {
			p_this->thrs_on_start = mip.last_sense_time;
			p_this->next_state = SENSOR_STATE_OFF_UNSTABLE;
		} else {
			p_this->next_state = SENSOR_STATE_ON_STABLE;
		}
	}
}

static void SensorState_offUnstable(SensorState* p_this) {
	p_this->next_state = SENSOR_STATE_OFF_UNSTABLE;

	if (p_this->sensor_comp_val <= p_this->thrs_on_val) {
		if (p_this->init_state == SENSOR_INIT_DONE) {
			p_this->next_state = SENSOR_STATE_OFF_STABLE;
		} else if (mip.last_sense_time-p_this->thrs_off_start >= p_this->thrs_off_interval) {
			p_this->next_state = SENSOR_STATE_OFF_STABLE;
		}
	} else {
		if (p_this->init_state == SENSOR_INIT_DONE) {
			if (mip.last_sense_time-p_this->thrs_on_start >= p_this->thrs_on_interval) {
				p_this->next_state = SENSOR_STATE_ON_STABLE;
			}
		} else {
			if (p_this->thrs_on_interval != 0) {
				p_this->thrs_on_start = mip.last_sense_time;
				p_this->next_state = SENSOR_STATE_ON_UNSTABLE;
			} else {
				p_this->next_state = SENSOR_STATE_ON_STABLE;
			}
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
			p_this->next_state = SENSOR_STATE_OFF_STABLE;
		}
	}
}

static void SensorState_onUnstable(SensorState* p_this) {
	p_this->next_state = SENSOR_STATE_ON_UNSTABLE;

	if (p_this->sensor_comp_val >= p_this->thrs_off_val) {
		if (p_this->init_state == SENSOR_INIT_DONE) {
			p_this->next_state = SENSOR_STATE_ON_STABLE;
		} else if (mip.last_sense_time-p_this->thrs_on_start >= p_this->thrs_on_interval) {
			p_this->next_state = SENSOR_STATE_ON_STABLE;
		}
	} else {
		if (p_this->init_state == SENSOR_INIT_DONE) {
			if (mip.last_sense_time-p_this->thrs_off_start >= p_this->thrs_off_interval) {
				p_this->next_state = SENSOR_STATE_OFF_STABLE;
			}
		} else {
			if (p_this->thrs_off_interval != 0) {
				p_this->thrs_off_start = mip.last_sense_time;
				p_this->next_state = SENSOR_STATE_OFF_UNSTABLE;
			} else {
				p_this->next_state = SENSOR_STATE_OFF_STABLE;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
 * State control functions
 * -------------------------------------------------------------------------------- */
#define RX_INTERVAL					( 2*1000ul )
#define TX_INTERVAL					( rand()&1000 )
#define RETRY_INTERVAL				( 10*1000ul+ (rand()&500) )
#define ACTIVATE_RETRY_INTERVAL		( 1800*1000ul )
#define MIN_BACKOFF_INTERVAL		( 1000ul ) // actual max interval is 2^8*1000=256 s
#define MAX_ACTIVATE_RETRY			( 14 )
#define MAX_UPD_PARAM_RETRY			( 3 )
#define MAX_FW_UPD_RETRY			( 3 )
#define MAX_TRIG_TX_FAIL_COUNT		( 1 )
#define MAX_REALTIME_TX_FAIL_COUNT	( 1 )
#define MAX_BACKOFF_COUNT			( 8 )
#define PARSE_ERROR					( -1 )
#define PARSE_NONE					( 0 )
#define PARSE_CONFIG				( 1 )
#define PARSE_START					( 2 )

static uint8_t activate_str[50];
const uint16_t pow_arr[MAX_BACKOFF_COUNT+1] = {1,2,4,8,16,32,64,128,256};

static SUBGHZ_MSG subghzSend(TX_PARAM *ptx) {
	SUBGHZ_MSG msg;

	SubGHz.begin(SUBGHZ_CH,ptx->panid,SUBGHZ_100KBPS,SUBGHZ_PWR_20MW);
	if (ptx->rx_on == true) SubGHz.rxEnable(NULL);
	else SubGHz.rxDisable();
#ifdef USE_DEBUG_LED
	digitalWrite(BLUE_LED,LOW);
#endif
	msg = SubGHz.send(ptx->panid,ptx->addr,ptx->str,strlen(ptx->str),NULL);
#ifdef USE_DEBUG_LED
	digitalWrite(BLUE_LED,HIGH);
#endif
	return msg;
}

static MAIN_IOT_STATE func_trigActivate(void) {
	MAIN_IOT_STATE mode = STATE_TRIG_ACTIVATE;
	SUBGHZ_MSG msg;

//	if (voltage_check_oneshot(VLS_2_800) == 0) { // for harvesting board
		tx_param.panid = 0xffff;
		tx_param.addr = 0xffff;
		tx_param.str = activate_str;
		tx_param.rx_on = true;
		msg = subghzSend(&tx_param);
		if (msg == SUBGHZ_OK) {
			mode = STATE_WAIT_ACTIVATE;
			BREAK("waiting...");
			tx_param.tx_time = millis();
			tx_param.fail = 0;
			mip.sleep_time = NO_SLEEP;
		} else {
			SubGHz.close();
			if (tx_param.fail >= MAX_TRIG_TX_FAIL_COUNT) {
				tx_param.fail = 0;
				mip.sleep_time = DEFAULT_SLEEP_INTERVAL;
			} else {
				tx_param.fail++;
				mip.sleep_time = TX_INTERVAL;
			}
			BREAKL("tx fail: ",(long)tx_param.fail,DEC);
		}
//	}
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
#ifdef IOT_QUEUE
			mode = STATE_SEND_QUEUE_DATA;
#else
			mode = STATE_SEND_REALTIME;
#endif
			SubGHz.close();
			if (mip.my_short_addr != 0xffff) SubGHz.setMyAddress(mip.my_short_addr);
			if (sensor_activate(&mip.sense_interval) == true) {
				mip.sleep_time = mip.sense_interval;
			} else {
				mip.sleep_time = NO_SLEEP;
			}
			mip.enable_sense = true;
			sensor_state_init();
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

static MAIN_IOT_STATE func_sendRealtime(void) {
	MAIN_IOT_STATE mode = STATE_SEND_REALTIME;
#ifdef IOT_QUEUE
#else
	SensorState *ssp;
	SUBGHZ_MSG msg;
	int i;

	if (mip.send_request == true) {
		if (tx_param.fail > MAX_REALTIME_TX_FAIL_COUNT) {
			mode = STATE_TRIG_ACTIVATE;
			tx_param.fail = 0; // clear
			sensor_deactivate();
			mip.enable_sense = false;
			mip.sleep_time = NO_SLEEP;
			mip.send_request = false;
		} else {
			if (tx_param.fail == 0) {
				sensor_genPayload();
				tx_param.panid = mip.gateway_panid;
				tx_param.addr = mip.gateway_addr;
				tx_param.str = tx_buf;
				tx_param.rx_on = false;
			}
			BREAKS("tx_buf: ",tx_buf);
			msg = subghzSend(&tx_param);
			SubGHz.close();
			if (msg == SUBGHZ_OK) {
				mip.send_request = false;
				// update last send time
				ssp = &Sensor[0];
				for (i=0; i<mip.sensor_num; i++,ssp++) {
					if (ssp->save_request == true) {
						ssp->save_request = false;
						ssp->last_save_time = mip.last_sense_time;
					}
				}
				// check enhance ack
				tx_param.fail = 0; // clear
				mip.sleep_time = sensor_checkEack(&mode);
				if (mode != STATE_SEND_REALTIME) {
					sensor_deactivate();
					mip.enable_sense = false;
					mip.sleep_time = NO_SLEEP;
				}
			} else { // fail
				BREAK("func_sendRealtime: tx fail");
				tx_param.fail++;
				mip.sleep_time = TX_INTERVAL;
			}
			//BREAKL("func_sendRealtime: mip.sleep_time = ",(long)mip.sleep_time,DEC);
		}
	}
#endif
	return mode;
}

static MAIN_IOT_STATE func_sendQueueData(void) {
	MAIN_IOT_STATE mode = STATE_SEND_QUEUE_DATA;
#ifdef IOT_QUEUE
	SUBGHZ_MSG msg;
	uint8_t num;

	if (queue_length() == 0) {
		mip.sleep_time = mip.sense_interval;
	} else {
		num = sensor_genPayload();
		BREAK(tx_buf);
		tx_param.panid = mip.gateway_panid;
		tx_param.addr = mip.gateway_addr;
		tx_param.str = tx_buf;
		tx_param.rx_on = false;
		msg = subghzSend(&tx_param);
		SubGHz.close();
		if (msg == SUBGHZ_OK) {
			mip.sense_interval = sensor_checkEack(&mode);
			if (mode != STATE_SEND_QUEUE_DATA) {
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
#endif
	return mode;
}

#ifdef IOT_QUEUE
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
#endif

static MAIN_IOT_STATE func_trigReconnect(void) {
	MAIN_IOT_STATE mode = STATE_TRIG_RECONNECT;
#ifdef IOT_QUEUE
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
		msg = subghzSend(&tx_param);
		if (msg == SUBGHZ_OK) {
			mode = STATE_WAIT_RECONNECT;
			BREAK("waiting...");
			tx_param.tx_time = millis();
			mip.sleep_time = NO_SLEEP;
		} else { // fail
			tx_param.retry++;
			tx_param.set_backoff_time = true;
		}
	}
#endif
	return mode;
}

static MAIN_IOT_STATE func_waitReconnect(void) {
	MAIN_IOT_STATE mode = STATE_WAIT_RECONNECT;
#ifdef IOT_QUEUE
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
			if (ret == PARSE_PARAM_CHANGE) sensor_state_init();
			mode = STATE_SEND_QUEUE_DATA;
			SubGHz.close();
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
#endif
	return mode;
}

static MAIN_IOT_STATE func_trigUpdParam(void) {
	MAIN_IOT_STATE mode = STATE_TRIG_UPD_PARAM;
	uint8_t update_str[] = "update";
	SUBGHZ_MSG msg;

	tx_param.panid = mip.gateway_panid;
	tx_param.addr = mip.gateway_addr;
	tx_param.str = update_str;
	tx_param.rx_on = true;
	msg = subghzSend(&tx_param);
	if (msg == SUBGHZ_OK) {
		mode = STATE_WAIT_UPD_PARAM;
		BREAK("waiting...");
		tx_param.tx_time = millis();
		tx_param.fail = 0;
		mip.sleep_time = NO_SLEEP;
	} else {
		SubGHz.close();
		if (tx_param.fail >= MAX_TRIG_TX_FAIL_COUNT) {
			tx_param.fail = 0;
			mip.sleep_time = DEFAULT_SLEEP_INTERVAL;
		} else {
			tx_param.fail++;
			mip.sleep_time = TX_INTERVAL;
		}
		BREAKL("tx fail: ",(long)tx_param.fail,DEC);
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
#ifdef IOT_QUEUE
			mode = STATE_SEND_QUEUE_DATA;
#else
			mode = STATE_SEND_REALTIME;
#endif
			SubGHz.close();
			if (mip.my_short_addr != 0xffff) SubGHz.setMyAddress(mip.my_short_addr);
			if (sensor_activate(&mip.sense_interval) == true) {
				mip.sleep_time = mip.sense_interval;
			} else {
				mip.sleep_time = NO_SLEEP;
			}
			mip.enable_sense = true;
			sensor_state_init();
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
			tx_param.fail = 0; // clear
			tx_param.retry = 0; // clear
			mip.enable_sense = false;
			mip.sleep_time = NO_SLEEP;
		}
	}
	return mode;
}

// Return 
//  -1 = Parameter error
//   0 = No action
//   1 = Configuration: payload is "OTA,config,(hw_type),(name)", not encrypted
//   2 = Start OTA: payload is "OTA,start,(hw_type),(name),(ver)", encrypted
static int otaPayloadCheck(uint8_t *payload, uint8_t *hw_type, uint8_t *ver)
{
	uint8_t *s,*d,*n;
	int result=PARSE_NONE;

	s = strtok(payload, ",");
	if (strncmp(s,"OTA",3) == 0) {
		s = strtok(NULL,",");
		d = strtok(NULL,",");
		n = strtok(NULL,",");
		*hw_type = (uint8_t)strtol(d,NULL,0);

		if ((OTA.getHwType() == *hw_type) && \
			(strncmp(n,ota_param.name,OTA_PRGM_NAME_SIZE) == 0)) {
			if (strncmp(s,"config",6) == 0) {
				result = PARSE_CONFIG;
			} else if (strncmp(s,"start",5) == 0) {
				d = strtok(NULL,",");
				*ver = (uint8_t)strtol(d,NULL,0);
				result = PARSE_START;
			}
		} else {
			result = PARSE_ERROR;
		}
	}
	return result;
}

static MAIN_IOT_STATE func_trigFwUpd(void) {
	MAIN_IOT_STATE mode = STATE_TRIG_FW_UPD;
	uint8_t ota_str[] = "ota-ready";
	SUBGHZ_MSG msg;

	tx_param.panid = mip.gateway_panid;
	tx_param.addr = mip.gateway_addr;
	tx_param.str = ota_str;
	tx_param.rx_on = false;
	msg = subghzSend(&tx_param);
	SubGHz.close();
	if (msg == SUBGHZ_OK) {
		if (OTA.checkAesKey()) SubGHz.setKey(ota_aes_key);
		SubGHz.begin(SUBGHZ_CH,mip.gateway_panid,SUBGHZ_100KBPS,SUBGHZ_PWR_20MW);
		SubGHz.rxEnable(NULL);
		mode = STATE_WAIT_FW_UPD;
		BREAK("waiting...");
		tx_param.tx_time = millis();
		tx_param.fail = 0;
		mip.sleep_time = NO_SLEEP;
	} else {
		if (tx_param.fail >= MAX_TRIG_TX_FAIL_COUNT) {
			tx_param.fail = 0;
			mip.sleep_time = DEFAULT_SLEEP_INTERVAL;
		} else {
			tx_param.fail++;
			mip.sleep_time = TX_INTERVAL;
		}
		BREAKL("tx fail: ",(long)tx_param.fail,DEC);
	}
	return mode;
}

static MAIN_IOT_STATE func_waitFwUpd(void) {
	MAIN_IOT_STATE mode = STATE_WAIT_FW_UPD;
	SUBGHZ_MAC_PARAM mac;
	uint8_t hw_type, ver;
	int rx_len,result;

	rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
	if (rx_len > 0) { // receive
		rx_buf[rx_len] = 0;
		SubGHz.decMac(&mac,rx_buf,rx_len);
		result = otaPayloadCheck(mac.payload, &hw_type, &ver);
		if (result == PARSE_START) {	// start OTA
			ota_param.hw_type = hw_type;
			ota_param.ver = ver;
			ota_param.panid = mip.gateway_panid;
			ota_param.hostAddr = mip.gateway_addr;
			result = OTA.start(&ota_param);
			if (result == OTA_ERR_EEPROM_VERIFY) OTA.start(&ota_param); // retry
			SubGHz.setKey(NULL);	// return to here, if param is not good.
			mip.sleep_time = DEFAULT_SLEEP_INTERVAL;
		}
	} else if (millis() - tx_param.tx_time > RX_INTERVAL) { // timeout
		SubGHz.close();
		tx_param.retry++;
		BREAKL("tx_param.retry: ",(long)tx_param.retry,DEC);
		if (tx_param.retry <= MAX_FW_UPD_RETRY) {
			mode = STATE_TRIG_FW_UPD;
			mip.sleep_time = RETRY_INTERVAL;
		} else {
			mode = STATE_TRIG_ACTIVATE;
			tx_param.fail = 0; // clear
			tx_param.retry = 0; // clear
			mip.enable_sense = false;
			mip.sleep_time = NO_SLEEP;
		}
	}
	return mode;
}

static MAIN_IOT_STATE (*functions[])(void) = {
	func_trigActivate,
	func_waitActivate,
	func_sendRealtime,
	func_sendQueueData,
	func_trigReconnect,
	func_waitReconnect,
	func_trigUpdParam,
	func_waitUpdParam,
	func_trigFwUpd,
	func_waitFwUpd
};

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

	SubGHz.init();
	digitalWrite(ORANGE_LED,HIGH);
	pinMode(ORANGE_LED,OUTPUT);
	pinMode(BLUE_LED,OUTPUT);
	digitalWrite(BLUE_LED,HIGH);

	Serial.begin(115200);
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
#ifdef IOT_QUEUE
	queue_init();
#endif
	sensor_construct();
//	mip.sleep_time = 30*1000ul; // for harvesting board
}

void loop() {
	// put your main code here, to run repeatedly:
	uint32_t remain_time=0;

	wdt_clear();
	// switching tasks
	mip.func_mode = functions[mip.func_mode]();
	// sleep or wait event
	if (mip.sleep_time != NO_SLEEP) {
		//BREAKL("mip.sleep_time: ",mip.sleep_time,DEC);
		remain_time = wait_event_timeout(&waitEventFlag,mip.sleep_time);
	}
	// always running task
	if (mip.enable_sense == true) { // sensor is enabled
#ifdef IOT_QUEUE
		if (mip.sensor_init_state == SENSOR_INIT_DONE) {
			// case 1. int flag false, sense_time over
			if (useInterruptFlag == false) {
				if (millis() - mip.last_sense_time >= mip.sense_interval) {
					// call sensor_main()
				} else {
					return;
				}
			} else {
				// case 2. int flag true, int event exist
				if ((remain_time != 0) || (waitEventFlag == true)) {
					waitEventFlag = false;
					// call sensor_main()
				} else {
					return;
				}
			}
		}
#endif
		sensor_main();
		//BREAKL("sensor_main: ",mip.last_sense_time,DEC);
	}
}
