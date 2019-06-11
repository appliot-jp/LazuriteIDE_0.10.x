
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

/* --------------------------------------------------------------------------------
 * Please note that this sample program needs OTA library and the below AES key
 * must be changed apporpriately.
 * -------------------------------------------------------------------------------- */

#include "lazurite.h"

#pragma SEGCONST "OTA_USER_PARAM"
const uint8_t ota_aes_key[OTA_AES_KEY_SIZE] = {
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
#pragma SEGCONST

//#define DEBUG	// uncomment, if using debug message

#define ORANGE_LED				( 25 )
#define BLUE_LED				( 26 )
#define SUBGHZ_CH				( 36 )
#define BAUD					( 100 )
#define PWR						( 20 )
#define EACK_NORMAL				( 0x00 )
#define EACK_FORCIBLE_FLAG		( 0x01 )
#define EACK_PARAM_UPD			( 0x02 )
#define EACK_GW_SEARCH			( 0x03 )
#define EACK_FW_UPD				( 0xF0 )
#define EACK_ERR_FLAG			( 0x01 )
#define EACK_ERR_INTERVAL		( 0x02 )
#define EACK_ERR_SIZE			( 0x04 )
#define GW_SEARCH_MODE			( 1 )
#define INIT_MODE				( 2 )
#define NORMAL_MODE				( 3 )
#define PARAM_UPD_MODE			( 4 )
#define FW_UPD_MODE				( 5 )
#define DEFAULT_SLEEP_INTERVAL	( 5 * 1000ul )
#define KEEP_ALIVE_INTERVAL		( 1800 * 1000ul )
#define MAX_INTERVAL			( 3600 * 1000ul )
#define WAIT_RX_TIMEOUT			( 2 * 1000ul )
#define TX_RETRY_INTERVAL		( 10 * 1000ul )
#define GW_SEARCH_INTERVAL		( 1800 * 1000ul )
#define GW_SEARCH_RETRY_INTERVAL ( 20 * 1000ul )
#define GW_SEARCH_RETRY_TIMES	( 15 )
#define PARAM_UPD_RETRY_TIMES	( 4 )
#define SEND_DATA_RETRY_TIMES	( 1 )
#define MAX_BUF_SIZE			( 250 )

bool waitEventFlag = false;
void (*sensor_state_change_callback)(uint8_t state);
static uint8_t mode;

static double thrs_on_val=0.03;
static double thrs_off_val=0.03;
static uint32_t thrs_on_interval=0;
static uint32_t thrs_off_interval=15*1000ul;
static uint32_t thrs_on_start=0;
static uint32_t thrs_off_start=0;
static uint32_t sleep_interval=DEFAULT_SLEEP_INTERVAL;
static uint32_t prev_send_time=0;
static uint32_t current_time;
static uint16_t gateway_addr=0xffff;
static uint16_t gateway_panid=0xffff;
static uint16_t my_short_addr=0xffff;
static bool send_data_flag=false;
static bool forcible_flag=false;
static uint8_t rx_buf[MAX_BUF_SIZE];
static uint8_t level;

static double sensor_comp_val;

__packed typedef struct {
	uint8_t eack_flag;
	uint16_t sleep_interval_sec;	// unit sec
} EACK_DATA;

typedef struct {
	uint16_t	gateway_panid;
	uint16_t	gateway_addr;
	uint8_t		tx_str[50];
	uint8_t		retry;
} TRX_RETRY;

typedef enum {
	SENSOR_STATE_OFF_STABLE = 0,
	SENSOR_STATE_OFF_UNSTABLE,
	SENSOR_STATE_ON_STABLE,
	SENSOR_STATE_ON_UNSTABLE
} SENSOR_STATE;

SENSOR_STATE state_func;

static SENSOR_STATE func_off_stable(void);
static SENSOR_STATE func_off_unstable(void);
static SENSOR_STATE func_on_stable(void);
static SENSOR_STATE func_on_unstable(void);

SENSOR_STATE (*_funcs[4])(void) = {
	func_off_stable,
	func_off_unstable,
	func_on_stable,
	func_on_unstable
};

OTA_PARAM ota_param = {
	0,						// hw type
	0,						// version
	"",		// must match with file name
	SUBGHZ_CH,
	0xffff,
	BAUD,
	PWR,
	0xffff
};

const char vls_val[][8] ={
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
double getDoubleData(SENSOR_VAL *val) {
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
	}
	return 0;
}
static int parse_payload(uint8_t *payload)
{
	int i;
	uint8_t *p[10];

// payload : "'activate'/'debug',(panid),(shortaddr),(id),(thrs_on_val),
//				(thrs_on_interval[sec]),(thrs_off_val),(thrs_off_interval[sec])"
	for (i=0;;i++) {
		if (i == 0) {
			p[i] = strtok(payload, ",");
		} else {
			p[i] = strtok(NULL, ",");
		}
		if (p[i] == NULL) break;
	}
	if (i != 8) return -1;					// number of parameter unmatched
	if (strncmp(p[0],"debug",5) == 0) {
		forcible_flag = true;
	} else {
		forcible_flag = false;
	}
	if ((strncmp(p[0],"activate",8) == 0) || forcible_flag) {
		gateway_panid = (uint16_t)strtoul(p[1],NULL,0);
		gateway_addr = (uint16_t)strtoul(p[2],NULL,0);
		my_short_addr = (uint16_t)strtoul(p[3],NULL,0);
		thrs_on_val = strtod(p[4],NULL);
		thrs_on_interval = strtoul(p[5],NULL,0) * 1000ul;
		thrs_off_val = strtod(p[6],NULL);
		thrs_off_interval = strtoul(p[7],NULL,0) * 1000ul;
		send_data_flag = true;		// forcibly send data, because parameter might be changed
#ifdef DEBUG
		Serial.println(p[0]);
		Serial.print("gateway_panid: ");
		Serial.println_long((long)gateway_panid,HEX);
		Serial.print("gateway_addr: ");
		Serial.println_long((long)gateway_addr,HEX);
		Serial.print("my_short_addr: ");
		Serial.println_long((long)my_short_addr,HEX);
		Serial.print("thrs_on_val: ");
		Serial.println_double(thrs_on_val,2);
		Serial.print("thrs_on_interval: ");
		Serial.println_long(thrs_on_interval,DEC);
		Serial.print("thrs_off_val: ");
		Serial.println_double(thrs_off_val,2);
		Serial.print("thrs_off_interval: ");
		Serial.println_long(thrs_off_interval,DEC);
#endif
		return 0;
	} else {
		return -2;	// string pattern unmatched
	}
}

static bool activate_update(TRX_RETRY *p)
{
	int rx_len;
	SUBGHZ_MAC_PARAM mac;
	bool setting_done = false;
	int i = 0;

	while (1) {
		SubGHz.rxEnable(NULL);
		SubGHz.begin(SUBGHZ_CH,p->gateway_panid,BAUD,PWR);
		digitalWrite(BLUE_LED,LOW);
		SubGHz.send(p->gateway_panid,p->gateway_addr,p->tx_str,strlen(p->tx_str),NULL);
		digitalWrite(BLUE_LED,HIGH);
		prev_send_time = millis();
#ifdef DEBUG
		Serial.println("Waiting for receiving data...");
#endif
		while (1) {
			rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
			rx_buf[rx_len] = 0;											// null terminate
			if (rx_len > 0) {
				SubGHz.decMac(&mac,rx_buf,rx_len);
				if (parse_payload(mac.payload) == 0) {					// parse ok
					setting_done = true;
					break;
				}
			}
			if ((millis() - prev_send_time) > WAIT_RX_TIMEOUT ) break;	// timed out
		}
		SubGHz.close();
		SubGHz.rxDisable();
		if (setting_done) break;
		if (i++ < p->retry) {
			sleep(GW_SEARCH_RETRY_INTERVAL);
		} else {
#ifdef DEBUG
			Serial.println("Retry count exceeds the specified value.");
#endif
			break;
		}
	}
	return setting_done;
}

static bool gw_search(void)
{
	TRX_RETRY d;

	d.gateway_panid = 0xffff;
	d.gateway_addr = 0xffff;
	d.retry = GW_SEARCH_RETRY_TIMES;
	Print.init(d.tx_str,50);
	Print.p("factory-iot,");
	Print.p(ota_param.name);
	Print.p(",");
	Print.l((long)ota_param.ver,DEC);

	return activate_update(&d);
}

static bool param_update(void)
{
	TRX_RETRY d;

	d.gateway_panid = gateway_panid;
	d.gateway_addr = gateway_addr;
	d.retry = PARAM_UPD_RETRY_TIMES;
	Print.init(d.tx_str,50);
	Print.p("update");

	return activate_update(&d);
}

static SENSOR_STATE func_off_stable(void)
{
	SENSOR_STATE state = SENSOR_STATE_OFF_STABLE;

	if (sensor_comp_val > thrs_on_val) {
		if (thrs_on_interval != 0) {
			thrs_on_start = current_time;
			state = SENSOR_STATE_OFF_UNSTABLE;
		} else {
			send_data_flag = true;
			state = SENSOR_STATE_ON_STABLE;
		}
	}
	return state;
}

static SENSOR_STATE func_off_unstable(void)
{
	SENSOR_STATE state = SENSOR_STATE_OFF_UNSTABLE;

	if (sensor_comp_val <= thrs_on_val) {
		state = SENSOR_STATE_OFF_STABLE;
	} else {
		if ((current_time - thrs_on_start) > thrs_on_interval) {
			send_data_flag = true;
			state = SENSOR_STATE_ON_STABLE;
		}
	}
	return state;
}

static SENSOR_STATE func_on_stable(void)
{
	SENSOR_STATE state = SENSOR_STATE_ON_STABLE;

	if (sensor_comp_val < thrs_off_val) {
		if (thrs_off_interval != 0) {
			thrs_off_start = current_time;
			state = SENSOR_STATE_ON_UNSTABLE;
		} else {
			send_data_flag = true;
			state = SENSOR_STATE_OFF_STABLE;
		}
	}
	return state;
}

static SENSOR_STATE func_on_unstable(void)
{
	SENSOR_STATE state = SENSOR_STATE_ON_UNSTABLE;

	if (sensor_comp_val >= thrs_off_val) {
		state = SENSOR_STATE_ON_STABLE;
	} else {
		if ((current_time - thrs_off_start) > thrs_off_interval) {
			send_data_flag = true;
			state = SENSOR_STATE_OFF_STABLE;
		}
	}
	return state;
}

void sensor_set_state_change_callback(void (*f)(uint8_t state)) {
	sensor_state_change_callback = f;
}

static uint32_t sensor_main(void) {
	uint8_t tx_buf[50], on_off_state;
	SUBGHZ_MSG msg;
	EACK_DATA *eack_data;
	uint8_t *p;
	int eack_size, i, eack_error=0;
	uint32_t tmp;
	SENSOR_VAL sensor_val;
	
	sensor_val.reason = 0;
	sensor_meas(&sensor_val);				// start measuring
	switch(sensor_val.type) {
	case INT8_VAL:
		sensor_comp_val = sensor_val.data.int8_val;
		break;
	case UINT8_VAL:
		sensor_comp_val = sensor_val.data.uint8_val;
		break;
	case INT16_VAL:
		sensor_comp_val = sensor_val.data.int16_val;
		break;
	case UINT16_VAL:
		sensor_comp_val = sensor_val.data.uint16_val;
		break;
	case INT32_VAL:
		sensor_comp_val = sensor_val.data.int32_val;
		break;
	case UINT32_VAL:
		sensor_comp_val = sensor_val.data.uint32_val;
		break;
	case FLOAT_VAL:
		sensor_comp_val = sensor_val.data.float_val;
		break;
	case DOUBLE_VAL:
		sensor_comp_val = sensor_val.data.double_val;
		break;
	}
	current_time = millis();
	if (forcible_flag || (prev_send_time == 0) || ((current_time - prev_send_time) >= KEEP_ALIVE_INTERVAL)) {
		send_data_flag = true;
#ifdef DEBUG
		if (!forcible_flag && (prev_send_time != 0)) Serial.println("keep alive");
#endif
	}
#ifdef DEBUG
	Serial.print("sensor_state: ");
	Serial.print_long((long)state_func,DEC);
	Serial.print(" -> ");
#endif
	state_func = _funcs[state_func]();	// state machine
#ifdef DEBUG
	Serial.println_long((long)state_func,DEC);
#endif
	if (send_data_flag) {
		level = voltage_check(VLS_3_068);
		send_data_flag = false;
		Print.init(tx_buf,sizeof(tx_buf));
		if ((state_func == SENSOR_STATE_ON_STABLE) || (state_func == SENSOR_STATE_ON_UNSTABLE)) {
			Print.p("on,");
			on_off_state = STATE_TO_ON;
		} else {
			Print.p("off,");
			on_off_state = STATE_TO_OFF;
		}
		switch(sensor_val.type) {
		case INT8_VAL:
			Print.l(sensor_val.data.int8_val,DEC);
			break;
		case UINT8_VAL:
			Print.l(sensor_val.data.uint8_val,DEC);
			break;
		case INT16_VAL:
			Print.l(sensor_val.data.int16_val,DEC);
			break;
		case UINT16_VAL:
			Print.l(sensor_val.data.uint16_val,DEC);
			break;
		case INT32_VAL:
			Print.l(sensor_val.data.int32_val,DEC);
			break;
		case UINT32_VAL:
			Print.l(sensor_val.data.uint32_val,DEC);
			break;
		case FLOAT_VAL:
			Print.d(sensor_val.data.float_val,sensor_val.digit);
			break;
		case DOUBLE_VAL:
			Print.d(sensor_val.data.double_val,sensor_val.digit);
			break;
		}
		Print.p(",");
		Print.p(vls_val[level]);
		if ((on_off_state != STATE_TO_OFF) && (sensor_val.reason != 0)) {
			Print.p(",");
			Print.l((long)sensor_val.reason,DEC);
			sensor_val.reason = 0;				// clear stop reason
		}
#ifdef DEBUG
		Serial.println(tx_buf);
#endif
		digitalWrite(BLUE_LED,LOW);
		SubGHz.rxDisable();
		SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
		for (i=0; i<=SEND_DATA_RETRY_TIMES; i++) {
#ifdef DEBUG
			if (i != 0) Serial.println("retry occured.");
#endif
			msg = SubGHz.send(gateway_panid,gateway_addr,tx_buf,Print.len(),NULL);
#ifdef DEBUG
			SubGHz.msgOut(msg);
#endif
			if (msg == SUBGHZ_OK) break;
			sleep(rand()%1000);
		}
		SubGHz.close();
		digitalWrite(BLUE_LED,HIGH);
		if (msg == SUBGHZ_OK) {
			prev_send_time = millis();
			SubGHz.getEnhanceAck(&p,&eack_size);
			eack_data = (EACK_DATA *)p;
			/* no problem : Warning : W4027 : Cast operation may lead to odd boundary access */
#ifdef DEBUG
			Serial.print("eack_size: ");
			Serial.println_long((long)eack_size,DEC);
#endif
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
					forcible_flag = false;
					break;
				case EACK_FORCIBLE_FLAG:
					forcible_flag = true;
					break;
				case EACK_PARAM_UPD:
					forcible_flag = false;
					mode = PARAM_UPD_MODE;
					break;
				case EACK_GW_SEARCH:
					forcible_flag = false;
					mode = GW_SEARCH_MODE;
					break;
				case EACK_FW_UPD:
					forcible_flag = false;
					mode = FW_UPD_MODE;
					break;
				default:
					eack_error |= EACK_ERR_FLAG;
					forcible_flag = false;
					break;
				}
				tmp = eack_data->sleep_interval_sec * 1000ul;
				if (tmp > MAX_INTERVAL) {
					eack_error |= EACK_ERR_INTERVAL;
				} else if(tmp > KEEP_ALIVE_INTERVAL) {
					sleep_interval = KEEP_ALIVE_INTERVAL;
				}   else {
					sleep_interval = tmp;
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
#ifdef DEBUG
				Serial.println(tx_buf);
#endif
				SubGHz.rxDisable();
				SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
				SubGHz.send(gateway_panid,gateway_addr,tx_buf,Print.len(),NULL);
				SubGHz.close();
			} else {
				if (sensor_state_change_callback != NULL) sensor_state_change_callback(on_off_state);
			}
		} else {
			mode = GW_SEARCH_MODE;
		}
	}

#ifdef DEBUG
	Serial.print("go to sleep: ");
	Serial.println_long(sleep_interval,DEC);
#endif
	return sleep_interval;
}

static SENSOR_STATE func_init_state(void)
{
	SENSOR_VAL sensor_val;
	sensor_meas(&sensor_val);
	sensor_comp_val = getDoubleData(&sensor_val);
	thrs_on_start = 0;
	thrs_off_start = 0;
	if (sensor_comp_val >= thrs_off_val) {
		return SENSOR_STATE_ON_STABLE;
	} else {
		return SENSOR_STATE_OFF_STABLE;
	}
}

static void fw_update(void)
{
	int rx_len;
	uint16_t src_addr;
	uint8_t hw_type;
	uint8_t *s,*d;
	uint8_t tx_str[] = "ota-ready";
	SUBGHZ_MAC_PARAM mac;
	SUBGHZ_MSG msg;

	digitalWrite(BLUE_LED,LOW);
	SubGHz.rxDisable();
	SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
	msg = SubGHz.send(gateway_panid,gateway_addr,tx_str,strlen(tx_str),NULL);
	SubGHz.close();
	digitalWrite(BLUE_LED,HIGH);

	if (msg == SUBGHZ_OK) {
		SubGHz.setKey(ota_aes_key);
		SubGHz.rxEnable(NULL);
		SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
		prev_send_time = millis();

		while (1) {
			rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
			rx_buf[rx_len] = 0;												// null terminate
			if (rx_len > 0) {
				SubGHz.decMac(&mac,rx_buf,rx_len);
				src_addr = *(uint16_t *)mac.src_addr;
				if (src_addr == gateway_addr) {
					// payload : "ota-start,(hw_type),(name),(ver)", encrypted
					s = strtok(mac.payload, ",");
					if (strncmp(s,"ota-start",9) == 0) {
						d = strtok(NULL,",");	// hw_type
						s = strtok(NULL,",");	// name
						hw_type = (uint8_t)strtol(d,NULL,0);
						if ((OTA.getHwType() == hw_type) && \
							(strncmp(s,ota_param.name,OTA_PRGM_NAME_SIZE) == 0)) {
							d = strtok(NULL,",");
							ota_param.hw_type = hw_type;
							ota_param.ver = (uint8_t)strtol(d,NULL,0);
							ota_param.panid = gateway_panid;
							ota_param.hostAddr = gateway_addr;
							OTA.start(&ota_param);
							break;
						}
					}
				}
				memset(rx_buf,0,sizeof(rx_buf));	// clear memory
			}
			if ((millis() - prev_send_time) > WAIT_RX_TIMEOUT) break;
		}
		prev_send_time = 0;
		SubGHz.close();
		SubGHz.rxDisable();
	}
	SubGHz.setKey(NULL);	// timed out
}

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
	sensor_state_change_callback = NULL;
	pathname = sensor_init();
	filename = strtok(pathname,"\\");
	do {
		tmp = strtok(NULL,"\\");
		if(tmp == NULL) {
			tmp=strtok(filename,".");
			tmp=strtok(filename,"_");
			cpVersion=strtok(NULL,"_");
			strncpy(ota_param.name,filename, OTA_PRGM_NAME_SIZE);
			ota_param.ver = atoi(cpVersion);
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
	mode = GW_SEARCH_MODE;
#ifndef DEBUG
	Serial.end();
#endif
}

void loop() {
	uint32_t sleep_time=0;
	// put your main code here, to run repeatedly:
	switch (mode) {
	case GW_SEARCH_MODE:			// search gateway
#ifdef DEBUG
		Serial.println("GW SEARCH MODE");
#endif
		if (gw_search()) {			// found gateway successfully
			if (my_short_addr != 0xffff) SubGHz.setMyAddress(my_short_addr);
			if(sensor_activate() == true) {
				sleep_time = sleep_interval;
			} else {
				sleep_time = 0;
			}
			mode = INIT_MODE;
		} else {
			sleep_time = GW_SEARCH_INTERVAL;
		}
		break;
	case INIT_MODE:			// search gateway
		state_func = func_init_state();
		sleep_time = sleep_interval;
		mode = NORMAL_MODE;
		break;
	case NORMAL_MODE:
#ifdef DEBUG
		Serial.println("NORMAL MODE");
#endif
		sleep_time = sensor_main();
		if(mode != NORMAL_MODE) {
			sensor_deactivate();
			sleep_time = DEFAULT_SLEEP_INTERVAL;
		}
		break;
	case PARAM_UPD_MODE:			// update paramter
#ifdef DEBUG
		Serial.println("PARAM UPDATE MODE");
#endif
		if (param_update()) {
			if (my_short_addr != 0xffff) SubGHz.setMyAddress(my_short_addr);
			if(sensor_activate() == true) {
				sleep_time = sleep_interval;
			} else {
				sleep_time = 0;
			}
			mode = INIT_MODE;
		} else {
			mode = GW_SEARCH_MODE;
			sensor_deactivate();
			sleep_time = GW_SEARCH_INTERVAL;
		}
		break;
	case FW_UPD_MODE:
#ifdef DEBUG
		Serial.println("FW UPDATE MODE");
#endif
		fw_update();
		mode = GW_SEARCH_MODE;			// return to normal mode, because fw update failed
		sleep_interval = DEFAULT_SLEEP_INTERVAL;
		sleep_time = DEFAULT_SLEEP_INTERVAL;
		break;
	default:
		break;
	}
	if(sleep_time > 0) {
		wait_event_timeout(&waitEventFlag,sleep_time);
	}
}
