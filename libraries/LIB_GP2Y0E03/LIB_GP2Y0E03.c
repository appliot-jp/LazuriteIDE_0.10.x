
#include "LIB_GP2Y0E03.h"
#include "wire.h"


//#define _DEBUG


static unsigned char gp2Y0e03_i2c_addr;
static unsigned char shift_bit;

static void init(unsigned char i2c_addr)
{
	gp2Y0e03_i2c_addr = i2c_addr;
	/*
	Wire.beginTransmission(gp2Y0e03_i2c_addr);
	Wire.write_byte(0x35);
	Wire.endTransmission(false);
	Wire.requestFrom(gp2Y0e03_i2c_addr,1,true);
	*/
	shift_bit = 2+4;
	
	return;
}

static int measure(void)
{
	unsigned char d[2];
	unsigned int result;
	Wire.beginTransmission(gp2Y0e03_i2c_addr);
	Wire.write_byte(0x5E);
	Wire.endTransmission(false);
	
	Wire.requestFrom(gp2Y0e03_i2c_addr,1,true);
	
	d[0] = Wire.read();
	
	Wire.beginTransmission(gp2Y0e03_i2c_addr);
	Wire.write_byte(0x5F);
	Wire.endTransmission(false);
	
	Wire.requestFrom(gp2Y0e03_i2c_addr,1,true);
	
	d[1] = Wire.read();
	
	#ifdef _DEBUG
	Serial.print("ShiftBit=");
	Serial.print_long(shift_bit,HEX);
	Serial.print("\tD[0]=");
	Serial.print_long(d[0],HEX);
	Serial.print("\tD[1]=");
	Serial.println_long(d[1],HEX);
	#endif
	
	result = (d[0]*16+d[1]) >> shift_bit;
	return result;
}


static void meas_mode(unsigned char mode)
{
	// check data
	if((mode == 1) || (mode == 2)) shift_bit = mode+4;
	
	// set bit shift
	Wire.beginTransmission(gp2Y0e03_i2c_addr);
	Wire.write_byte(0x35);
	Wire.write_byte(mode);
	Wire.endTransmission(true);
	
	return;
}

static void stby_mode(bool mode)
{
	// check data
	if((mode == 0) || (mode == 1))
	{
		// set bit shift
		Wire.beginTransmission(gp2Y0e03_i2c_addr);
		Wire.write_byte(0xe8);
		Wire.write_byte(mode);
		Wire.endTransmission(true);
	}
	return;
}

tGP2Y0E03 gp2y0e03 =
{
	init,
	measure,
	meas_mode,
	stby_mode
};


