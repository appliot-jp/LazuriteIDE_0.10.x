/* FILE NAME: KX022.c
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
#include "lazurite.h"
#include "KX022.h"

static uint8_t _device_address;
static unsigned short _g_sens;

byte kx022_write(unsigned char memory_address, unsigned char *data, uint8_t size)
{
  byte rc;

  Wire.beginTransmission(_device_address);
  Wire.write_byte(memory_address);
  Wire.write(data, size);
  rc = Wire.endTransmission(true);
  return (rc);
}

byte kx022_read(unsigned char memory_address, unsigned char *data, uint8_t size)
{
  byte rc;
  unsigned char cnt;

  Wire.beginTransmission(_device_address);
  Wire.write_byte(memory_address);
  rc = Wire.endTransmission(false);
  if (rc != 0) {
    return (rc);
  }
  Wire.requestFrom(_device_address, size, true);
  cnt = 0;
  while(Wire.available()) {
  	data[cnt] = (uint8_t)Wire.read();
    cnt++;
  }

  return (0);
}


byte kx022_init(uint8_t slave_address)
{
  byte rc;
  unsigned char reg;
  unsigned char gsel;
  int i;
	
	_device_address = slave_address;

  rc = kx022_read(KX022_WHO_AM_I, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access KX022");
    return (rc);
  } 
  Serial.print("KX022_WHO_AMI Register Value = 0x");
  Serial.println_long((long)reg, HEX);
  
  if (reg != KX022_WAI_VAL) {
    Serial.println("Can't find KX022");
    return (rc);
  }

  reg = KX022_CNTL1_VAL;
  rc = kx022_write(KX022_CNTL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX022 CNTL1 register at first");
    return (rc);
  }

  reg = KX022_ODCNTL_VAL;
  rc = kx022_write(KX022_ODCNTL, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX022 ODCNTL register");
    return (rc);
  }

  rc = kx022_read(KX022_CNTL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't read KX022 CNTL1 register");
    return (rc);
  }
  gsel = reg & KX022_CNTL1_GSELMASK;

  reg |= KX022_CNTL1_PC1;
  rc = kx022_write(KX022_CNTL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX022 CNTL1 register at second");
    return (rc);
  }
  
  switch(gsel) {
    case KX022_CNTL1_GSEL_2G : _g_sens = 16384; break;
    case KX022_CNTL1_GSEL_4G : _g_sens = 8192;  break;
    case KX022_CNTL1_GSEL_8G : _g_sens = 4096;  break;
    default: break;
  }
}

byte kx022_get_rawval(unsigned char *data)
{
  byte rc;

  rc = kx022_read(KX022_XOUT_L, data, 6);
  if (rc != 0) {
    Serial.println("Can't get KX022 accel value");
  }

  return (rc);
}

byte kx022_get_val(float *data)
{
  byte rc;
  unsigned char val[6];
  signed short acc[3];

  rc = kx022_get_rawval(val);
  if (rc != 0) {
    return (rc);
  }

  acc[0] = ((signed short)val[1] << 8) | (val[0]);
  acc[1] = ((signed short)val[3] << 8) | (val[2]);
  acc[2] = ((signed short)val[5] << 8) | (val[4]);

  // Convert LSB to g
  data[0] = (float)acc[0] / _g_sens;
  data[1] = (float)acc[1] / _g_sens;
  data[2] = (float)acc[2] / _g_sens;

  return (rc);  
}



const t_KX022 kx022 = {
	kx022_init,
	kx022_get_val,
};

