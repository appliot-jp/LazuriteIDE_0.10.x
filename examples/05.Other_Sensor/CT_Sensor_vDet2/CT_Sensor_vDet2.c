#include "CT_Sensor_vDet2_ide.h"		// Additional Header

/* FILE NAME: CT_Sensor.c
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
#define SETTING_MODE			( 1 )
#define NORMAL_MODE				( 2 )
#define FW_UPD_MODE				( 3 )
#define DEFAULT_SLEEP_INTERVAL	( 5 * 1000ul )
#define MAX_SLEEP_INTERVAL		( 3599 * 1000ul )
#define BROADCAST_INTERVAL		( 10 * 1000ul )
#define FW_UPD_MODE_TIMEOUT		( 300 * 1000ul )

uint8_t txbuf[MAX_BUF_SIZE], mode;
double thrs_high_amp=0.03;		// unit mA
double thrs_low_amp=0.03;
uint16_t thrs_high_interval_sec=0;
uint16_t thrs_low_interval_sec=15;
uint32_t thrs_high_start=0;
uint32_t thrs_low_start=0;
uint32_t sleep_interval=DEFAULT_SLEEP_INTERVAL;
uint32_t prev_send_time;
uint16_t gateway_addr=0xffff;
uint16_t gateway_panid=0xffff;
bool send_data_flag=false;
bool debug_flag=false;

__packed typedef struct {
	uint8_t setting_flag:1;
	uint8_t fw_update_flag:1;
	uint16_t sleep_interval_sec;	// unit sec
} EACK_DATA;

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

static void setting_mode(void)
{
	int rx_len,i;
	SUBGHZ_MAC_PARAM mac;
	uint8_t *p[10];
	bool setting_done = false;

	SubGHz.begin(SUBGHZ_CH,0xffff,BAUD,PWR);
	SubGHz.rxEnable(NULL);
	do {
		Print.init(txbuf,sizeof(txbuf));
		Print.p("GET_PARAMETER");
		digitalWrite(TX_LED,LOW);
		SubGHz.send(0xffff,0xffff,txbuf,Print.len(),NULL);		// broadcast
		digitalWrite(TX_LED,HIGH);
		prev_send_time = millis();
		while (1) {
			rx_len = SubGHz.readData(txbuf,MAX_BUF_SIZE);
			if (rx_len > 0) {
				SubGHz.decMac(&mac,txbuf,rx_len);
// payload : "'activate'/'debug',(panid),(shortaddr),(id),(thrs_high_amp),
//				(thrs_high_interval_sec),(thrs_low_amp),(thrs_low_interval_sec)"
				for (i=0;;i++) {
					if (i == 0) {
						p[i] = strtok(mac.payload, ",");
					} else {
						p[i] = strtok(NULL, ",");
					}
					if (p[i] == NULL) break;
				}
				if ((i != 4) && (i != 8)) break;
				if (strncmp(p[0],"activate",8) == 0) {
					gateway_panid = (uint16_t)strtoul(p[1],NULL,0);
					gateway_addr = (uint16_t)strtoul(p[2],NULL,0);
					thrs_high_amp = strtod(p[4],NULL);
					thrs_high_interval_sec = (uint16_t)strtoul(p[5],NULL,0);
					thrs_low_amp = strtod(p[6],NULL);
					thrs_low_interval_sec = (uint16_t)strtoul(p[7],NULL,0);
					send_data_flag = true;	// forcibly sending data, because parameter might be changed
					debug_flag = false;
					setting_done = true;
					break;
				} else if (strncmp(p[0],"debug",5) == 0) {
					gateway_panid = (uint16_t)strtoul(p[1],NULL,0);
					gateway_addr = (uint16_t)strtoul(p[2],NULL,0);
					debug_flag = true;
					setting_done = true;
					break;
				} else {
					// do nothing
				}
			}
			if ((millis() - prev_send_time) > BROADCAST_INTERVAL ) {
				prev_send_time = 0;
				break;
			}
		}
	} while (!setting_done);
	SubGHz.close();
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

static void check_enhance_ack(void)
{
	EACK_DATA *eack_data;
	uint8_t *p;
	int eack_size;

	SubGHz.getEnhanceAck(&p,&eack_size);
	eack_data = (EACK_DATA *)p;
	if (eack_size == sizeof(EACK_DATA)) {
		if (eack_data->setting_flag) mode = SETTING_MODE;
		if (eack_data->fw_update_flag) mode = FW_UPD_MODE;		// higher priority
		sleep_interval = eack_data->sleep_interval_sec * 1000ul;
	}
}

static void ct_sensor_main(void)
{
	static double amps;
	static double prev_amps=0;
	static uint8_t current_level=LOW;
	uint32_t current_time;
	uint8_t level;
	SUBGHZ_MSG msg;

	level = voltage_check(VLS_4_667);	

	if (level >= 10) {
		// start measuring
		amps = ct_meas();

		current_time = millis();
		if (debug_flag || ((prev_send_time != 0) && ((current_time - prev_send_time) >= MAX_SLEEP_INTERVAL))) {
			send_data_flag = true;
		} else {
			if ((current_level == LOW) && (prev_amps <= thrs_high_amp) && (amps > thrs_high_amp)) {
				thrs_high_start = millis();
				thrs_low_start = 0;
			} else if ((current_level == HIGH) && (prev_amps >= thrs_low_amp) && (amps < thrs_low_amp)) {
				thrs_low_start = millis();
				thrs_high_start = 0;
			} else {
				// do nothing
			}
			prev_amps = amps;
			if (thrs_high_start) {
				if (amps < thrs_high_amp) {
					thrs_high_start = 0;
				} else if ((current_time-thrs_high_start) >= (thrs_high_interval_sec * 1000ul)) {
					current_level = HIGH;
					send_data_flag = true;
				} else {
					// do nothing
				}
			} else if (thrs_low_start) {
				if (amps > thrs_low_amp) {
					thrs_low_start = 0;
				} else if ((current_time-thrs_low_start) >= (thrs_low_interval_sec * 1000ul)) {
					current_level = LOW;
					send_data_flag = true;
				} else {
					// do nothing
				}
			} else {
				// do nothing
			}
		}
		if (send_data_flag) {
			send_data_flag = false;
			Print.init(txbuf,sizeof(txbuf));
			Print.d(amps,2);
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
				mode = SETTING_MODE;
			}
			thrs_high_start = 0;
			thrs_low_start = 0;
		}
	} else {
		thrs_high_start = 0;
		thrs_low_start = 0;
	}
}

static void fw_update_mode(void)
{
	uint16_t src_addr;
	uint8_t hw_type;
	uint8_t *s,*d;
	SUBGHZ_MAC_PARAM mac;
	SUBGHZ_MSG msg;
	int rx_len;
	uint32_t timemout_cnt_start = 0;

	Print.init(txbuf,sizeof(txbuf));
	Print.p("FW_UPD_READY");

	digitalWrite(TX_LED,LOW);
	SubGHz.begin(SUBGHZ_CH,gateway_panid,BAUD,PWR);
	msg = SubGHz.send(gateway_panid,gateway_addr,txbuf,Print.len(),NULL);
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
					if (strncmp(s,"FW_UPD_START",12) == 0) {
						d = strtok(NULL,",");	// hw_type
						s = strtok(NULL,",");	// name
						hw_type = (uint8_t)strtol(d,NULL,0);

						if ((OTA.getHwType() == hw_type) && \
							(strncmp(s,ota_param.name,OTA_PRGM_NAME_SIZE) == 0)) {
							d = strtok(NULL,",");
							ota_param.ver = (uint8_t)strtol(d,NULL,0);
							ota_param.hostAddr = gateway_addr;
						}
					}
				}
			}
			if (prev_send_time != 0) {
				if ((millis() - prev_send_time) > FW_UPD_MODE_TIMEOUT) {
					prev_send_time = 0;
					break;
				}
			}
		}
		SubGHz.close();
	}
	SubGHz.setKey(NULL);	// timed out
}

void setup() {
	// put your setup code here, to run once:
	SUBGHZ_PARAM param;

	ct_init();
	
	digitalWrite(MEAS_LED,HIGH);
	pinMode(MEAS_LED,OUTPUT);
	digitalWrite(TX_LED,HIGH);
	pinMode(TX_LED,OUTPUT);
  
	SubGHz.init();
	SubGHz.getSendMode(&param);
	param.txRetry = 10;
	SubGHz.setSendMode(&param);

	Serial.begin(115200);
	mode = SETTING_MODE;
}

void loop() {
	// put your main code here, to run repeatedly:

	switch (mode) {
	case SETTING_MODE:			// search gateway
		Serial.println("SETTING MODE");
		setting_mode();
		mode = NORMAL_MODE;
		break;
	case NORMAL_MODE:
		Serial.println("NORMAL MODE");
		ct_sensor_main();
		if (debug_flag) sleep_interval = DEFAULT_SLEEP_INTERVAL;
		if (mode != SETTING_MODE) {
			Serial.print("go to sleep : ");
			Serial.println_long(sleep_interval,DEC);
			sleep(sleep_interval);
		}
		break;
	case FW_UPD_MODE:
		Serial.println("FW UPDATE MODE");
		fw_update_mode();
		mode = NORMAL_MODE;		// return to normal mode, because fw update failed
		break;
	default:
		break;
	}
}
