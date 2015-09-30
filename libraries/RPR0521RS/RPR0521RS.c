
#include <Wire.h>
#include "RPR0521RS.h"

// 初期化処理を行う関数
// コンストラクタは　クラス名::クラス名と同じ名前で構成します
byte RPR0521RS_write(unsigned char memory_address, unsigned char *data, unsigned char size)
{
  byte rc;
  
  Wire.beginTransmission(RPR0521RS_DEVICE_ADDRESS);
  Wire.write_byte(memory_address);
  Wire.write(data, size);
  rc = Wire.endTransmission(true);
  return (rc);
	
	
	
}

byte RPR0521RS_read(unsigned char memory_address, unsigned char *data, int size)
{
  byte rc;
  unsigned char cnt;
 
  Wire.beginTransmission(RPR0521RS_DEVICE_ADDRESS);
  Wire.write_byte(memory_address);
  rc = Wire.endTransmission(false);
  if (rc != 0) {
    return (rc);
  }

  Wire.requestFrom(RPR0521RS_DEVICE_ADDRESS, size, false);
  cnt = 0;
  while(Wire.available()) {
    data[cnt] = Wire.read();
    cnt++;
  }
  rc = Wire.endTransmission(true);
  return (rc);
}

byte RPR0521RS_init(void)
{
  byte rc;
  unsigned char reg;

  rc = RPR0521RS_read(RPR0521RS_SYSTEM_CONTROL, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access RPR0521RS");
    return (rc);
  }
  reg &= 0x3F;
  Serial.print("RPR0521RS Part ID Register Value = 0x");
  Serial.println_long(reg, HEX);
  
  if (reg != RPR0521RS_PART_ID_VAL) {
    Serial.println("Can't find RPR0521RS");
    return (rc);
  }

  rc = RPR0521RS_read(RPR0521RS_MANUFACT_ID, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't access RPR0521RS");
    return (rc);
  }
  Serial.print("RPR0521RS MANUFACT_ID Register Value = 0x");
  Serial.println_long(reg, HEX);
  
  if (reg != RPR0521RS_MANUFACT_ID_VAL) {
    Serial.println("Can't find RPR0521RS");
    return (rc);
  }

  reg = 0xE6;
  rc = RPR0521RS_write(RPR0521RS_MODE_CONTROL, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write RPR0521RS MODE CONTROL register");
    return (rc);
  }

  reg = 0x00;
  rc = RPR0521RS_write(RPR0521RS_ALS_PS_CONTROL, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write RPR0521RS ALS_PS_CONTROL register");
    return (rc);
  }

  reg = 0x20;
  rc = RPR0521RS_write(RPR0521RS_PS_CONTROL, &reg, sizeof(reg));
  if (rc != 0) {
    Serial.println("Can't write RPR0521RS PS_CONTROL register");
  }

  return (rc);  
}

byte RPR0521RS_get_rawpsalsval(unsigned char *data)
{
  byte rc;

  rc = RPR0521RS_read(RPR0521RS_PS_DATA_LSB, data, 6);
  if (rc != 0) {
    Serial.println("Can't get RPR0521RS PS/ALS_DATA value");
  }   

  return (rc);  
}

float RPR0521RS_convert_lux(unsigned short *data)
{
  float lux;
  float raito;
  
  raito = (float)data[1] / data[0];
    
  if(raito < 0.595){
    lux = (1.682*(float)data[0] - 1.877*(float)data[1]);
  }
  else if(raito < 1.015){
    lux = (0.644*(float)data[0] - 0.132*(float)data[1]);
  }
  else if(raito < 1.352){
    lux = (0.756*(float)data[0] - 0.243*(float)data[1]);
  }
  else if(raito < 3.053){
    lux = (0.766*(float)data[0] - 0.25*(float)data[1]);
  }
  else{
    lux = 0;
  }  
  
  return (lux);
}



byte RPR0521RS_get_psalsval(unsigned short *ps, float *als)
{
  byte rc;
  unsigned char val[6];
  unsigned short rawps;
  unsigned short rawals[2];
  
  rc = RPR0521RS_get_rawpsalsval(val);
  if (rc != 0) {
    return (rc);
  }
  
  rawps     = ((unsigned short)val[1] << 8) | val[0];
  rawals[0] = ((unsigned short)val[3] << 8) | val[2];
  rawals[1] = ((unsigned short)val[5] << 8) | val[4];
  
  *ps  = rawps;
  *als = RPR0521RS_convert_lux(rawals);
  
  return (rc);  
}


const t_RPR0521RS rpr0521rs =
{
    RPR0521RS_init,
    RPR0521RS_get_rawpsalsval,
    RPR0521RS_get_psalsval,
    RPR0521RS_convert_lux,
    RPR0521RS_write,
    RPR0521RS_read
} ;
