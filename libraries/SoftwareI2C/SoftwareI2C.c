
#include "SoftwareI2C.h"
#include "driver_gpio.h"

#define I2C_BUF_SIZE	64
static uint8_t buf[I2C_BUF_SIZE];
static uint16_t buf_index;
static uint16_t buf_length;
static uint8_t i2c_address;

static uint8_t* p_scl;
static uint8_t* p_sda;
static uint8_t  b_scl;
static uint8_t  b_sda;
static uint32_t  interval = 5;
static uint8_t transmit;

extern const unsigned char digital_pin_to_port[];
extern const int ml620504f_pin_to_port[];
extern const unsigned char ml620504f_pin_to_bit[];

#define ACK		0
#define NACK	1

#define LIB_DEBUG
#include "libdebug.h"

static uint8_t i2c_start(void)
{
	// start condition
	*p_sda &= ~b_sda;			// SDA -> 0
	delayMicroseconds(10);
	*p_scl &= ~b_scl;			// SCL -> 0
	delayMicroseconds(3);
	return 0;
}


// retval: true   ACK
//         false  NACK
static uint8_t i2c_read(uint8_t *data)
{
	int i;
	bool ack;
	uint8_t rcv=0;

	*(p_sda+1) |= b_sda;											// SDA set to inpuut
	*p_scl &= ~b_scl;												// SCL -> 0(should be 0 in default)
	
	for(i=0;i<8;i++) {
		delayMicroseconds(interval);								// tHIGH
		*(p_scl+1) |= b_scl;										// SCL HIGH as input
		while(!(*p_scl&b_scl)){} 									// clock strecher
		rcv = (rcv<<1) | ((*p_sda & b_sda )? 1 : 0);				// get SDA
		delayMicroseconds(interval);								// tLOW
		*(p_scl+1) &= ~b_scl;										// SCL LOW as output
	}
	*(p_sda+1) |= b_sda;											// SDA set to INPUT
	delayMicroseconds(interval);										// tLOW
	*(p_scl+1) |= b_scl;											// SCL -> 1 as input
	while(!(*p_scl & b_scl));										// strecher
	ack = (*p_sda & b_sda) ? NACK: ACK;								// SDA -> Hi-Z
	delayMicroseconds(interval);										// tLOW
	*(p_scl+1) &= ~b_scl;											// SCL -> 1 as input
	*p_sda &= ~b_sda;												// SDA -> 0
	*(p_sda+1) &= ~b_sda;											// SDA set to OUTPUT
	*data = rcv;
	return ack;
}

// retval: true   ACK
//         false  NACK
static bool i2c_write(uint8_t data)
{
	int i;
	bool ack;
	*(p_sda+1) &= ~b_sda;												// SDA output mode
	for(i=0;i<8;i++) {
		(data & 0x80) ?  (*p_sda |= b_sda ) :(*p_sda &= ~b_sda);		// SDA
		delayMicroseconds(interval);									// tLOW
		data <<= 1;
		*(p_scl+1) |= b_scl;											// SCL set to pullup input
		while(!(*p_scl & b_scl));										// strecher
		delayMicroseconds(interval);									// tHIGH
		*(p_scl+1) &= ~b_scl;											// SCL set to LOW
		*p_scl &= ~b_scl;											// SCL -> 0
	}
	*(p_sda+1) |= b_sda;											// SDA set to INPUT
	delayMicroseconds(interval);										// tLOW
	*(p_scl+1) |= b_scl;											// SCL -> 1 as input
	while(!(*p_scl & b_scl));										// strecher
	ack = (*p_sda & b_sda) ? NACK: ACK;								// SDA -> Hi-Z
	delayMicroseconds(interval);										// tLOW
	*(p_scl+1) &= ~b_scl;											// SCL -> 1 as input
	*p_sda &= ~b_sda;												// SDA -> 0
	*(p_sda+1) &= ~b_sda;											// SDA set to OUTPUT
	return ack;
}

// setting gpio
static void begin(uint8_t sda,uint8_t scl)
{
	p_scl = (uint8_t*)ml620504f_pin_to_port[digital_pin_to_port[scl]];
	b_scl = ml620504f_pin_to_bit[digital_pin_to_port[scl]];
	p_sda = (uint8_t*)ml620504f_pin_to_port[digital_pin_to_port[sda]];
	b_sda = ml620504f_pin_to_bit[digital_pin_to_port[sda]];
	transmit = false;
	pinMode(scl,OPEN_DRAIN);
	pinMode(sda,OPEN_DRAIN);
	
	*p_scl &= ~b_scl;
	*p_sda &= ~b_sda;
	delayMicroseconds(interval);
	*p_scl |= b_scl;
	delayMicroseconds(interval);
	*p_sda |= b_sda;
	delayMicroseconds(interval);
}

static void end(void)
{
}


static void setClock(uint32_t freq)
{
	interval = 1000000 / (freq*2) + 1;
}

static uint8_t stopCondition(bool sendStop)
{
	bool flag;
	if(sendStop) {
		*p_scl |= b_scl;											// SDA -> Hi-Z
		*(p_scl+1) |= b_scl;										// SDA Hi-Z as input
		while((*p_scl&b_scl)==0);
		delayMicroseconds(interval);								// tLOW
		*p_sda |= b_sda;												// SCL -> Hi-Z
		delayMicroseconds(interval);								// tLOW
		*(p_scl+1) &= ~b_scl;										// SDA set to output
	} else {
		*p_sda |= b_sda;													// SDA -> Hi-Z
		*(p_sda+1) |= b_sda;												// SDA Hi-Z as input
		while((*p_sda&b_sda)==0);
		delayMicroseconds(interval);									// tLOW
		*p_scl |= b_scl;												// SCL -> Hi-Z
		*(p_scl+1) |= b_scl;											// SCL Hi-Z as input
		while((*p_scl&b_scl)==0);
		delayMicroseconds(interval);									// tLOW
		*(p_sda+1) &= ~b_sda;											// SDA -> Hi-Z
		*(p_scl+1) &= ~b_scl;											// SCL Hi-Z as input
	}
	return 0;
}

static byte beginTransmission(uint8_t address)
{
	byte rc;
	buf_index = 0;
	buf_length = 0;
	i2c_address = address;
	
	// send stop condition when state is transmit
	if(transmit == true)
	{
		rc = stopCondition(true);
	}
	delayMicroseconds(interval);										// tLOW
	
	return rc;
}


static uint8_t endTransmission(uint8_t sendStop)
{
	byte rc;
	buf_index = 0;
	
	if(!transmit){
		stopCondition(true);
	}
	
	transmit = true;
	// send start condition
	rc = i2c_start();
	if(rc != 0)
	{
		rc = I2C_RESULT_ERROR;
		goto err;
	}
	
	// send i2c slave address in write mode
	rc = i2c_write((i2c_address << 1)& 0xFE);
	if(rc != 0) {
		rc = I2C_RESULT_ANACK;
		goto err;
	}
	
	for(buf_index=0;buf_index<buf_length;buf_index++)
	{
		rc = i2c_write(buf[buf_index]);
		if(rc != 0)
		{
			rc = stopCondition(sendStop);
			rc = I2C_RESULT_DNACK;
			goto err;
		}
	}
	delayMicroseconds(20);
	// send stop condition accoding to flag
	rc = stopCondition(sendStop);
	if(rc != 0)
	{
		rc = I2C_RESULT_ERROR;
	}
err:
	transmit = false;
	return rc;
}

static size_t write_byte(uint8_t data)
{
	byte rc;
	if(buf_length<I2C_BUF_SIZE)
	{
		buf[buf_length] = data;
		buf_length++;
		rc = 1;
	} else {
		rc = 0;
	}
	return rc;
}

static size_t write(uint8_t* data,uint8_t size)
{
	int i;
	uint16_t len;
	len = ((buf_length+size) < I2C_BUF_SIZE) ? size : (I2C_BUF_SIZE-buf_length);
	memcpy(buf+buf_length,data,len);
	buf_length += len;
	return len;
}

static int read(void)
{
	int result;
	if(buf_index >= buf_length) result = -1;
	else
	{
		result = (int)buf[buf_index];
		buf_index++;
	}
	return result;
}

static size_t available(void)
{
	return (buf_length - buf_index);
}

static size_t requestFrom(uint8_t address, uint16_t size, uint8_t bStop)
{
	byte rc;
	buf_index = 0;
	buf_length = 0;
	i2c_start();
	rc = i2c_write((address<<1) | 0x01);
	if(rc != 0)
	{
		goto end;
	}
	
	for(buf_length = 0; buf_length < size; buf_length++)
	{
		rc = i2c_read(&buf[buf_length]);
		if(rc==NACK)
		{
			buf_length++;
			break;
		}
	}
	delayMicroseconds(10);
	stopCondition(bStop);
end:
	return rc;
}

static void setTimeout(uint16_t ms) {
	// not implemented
}

const TwoWire Wire ={
	begin,
	requestFrom,
	available,
	read,
	beginTransmission,
	write_byte,
	write,
	endTransmission,
	setTimeout,
	setClock
};

