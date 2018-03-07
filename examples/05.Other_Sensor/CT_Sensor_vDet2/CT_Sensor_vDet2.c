#include "CT_Sensor_vDet2_ide.h"		// Additional Header

/* FILE NAME: CT_Sensor_vDet2.c
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
#pragma SEGCONST "OTA_USER_PARAM"
const uint8_t ota_aes_key[OTA_AES_KEY_SIZE] = {
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
#pragma SEGCONST

#define DEBUG	// uncomment, if using debug message

#define CHB						( 2 )
#define MEAS 					( 3 )
#define MEAS_LED				( 25 )
#define TX_LED					( 26 )
#define SUBGHZ_CH				( 36 )
#define BAUD					( 100 )
#define PWR						( 20 )
#define EACK_DEBUG_FLAG			( 0x01 )
#define EACK_PARAM_UPD			( 0x02 )
#define EACK_GW_SEARCH			( 0x03 )
#define EACK_FW_UPD				( 0xF0 )
#define GW_SEARCH_MODE			( 1 )
#define NORMAL_MODE				( 2 )
#define PARAM_UPD_MODE			( 3 )
#define FW_UPD_MODE				( 4 )
#define DEFAULT_SLEEP_INTERVAL	( 5 * 1000ul )
#define KEEP_ALIVE_INTERVAL		( 3599 * 1000ul )
#define GW_SEARCH_TIMEOUT		( 10 * 1000ul )
#define PARAM_UPD_TIMEOUT		( 1 * 1000ul )
#define PARAM_UPD_RETRY_TIMES	( 5 )
#define PARAM_UPD_SLEEP_INTVAL	( 10 * 1000ul )
#define FW_UPD_MODE_TIMEOUT		( 120 * 1000ul )
#define MAX_BUF_SIZE			( 250 )

uint8_t mode;
double current_amps;				// unit mA
double thrs_on_amp=0.03;
double thrs_off_amp=0.03;
uint16_t thrs_on_interval_sec=0;
uint16_t thrs_off_interval_sec=15;
uint32_t thrs_on_start=0;
uint32_t thrs_off_start=0;
uint32_t sleep_interval=DEFAULT_SLEEP_INTERVAL;
uint32_t prev_send_time, current_time;
uint16_t gateway_addr=0xffff;
uint16_t gateway_panid=0xffff;
uint16_t my_short_addr=0xffff;
bool send_data_flag=false;
bool debug_flag=false;

__packed typedef struct {
	uint8_t eack_flag;
	uint16_t sleep_interval_sec;	// unit sec
} EACK_DATA;

typedef enum {
	CT_STATE_OFF_STABLE = 0,
	CT_STATE_OFF_UNSTABLE,
	CT_STATE_ON_STABLE,
	CT_STATE_ON_UNSTABLE
} CT_STATE;

CT_STATE state_func;

static CT_STATE func_off_stable(void);
static CT_STATE func_off_unstable(void);
static CT_STATE func_on_stable(void);
static CT_STATE func_on_unstable(void);

CT_STATE (*ct_funcs[4])(void) = {
	func_off_stable,
	func_off_unstable,
	func_on_stable,
	func_on_unstable
};

OTA_PARAM ota_param = {
	0,						// hw type
	0,						// version
	"CT_Sensor_vDet2",		// must match with file name
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

static int parse_payload(uint8_t *payload)
{
	int i;
	uint8_t *p[10];

// payload : "'activate'/'debug',(panid),(shortaddr),(id),(thrs_on_amp),
//				(thrs_on_interval_sec),(thrs_off_amp),(thrs_off_interval_sec)"
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
		debug_flag = true;
	} else {
		debug_flag = false;
	}
	if ((strncmp(p[0],"activate",8) == 0) || debug_flag) {
		gateway_panid = (uint16_t)strtoul(p[1],NULL,0);
		gateway_addr = (uint16_t)strtoul(p[2],NULL,0);
		my_short_addr = (uint16_t)strtoul(p[3],NULL,0);
		thrs_on_amp = strtod(p[4],NULL);
		thrs_on_interval_sec = (uint16_t)strtoul(p[5],NULL,0);
		thrs_off_amp = strtod(p[6],NULL);
		thrs_off_interval_sec = (uint16_t)strtoul(p[7],NULL,0);
		send_data_flag = true;		// forcibly send data, because parameter might be changed
		return 0;
	} else {
		return -2;	// string pattern unmatched
	}
}

static void gw_search(void)
{
	uint8_t rx_buf[MAX_BUF_SIZE]={0};
	int rx_len;
	SUBGHZ_MAC_PARAM mac;
	uint8_t tx_str[] = "factory-iot";
	bool setting_done = false;

	SubGHz.begin(SUBGHZ_CH,0xffff,BAUD,PWR);
	SubGHz.rxEnable(NULL);
	do {
		digitalWrite(TX_LED,LOW);
		SubGHz.send(0xffff,0xffff,tx_str,sizeof(tx_str),NULL);				// broadcast
		digitalWrite(TX_LED,HIGH);
		prev_send_time = millis();
#ifdef DEBUG
		Serial.println("Searching GW...");
#endif
		while (1) {
			rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
			if (rx_len > 0) {
				SubGHz.decMac(&mac,rx_buf,rx_len);
				if (parse_payload(mac.payload) == 0) {						// parse ok
					setting_done = true;
					break;
				}
				memset(rx_buf,0,sizeof(rx_buf));							// clear memory
			}
			if ((millis() - prev_send_time) > GW_SEARCH_TIMEOUT ) break;	// timed out
		}
	} while (!setting_done);
	SubGHz.close();
}

static void param_update(void)
{
	uint8_t rx_buf[MAX_BUF_SIZE]={0};
	int rx_len,i;
	SUBGHZ_MAC_PARAM mac;
	SUBGHZ_MSG msg;
	uint8_t tx_str[] = "update";
	bool setting_done = false;

	SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
	SubGHz.rxEnable(NULL);
	for (i = 0; i < PARAM_UPD_RETRY_TIMES; i++) {
		digitalWrite(TX_LED,LOW);
		msg = SubGHz.send(gateway_panid,gateway_addr,tx_str,sizeof(tx_str),NULL);	// unicast
		digitalWrite(TX_LED,HIGH);
		if (msg == SUBGHZ_OK) {
			prev_send_time = millis();
			while (1) {
				rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
				if (rx_len > 0) {
					SubGHz.decMac(&mac,rx_buf,rx_len);
					if (parse_payload(mac.payload) == 0) {						// parse ok
						setting_done = true;
						break;
					}
					memset(rx_buf,0,sizeof(rx_buf));							// clear memory
				}
				if ((millis() - prev_send_time) > PARAM_UPD_TIMEOUT ) break;	// timed out
			}
		}
		if (setting_done) break;
		sleep(PARAM_UPD_SLEEP_INTVAL);
	}
	SubGHz.close();
	if (setting_done) {
		mode = NORMAL_MODE;
	} else {
		mode = GW_SEARCH_MODE;
	}
}

static CT_STATE func_off_stable(void)
{
	CT_STATE state = CT_STATE_OFF_STABLE;

	if (current_amps > thrs_on_amp) {
		if (thrs_on_interval_sec != 0) {
			thrs_on_start = current_time;
			state = CT_STATE_OFF_UNSTABLE;
		} else {
			send_data_flag = true;
			state = CT_STATE_ON_STABLE;
		}
	}
	return state;
}

static CT_STATE func_off_unstable(void)
{
	CT_STATE state = CT_STATE_OFF_UNSTABLE;

	if (current_amps <= thrs_on_amp) {
		state = CT_STATE_OFF_STABLE;
	}
	if ((current_time - thrs_on_start) > (thrs_on_interval_sec * 1000ul)) {
		send_data_flag = true;
		state = CT_STATE_ON_STABLE;
	}
	return state;
}

static CT_STATE func_on_stable(void)
{
	CT_STATE state = CT_STATE_ON_STABLE;

	if (current_amps < thrs_off_amp) {
		if (thrs_off_interval_sec != 0) {
			thrs_off_start = current_time;
			state = CT_STATE_ON_UNSTABLE;
		} else {
			send_data_flag = true;
			state = CT_STATE_OFF_STABLE;
		}
	}
	return state;
}

static CT_STATE func_on_unstable(void)
{
	CT_STATE state = CT_STATE_ON_UNSTABLE;

	if (current_amps >= thrs_off_amp) {
		state = CT_STATE_ON_STABLE;
	}
	if ((current_time - thrs_off_start) > (thrs_off_interval_sec * 1000ul)) {
		send_data_flag = true;
		state = CT_STATE_OFF_STABLE;
	}
	return state;
}

void ct_init() {
	pinMode(CHB,OUTPUT);
	pinMode(MEAS,OUTPUT);
	analogReadResolution(12);
}

double ct_meas() {
	unsigned long  st_time,en_time;
	double amps;
	volatile int st_voltage,en_voltage,dif_vol;
	digitalWrite(MEAS,HIGH);
	digitalWrite(CHB,HIGH);
	st_voltage = analogRead(A0);
	sleep(1);
	st_voltage = analogRead(A0);
	sleep(1);
	st_voltage = analogRead(A0);
	sleep(1);
	st_voltage = analogRead(A0);
	st_time = millis();
	do {
		sleep(1);
		en_voltage = analogRead(A0);
		en_time = millis();
	} while(((en_time - st_time) < 1000) && (en_voltage < 1024));
	digitalWrite(CHB,LOW);
	digitalWrite(MEAS,LOW);
	amps = 3.3*1.1;
	dif_vol = en_voltage-st_voltage;
	dif_vol = (dif_vol < 0) ? 0 : dif_vol;
	amps = amps * dif_vol /4.096/(en_time - st_time);  
	return amps;
}

static void ct_sensor_main(void)
{
	uint8_t tx_buf[20]={0};
	uint8_t level;
	SUBGHZ_MSG msg;
	EACK_DATA *eack_data;
	uint8_t *p;
	int eack_size,i;

	prev_send_time = 0;
	while(1) {
		level = voltage_check(VLS_4_667);	
		if (level >= 10) {
			current_amps = ct_meas();				// start measuring
			current_time = millis();
			if (debug_flag || (prev_send_time == 0) || ((current_time - prev_send_time) >= KEEP_ALIVE_INTERVAL)) {
				send_data_flag = true;
#ifdef DEBUG
				if (!debug_flag && (prev_send_time != 0)) Serial.println("keep alive");
#endif
			}
#ifdef DEBUG
			Serial.print("ct_state: ");
			Serial.print_long((long)state_func,DEC);
			Serial.print(" -> ");
#endif
			state_func = ct_funcs[state_func]();	// state machine
#ifdef DEBUG
			Serial.println_long((long)state_func,DEC);
#endif
			if (send_data_flag) {
				send_data_flag = false;
				Print.init(tx_buf,sizeof(tx_buf));
				if ((state_func == CT_STATE_ON_STABLE) || (state_func == CT_STATE_ON_UNSTABLE)) {
					Print.p("on,");
				} else {
					Print.p("off,");
				}
				Print.d(current_amps,2);
				Print.p(",");
				Print.p(vls_val[level]);
				Print.ln();

				digitalWrite(TX_LED,LOW);
				SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
				msg = SubGHz.send(gateway_panid,gateway_addr,tx_buf,Print.len(),NULL);
				SubGHz.close();
				digitalWrite(TX_LED,HIGH);
#ifdef DEBUG
				SubGHz.msgOut(msg);
#endif
				if (msg == SUBGHZ_OK) {
					prev_send_time = millis();
					SubGHz.getEnhanceAck(&p,&eack_size);
					eack_data = (EACK_DATA *)p;
#ifdef DEBUG
					Serial.print("eack_size: ");
					Serial.println_long((long)eack_size,DEC);
#endif
					if (eack_size == sizeof(EACK_DATA)) {
#ifdef DEBUG
						Serial.print("eack_data: ");
						for (i=0;i<sizeof(EACK_DATA);i++,p++) {
							Serial.print_long((long)*p,HEX);
							Serial.print(" ");
						}
						Serial.println("");
#endif
						switch (eack_data->eack_flag) {
						case EACK_DEBUG_FLAG:
							debug_flag = true;
							break;
						case EACK_PARAM_UPD:
							debug_flag = false;
							mode = PARAM_UPD_MODE;
							break;
						case EACK_GW_SEARCH:
							debug_flag = false;
							mode = GW_SEARCH_MODE;
							break;
						case EACK_FW_UPD:
							debug_flag = false;
							mode = FW_UPD_MODE;
							break;
						default:
							debug_flag = false;
							break;
						}
						if (debug_flag) {
							sleep_interval = DEFAULT_SLEEP_INTERVAL;
						} else if (eack_data->sleep_interval_sec <= KEEP_ALIVE_INTERVAL) {
							sleep_interval = eack_data->sleep_interval_sec * 1000ul;
						} else {
							// do nothing
						}
					}
				} else {
					mode = GW_SEARCH_MODE;
					break;
				}
			}
		}
#ifdef DEBUG
		Serial.print("go to sleep: ");
		Serial.println_long(sleep_interval,DEC);
#endif
		sleep(sleep_interval);
		if (mode != NORMAL_MODE) break;
	}
}

static CT_STATE func_init_state(void)
{
	current_amps = ct_meas();
	thrs_on_start = 0;
	thrs_off_start = 0;
	if (current_amps > thrs_off_amp) {
		return CT_STATE_ON_STABLE;
	} else {
		return CT_STATE_OFF_STABLE;
	}
}

static void fw_update(void)
{
	uint8_t rx_buf[MAX_BUF_SIZE]={0};
	int rx_len;
	uint16_t src_addr;
	uint8_t hw_type;
	uint8_t *s,*d;
	uint8_t tx_str[] = "ota-ready";
	SUBGHZ_MAC_PARAM mac;
	SUBGHZ_MSG msg;

	digitalWrite(TX_LED,LOW);
	SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
	msg = SubGHz.send(gateway_panid,gateway_addr,tx_str,sizeof(tx_str),NULL);
	SubGHz.close();
	digitalWrite(TX_LED,HIGH);

	if (msg == SUBGHZ_OK) {
		SubGHz.setKey(ota_aes_key);
		SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
		SubGHz.rxEnable(NULL);
		prev_send_time = millis();

		while (1) {
			rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
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
			if ((millis() - prev_send_time) > FW_UPD_MODE_TIMEOUT) break;
		}
		SubGHz.close();
	}
	SubGHz.setKey(NULL);	// timed out
}

void setup() {
	// put your setup code here, to run once:
	digitalWrite(MEAS_LED,HIGH);
	pinMode(MEAS_LED,OUTPUT);
	digitalWrite(TX_LED,HIGH);
	pinMode(TX_LED,OUTPUT);

#ifdef DEBUG
	Serial.begin(115200);
#endif
	SubGHz.init();
	ct_init();
	mode = GW_SEARCH_MODE;
}

void loop() {
	// put your main code here, to run repeatedly:
	switch (mode) {
	case GW_SEARCH_MODE:			// search gateway
#ifdef DEBUG
		Serial.println("GW SEARCH MODE");
#endif
		gw_search();
		if (my_short_addr != 0xffff) SubGHz.setMyAddress(my_short_addr);
		state_func = func_init_state();
		sleep_interval = DEFAULT_SLEEP_INTERVAL;
		mode = NORMAL_MODE;
		break;
	case NORMAL_MODE:
#ifdef DEBUG
		Serial.println("NORMAL MODE");
#endif
		ct_sensor_main();
		break;
	case PARAM_UPD_MODE:			// update paramter
#ifdef DEBUG
		Serial.println("PARAM UPDATE MODE");
#endif
		param_update();
		if (my_short_addr != 0xffff) SubGHz.setMyAddress(my_short_addr);
		state_func = func_init_state();
		break;
	case FW_UPD_MODE:
#ifdef DEBUG
		Serial.println("FW UPDATE MODE");
#endif
		fw_update();
		mode = NORMAL_MODE;			// return to normal mode, because fw update failed
		state_func = func_init_state();
		sleep_interval = DEFAULT_SLEEP_INTERVAL;
		break;
	default:
		break;
	}
}
