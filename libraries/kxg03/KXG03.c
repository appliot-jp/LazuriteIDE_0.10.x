/* FILE NAME: KXG03.c
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
#include "KXG03.h"

// 初期化処理を行う関数
// コンストラクタは　クラス名::クラス名と同じ名前で構成します
static uint8_t device_address = KXG03_DEVICE_ADDRESS_4E;

static byte kxg03_write(unsigned char memory_address, unsigned char *data, unsigned char size)
{
  byte rc;
  
  Wire.beginTransmission(device_address);
  Wire.write_byte(memory_address);
  Wire.write(data, size);
  rc = Wire.endTransmission(true);
  return (rc);
}


static byte kxg03_read(unsigned char memory_address, unsigned char *data, int size)
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

static byte kxg03_init(uint8_t dev_addr)
{
  byte rc;
  unsigned char reg;
	
	if(dev_addr == KXG03_DEVICE_ADDRESS_4F)
	{
		device_address = KXG03_DEVICE_ADDRESS_4F;
	}
  rc = kxg03_read(KXG03_WHO_AM_I, &reg, sizeof(reg));
  if (rc != 0) {
  	Serial.print_long(rc,DEC);
  	while(1){}
    Serial.println("Can't access KXG03");
    return (rc);
  } 
  Serial.print("KXG03_WHO_AM_I Register Value = 0x");
  Serial.println_long(reg, HEX);
  
  if (reg != KXG03_WHO_AM_I_VAL) {
    Serial.println("Can't find KXG03");
    // return (rc);
  }

  // rc = write(KXG03_ACCEL_ODR_WAKE, &reg, sizeof(reg));
  reg = 0xEC;
  rc = kxg03_write(KXG03_STDBY, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KXG03 STDBY Register");
    return (rc);
  } 

  return (rc);  
}

static byte kxg03_get_rawval(unsigned char *data)
{
  byte rc;
	
  rc = kxg03_read(KXG03_GYRO_XOUT_L, data, 12);
  if (rc != 0) {
    Serial.println("Can't get KXG03 accel value");
  } 

  return (rc);  
}

static byte kxg03_get_val(float *data)
{
  byte rc;
  unsigned char val[12];
  signed short acc[3];
  signed short gyro[3];

  rc = kxg03_get_rawval(val);
  if (rc != 0) {
    return (rc);
  } 

  gyro[0] = ((signed short)val[1] << 8) | (val[0]);
  gyro[1] = ((signed short)val[3] << 8) | (val[2]);
  gyro[2] = ((signed short)val[5] << 8) | (val[4]);

  acc[0] = ((signed short)val[7] << 8) | (val[6]);
  acc[1] = ((signed short)val[9] << 8) | (val[8]);
  acc[2] = ((signed short)val[11] << 8) | (val[10]);

  data[0] = (float)gyro[0]/128;
  data[1] = (float)gyro[1]/128;
  data[2] = (float)gyro[2]/128;
  data[3] = (float)acc[0]/16384;
  data[4] = (float)acc[1]/16384;
  data[5] = (float)acc[2]/16384;

  return (rc);  
}

const t_KXG03 kxg03 =
{
	kxg03_init,
	kxg03_get_val,
};

