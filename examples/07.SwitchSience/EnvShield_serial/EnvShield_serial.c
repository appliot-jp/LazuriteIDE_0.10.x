#include "EnvShield_serial_ide.h"		// Additional Header

/* FILE NAME: Serial_Echo.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015  Lapis Semiconductor Co.,Ltd.
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
#define SUBGHZ_HOST_ADDR	0x7fba
#define INTERVAL			10*1000



uint8_t i2c_addr = 0x76;        //I2C Address
uint8_t osrs_t = 1;             //Temperature oversampling x 1
uint8_t osrs_p = 1;             //Pressure oversampling x 1
uint8_t osrs_h = 1;             //Humidity oversampling x 1
uint8_t bme280mode = 1;         //Normal mode
uint8_t t_sb = 5;               //Tstandby 1000ms
uint8_t filter = 0;             //Filter off
uint8_t spi3w_en = 0;           //3-wire SPI Disable
bool timer_flag=false;

uint8_t txdata[256];

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
  
  wait_event(&timer_flag);
  timer_flag = false;
  bme280.setMode(i2c_addr, osrs_t, osrs_p, osrs_h, bme280mode, t_sb, filter, spi3w_en);
  bme280.readData(&temp_act, &press_act, &hum_act);
  rc = rpr0521rs.get_oneShot(NULL, &als_val);

  Print.init(txdata,sizeof(txdata));
//  Print.p("T=");
  Print.d(temp_act,2);
//  Print.p(",P=");
  Print.p(",");
  Print.d(press_act,2);
//  Print.p(",H=");
  Print.p(",");
  Print.d(hum_act,2);
//  Print.p(",L=");
  Print.p(",");
  Print.d(als_val,0);
  Print.ln();

//  Serial.print(txdata);
//  SubGHz.begin(SUBGHZ_CH,SUBGHZ_PANID,SUBGHZ_BPS,SUBGHZ_PWR);
//  SubGHz.send(SUBGHZ_PANID,SUBGHZ_HOST_ADDR,txdata,Print.len(),NULL);
//  SubGHz.close();
  Serial.println(txdata);

//  delay(1000);

}
