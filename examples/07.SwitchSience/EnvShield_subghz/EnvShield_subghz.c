#include "EnvShield_subghz_ide.h"		// Additional Header

/* FILE NAME: EnvShield_subghz.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
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

#define SUBGHZ_CH			36
#define SUBGHZ_PANID		0xabcd
#define SUBGHZ_BPS			100
#define SUBGHZ_PWR			20
#define SUBGHZ_HOST_ADDR	0xFFFF
#define INTERVAL			( 5 * 1000L )

uint8_t i2c_addr = 0x76;        //I2C Address
uint8_t osrs_t = 1;             //Temperature oversampling x 1
uint8_t osrs_p = 1;             //Pressure oversampling x 1
uint8_t osrs_h = 1;             //Humidity oversampling x 1
uint8_t bme280mode = 1;         //Normal mode
uint8_t t_sb = 5;               //Tstandby 1000ms
uint8_t filter = 0;             //Filter off
uint8_t spi3w_en = 0;           //3-wire SPI Disable
uint8_t txdata[256];
bool timer_flag = false;


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


void timer_isr(void)
{
  timer_flag = true;
}

void setup() {
  // put your setup code here, to run once:
  byte rc;
   
  timer2.set(INTERVAL,timer_isr);
  timer2.start();

  Serial.begin(115200);
  SubGHz.init();
  Wire.begin();

  bme280.setMode(i2c_addr, osrs_t, osrs_p, osrs_h, bme280mode, t_sb, filter, spi3w_en);
  bme280.readTrim();
  rc = rpr0521rs.init();							// not standby mode after this process;
  rc = rpr0521rs.get_oneShot(NULL, NULL);			// set standby after data is read
}

void loop() {
  // put your main code here, to run repeatedly:
  byte rc;
  double temp_act, press_act, hum_act; //最終的に表示される値を入れる変数
  unsigned short ps_val;
  float als_val;
  uint8_t level;
  
  wait_event(&timer_flag);

  bme280.setMode(i2c_addr, osrs_t, osrs_p, osrs_h, bme280mode, t_sb, filter, spi3w_en);
  bme280.readData(&temp_act, &press_act, &hum_act);
  rc = rpr0521rs.get_oneShot(NULL, &als_val);
  level = voltage_check(VLS_4_667);
  
  Print.init(txdata,sizeof(txdata));
  Print.d(temp_act,2);
  Print.p(",");
  Print.d(press_act,2);
  Print.p(",");
  Print.d(hum_act,2);
  Print.p(",");
  Print.d(als_val,0);
  Print.p(",");
  Print.p(vls_val[level]);

  Print.ln();

  SubGHz.begin(SUBGHZ_CH,SUBGHZ_PANID,SUBGHZ_BPS,SUBGHZ_PWR);
  SubGHz.send(SUBGHZ_PANID,SUBGHZ_HOST_ADDR,txdata,Print.len(),NULL);
  SubGHz.close();

  Serial.print(txdata);
}
