/* FILE NAME: bm1422.c
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
#include "bm1422.h"

// 初期化処理を行う関数
// コンストラクタは　クラス名::クラス名と同じ名前で構成します
static uint8_t device_address = BM1422_DEVICE_ADDRESS_0E;

static byte bm1422_write(unsigned char memory_address, unsigned char *data, unsigned char size)
{
  byte rc;
  
  Wire.beginTransmission(device_address);
  Wire.write_byte(memory_address);
  Wire.write(data, size);
  rc = Wire.endTransmission(true);
  return (rc);
}

static byte bm1422_read(unsigned char memory_address, unsigned char *data, int size)
{
  byte rc;
  unsigned char cnt;
 
  Wire.beginTransmission(device_address);
  Wire.write_byte(memory_address);
  rc = Wire.endTransmission(false);
  if (rc != 0) {
    return (rc);
  }

  Wire.requestFrom(device_address, size, true);
  cnt = 0;
  while(Wire.available()) {
    data[cnt] = Wire.read();
    cnt++;
  }

  return (rc);
}


static byte bm1422_init(int slave_address)
{
  byte rc;
  unsigned char reg;
  unsigned char buf[2];

 	device_address = (slave_address == BM1422_DEVICE_ADDRESS_0F) ? BM1422_DEVICE_ADDRESS_0F :BM1422_DEVICE_ADDRESS_0E ;
  rc = bm1422_read(BM1422_WIA, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access BM1422");
    return (rc);
  } 
  Serial.print("BM1422_WHO_AMI Register Value = 0x");
  Serial.println_long(reg, HEX);
  
  if (reg != BM1422_WIA_VAL) {
    Serial.println("Can't find BM1422");
    return (rc);
  }

  // Step1
  reg = 0xC2;
  rc = bm1422_write(BM1422_CNTL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write BM1422_CNTL1 Register");
    return (rc);
  }

  buf[0] = 0x00;
  buf[1] = 0x00;
  rc = bm1422_write(BM1422_CNTL4, buf, sizeof(buf));
  if (rc != 0) {
    Serial.println("Can't write BM1422_CNTL4 Register");
    return (rc);
  }

  // Step2
  reg = 0x08;
  rc = bm1422_write(BM1422_CNTL2, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write BM1422_CNTL2 Register");
    return (rc);
  }

  // Step3
  
  return (rc);  
}

static byte bm1422_get_rawval(unsigned char *data)
{
  byte rc;
  unsigned char reg;

  // Step4
  reg = 0x40;
  rc = bm1422_write(BM1422_CNTL3, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write BM1422_CNTL3 Register");
    return (rc);
  }

  rc = bm1422_read(BM1422_DATAX, data, 6);
  if (rc != 0) {
    Serial.println("Can't get BM1422 magnet values");
  }   

  return (rc);  
}

static byte bm1422_get_val(float *data)
{
  byte rc;
  unsigned char val[6];
  signed short mag[3];

  rc = bm1422_get_rawval(val);
  if (rc != 0) {
    return (rc);
  } 

  mag[0] = ((signed short)val[1] << 8) | (val[0]);
  mag[1] = ((signed short)val[3] << 8) | (val[2]);
  mag[2] = ((signed short)val[5] << 8) | (val[4]);

  data[0] = (float)mag[0];
  data[1] = (float)mag[1];
  data[2] = (float)mag[2];

  return (rc);  
}

const t_BM1422 bm1422 =
{
	bm1422_init,
	bm1422_get_val,
};
