/* FILE NAME: BH1790GLC.c
 *
 * Copyright (c) 2018  Lapis Semiconductor Co.,Ltd.
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
#include "BH1790GLC.h"

static byte bh1790glc_init(void);
static byte bh1790glc_get_rawval(unsigned char *data);
static byte bh1790glc_get_val(unsigned short *data);
static byte bh1790glc_write(unsigned char memory_address, unsigned char *data, unsigned char size);
static byte bh1790glc_read(unsigned char memory_address, unsigned char *data, int size);

static byte bh1790glc_init(void)
{
  byte rc;
  unsigned char reg;
  unsigned char val[3];

  rc = bh1790glc_read(BH1790GLC_PART_ID, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access BH1790GLC");
    return (rc);
  }
  Serial.print("BH1790GLC Part ID Value = 0x");
  Serial.println_long(reg, HEX);

  if (reg != BH1790GLC_PID_VAL) {
    Serial.println("Can't find BH1790GLC");
    return (rc);
  }

  rc = bh1790glc_read(BH1790GLC_MANUFACTURER_ID, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access BH1790GLC");
    return (rc);
  }
  Serial.print("BH1790GLC MANUFACTURER ID Register Value = 0x");
  Serial.println_long(reg, HEX);

  if (reg != BH1790GLC_MID_VAL) {
    Serial.println("Can't find BH1790GLC");
    return (rc);
  }

  val[0] = BH1790GLC_MEAS_CONTROL1_VAL;
  val[1] = BH1790GLC_MEAS_CONTROL2_VAL;
  val[2] = BH1790GLC_MEAS_START_VAL;  
  rc = bh1790glc_write(BH1790GLC_MEAS_CONTROL1, val, sizeof(val));
  if (rc != 0) {
    Serial.println("Can't write BH1790GLC MEAS_CONTROL1-MEAS_START register");
  }
  
  return (rc);
}

static byte bh1790glc_get_rawval(unsigned char *data)
{
  byte rc;

  rc = bh1790glc_read(BH1790GLC_DATAOUT_LEDOFF, data, 4);
  if (rc != 0) {
    Serial.println("Can't get BH1790GLC value");
  }

  return (rc);
}

static byte bh1790glc_get_val(unsigned short *data)
{
  byte rc;
  unsigned char val[4];

  rc = bh1790glc_get_rawval(val);
  if (rc != 0) {
    return (rc);
  }

  data[0] = ((unsigned short)val[1] << 8) | (val[0]);
  data[1] = ((unsigned short)val[3] << 8) | (val[2]);

  return (rc);  
}

static byte bh1790glc_write(unsigned char memory_address, unsigned char *data, unsigned char size)
{
  byte rc;

  Wire.beginTransmission(BH1790GLC_DEVICE_ADDRESS);
  Wire.write_byte(memory_address);
  Wire.write(data, size);
  rc = Wire.endTransmission(true);
  return (rc);
}

static byte bh1790glc_read(unsigned char memory_address, unsigned char *data, int size)
{
  byte rc;
  unsigned char cnt;

  Wire.beginTransmission(BH1790GLC_DEVICE_ADDRESS);
  Wire.write_byte(memory_address);
  rc = Wire.endTransmission(false);
  if (rc != 0) {
    return (rc);
  }

  Wire.requestFrom(BH1790GLC_DEVICE_ADDRESS, size, true);
  cnt = 0;
  while(Wire.available()) {
    data[cnt] = Wire.read();
    cnt++;
  }

  return (0);
}

const f_BH1790GLC bh1790glc =
{
	bh1790glc_init,
	bh1790glc_get_rawval,
	bh1790glc_get_val,
	bh1790glc_write,
	bh1790glc_read
};
