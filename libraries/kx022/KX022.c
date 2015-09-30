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


#include "kx022.h"
#include "wire.h"
static unsigned char device_address;

static byte kx022_write(unsigned char memory_address, unsigned char *data, unsigned char size)
{
  byte rc;
  
  Wire.beginTransmission(device_address);
  Wire.write_byte(memory_address);
  Wire.write(data, size);
  rc = Wire.endTransmission(true);
  return (rc);
}

static byte kx022_read(unsigned char memory_address, unsigned char *data, int size)
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
    if (data[cnt] < 0) {
      return (-1);
    }
    cnt++;
  }
  // rc = Wire.endTransmission(true);
  return (0);
}

static byte kx022_init(int slave_address)
{
  byte rc;
  unsigned char reg;
  int i;
	
	device_address = (slave_address == KX022_DEVICE_ADDRESS_1F) ? (unsigned char)KX022_DEVICE_ADDRESS_1F: (unsigned char)KX022_DEVICE_ADDRESS_1E;
	
  rc = kx022_read(KX022_WHO_AM_I, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access KX022");
    return (rc);
  } 
  Serial.print("KX022_WHO_AMI Register Value = 0x");
 Serial.println_long(reg, HEX);
  
  if (reg != KX022_WAI_VAL) {
    Serial.println("Can't find KX022");
    return (rc);
  }

  reg = 0x41;
  rc = kx022_write(KX022_CNTL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX022 CNTL1 register at first");
    return (rc);
  }

  reg = 0x02;
  rc = kx022_write(KX022_ODCNTL, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX022 ODCNTL register");
    return (rc);
  }

  reg = 0xD8;
  rc = kx022_write(KX022_CNTL3, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX022 CNTL3 register");
    return (rc);
  }  

  reg = 0x01;
  rc = kx022_write(KX022_TILT_TIMER, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX022 TILT_TIMER register");
    return (rc);
  }

  reg = 0xC1;
  rc = kx022_write(KX022_CNTL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX022 CNTL1 register at second");
    return (rc);
  }
  
}

static byte kx022_get_rawval(unsigned char *data)
{
  byte rc;

  rc = kx022_read(KX022_XOUT_L, data, 6);
  if (rc != 0) {
    Serial.println("Can't get KX022 accel value");
  }   

  return (rc);  
}

static byte kx022_get_val(float *data)
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
/*
  Serial.print("KX022 (X) = ");
  Serial.println(acc[0]);
  Serial.print("KX022 (Y) = ");
  Serial.println(acc[1]);
  Serial.print("KX022 (Z) = ");
  Serial.println(acc[2]);
*/
  data[0] = (float)acc[0] / 16384;
  data[1] = (float)acc[1] / 16384;
  data[2] = (float)acc[2] / 16384;

  return (rc);  
}

const t_KX022 kx022 = {
	kx022_init,
	kx022_get_val,
};

