/*****************************************************************************
  KX224.c

 Copyright (c) 2017 LAPIS Semiconductor Co.,Ltd.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
******************************************************************************/
#include <Lazurite.h>
#include <Wire.h>
#include <KX224.h>

static int _device_address;
static uint16_t _g_sens;
static uint8_t KX224_write(unsigned char memory_address, unsigned char *data, unsigned char size)
{
  byte rc;

  Wire.beginTransmission(_device_address);
  Wire.write_byte(memory_address);
  Wire.write(data, size);
  rc = Wire.endTransmission(true);
  return (rc);
}

uint8_t KX224_read(unsigned char memory_address, unsigned char *data, int size)
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
    data[cnt] = Wire.read();
    cnt++;
  }

  return (0);
}

uint8_t KX224_init(int slave_address)
{
  byte rc;
  unsigned char reg;
  unsigned char gsel;

  _device_address = slave_address;
  _g_sens = 4096;

	rc = KX224_read(KX224_WHO_AM_I, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access KX224");
    return (rc);
  } 
  Serial.print("KX224_WHO_AMI Register Value = 0x");
  Serial.println_long(reg, HEX);
  
  if (reg != KX224_WAI_VAL) {
    Serial.println("Can't find KX224");
    return (rc);
  }

  reg = KX224_CNTL1_VAL;
  rc = KX224_write(KX224_CNTL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX224 CNTL1 register at first");
    return (rc);
  }

  reg = KX224_ODCNTL_VAL;
  rc = KX224_write(KX224_ODCNTL, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX224 ODCNTL register");
    return (rc);
  }

  rc = KX224_read(KX224_CNTL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't read KX224 CNTL1 register");
    return (rc);
  }
  gsel = reg & KX224_CNTL1_GSELMASK;

  reg |= KX224_CNTL1_PC1;
  rc = KX224_write(KX224_CNTL1, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write KX224 CNTL1 register at second");
    return (rc);
  }
  
  switch(gsel) {
    case KX224_CNTL1_GSEL_8G :
      // (Equivalent counts) / (Range) = (32768 / 8)
      _g_sens = 4096;
    break;

    case KX224_CNTL1_GSEL_16G :
      // (Equivalent counts) / (Range) = (32768 / 16)
      _g_sens = 2048;
    break;

    case KX224_CNTL1_GSEL_32G :
      // (Equivalent counts) / (Range) = (32768 / 32)
      _g_sens = 1024;
    break;

    default:
    break;
  }
  return (rc);
}

byte KX224_get_rawval(unsigned char *data)
{
  byte rc;

  rc = KX224_read(KX224_XOUT_L, data, 6);
  if (rc != 0) {
    Serial.println("Can't get KX224 accel value");
  }

  return (rc);
}

byte KX224_get_val(float *data)
{
  byte rc;
  unsigned char val[6];
  signed short acc[3];

  rc = KX224_get_rawval(val);
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

const t_KX224 kx224 = {
	KX224_init,
	KX224_get_rawval,
	KX224_get_val,
	KX224_write,
	KX224_read
};

