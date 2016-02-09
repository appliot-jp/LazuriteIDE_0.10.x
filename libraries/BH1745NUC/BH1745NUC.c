/* FILE NAME: BH1745NUC.C
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
#include "BH1745NUC.h"

static uint8_t _device_address;


byte bh1745_write(unsigned char memory_address, unsigned char *data, unsigned char size)
{
  byte rc;

  Wire.beginTransmission(_device_address);
  Wire.write_byte(memory_address);
  Wire.write(data, size);
  rc = Wire.endTransmission(true);
  return (rc);
}

byte bh1745_read(unsigned char memory_address, uint8_t *data, uint8_t size)
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

byte bh1745_init(uint8_t slave_address)
{
  byte rc;
  unsigned char reg;
	
	_device_address = slave_address;

  rc = bh1745_read(BH1745NUC_SYSTEM_CONTROL, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access BH1745NUC");
    return (rc);
  }
	reg = (uint8_t)(reg & 0x3F);
  Serial.print("BH1745NUC Part ID Value = ");
  Serial.println_long((long)reg, HEX);

  if (reg != BH1745NUC_PART_ID_VAL) {
    Serial.println("Can't find BH1745NUC");
    return (rc);
  }

  rc = bh1745_read(BH1745NUC_MANUFACTURER_ID, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access BH1745NUC");
    return (rc);
  }
  Serial.print("BH1745NUC MANUFACTURER ID Register Value = ");
  Serial.println_long((long)reg, HEX);

  if (reg != BH1745NUC_MANUFACT_ID_VAL) {
    Serial.println("Can't find BH1745NUC");
    return (rc);
  }

  reg = BH1745NUC_MODE_CONTROL1_VAL;
  rc = bh1745_write(BH1745NUC_MODE_CONTROL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write BH1745NUC MODE_CONTROL1 register");
    return (rc);
  }

  reg = BH1745NUC_MODE_CONTROL2_VAL;
  rc = bh1745_write(BH1745NUC_MODE_CONTROL2, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write BH1745NUC MODE_CONTROL2 register");
    return (rc);
  }

  reg = BH1745NUC_MODE_CONTROL3_VAL;
  rc = bh1745_write(BH1745NUC_MODE_CONTROL3, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write BH1745NUC MODE_CONTROL3 register");
    return (rc);
  }

}

byte bh1745_get_rawval(unsigned char *data)
{
  byte rc;

  rc = bh1745_read(BH1745NUC_RED_DATA_LSB, data, 8);
  if (rc != 0) {
    Serial.println("Can't get BH1745NUC RGBC value");
  }

  return (rc);
}

byte bh1745_get_val(unsigned short *data)
{
  byte rc;
  unsigned char val[8];

  rc = bh1745_get_rawval(val);
  if (rc != 0) {
    return (rc);
  }

  data[0] = ((unsigned short)val[1] << 8) | val[0];
  data[1] = ((unsigned short)val[3] << 8) | val[2];
  data[2] = ((unsigned short)val[5] << 8) | val[4];
  data[3] = ((unsigned short)val[7] << 8) | val[6];

  return (rc);
}

const t_BH1745NUC bh1745 = {
	bh1745_init,
	bh1745_get_val
};
