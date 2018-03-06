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

#define CHB						( 2 )
#define MEAS 					( 3 )
#define MEAS_LED				( 25 )
#define TX_LED					( 26 )
#define SUBGHZ_CH				( 36 )
#define BAUD					( 100 )
#define PWR						( 20 )

#define OTA_PANID				( 0xCDEF )
#define MAX_BUF_SIZE			( 250 )
#define GW_SEARCH_MODE			( 1 )
#define NORMAL_MODE				( 2 )
#define PARAM_UPD_MODE			( 3 )
#define FW_UPD_MODE				( 4 )
#define DEFAULT_SLEEP_INTERVAL	( 5 * 1000ul )
#define MAX_SLEEP_INTERVAL		( 3599 * 1000ul )
#define GW_SEARCH_TIMEOUT		( 10 * 1000ul )
#define PARAM_UPD_TIMEOUT		( 1 * 1000ul )
#define PARAM_UPD_RETRY_TIMES	( 5 )
#define PARAM_UPD_SLEEP_INTVAL	( 10 * 1000ul )
#define FW_UPD_MODE_TIMEOUT		( 300 * 1000ul )

uint8_t txbuf[MAX_BUF_SIZE], mode;
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
uint16_t my_short_addr=0;
bool send_data_flag=false;
bool debug_flag=false;

__packed typedef struct {
	uint8_t param_upd:1;
	uint8_t fw_update:1;
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
	OTA_PANID,
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
		while (1) {
			rx_len = SubGHz.readData(txbuf,MAX_BUF_SIZE);
			if (rx_len > 0) {
				SubGHz.decMac(&mac,txbuf,rx_len);
				if (parse_payload(mac.payload) == 0) {						// parse ok
					setting_done = true;
					break;
				}
			}
			if ((millis() - prev_send_time) > GW_SEARCH_TIMEOUT ) break;	// timed out
		}
	} while (!setting_done);
	SubGHz.close();
}

static void param_update(void)
{
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
				rx_len = SubGHz.readData(txbuf,MAX_BUF_SIZE);
				if (rx_len > 0) {
					SubGHz.decMac(&mac,txbuf,rx_len);
					if (parse_payload(mac.payload) == 0) {						// parse ok
						setting_done = true;
						break;
					}
				}
				if ((millis() - prev_send_time) > PARAM_UPD_TIMEOUT ) break;	// timed out
			}
		}
		if (setting_done) break;
		sleep(PARAM_UPD_SLEEP_INTVAL);
	}
	SubGHz.close();
	if (!setting_done) mode = GW_SEARCH_MODE;
}

static void check_enhance_ack(void)
{
	EACK_DATA *eack_data;
	uint8_t *p;
	int eack_size;

	SubGHz.getEnhanceAck(&p,&eack_size);
	eack_data = (EACK_DATA *)p;
	if (eack_size == sizeof(EACK_DATA)) {
		if (eack_data->param_upd) mode = PARAM_UPD_MODE;
		if (eack_data->fw_update) mode = FW_UPD_MODE;		// higher priority
		sleep_interval = eack_data->sleep_interval_sec * 1000ul;
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

static CT_STATE fix_current_level(void)
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

static void ct_sensor_main(void)
{
	uint8_t level;
	SUBGHZ_MSG msg;

	level = voltage_check(VLS_4_667);	
	if (level >= 10) {
		current_amps = ct_meas();				// start measuring
		current_time = millis();
		state_func = ct_funcs[state_func]();	// state machine

		if (debug_flag || ((prev_send_time != 0) && ((current_time - prev_send_time) >= MAX_SLEEP_INTERVAL))) {
			send_data_flag = true;
		}
		if (send_data_flag) {
			send_data_flag = false;
			Print.init(txbuf,sizeof(txbuf));
			if (debug_flag) {
				if (current_amps > thrs_off_amp) {
					Print.p("on,");
				} else {
					Print.p("off,");
				}
			} else {
				if ((state_func == CT_STATE_ON_STABLE) || (state_func == CT_STATE_ON_UNSTABLE)) {
					Print.p("on,");
				} else {
					Print.p("off,");
				}
			}
			Print.d(current_amps,2);
			Print.p(",");
			Print.p(vls_val[level]);
			Print.ln();

			digitalWrite(TX_LED,LOW);
			SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
			msg = SubGHz.send(gateway_panid,gateway_addr,txbuf,Print.len(),NULL);
			SubGHz.close();
			digitalWrite(TX_LED,HIGH);
			SubGHz.msgOut(msg);
			if (msg == SUBGHZ_OK) {
				prev_send_time = millis();
				check_enhance_ack();
			} else {
				mode = GW_SEARCH_MODE;
			}
		}
	}
}

static void fw_update(void)
{
	uint16_t src_addr;
	uint8_t hw_type;
	uint8_t *s,*d;
	uint8_t tx_str[] = "ota-ready";
	SUBGHZ_MAC_PARAM mac;
	SUBGHZ_MSG msg;
	int rx_len;

	digitalWrite(TX_LED,LOW);
	SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
	msg = SubGHz.send(gateway_panid,gateway_addr,tx_str,sizeof(tx_str),NULL);
	SubGHz.close();
	digitalWrite(TX_LED,HIGH);

	if (msg == SUBGHZ_OK) {
		SubGHz.setKey(ota_aes_key);
		SubGHz.begin(SUBGHZ_CH,OTA_PANID,BAUD,PWR);
		SubGHz.rxEnable(NULL);
		prev_send_time = millis();

		while (1) {
			rx_len = SubGHz.readData(txbuf,MAX_BUF_SIZE);
			if (rx_len > 0) {
				SubGHz.decMac(&mac,txbuf,rx_len);
				src_addr = *(uint16_t *)mac.src_addr;
				if (src_addr == gateway_addr) {
					// payload : "FW_UPD_START,(hw_type),(name),(ver)", encrypted
					s = strtok(mac.payload, ",");
					if (strncmp(s,"ota-start",9) == 0) {
						d = strtok(NULL,",");	// hw_type
						s = strtok(NULL,",");	// name
						hw_type = (uint8_t)strtol(d,NULL,0);

						if ((OTA.getHwType() == hw_type) && \
							(strncmp(s,ota_param.name,OTA_PRGM_NAME_SIZE) == 0)) {
							d = strtok(NULL,",");
							ota_param.ver = (uint8_t)strtol(d,NULL,0);
							ota_param.hostAddr = gateway_addr;
							OTA.start(&ota_param);
							break;
						}
					}
				}
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
  
	Serial.begin(115200);
	SubGHz.init();
	ct_init();
	mode = GW_SEARCH_MODE;
}

void loop() {
	// put your main code here, to run repeatedly:
	switch (mode) {
	case GW_SEARCH_MODE:			// search gateway
		Serial.println("GW SEARCH MODE");
		gw_search();
		if (my_short_addr) SubGHz.setMyAddress(my_short_addr);
		state_func = fix_current_level();
		mode = NORMAL_MODE;
		break;
	case NORMAL_MODE:
		Serial.println("NORMAL MODE");
		ct_sensor_main();
		if (debug_flag) sleep_interval = DEFAULT_SLEEP_INTERVAL;
		if (mode != GW_SEARCH_MODE) {
			Serial.print("go to sleep : ");
			Serial.println_long(sleep_interval,DEC);
			sleep(sleep_interval);
		}
		break;
	case PARAM_UPD_MODE:			// update paramter
		Serial.println("PARAM UPDATE MODE");
		param_update();
		if (my_short_addr) SubGHz.setMyAddress(my_short_addr);
		state_func = fix_current_level();
		mode = NORMAL_MODE;
		break;
	case FW_UPD_MODE:
		Serial.println("FW UPDATE MODE");
		fw_update();
		mode = NORMAL_MODE;			// return to normal mode, because fw update failed
		state_func = fix_current_level();
		break;
	default:
		break;
	}
}
