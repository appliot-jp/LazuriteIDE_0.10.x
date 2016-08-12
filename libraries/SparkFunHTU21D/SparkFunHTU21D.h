/* 
 HTU21D Humidity Sensor Library
 */
 
#ifndef _HTU21D_H_
#define _HTU21D_H_

#include "lazurite.h"
#include <Wire.h>

#define HTDU21D_ADDRESS 0x40  //Unshifted 7-bit I2C address for the sensor

#define TRIGGER_TEMP_MEASURE_HOLD  0xE3
#define TRIGGER_HUMD_MEASURE_HOLD  0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD  0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD  0xF5
#define WRITE_USER_REG  0xE6
#define READ_USER_REG  0xE7
#define SOFT_RESET  0xFE

#define USER_REGISTER_RESOLUTION_MASK 0x81
#define USER_REGISTER_RESOLUTION_RH12_TEMP14 0x00
#define USER_REGISTER_RESOLUTION_RH8_TEMP12 0x01
#define USER_REGISTER_RESOLUTION_RH10_TEMP13 0x80
#define USER_REGISTER_RESOLUTION_RH11_TEMP11 0x81

#define USER_REGISTER_END_OF_BATTERY 0x40
#define USER_REGISTER_HEATER_ENABLED 0x04
#define USER_REGISTER_DISABLE_OTP_RELOAD 0x02

typedef struct {
	void (*begin)(void);
	float (*readHumidity)(void);
	float (*readTemperature)(void);
	void (*setResolution)(byte resBits);
	byte (*readUserRegister)(void);
	void (*writeUserRegister)(byte val);
} t_HTU21D;

extern const t_HTU21D htu21d;

#endif //_HTU21D_H_
