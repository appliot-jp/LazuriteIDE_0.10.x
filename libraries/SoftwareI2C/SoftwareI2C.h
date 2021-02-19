



#ifndef _SOFTWARE_I2C_H_

#define _SOFTWARE_I2C_H_


#include "common.h"
#include "lazurite.h"

#define I2C_BUFFER_LENGTH 32

#define I2C_RESULT_OK		0
#define I2C_RESULT_LEN_ERR	1		// length is over. may not use
#define I2C_RESULT_ANACK	2		// address send, nack receive
#define I2C_RESULT_DNACK	3		// data send, nack receive
#define I2C_RESULT_ERROR	4		// other error


//********************************************************************************
//   global definitions
//********************************************************************************

//********************************************************************************
//   global parameters
//********************************************************************************
//********************************************************************************
//   extern function definitions
//********************************************************************************
// to match API with arduino 
typedef struct {
	void (*begin)(uint8_t sda,uint8_t scl);
	size_t (*requestFrom)(uint8_t address,uint16_t size, uint8_t sendStop);
	int (*available)(void);
	int (*read)(void);
	void (*beginTransmission)(uint8_t address);
	size_t (*write_byte)(uint8_t data);
	size_t (*write)(const uint8_t *data, size_t quantity);
	uint8_t (*endTransmission)(uint8_t sendStop);
	void (*setTimeout)(uint16_t ms);
	void (*setClock)(uint32_t freq);
}TwoWire;

extern const TwoWire Wire;


#endif // _SOFTWARE_I2C_H_
