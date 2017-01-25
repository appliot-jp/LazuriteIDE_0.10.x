/* FILE NAME: BME280.c
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

#include "lazurite.h"
#include "spi.h"
#include "bme280.h"

#define BME280_WRITE		0x7F
#define BME280_READ			0xFF

#define BME280_HUM_LSB		0xFE
#define BME280_HUM_MSB		0xFD
#define BME280_TEMP_XLSB	0xFC
#define BME280_TEMP_LSB		0xFB
#define BME280_TEMP_MSB		0xFA
#define BME280_PRESS_XLSB	0xF9
#define BME280_PRESS_LSB	0xF8
#define BME280_PRESS_MSB	0xF7
#define BME280_CONFIG		0xF5
#define BME280_CTRL_MEAS	0xF4
#define BME280_STATUS		0xF3
#define BME280_CTRL_HUM		0xF2
#define BME280_RESET		0xE0
#define BME280_ID			0xD0

//#define BME280_CALIB(v)		((v>=0)&&(v<=25))? 0x88+v : 0xE1-26+v

static unsigned long int hum_raw,temp_raw,pres_raw;
static unsigned char bme280_csb;

static long		t_fine;
static unsigned short	dig_T1;
static short	dig_T2;
static short	dig_T3;
static unsigned short	dig_P1;
static short	dig_P2;
static short	dig_P3;
static short	dig_P4;
static short	dig_P5;
static short	dig_P6;
static short	dig_P7;
static short	dig_P8;
static short	dig_P9;
static char		dig_H1;
static short	dig_H2;
static char		dig_H3;
static short	dig_H4;
static short	dig_H5;
static char		dig_H6;

#define BME280_CALIB(v)		((v>=0)&&(v<=25))? 0x88+v : 0xE1-26+v

static unsigned char bme280_transfer(unsigned char addr, unsigned char data,unsigned char read)
{
	volatile unsigned char result;
	digitalWrite(bme280_csb,LOW);
	result = SPI.transfer(addr&read);
	result = SPI.transfer(data);
	digitalWrite(bme280_csb,HIGH);
	return result;
}

static void readTrim()
{
	unsigned char data[31];
	unsigned char i;
	for(i=0;i<=23;i++)
	{
		data[i] = (unsigned short)bme280_transfer((unsigned char)0x88+i,0,BME280_READ);
#ifdef	DEBUG
		Serial.print_long((long)(0x88+i),HEX);
		Serial.print(" ");		
		Serial.print_long((long)data[i],HEX);
		Serial.println("");
#endif
	}
	data[24] = bme280_transfer((unsigned char)0xA1,0,BME280_READ);
#ifdef	DEBUG
		Serial.print_long((long)(0xA1),HEX);
		Serial.print(" ");		
		Serial.print_long((long)data[i],HEX);
		Serial.println("");
#endif
	for(i=25;i<=31;i++)
	{
		data[i] = (unsigned short)bme280_transfer((unsigned char)0xE1-25+i,0,BME280_READ);
#ifdef	DEBUG
		Serial.print_long((long)(0xE1-25+i),HEX);
		Serial.print(" ");		
		Serial.print_long((long)data[i],HEX);
		Serial.println("");
#endif
	}
#ifdef DEBUG
	Serial.println("");
#endif
    dig_T1 = (data[1] << 8) | data[0];
    dig_T2 = (data[3] << 8) | data[2];
    dig_T3 = (data[5] << 8) | data[4];
    dig_P1 = (data[7] << 8) | data[6];
    dig_P2 = (data[9] << 8) | data[8];
    dig_P3 = (data[11]<< 8) | data[10];
    dig_P4 = (data[13]<< 8) | data[12];
    dig_P5 = (data[15]<< 8) | data[14];
    dig_P6 = (data[17]<< 8) | data[16];
    dig_P7 = (data[19]<< 8) | data[18];
    dig_P8 = (data[21]<< 8) | data[20];
    dig_P9 = (data[23]<< 8) | data[22];
	dig_H1 = (char)data[24];
    dig_H2 = (data[26]<< 8) | data[25];
	dig_H3 = (char)data[27];
    dig_H4 = (((signed short)data[28])<< 4) | (0x0F & data[29]);
    dig_H5 = ((data[29] >> 4) & 0x0F) | (((signed short)data[30]) << 4) ;
	dig_H6 = (char)data[31]; 
}


static signed long int calibration_T(signed long int adc_T)
{
    
    signed long int var1, var2, T;
    var1 = ((((adc_T >> 3) - ((signed long int)dig_T1<<1))) * ((signed long int)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int)dig_T1)) * ((adc_T>>4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T; 
}

static unsigned long int calibration_P(signed long int adc_P)
{
    signed long int var1, var2;
    unsigned long int P;
    var1 = (((signed long int)t_fine)>>1) - (signed long int)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)dig_P6);
    var2 = var2 + ((var1*((signed long int)dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((signed long int)dig_P1))>>15);
    if (var1 == 0)
    {
        return 0;
    }    
    P = (((unsigned long int)(((signed long int)1048576)-adc_P)-(var2>>12)))*3125;
    if(P<0x80000000)
    {
       P = (P << 1) / ((unsigned long int) var1);   
    }
    else
    {
        P = (P / (unsigned long int)var1) * 2;    
    }
    var1 = (((signed long int)dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((signed long int)(P>>2)) * ((signed long int)dig_P8))>>13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));
    return P;
}

static unsigned long int calibration_H(signed long int adc_H)
{
    signed long int v_x1;
    
    v_x1 = (t_fine - ((signed long int)76800));
    v_x1 = (((((adc_H << 14) -(((signed long int)dig_H4) << 20) - (((signed long int)dig_H5) * v_x1)) + 
              ((signed long int)16384)) >> 15) * (((((((v_x1 * ((signed long int)dig_H6)) >> 10) * 
              (((v_x1 * ((signed long int)dig_H3)) >> 11) + ((signed long int) 32768))) >> 10) + (( signed long int)2097152)) * 
              ((signed long int) dig_H2) + 8192) >> 14));
   v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long int)dig_H1)) >> 4));
   v_x1 = (v_x1 < 0 ? 0 : v_x1);
   v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
   return (unsigned long int)(v_x1 >> 12);   
}

void readData()
{
    int i = 0;
    unsigned long data[8];
	
	for(i=0;i<8;i++)
	{
		data[i] = (unsigned long)bme280_transfer((unsigned char)(BME280_PRESS_MSB+i),0,BME280_READ);
#ifdef DEBUG
		Serial.print_long((long)((unsigned char)(BME280_PRESS_MSB+i)),HEX);
		Serial.print(" ");
		Serial.print_long((long)data[i],HEX);
		Serial.print(",");
#endif
	}
#ifdef DEBUG
	Serial.println("");
#endif
	
    pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    hum_raw  = (data[6] << 8) | data[7];

}

static void bme280_init(unsigned char csb)
{
	unsigned char tmp;
	unsigned char osrs_t = 1;             //Temperature oversampling x 1
    unsigned char osrs_p = 1;             //Pressure oversampling x 1
    unsigned char osrs_h = 1;             //Humidity oversampling x 1
    unsigned char mode = 3;               //Normal mode
    unsigned char t_sb = 5;               //Tstandby 1000ms
    unsigned char filter = 0;             //Filter off 
    unsigned char spi3w_en = 0;           //3-wire SPI Disable
    unsigned char ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
    unsigned char config_reg    = (t_sb << 5) | (filter << 2) | spi3w_en;
    unsigned char ctrl_hum_reg  = osrs_h;
	
#ifdef DEBUG
	Serial.begin(115200);
#endif
	
	bme280_csb = csb;

	// BME280 IO setting
	SPI.setDataMode(SPI_MODE3);
	SPI.setClockDivider(SPI_CLOCK_DIV8);
	SPI.begin();
	
	digitalWrite(bme280_csb, HIGH);
	pinMode(bme280_csb, OUTPUT);
	
	// get ID
	tmp = bme280_transfer(BME280_ID,0,BME280_READ);
#ifdef DEBUG
	Serial.println_long((long)tmp,HEX);
#endif
	
	// initializing
	bme280_transfer(BME280_CTRL_HUM,ctrl_hum_reg,BME280_WRITE);
	bme280_transfer(BME280_CTRL_MEAS,ctrl_meas_reg,BME280_WRITE);
	bme280_transfer(BME280_CONFIG,config_reg,BME280_WRITE);
	
	// get triming data
	readTrim();
}

static void bme280_read(double *temp, double *humi, double *press)
{
	
    readData();
    
	if(temp)
	{
	    signed long int temp_cal;
		temp_cal = calibration_T(temp_raw);
		*temp = (double)temp_cal / 100.0;
	}
	if(press)
	{
	    unsigned long int press_cal;
		press_cal = calibration_P(pres_raw);
	    *press = (double)press_cal / 100.0;
	}
	if(humi)
	{
	    unsigned long int hum_cal;
		hum_cal = calibration_H(hum_raw);
	    *humi = (double)hum_cal / 1024.0;
	}
}


const t_BME280 bme280 =
{
	bme280_init,
	bme280_read,
};

