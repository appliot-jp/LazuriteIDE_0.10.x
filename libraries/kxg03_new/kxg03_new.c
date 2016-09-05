#include "lazurite.h"
#include "kxg03_new.h"
#include "wire.h"

static uint8_t slave_addr;

static uint8_t kxg03_read(uint8_t addr,uint8_t* data,uint8_t size)
{
	byte rc;
	unsigned char cnt;
	
	Wire.beginTransmission(slave_addr);
	Wire.write_byte(addr);
	rc = Wire.endTransmission(false);
	if (rc != 0) {
		Serial.print("read.endTransmission=0x");
		Serial.print_long((long)addr,HEX);
		Serial.print(",");
		Serial.println_long((long)rc,HEX);
//		return (rc);
	}
	rc=Wire.requestFrom(slave_addr, size, true);
	cnt = 0;
	while(Wire.available())
	{
		data[cnt] = (uint8_t)Wire.read();
		cnt++;
	}
	return (rc);
}

static uint8_t kxg03_write(uint8_t addr,uint8_t* data, uint8_t size)
{
	byte rc;
	
	Wire.beginTransmission(slave_addr);
	Wire.write_byte(addr);
	Wire.write(data, size);
	rc = Wire.endTransmission(true);
	return (rc);
}

static uint8_t kxg03_init(uint8_t i2c_addr)
{
	byte rc;
	unsigned char reg;
	
	if(i2c_addr == KXG03_DEVICE_ADDRESS_4F)
	{
		slave_addr = KXG03_DEVICE_ADDRESS_4F;
	} else {
		slave_addr = KXG03_DEVICE_ADDRESS_4E;
//		slave_addr = 0x11;
	}
	Wire.setTimeout(100);
	rc = kxg03_read(KXG03_WHO_AM_I, &reg, sizeof(reg));
	if (rc != 1) {
		Serial.print_long((long)rc,DEC);
//		while(1){}
			Serial.println("Can't access KXG03");
			return (rc);
	} 
	Serial.print("KXG03_WHO_AM_I Register Value = 0x");
	Serial.println_long((long)reg, HEX);

	if (reg != KXG03_WHO_AM_I_VAL) {
		Serial.println("Can't find KXG03");
	// return (rc);
	}
	/*
	// rc = write(KXG03_ACCEL_ODR_WAKE, &reg, sizeof(reg));
	reg = 0xEC;
	rc = kxg03_write(KXG03_STDBY, &reg, sizeof(reg));
	if (rc != 0) {
		Serial.println("Can't write KXG03 STDBY Register");
		return (rc);
	} 
	*/
	return (rc);  
}

static uint8_t kxg03_regInit(KXG03_REG *reg)
{
	int i=0;
	uint8_t rc;
/*	while(reg[i]->addr <= 0x7F)
	{
		rc=kxg03_write(reg[i]->addr,&reg[i]->data,1);
		if(!rc) {
			Serial.print("I2C Access Error num=");
			Serial.println_long((long)i,DEC);
			return rc;
		}
		i++;
	}
	*/
	return rc;
}
static uint8_t kxg03_getVal(float* val)
{
	uint8_t rc;
	int i;

	union {
		uint8_t d8[12];
		short d16[6];
	} data;
	
	rc = kxg03_read(KXG03_GYRO_XOUT_L,data.d8,12);
	if(rc!=12) {
		Serial.print("I2C ERROR=");
		Serial.println_long((long)rc,DEC);
	}
		
	val[0] = (float)data.d16[0]/128;
	val[1] = (float)data.d16[1]/128;
	val[2] = (float)data.d16[2]/128;
	val[3] = (float)data.d16[3]/16384;
	val[4] = (float)data.d16[4]/16384;
	val[5] = (float)data.d16[5]/16384;
	
	return rc;
}

static uint8_t kxg03_getAcc(float* acc)
{
	uint8_t rc;
	union {
		uint8_t d8[6];
		short d16[3];
	} data;
	
	rc = kxg03_read(KXG03_GYRO_XOUT_L,data.d8,6);
	if(!rc) return rc;

	acc[0] = data.d16[0]/128;
	acc[1] = data.d16[1]/128;
	acc[2] = data.d16[2]/128;
	
	return rc;
}

static uint8_t kxg03_getGyro(float* gyro)
{
	uint8_t rc;
	union {
		uint8_t d8[6];
		short d16[3];
	} data;
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	rc = kxg03_read(KXG03_ACCEL_XOUT_L,data.d8,6);
	if(!rc) return rc;
	
	gyro[0] = data.d16[0]/16384;
	gyro[1] = data.d16[1]/16384;
	gyro[2] = data.d16[2]/16384;
	
	return rc;
}

static uint8_t kxg03_callib(int cycle)
{
	int i,j;
	float val[6];
	float total_val[6];
	for(j=0;j<6;j++)
	{
		total_val[j] = 0;
	}
	for(i=0;i<cycle;i++)
	{
		for(j=0;j<6;j++)
		{
			kxg03.getVal(val);
			total_val[j] = val[j];
		}
	}
	
	return cycle;
}

static void kalman_init(KALMAN_PARAM *param)
{
	param->angle = 0.0;
	param->bias = 0.0;
	param->P[0][0] = 0.0;
	param->P[0][1] = 0.0;
	param->P[1][0] = 0.0;
	param->P[1][1] = 0.0;
	
	return;
}
static void kalman_set(KALMAN_PARAM *param,float newAngle)
{
	param->angle = 0.0;
	param->bias = 0.0;
	param->P[0][0] = 0.0;
	param->P[0][1] = 0.0;
	param->P[1][0] = 0.0;
	param->P[1][1] = 0.0;
	param->lastTime=millis();
	
	return;
}

static uint8_t kalman_cal(KALMAN_PARAM *param,float newAngle, float newRate){
	
	uint32_t now;
	float dt;
	float rate,y,S,K[2],P00_temp,P01_temp;
	
	// variances
	float Q_angle = 0.001;
	float Q_bias = 0.003;
	float R_measure = 0.03;
//	float Q_angle = 0.01;
//	float Q_bias = 0.03;
//	float R_measure = 0.03;

	now = millis();;
	dt = (now - param->lastTime)/1000;
	
	// step 1
	rate = newRate - param->bias;
	param->angle += dt * rate;

	// step 2
	param->P[0][0] += dt * (dt * param->P[1][1] - param->P[0][1] - param->P[1][0] + Q_angle);
	param->P[0][1] -= dt * param->P[1][1];
	param->P[1][0] -= dt * param->P[1][1];
	param->P[1][1] += Q_bias * dt;

	// step 3
	y = newAngle - param->angle;

	// step 4
	S = param->P[0][0] + R_measure;

	// step 5
	K[0] = param->P[0][0] / S;
	K[1] = param->P[1][0] / S;

	// step 6
	param->angle += K[0] * y;
	param->bias += K[1] * y;

	// step 7
	P00_temp = param->P[0][0];
	P01_temp = param->P[0][1];
	param->P[0][0] -= K[0] * P00_temp;
	param->P[0][1] -= K[0] * P01_temp;
	param->P[1][0] -= K[1] * P00_temp;
	param->P[1][1] -= K[1] * P01_temp;
	
	param->lastTime = now;
	
	return param->angle;
}

const KXG03 kxg03 ={
	kxg03_init,
	kxg03_regInit,
	kxg03_read,
	kxg03_write,
	kxg03_getVal,
	kxg03_getGyro,
	kxg03_getAcc,
	kxg03_callib
};

const KALMAN kalman = {
	kalman_init,
	kalman_set,
	kalman_cal
};