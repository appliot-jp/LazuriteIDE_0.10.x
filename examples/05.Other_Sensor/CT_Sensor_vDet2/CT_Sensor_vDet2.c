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

//#define DEBUG	// uncomment, if using debug message

#define CHB						( 2 )
#define MEAS 					( 3 )
#define ORANGE_LED				( 25 )
#define BLUE_LED				( 26 )
#define SUBGHZ_CH				( 36 )
#define BAUD					( 100 )
#define PWR						( 20 )
#define EACK_FORCIBLE_FLAG		( 0x01 )
#define EACK_PARAM_UPD			( 0x02 )
#define EACK_GW_SEARCH			( 0x03 )
#define EACK_FW_UPD				( 0xF0 )
#define GW_SEARCH_MODE			( 1 )
#define NORMAL_MODE				( 2 )
#define PARAM_UPD_MODE			( 3 )
#define FW_UPD_MODE				( 4 )
#define DEFAULT_SLEEP_INTERVAL	( 5 * 1000ul )
#define KEEP_ALIVE_INTERVAL		( 3480 * 1000ul )
#define WAIT_RX_TIMEOUT			( 2 * 1000ul )
#define TX_RETRY_INTERVAL		( 10 * 1000ul )
#define LOW_VOLTAGE_SLEEP_INTVAL	( 60 * 1000ul )
#define PARAM_UPD_RETRY_TIMES	( 5 )
#define SEND_DATA_RETRY_TIMES	( 1 )
#define MAX_BUF_SIZE			( 250 )

uint8_t mode;
double current_val;
double thrs_on_val=0.03;
double thrs_off_val=0.03;
uint32_t thrs_on_interval=0;
uint32_t thrs_off_interval=15*1000ul;
uint32_t thrs_on_start=0;
uint32_t thrs_off_start=0;
uint32_t sleep_interval=DEFAULT_SLEEP_INTERVAL;
uint32_t prev_send_time=0;
uint32_t current_time;
uint16_t gateway_addr=0xffff;
uint16_t gateway_panid=0xffff;
uint16_t my_short_addr=0xffff;
bool send_data_flag=false;
bool forcible_flag=false;
uint8_t rx_buf[MAX_BUF_SIZE];
uint8_t level;
bool low_voltage=false;

__packed typedef struct {
	uint8_t eack_flag;
	uint16_t sleep_interval_sec;	// unit sec
} EACK_DATA;

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

static bool gw_search(void)
{
	int rx_len;
	SUBGHZ_MAC_PARAM mac;
	uint8_t tx_str[] = "factory-iot";
	bool setting_done = false;

	SubGHz.rxEnable(NULL);
	SubGHz.begin(SUBGHZ_CH,0xffff,BAUD,PWR);
	digitalWrite(BLUE_LED,LOW);
	SubGHz.send(0xffff,0xffff,tx_str,sizeof(tx_str),NULL);				// broadcast
	digitalWrite(BLUE_LED,HIGH);
	prev_send_time = millis();
#ifdef DEBUG
	Serial.println("Searching GW...");
#endif
	while (1) {
		rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
		rx_buf[rx_len] = 0;												// null terminate
		if (rx_len > 0) {
			SubGHz.decMac(&mac,rx_buf,rx_len);
			if (parse_payload(mac.payload) == 0) {						// parse ok
				setting_done = true;
				break;
			}
			memset(rx_buf,0,sizeof(rx_buf));							// clear memory
		}
		if ((millis() - prev_send_time) > WAIT_RX_TIMEOUT ) break;	// timed out
	}
	SubGHz.close();
	SubGHz.rxDisable();
	sleep(TX_RETRY_INTERVAL);
	prev_send_time = 0;
	return setting_done;
}

static bool param_update(void)
{
	int rx_len,i;
	SUBGHZ_MAC_PARAM mac;
	SUBGHZ_MSG msg;
	uint8_t tx_str[] = "update";
	bool setting_done = false;

	SubGHz.rxEnable(NULL);
	SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
	for (i=0; i<PARAM_UPD_RETRY_TIMES; i++) {
		digitalWrite(BLUE_LED,LOW);
		msg = SubGHz.send(gateway_panid,gateway_addr,tx_str,sizeof(tx_str),NULL);	// unicast
		digitalWrite(BLUE_LED,HIGH);
		if (msg == SUBGHZ_OK) {
			prev_send_time = millis();
			while (1) {
				rx_len = SubGHz.readData(rx_buf,MAX_BUF_SIZE);
				rx_buf[rx_len] = 0;												// null terminate
				if (rx_len > 0) {
					SubGHz.decMac(&mac,rx_buf,rx_len);
					if (parse_payload(mac.payload) == 0) {						// parse ok
						setting_done = true;
						break;
					}
					memset(rx_buf,0,sizeof(rx_buf));							// clear memory
				}
				if ((millis() - prev_send_time) > WAIT_RX_TIMEOUT ) break;	// timed out
			}
			prev_send_time = 0;
		}
		if (setting_done) break;
		sleep(TX_RETRY_INTERVAL);
	}
	SubGHz.close();
	SubGHz.rxDisable();
	return setting_done;
}

static SENSOR_STATE func_off_stable(void)
{
	SENSOR_STATE state = SENSOR_STATE_OFF_STABLE;

	if (current_val > thrs_on_val) {
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

	if (current_val <= thrs_on_val) {
		state = SENSOR_STATE_OFF_STABLE;
	}
	if ((current_time - thrs_on_start) > thrs_on_interval) {
		send_data_flag = true;
		state = SENSOR_STATE_ON_STABLE;
	}
	return state;
}

static SENSOR_STATE func_on_stable(void)
{
	SENSOR_STATE state = SENSOR_STATE_ON_STABLE;

	if (current_val < thrs_off_val) {
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

	if (current_val >= thrs_off_val) {
		state = SENSOR_STATE_ON_STABLE;
	}
	if ((current_time - thrs_off_start) > thrs_off_interval) {
		send_data_flag = true;
		state = SENSOR_STATE_OFF_STABLE;
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

static void ct_sensor_main(void) {
	uint8_t tx_buf[20]={0};
	SUBGHZ_MSG msg;
	EACK_DATA *eack_data;
	uint8_t *p;
	int eack_size,i;
	uint32_t tmp;

	current_val = ct_meas();				// start measuring
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
		send_data_flag = false;
		Print.init(tx_buf,sizeof(tx_buf));
		if ((state_func == SENSOR_STATE_ON_STABLE) || (state_func == SENSOR_STATE_ON_UNSTABLE)) {
			Print.p("on,");
		} else {
			Print.p("off,");
		}
		Print.d(current_val,2);
		Print.p(",");
		Print.p(vls_val[level]);
		Print.ln();
#ifdef DEBUG
		Serial.print(tx_buf);
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
					forcible_flag = false;
					break;
				}
				tmp = eack_data->sleep_interval_sec * 1000ul;
				if (tmp <= KEEP_ALIVE_INTERVAL) sleep_interval = tmp;
			}
		} else {
			mode = GW_SEARCH_MODE;
		}
	}

#ifdef DEBUG
	Serial.print("go to sleep: ");
	Serial.println_long(sleep_interval,DEC);
#endif
	sleep(sleep_interval);
}

static SENSOR_STATE func_init_state(void)
{
	current_val = ct_meas();
	thrs_on_start = 0;
	thrs_off_start = 0;
	if (current_val > thrs_off_val) {
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
	msg = SubGHz.send(gateway_panid,gateway_addr,tx_str,sizeof(tx_str),NULL);
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
	digitalWrite(ORANGE_LED,HIGH);
	pinMode(ORANGE_LED,OUTPUT);
	digitalWrite(BLUE_LED,HIGH);
	pinMode(BLUE_LED,OUTPUT);

#ifdef DEBUG
	Serial.begin(115200);
	Serial.print("Program version: ");
	Serial.println_long((long)OTA.getVersion(),DEC);
#endif

	SubGHz.init();
	ct_init();
	mode = GW_SEARCH_MODE;
}

void loop() {
	// put your main code here, to run repeatedly:
	level = voltage_check(VLS_3_068);
	switch (mode) {
	case GW_SEARCH_MODE:			// search gateway
#ifdef DEBUG
		Serial.println("GW SEARCH MODE");
#endif
		if (gw_search()) {			// found gateway successfully
			if (my_short_addr != 0xffff) SubGHz.setMyAddress(my_short_addr);
			state_func = func_init_state();
			sleep_interval = DEFAULT_SLEEP_INTERVAL;
			mode = NORMAL_MODE;
		}
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
		if (param_update()) {
			if (my_short_addr != 0xffff) SubGHz.setMyAddress(my_short_addr);
			state_func = func_init_state();
			mode = NORMAL_MODE;
		} else {
			mode = GW_SEARCH_MODE;
		}
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
