/* FILE NAME: WindHack_SubGHz_master.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2017  LAPIS Semiconductor Co.,Ltd.
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

#include "PumpMonitor_ide.h"		// Additional Header
#include "driver_extirq.h"
#include "driver_gpio.h"

#define BLUE_LED				( 26 )
#define LSM9DS1_INT1_AG_DRV_PIN	( 23 )		// pin# 5
#define MAX31855_SPI_SS_PIN		( 10 )
#define FET_PIN					( 6 )
#define BUF_SIZE				( 60 )
#define TXDATA_SIZE				( 100 )
#define SUBGHZ_CH				( 36 )
#define SUBGHZ_PANID			( 0xabcd )
#define SUBGHZ_SEND_ADDR		( 0xffff )	// group cast
#define CHB						( 2 )
#define MEAS					( 3 )
#define MEAS_LED				( 25 )
#define TIMER_INT_MS			( 1000 )
#define CT_COUNT				( 30 )
#define THRMCPL_COUNT			( 15 )
#define SOUND_COUNT				( 1 )
#define EVENT_LSM9DS1_INT		( 0x01 )
#define EVENT_TIMER				( 0x02 )
#define OUTPUT_CT_SENSOR		( 0x01 )
#define OUTPUT_ACC				( 0x02 )
#define OUTPUT_THRMCPL			( 0x04 )
#define OUTPUT_SOUND			( 0x08 )

uint8_t event_flag = 0x0;
bool event = false;
uint8_t txdata[TXDATA_SIZE];
float acc_f[3] = {0};
uint8_t output_flag = 0x0;
float thermocouple_f = 0.0;
float internal_f = 0.0;
double amps = 0;
uint8_t level = 0;
uint32_t snd = 0;
uint8_t ct_count_num = 1;
uint8_t thrmcpl_count_num = 1;
uint8_t snd_count_num = 1;

void int1_ag_isr(void)
{
	event = true;
	event_flag |= EVENT_LSM9DS1_INT;
}

void timer_isr(void)
{
	event = true;
	event_flag |= EVENT_TIMER;
}

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

void ct_init(void) {
	pinMode(MEAS_LED,OUTPUT);
	digitalWrite(MEAS_LED,HIGH);
	pinMode(CHB,OUTPUT);
	pinMode(MEAS,OUTPUT);
	analogReadResolution(12);
}

double ct_meas(void) {
	unsigned long  st_time,en_time;
	double amps;
	volatile int st_voltage,en_voltage,dif_vol;

	digitalWrite(MEAS,HIGH);
	digitalWrite(CHB,HIGH);
	st_voltage = analogRead(A0);
	delayMicroseconds(1000);
	st_voltage = analogRead(A0);
	delayMicroseconds(1000);
	st_voltage = analogRead(A0);
	delayMicroseconds(1000);
	st_voltage = analogRead(A0);
	st_time = millis();
	do {
		delayMicroseconds(1000);
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

void acc_init(void)
{
	Wire.begin();
	LSM9DS1.init(LSM9DS1_OPMODE_XL, LSM9DS1_I2C_ADDR_XLG_6A, NULL);
	LSM9DS1.setAccRange(LSM9DS1_XL_FS_RANGE_2G);
	LSM9DS1.begin(LSM9DS1_ODR_XL_10Hz, NULL);
	drv_pinMode(LSM9DS1_INT1_AG_DRV_PIN, INPUT);
	drv_attachInterrupt(LSM9DS1_INT1_AG_DRV_PIN, 6, int1_ag_isr, RISING, false, false);
}

void thrmcpl_init(void)
{
	SPI.begin();
	pinMode(MAX31855_SPI_SS_PIN, OUTPUT);
	digitalWrite(MAX31855_SPI_SS_PIN, HIGH);	
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
}

uint8_t thrmcpl_update(void)
{
	uint8_t ret = 0;
	unsigned int thermocouple;		// 14-Bit Thermocouple Temperature Data + 2-Bit
	unsigned int internal;			// 12-Bit Internal Temperature Data + 4-Bit

	digitalWrite(MAX31855_SPI_SS_PIN, LOW);					//  Enable the chip
	thermocouple = (unsigned int)SPI.transfer(0x00) << 8;	//  Read high byte thermocouple
	thermocouple |= (unsigned int)SPI.transfer(0x00);		//  Read low byte thermocouple 
	internal = (unsigned int)SPI.transfer(0x00) << 8;		//  Read high byte internal
	internal |= (unsigned int)SPI.transfer(0x00);			//  Read low byte internal 
	digitalWrite(MAX31855_SPI_SS_PIN, HIGH);				//  Disable the chip

	if ((thermocouple & 0x0001) != 0) {			// any error detected
		ret = 1;
		Serial.print("ERROR: ");
		if ((internal & 0x0004) !=0) {
			Serial.print("Short to Vcc, ");
		}
		if ((internal & 0x0002) !=0) {
			Serial.print("Short to GND, ");
		}
		if ((internal & 0x0001) !=0) {
			Serial.print("Open Circuit, ");
		}
	    Serial.println("");
	} else {									// no error
		if ((thermocouple & 0x8000) == 0) {		// 0℃以上   above 0 Degrees Celsius
			thermocouple_f = (thermocouple >> 2) * 0.25;
		} else {								// 0℃未満   below zero
			thermocouple_f = (0x3fff - (thermocouple >> 2) + 1)  * -0.25;
		}
	    if ((internal & 0x8000) == 0) {			// 0℃以上   above 0 Degrees Celsius
			internal_f = (internal >> 4) * 0.0625;
		} else {								// 0℃未満   below zero
			internal_f = (((0xffff - internal) >> 4) + 1)  * -0.0625;
		}
    }
	return ret;
}

void sound_init(void)
{
	pinMode(FET_PIN,OUTPUT);
	digitalWrite(FET_PIN,LOW);
}

void setup() {
  // put your setup code here, to run once:
	Serial.begin(115200);
	pinMode(BLUE_LED,OUTPUT);
	digitalWrite(BLUE_LED,HIGH);

	// SubGHz
	SubGHz.init();

	// CT sensor
	ct_init();

	// Accelerometer
	acc_init();

	// Thermocouple
	thrmcpl_init();

	// Sound detector
//	sound_init();

	timer2.set(TIMER_INT_MS, timer_isr);
	timer2.start();
	event = true;
	event_flag = EVENT_TIMER;
}

void loop() {
  // put your main code here, to run repeatedly:
	wait_event(&event);

	if (event_flag & EVENT_LSM9DS1_INT) {
		event_flag &= ~EVENT_LSM9DS1_INT;

		// Accelerometer
		if (LSM9DS1.availableAcc()) {
			LSM9DS1.getAcc(acc_f);
			output_flag |= OUTPUT_ACC;
		}
	}

	if (event_flag & EVENT_TIMER) {
		event_flag &= ~EVENT_TIMER;

		// CT sensor
		if (ct_count_num >= CT_COUNT) {
			ct_count_num = 1;
			amps = ct_meas();
			level = voltage_check(VLS_4_667);
			output_flag |= OUTPUT_CT_SENSOR;
		} else {
			ct_count_num++;
		}

		// Thermocouple
		if (thrmcpl_count_num >= THRMCPL_COUNT) {
			thrmcpl_count_num = 1;
			if (thrmcpl_update() == 0) {
				output_flag |= OUTPUT_THRMCPL;
			}
		} else {
			thrmcpl_count_num++;
		}

		// Sound detector
/* 		if (snd_count_num >= SOUND_COUNT) {
			snd_count_num = 1;
			digitalWrite(FET_PIN,LOW);
			snd = analogRead(A1);
			digitalWrite(FET_PIN,HIGH);
			output_flag |= OUTPUT_SOUND;
		} else {
			snd_count_num++;
		} */
	}

	if (output_flag) {
		Print.init(txdata, TXDATA_SIZE);

		// CT sensor
		if (output_flag & OUTPUT_CT_SENSOR) {
			output_flag &= ~OUTPUT_CT_SENSOR;
			Print.d(amps, 2);
			Print.p(",");
			Print.p(vls_val[level]);
			Print.p(",");
		} else {
			Print.p(",,");
		}

		// Accelerometer
		if (output_flag & OUTPUT_ACC) {
			output_flag &= ~OUTPUT_ACC;
			Print.f(acc_f[0], 3);
			Print.p(",");
			Print.f(acc_f[1], 3);
			Print.p(",");
			Print.f(acc_f[2], 3);
			Print.p(",");
		} else {
			Print.p(",,,");
		}

		// Thermocouple
		if (output_flag & OUTPUT_THRMCPL) {
			output_flag &= ~OUTPUT_THRMCPL;
			Print.f(thermocouple_f, 2);
			Print.p(",");
			Print.f(internal_f, 4);
			Print.p(",");
		} else {
			Print.p(",,");
		}

		// Sound detector
		if (output_flag & OUTPUT_SOUND) {
			output_flag &= ~OUTPUT_SOUND;
			Print.l(snd, DEC);
		}
		Print.ln();

		Serial.print(txdata);
		digitalWrite(BLUE_LED,LOW);
		SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID, SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);
		SubGHz.send(SUBGHZ_PANID, SUBGHZ_SEND_ADDR, txdata, strlen(txdata), NULL);
		SubGHz.close();
		digitalWrite(BLUE_LED,HIGH);
	}
}
