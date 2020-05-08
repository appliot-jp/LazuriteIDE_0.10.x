#include "Bridge_1_ide.h"		// Additional Header

/* FILE NAME: Bridge_1.c
 * The MIT License (MIT)
 *
 * Copyright (c) 2020  Lapis Semiconductor Co.,Ltd.
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

#define DEBUG // uncomment, if debuggging
#define LIB_DEBUG // uncomment, if use libdebug
//#define BREAK_MODE // uncomment, if use BREAK_MODE of libdebug

#include "..\..\..\..\libraries\libdebug\libdebug.h"
#include "wdt.h"

#if defined(LIB_DEBUG) && !defined(DEBUG)
	#error Missing DEBUG macro.
#endif

#define ORANGE_LED				( 25 )
#define BLUE_LED				( 26 )
#define SUBGHZ_CH				( 36 )
#define BRIDGE_ADDR				( 0xFFFD )
#define DEFAULT_SLEEP_INTERVAL	( 5*1000ul )
#define MAX_BUF_SIZE			( 240 )
#define NO_SLEEP 				( 0 )
#define MAX_SENSOR_NUM			( 10 )
#define EACK_SIZE				( 3 )
#define MAX_THRES_STR_SIZE		( 50 )
#define MAX_EACK_STR_SIZE		( EACK_SIZE * 4 ) // 4bytes=3桁+1セミコロン「:」

typedef enum {
	STATE_INIT_BRIDGE = 0,
	STATE_TRIG_BRIDGE_ACTIVATE,
	STATE_WAIT_BRIDGE_ACTIVATE,
	STATE_MAIN_TASK
} BRIDGE_STATE;

__packed typedef struct {
	uint16_t num;
	uint16_t size;
	struct {
		uint16_t src_addr;
		uint8_t data[EACK_SIZE];
	} arr[MAX_SENSOR_NUM+1];
} EACK_DATA;

typedef struct sensor_param_t {
	uint16_t short_addr;
	uint16_t id;
	uint8_t thres_str[MAX_THRES_STR_SIZE];
	uint8_t eack_str[MAX_EACK_STR_SIZE];
} SENSOR_PARAM;

typedef struct {
	BRIDGE_STATE func_mode;
	uint32_t sleep_time; // control sleep interval in loop()
	uint8_t gateway_addr64[8];
	uint16_t my_panid;
	EACK_DATA eack;
	SENSOR_PARAM sensor[MAX_SENSOR_NUM];
} BRIDGE_PARAM;

typedef struct {
	uint32_t tx_time; // last timestamp of tx ok
	uint8_t fail; // to count up tx fail for triggering
	uint8_t retry; // to count up retry for waiting
} TX_PARAM;

/* --------------------------------------------------------------------------------
 * Global variable
 * -------------------------------------------------------------------------------- */
static uint8_t rx_buf[MAX_BUF_SIZE];
static uint8_t tx_buf[MAX_BUF_SIZE];
TX_PARAM tx_param = {0};
BRIDGE_PARAM brp;

/* --------------------------------------------------------------------------------
 * Bridge related task
 * -------------------------------------------------------------------------------- */
#define PARSE_ERR_UNDEF_FORMAT ( -1 )
#define PARSE_ERR_UNDEF_HEADER ( -2 )
#define PAYLOAD_HEADER_NUM ( 1 )
#define PAYLOAD_EACK_PARAM_NUM ( 2 )
#define PAYLOAD_THRES_PARAM_NUM ( 4 )
#define PAYLOAD_V2_PARAM_NUM ( 5 )
#define MAX_BRIDGE_RETRY ( 4 )
#define BRIDGE_RETRY_INTERVAL ( rand()&500 )

static void bridge_hexConv(uint16_t data, char *buf, uint16_t size) {
	Print.init(buf,size);
	if (data >= 0x1000) {
		Print.p("0x");
	} else if (data >= 0x0100) {
		Print.p("0x0");
	} else if (data >= 0x0010) {
		Print.p("0x00");
	} else {
		Print.p("0x000");
	}
	Print.l((long)data,HEX);
}

/*
 * [input]
 *    payload: address of mac payload
 * [output]
 *    0 - success
 *  < 0 - error
 */
static int bridge_parseThresParams(uint8_t *payload) {
	int i,j,num,ret=0;
	uint8_t *p[PAYLOAD_HEADER_NUM+PAYLOAD_THRES_PARAM_NUM*10+1];
	uint16_t short_addr;
	SENSOR_PARAM *sp;

	for (i=0;;i++) {
		if (i == 0) {
			p[i] = strtok(payload, ":");
		} else {
			p[i] = strtok(NULL, ":");
		}
		if (p[i] == NULL) break;
	}
	num = (i - PAYLOAD_HEADER_NUM) / PAYLOAD_THRES_PARAM_NUM;

	// payload :
	// "'sensor':
	//  (short_addr):(id):(thres_str):(eack_str):
	//   ...
	//  (short_addr):(id):(thres_str):(eack_str)"

	if ((num > 0) && ((i - PAYLOAD_HEADER_NUM) % PAYLOAD_THRES_PARAM_NUM == 0)) {
		for (i=0; i<num; i++) {
			short_addr = (uint16_t)strtoul(p[1+i*PAYLOAD_THRES_PARAM_NUM],NULL,0);
			sp = brp.sensor;
			for (j=0; (j<MAX_SENSOR_NUM) && (sp->short_addr != 0xffff); j++,sp++) {
				if (sp->short_addr == short_addr) break;
			}
			sp->short_addr = short_addr;
			sp->id = (uint16_t)strtoul(p[2+i*PAYLOAD_THRES_PARAM_NUM],NULL,0);
			strcpy(sp->thres_str,p[3+i*PAYLOAD_THRES_PARAM_NUM]);
			strcpy(sp->eack_str,p[4+i*PAYLOAD_THRES_PARAM_NUM]);
			BREAKL("short_addr: ",(long)sp->short_addr,DEC);
			BREAKL("id: ",(long)sp->id,DEC);
			BREAKS("thres_str: ",sp->thres_str);
			BREAKS("eack_str: ",sp->eack_str);
		}
	} else {
		ret = PARSE_ERR_UNDEF_FORMAT; // string pattern unmatched
	}
	return ret;
}

/*
 * [input]
 *    payload: address of mac payload
 * [output]
 *    0 - success
 *  < 0 - error
 */
static int bridge_parseEackParams(uint8_t *payload) {
	int i,j,num,ret=0;
	uint8_t *p[PAYLOAD_HEADER_NUM+PAYLOAD_EACK_PARAM_NUM*10+1];
	uint16_t id;
	SENSOR_PARAM *sp;

	for (i=0;;i++) {
		if (i == 0) {
			p[i] = strtok(payload, ":");
		} else {
			p[i] = strtok(NULL, ":");
		}
		if (p[i] == NULL) break;
	}
	num = (i - PAYLOAD_HEADER_NUM) / PAYLOAD_EACK_PARAM_NUM;

	// payload :
	// "'eack':
	//  (id):(eack_str):
	//   ...
	//  (id):(eack_str)"

	if ((num > 0) && ((i - PAYLOAD_HEADER_NUM) % PAYLOAD_EACK_PARAM_NUM == 0)) {
		sp = brp.sensor;
		for (i=0; i<num; i++) {
			id = (uint16_t)strtoul(p[1+i*PAYLOAD_EACK_PARAM_NUM],NULL,0);
			sp = brp.sensor;
			for (j=0; (j<MAX_SENSOR_NUM) && (sp->short_addr != 0xffff); j++,sp++) {
				if (sp->id == id) break;
			}
			strcpy(sp->eack_str,p[2+i*PAYLOAD_THRES_PARAM_NUM]);
		}
	} else {
		ret = PARSE_ERR_UNDEF_FORMAT; // string pattern unmatched
	}
	return ret;
}

/*
 * [input]
 *    type: true - compare with short_addr
 *          false - compare with id
 *    src_addr: 16 bit source address
 * [output]
 *    0 - success
 *  < 0 - error
 */
static bool bridge_isSensorRegistered(bool type, uint16_t src_addr) {
	SENSOR_PARAM *sp=brp.sensor;
	bool ret=false;
	int i;

	for (i=0; (i<MAX_SENSOR_NUM) && (sp->short_addr != 0xffff); i++,sp++) {
		if (type) {
			if (sp->short_addr == src_addr) {
				ret = true;
				break;
			}
		} else {
			if (sp->id == src_addr) {
				ret = true;
				break;
			}
		}
	}
	return ret;
}

static void bridge_convV2Format(uint8_t *buf, uint16_t id) {
	uint8_t *p[8];
	int i,j;

	for (i=0;;i++) {
		if (i == 0) {
			p[i] = strtok(buf, ",");
		} else {
			p[i] = strtok(NULL, ",");
		}
		if (p[i] == NULL) break;
	}
	//BREAKL("conV2Format: ",(long)i,DEC);
	Print.init(tx_buf,sizeof(tx_buf));
	Print.p("v2,");
	Print.l((long)id,DEC);
	for (j=0;j<i;j++) {
		Print.p(",");
		if (p[j] != NULL) Print.p(p[j]);
	}
	for (;j<PAYLOAD_V2_PARAM_NUM;j++) {
		Print.p(",");
	}
	return;
}

/*
 * [input]
 *    mac: SUBGHZ_MAC_PARAM
 * [output]
 *    SUBGHZ_MSG between sensor <-> bridge
 */
static SUBGHZ_MSG bridge_tx64ActivateSensor(SUBGHZ_MAC_PARAM *mac) {
	SENSOR_PARAM *sp=brp.sensor;
	uint16_t short_addr=*((uint16_t *)mac->src_addr);
	SUBGHZ_MSG msg2gw,msg2sensor;
	int ret=0,i;
	char str[8];

	BREAK("tx64ActivateSensor");
	// find sensor addr
	for (i=0; (i<MAX_SENSOR_NUM) && (sp->short_addr != 0xffff); i++,sp++) {
		if (sp->short_addr == short_addr) break;
	}

	Print.init(tx_buf,sizeof(tx_buf));
	Print.p("activate,");
	Print.l((long)brp.my_panid,DEC);
	Print.p(",");
	Print.l((long)BRIDGE_ADDR,DEC);
	Print.p(",");
	Print.l((long)sp->id,DEC);
	Print.p(",");
	Print.p(sp->thres_str);

	// send to Sensor
	for (i=0; i<MAX_BRIDGE_RETRY; i++) {
		BREAKS("tx_buf: ",tx_buf);
		digitalWrite(BLUE_LED,LOW);
		msg2sensor = SubGHz.send64le(mac->src_addr,tx_buf,strlen(tx_buf),NULL);
		digitalWrite(BLUE_LED,HIGH);
		if (msg2sensor == SUBGHZ_OK) break;
		sleep(BRIDGE_RETRY_INTERVAL);
	}
	if (msg2sensor == SUBGHZ_OK) {
		// send to GW
		Print.init(tx_buf,sizeof(tx_buf));
		Print.p(mac->payload);
		Print.p(",");
		Print.l((long)short_addr,DEC);
	} else { // Sensor not respond
		bridge_hexConv(sp->short_addr,str,sizeof(str));
		Print.init(tx_buf,sizeof(tx_buf));
		Print.p("error,activate,tx64 fail [addr=");
		Print.p(str);
		Print.p("]");
	}
	for (i=0; i<MAX_BRIDGE_RETRY; i++) {
		BREAKS("tx_buf: ",tx_buf);
		digitalWrite(BLUE_LED,LOW);
		msg2gw = SubGHz.send64le(brp.gateway_addr64,tx_buf,strlen(tx_buf),NULL);
		digitalWrite(BLUE_LED,HIGH);
		if (msg2gw == SUBGHZ_OK) break;
		sleep(BRIDGE_RETRY_INTERVAL);
	}
	return msg2sensor;
}

/*
 * [input]
 *    type: true - normal data
 *          false - error data
 *    mac: SUBGHZ_MAC_PARAM
 * [output]
 *    SUBGHZ_MSG
 *    SUBGHZ_MSG between bridge <-> GW
 */
static SUBGHZ_MSG bridge_tx64SensorData(bool type, SUBGHZ_MAC_PARAM *mac) {
	SENSOR_PARAM *sp=brp.sensor;
	uint16_t id=*((uint16_t *)mac->src_addr);
	SUBGHZ_MSG msg2gw;
	int ret=0,i;

	BREAK("tx64SensorData");
	// find sensor addr
	for (i=0; (i<MAX_SENSOR_NUM) && (sp->short_addr != 0xffff); i++,sp++) {
		if (sp->id == id) break;
	}
		Print.init(tx_buf,sizeof(tx_buf));
	if (type) {
		bridge_convV2Format(mac->payload,id);
	} else {
		Print.p(mac->payload);
		Print.p(",");
		Print.l((long)id,DEC);
	}
	for (i=0; i<MAX_BRIDGE_RETRY; i++) {
		BREAKS("tx_buf: ",tx_buf);
		digitalWrite(BLUE_LED,LOW);
		msg2gw = SubGHz.send64le(brp.gateway_addr64,tx_buf,strlen(tx_buf),NULL);
		digitalWrite(BLUE_LED,HIGH);
		if (msg2gw == SUBGHZ_OK) break;
		sleep(BRIDGE_RETRY_INTERVAL);
	}
	return msg2gw;
}

static void bridge_setEackData(void) {
	SENSOR_PARAM *sp=brp.sensor;
	int i,j,k;
	uint8_t *p[16], eack_buf[MAX_EACK_STR_SIZE];

	for (i=0; (i<MAX_SENSOR_NUM) && (sp->short_addr != 0xffff); i++,sp++) {
		if (strlen(sp->eack_str) == 0) continue;
		strcpy(eack_buf,sp->eack_str);
		for (j=0;;j++) {
			if (j == 0) {
				p[j] = strtok(eack_buf, ",");
			} else {
				p[j] = strtok(NULL, ",");
			}
			if (p[j] == NULL) break;
		}
		if (j != EACK_SIZE) continue;
		brp.eack.arr[i].src_addr = sp->id;
		for (k=0; k<EACK_SIZE; k++) {
			brp.eack.arr[i].data[k] = (uint8_t)strtoul(p[k],NULL,0);
		}
	}
	// last parameter for address 0xffff
	brp.eack.arr[i].src_addr = 0xffff;
	brp.eack.arr[i].data[0] = 3;
	brp.eack.arr[i].data[1] = 5;
	brp.eack.arr[i].data[2] = 0;

	brp.eack.num = i+1;
	brp.eack.size = EACK_SIZE;
#ifdef DEBUG
	Serial.print("setEackData, num: ");
	Serial.print_long((long)brp.eack.num,DEC);
	Serial.print(", size: ");
	Serial.print_long((long)brp.eack.size,DEC);
	Serial.println("");
	for (i=0;i<(int)brp.eack.num;i++) {
		Serial.print("src_addr: ");
		Serial.print_long((long)brp.eack.arr[i].src_addr,DEC);
		Serial.print(", data:");
		for (j=0;j<EACK_SIZE;j++) {
			Serial.print(" ");
			Serial.print_long((long)brp.eack.arr[i].data[j],DEC);
		}
		Serial.println("");
	}
	delay(20);
#endif
	SubGHz.setEnhanceAck((uint8_t *)&brp.eack, sizeof(brp.eack));
}

/* --------------------------------------------------------------------------------
 * State control task
 * -------------------------------------------------------------------------------- */
#define RX_INTERVAL ( 2*1000ul )
#define MAX_TX_COUNT ( 1 )
#define TX_INTERVAL ( rand()&1000 )
#define RETRY_INTERVAL ( 10*1000ul+ (rand()&500) )

static uint8_t activate_str[50];

static BRIDGE_STATE func_initBridge(void) {
	BRIDGE_STATE mode = STATE_INIT_BRIDGE;
	int i;
	char str[8];

	SubGHz.close();
	sleep(rand()&1000);
	// initialize bridge parameters
	memset(&brp,0x00,sizeof(brp));
	brp.sleep_time = DEFAULT_SLEEP_INTERVAL;
	brp.my_panid = (uint16_t)(rand() & micros());
	for (i=0; i<MAX_SENSOR_NUM; i++) {
		brp.sensor[i].short_addr = 0xffff;
	}
	SubGHz.setEnhanceAck(NULL,0);
	SubGHz.begin(SUBGHZ_CH,brp.my_panid,SUBGHZ_100KBPS,SUBGHZ_PWR_20MW);
	SubGHz.rxEnable(NULL);
	bridge_hexConv(brp.my_panid,str,sizeof(str));
	BREAKS("PANID: ",str);
	return STATE_TRIG_BRIDGE_ACTIVATE;
}

static BRIDGE_STATE func_trigBridgeActivate(void) {
	BRIDGE_STATE mode = STATE_TRIG_BRIDGE_ACTIVATE;
	SUBGHZ_MSG msg2gw;

	digitalWrite(BLUE_LED,LOW);
	msg2gw = SubGHz.send(0xffff,0xffff,activate_str,strlen(activate_str),NULL);
	digitalWrite(BLUE_LED,HIGH);
	if (msg2gw == SUBGHZ_OK) {
		mode = STATE_WAIT_BRIDGE_ACTIVATE;
		BREAK("waiting...");
		tx_param.tx_time = millis();
		tx_param.fail = 0;
		brp.sleep_time = NO_SLEEP;
	} else {
		BREAK("tx fail");
		if (tx_param.fail >= MAX_TX_COUNT) {
			tx_param.fail = 0;
			brp.sleep_time = DEFAULT_SLEEP_INTERVAL;
		} else {
			tx_param.fail++;
			brp.sleep_time = TX_INTERVAL;
		}
	}
	return mode;
}

static BRIDGE_STATE func_waitBridgeActivate(void) {
	BRIDGE_STATE mode = STATE_WAIT_BRIDGE_ACTIVATE;
	SUBGHZ_MAC_PARAM mac;
	int rx_len,res;

	rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
	if (rx_len > 0) { // receive
		rx_buf[rx_len] = 0;
		SubGHz.decMac(&mac,rx_buf,rx_len);
		BREAK(mac.payload);
		if (strncmp(mac.payload,"sensor",6) == 0) {
			res = bridge_parseThresParams(mac.payload);
			if (res < 0) { // parse ng
				BREAKL("parseThresParam error 1: ",(long)res,DEC);
			} else {
				mode = STATE_MAIN_TASK;
				BREAK("bridge activation ok");
				memcpy(brp.gateway_addr64,mac.src_addr,8);
				bridge_setEackData();
				brp.sleep_time = NO_SLEEP;
			}
		}
	} else if (millis() - tx_param.tx_time > RX_INTERVAL) { // timeout
		mode = STATE_TRIG_BRIDGE_ACTIVATE;
		brp.sleep_time = RETRY_INTERVAL;
	}
	return mode;
}

static BRIDGE_STATE func_mainTask(void) {
	BRIDGE_STATE mode = STATE_MAIN_TASK;
	SUBGHZ_MAC_PARAM mac;
	SUBGHZ_MSG msg2gw,msg2sensor;
	int rx_len,res;
	uint16_t addr16;

	rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
	if (rx_len > 0) { // receive
		rx_buf[rx_len] = 0;
		SubGHz.decMac(&mac,rx_buf,rx_len);
		addr16 = *((uint16_t *)mac.src_addr);
		if (bridge_isSensorRegistered(true,addr16)) { // src_addr is short_addr, from sensor
			if ((mac.dst_panid == 0xffff) && (*((uint16_t *)mac.dst_addr) == 0xffff)) {
				// factory-iot
				msg2sensor = bridge_tx64ActivateSensor(&mac);
				if (msg2sensor == SUBGHZ_OK) {
					BREAKL("sensor activation ok: ",(long)addr16,DEC);
				} else {
					BREAK("sensor not respond");
				}
			}
		} else if (bridge_isSensorRegistered(false,addr16)) { // src_addr is id, from sensor
			if (mac.dst_panid == brp.my_panid) {
				if (strncmp(mac.payload,"error",5) == 0) {
					// eack error
					msg2gw = bridge_tx64SensorData(false,&mac);
					if (msg2gw == SUBGHZ_OK) {
						BREAK("tx ok: error info");
					} else {
						BREAK("GW not respond");
					}
				} else {
					// on/off data
					msg2gw = bridge_tx64SensorData(true,&mac);
					if (msg2gw == SUBGHZ_OK) {
						BREAK("tx ok: sensor data");
					} else {
						BREAK("GW not repsonding");
					}
				}
			}
		} else if (memcmp(mac.src_addr,brp.gateway_addr64,8) == 0) { // from gateway
			if (strncmp(mac.payload,"sensor",6) == 0) {
				res = bridge_parseThresParams(mac.payload);
				if (res < 0) { // parse ng
					BREAKL("parseThresParams error 2: ",(long)res,DEC);
				} else {
					BREAK("rx ok: sensor params");
					bridge_setEackData();
				}
			} else if (strncmp(mac.payload,"eack",4) == 0) {
				res = bridge_parseEackParams(mac.payload);
				if (res < 0) { // parse ng
					BREAKL("parseEackParams error: ",(long)res,DEC);
				} else {
					BREAK("rx ok: eack data");
					bridge_setEackData();
				}
			} else if (strncmp(mac.payload,"init",4) == 0) {
				BREAK("rcv init command");
				mode = STATE_INIT_BRIDGE;
			}
		}
	}
	return mode;
}

static BRIDGE_STATE (*functions[])(void) = {
	func_initBridge,
	func_trigBridgeActivate,
	func_waitBridgeActivate,
	func_mainTask
};

/* --------------------------------------------------------------------------------
 * setup and loop()
 * -------------------------------------------------------------------------------- */
void setup() {
	// put your setup code here, to run once:
	uint16_t addr16;
	char str[8];
	volatile char* filename;
	static char pathname[] = __FILE__;
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
	bridge_hexConv(addr16,str,sizeof(str));
	Serial.println(str);
	srand(addr16);
	filename = strtok(pathname,"\\");
	do {
		tmp = strtok(NULL,"\\");
		if(tmp == NULL) {
			tmp=strtok(filename,".");
			tmp=strtok(filename,"_");
			cpVersion=strtok(NULL,"_");
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
	Print.init(activate_str,sizeof(activate_str));
	Print.p("factory-iot-bridge,");
	Print.p(filename);
	Print.p(",");
	Print.l((long)atoi(cpVersion),DEC);
	SubGHz.setMyAddress(BRIDGE_ADDR);
	brp.func_mode = STATE_INIT_BRIDGE;
}

void loop() {
	// put your main code here, to run repeatedly:
	wdt_clear();
	// switching tasks
	brp.func_mode = functions[brp.func_mode]();
	// sleep
	if (brp.sleep_time != NO_SLEEP) sleep(brp.sleep_time);
}

