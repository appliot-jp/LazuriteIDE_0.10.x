/* FILE NAME: BM1383.c
 *
 * Copyright (c) 2015  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <Wire.h>
#include "BM1383.h"

// 初期化処理を行う関数
// コンストラクタは　クラス名::クラス名と同じ名前で構成します
static uint8_t device_address = BM1383GLV_DEVICE_ADDRESS;
static uint8_t device;		// 0 = BM1383, 1 = BM1383A
static byte bm1383_write(unsigned char memory_address, unsigned char *data, unsigned char size)
{
  byte rc;
  
  Wire.beginTransmission(device_address);
  Wire.write_byte(memory_address);
  Wire.write(data, size);
  rc = Wire.endTransmission(true);
  return (rc);
}

static byte bm1383_read(unsigned char memory_address, unsigned char *data, int size)
{
  byte rc;
  unsigned char cnt;
 
  Wire.beginTransmission(device_address);
  Wire.write_byte(memory_address);
  rc = Wire.endTransmission(false);
  if (rc != 0) {
    return (rc);
  }

  Wire.requestFrom(device_address, size, false);
  cnt = 0;
  while(Wire.available()) {
    data[cnt] = Wire.read();
    cnt++;
  }
  rc = Wire.endTransmission(true);
  return (rc);
}

static byte bm1383_init(int slave_address)
{
  byte rc;
  unsigned char reg;

	device_address = (uint8_t)(slave_address==0)?BM1383GLV_DEVICE_ADDRESS:slave_address;
  rc = bm1383_read(BM1383GLV_ID, &reg, sizeof(reg));
  if (rc != 0) {
//    Serial.println("Can't access BM1383GLV");
    return (rc);
  } 
//    Serial.print("BM1383GL ID Register Value = 0x");
//    Serial.println_long(reg, HEX);
  
	if (reg == BM1383GLV_ID_VAL) {
		device = 0;
	} else if(reg == BM1383AGLV_ID_VAL){
		device = 1;
	} else {
//      Serial.println("Can't find BM1383GLV");
	    return (rc);
	}

  reg = 0x01;
  rc = bm1383_write(BM1383GLV_POWER_DOWN, &reg, sizeof(reg));
  if (rc != 0) {
//    Serial.println("Can't write BM1383GLV POWER_DOWN register");
    return (rc);
  }
  
  delay(1);
  
  reg = 0x01;
  rc = bm1383_write(BM1383GLV_SLEEP, &reg, sizeof(reg));
  if (rc != 0) {
//    Serial.println("Can't write BM1383GLV SLEEP register");
    return (rc);
  }
	switch(device)
	{
	case 0:
		reg = 0xC4;
		break;
	case 1:
		reg = 0xC2;
		break;
	default:
		return(-1);
		break;
	}
  rc = bm1383_write(BM1383GLV_MODE_CONTROL, &reg, sizeof(reg));
  if (rc != 0) {
//    Serial.println("Can't write BM1383GLV MODE_CONTROL register");
    return (rc);
  }
	return (rc);
}

static byte bm1383_get_rawtemppressval(unsigned char *data)
{
  byte rc;

  rc = bm1383_read(BM1383GLV_TEMPERATURE_MSB, data, 5);
  if (rc != 0) {
//    Serial.println("Can't get BM1383GLV TEMP/PRESS value");
  }   

  return (rc);  
}

static byte bm1383_get_temppressval(float *temp, float *press)
{
  byte rc;
  unsigned char val[5];
  signed short rawtemp;
  unsigned long rawpress;

  rc = bm1383_get_rawtemppressval(val);
  if (rc != 0) {
    return (rc);
  } 

	switch(device) {
	case 0:
		rawtemp = ((signed short)val[0] << 8) | (val[1]);
		rawpress = (((unsigned long)val[2] << 16) | ((unsigned long)val[3] << 8) | val[4]&0xFC) >> 2;
		break;
	case 1:
		rawpress = (((unsigned long)val[0] << 16) | ((unsigned long)val[1] << 8) | val[2]&0xFC) >> 2;
		rawtemp = ((signed short)val[3] << 8) | (val[4]);
		break;
	default:
		return (-1);
	}

  if (rawpress == 0) {
    return (-1);    
  }

  *temp = (float)rawtemp / 32;
  *press = (float)rawpress / 2048;

  return (rc);  
}

static void bm1383_get_val(float *data) {
	float *result = data;
	bm1383_get_temppressval(&result[0],&result[1]);
	return;
}

static byte bm1383_power_down(void)
{
  byte rc;
  unsigned char reg;

  // stop continuos measurement mode, then make it enter standby mode
  reg = 0x00;
  rc = bm1383_write(BM1383GLV_MODE_CONTROL, &reg, sizeof(reg));
  if (rc != 0) {
//    Serial.println("Can't write BM1383GLV MODE_CONTROL register");
    return (rc);
  }

  // make it enter reset mode
  reg = 0x00;
  rc = bm1383_write(BM1383GLV_SLEEP, &reg, sizeof(reg));
  if (rc != 0) {
//    Serial.println("Can't write BM1383GLV SLEEP register");
    return (rc);
  }

  // make it enter power down mode
  reg = 0x00;
  rc = bm1383_write(BM1383GLV_POWER_DOWN, &reg, sizeof(reg));
  if (rc != 0) {
//    Serial.println("Can't write BM1383GLV POWER_DOWN register");
  }
  return (rc);
}

const t_BM1383 bm1383 =
{
	bm1383_init,
	bm1383_get_val,
	bm1383_get_rawtemppressval,
	bm1383_power_down
};

