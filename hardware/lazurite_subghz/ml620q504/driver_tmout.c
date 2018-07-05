/* FILE NAME: driver_tmout.c
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

#include "common.h"
#include "lazurite.h"
#include "driver_gpio.h"
#include "driver_pin_assignment.h"
#include "serial.h"
#include "mcu.h"
#include "rdwr_reg.h"

//#define PWM_TEST

//********************************************************************************
//   global parameters
//********************************************************************************
const unsigned char ml620504f_tmout_to_pin[] =
{
	2,	//	P02
	3,	//	P03
	10,	//	P22
	11,	//	P23
	14,	//	P32
	15,	//	P33
	18,	//	P36
	19,	//	P37
	22,	//	P42
	23,	//	P43
	26,	//	P46
	27,	//	P47
	30,	//	P52
	31,	//	P53
	34,	//	P56
	35,	//	P57
};
//********************************************************************************
//   local definitions
//********************************************************************************
#define	FTM_FTnP		0		//16bit access
#define	FTM_FTnEA		2/2		//16bit access
#define	FTM_FT0EB		4/2		//16bit
#define	FTM_FT0DT		6/2		//16bit
#define	FTM_FTnC		8/2		//16bit
#define	FTM_FTnCON0		10		//8bit access
#define	FTM_FTnCON1		11		//8bit
#define	FTM_FTnMODL		12		//8bit
#define	FTM_FTnMODH		13		//8bit
#define	FTM_FTnCLKL		14		//8bit
#define	FTM_FTnCLKH		15		//8bit
#define	FTM_FTnTRG0L	16		//8bit
#define	FTM_FTnTRG0H	17		//8bit
#define	FTM_FTnTRG1L	18		//8bit
#define	FTM_FTnTRG1H	19		//8bit
#define	FTM_FTnINTEL	24		//8bit
#define	FTM_FTnINTSL	26		//8bit
#define	FTM_FTnINTCL	28		//8bit
//********************************************************************************
//   local parameters
//********************************************************************************
// -------------------------------------------------------------------------------
//    The bit of "_pwm_ch_flag" means which PWM port is in use or not in use.
//    b7: FTM3N
//    ...
//    b1: FTM0N
//    b0: FTM0P
// -------------------------------------------------------------------------------
static unsigned char _pwm_ch_flag = 0;
//********************************************************************************
//   local function declaration
//********************************************************************************
#ifdef PWM_TEST
void reg_log_out(unsigned short adr, unsigned short val);
#endif
//********************************************************************************
//   local functions
//********************************************************************************
//--------------------------------------------------------------------------------
//   void drv_analogWrite(unsigned char pwmnum, unsigned char val)
//   parameters:
//       pwmnum = 0-11  0=TMOUT0, 1=TMOUT1, ... 15=TMOUTF
//       ftm = 0-7   0=FTM0P, 1=FTM0N, ....  6=FTM3P, 7=FTM3N
//   return:
//       none
//--------------------------------------------------------------------------------
void drv_analogWrite(unsigned char pwmnum,unsigned char ftm,unsigned char val)
{
	unsigned char  *adr8;			// FTnP pointer for 8bit access
	unsigned short *adr16;			// FTnP pointer for 16bit access
	unsigned char ftm_ch;			// ftm channel
	unsigned char ftma_b;			// ftm A or B  A=0, B=1
	unsigned char use_ch;			// use channel either of A or B
	unsigned char use_this;			// use this PWM channel
	unsigned char *adr8_out;		// TMOUT0 pointer
	unsigned char *port;			// PnD pointer
	unsigned char bit;				// bit of this port

	// check hardware PWM
	if(pwmnum > ML620504F_MAX_TMOUT_NO) return;
	if(ftm > ML620504F_MAX_FTM_NO) return;

	//*******************************************************
	// Initializaing pointer
	//*******************************************************	
	// channel and A/B
	ftm_ch = (unsigned char)(ftm >> 1);		// timer channel
	ftma_b = (unsigned char)(ftm & 0x01);	// A = 0, B = 1
	
	use_this = (unsigned char)((_pwm_ch_flag >> ftm) & 0x01);
	if( use_this == 0 ) use_this = false;
	else use_this = true;
	
	adr16 = &FT0P;							// setting pointer of FTMn
	adr16 += (ftm_ch << 4);
	adr8 = (CHAR *)adr16;					// seting address of FTM
	adr8_out = &FTO0SL;						// setting pointer of FTM output setting
	port = ml620504f_pin_to_port(ml620504f_tmout_to_pin[pwmnum]);		// setting pointer of GPIO
	bit = ml620504f_pin_to_bit[ml620504f_tmout_to_pin[pwmnum]];			// setting bit of GPIO

	
	
	//*******************************************************
	// PWM OFF process
	//*******************************************************
	if( (val == 0) || (val >= 255) )
	{
		drv_digitalWrite(ml620504f_tmout_to_pin[pwmnum], (unsigned char)(val & HIGH));
		drv_pinMode(ml620504f_tmout_to_pin[pwmnum],OUTPUT);
#ifdef PWM_TEST
		reg_log_out((unsigned short)port,*port);
#endif
		// check power down
		if(use_this == true)
		{
			// Mode of GPIO is reset
			*(port+4) &= ~bit;										// PnMOD0 = 0
#ifdef PWM_TEST
			reg_log_out((unsigned short)(port+4),*(port+4));
#endif

			*(port+5) &= ~bit;										// PnMOD1 = 0
#ifdef PWM_TEST
			reg_log_out((unsigned short)(port+5),*(port+5));
#endif

			// Update flag
			_pwm_ch_flag &= ~(0x01 << ftm);
#ifdef PWM_TEST
			Serial.print("_pwm_ch_flag =");
			Serial.println_long((long)_pwm_ch_flag, HEX);	
#endif
			// use_ch  0= not in use, 1 = another side is in use
			use_ch =  (unsigned char)((_pwm_ch_flag >> (ftm_ch <<1)) & 0x03);		// update use channel
			if( use_ch == 0)
			{
				//Stop FTM
				*(adr8 + FTM_FTnCON0) = 0x00;							// Stop FTM
#ifdef PWM_TEST
				reg_log_out((unsigned short)(adr8 + FTM_FTnCON0),0x00);
#endif
				
				//Disenabling this channel of FTM
				BLKCON1 |= (0x01 << ftm_ch);							// Enabling PWM block
#ifdef PWM_TEST
				reg_log_out((unsigned short)&BLKCON1, BLKCON1);
#endif
			}

		}
		return;		// end of stoping process
	}
	
	//*******************************************************
	// PWM ON process
	//*******************************************************
	// check use or not use
	use_ch =  (unsigned char)((_pwm_ch_flag >> (ftm_ch <<1)) & 0x03);
	if( use_ch == 0)	use_ch = false;		// not use
	else	use_ch = true;		// use
	
	// PWM setting process
	if(use_ch == false)
	{
		// BLKCON setting
		BLKCON1 &= (~(0x01 << ftm_ch));							// Enabling PWM block
#ifdef PWM_TEST
		reg_log_out((unsigned short)&BLKCON1, BLKCON1);
#endif
		// FTM initializing
		*(adr8 + FTM_FTnMODL) = 0x02;							// set PWM1 mode
#ifdef PWM_TEST
		reg_log_out((unsigned short)(adr8 + FTM_FTnMODL), 0x02);
#endif
		
		*(adr8 + FTM_FTnCLKL) = 0x61;							// set PWM base clock = 1/64 OSCLK
#ifdef PWM_TEST
		reg_log_out((unsigned short)(adr8 + FTM_FTnCLKL), 0x61);
#endif
		
		*(adr16 + FTM_FTnP)  = 0x01FF;							// set PWM frequency
#ifdef PWM_TEST
		reg_log_out((unsigned short)(adr16 + FTM_FTnP), 0x01FF);
#endif
	}
	*(adr16  + FTM_FTnEA + ftma_b)  = (val << 1);			// set PWM value
#ifdef PWM_TEST
	reg_log_out((unsigned short)(adr16  + FTM_FTnEA + ftma_b), (val << 1));
#endif
	
	
	//set GPIO
	if(use_this == false)	// If this channel of PWM is not use...
	{
		*(adr8_out + pwmnum) = ftm;								// assign PWM to FTM0-F;
#ifdef PWM_TEST
		reg_log_out((unsigned short)(adr8_out + pwm_ch_to_tmout[digital_pin_to_pwm_ch[pwmnum]]),digital_pin_to_pwm_ch[pwmnum]);
#endif
		
		drv_pinMode(ml620504f_tmout_to_pin[pwmnum],OUTPUT);									// set OUTPUT mode to the pin.

		*(port+4) |= bit;										// PnMOD0 = 1 (TMOUTn mode)
#ifdef PWM_TEST
		reg_log_out((unsigned short)(port+4),*(port+4));
#endif

		*(port+5) |= bit;										// PnMOD1 = 1 (TMOUTn mode)
#ifdef PWM_TEST
		reg_log_out((unsigned short)(port+5),*(port+5));
#endif
	}
	
	if(use_ch == false)
	{
	//Start FTM
		*(adr8 + FTM_FTnCON0) = 0x01;							// Start FTM, if PWM channel have been not started yet.
#ifdef PWM_TEST
		reg_log_out((unsigned short)(adr8 + FTM_FTnCON0),0x01);
#endif
	}
	else
	{
		*(adr8 + FTM_FTnCON1) = 0x01;							// Update FTM data, if PWM channel is turned on already.
#ifdef PWM_TEST
		reg_log_out((unsigned short)(adr8 + FTM_FTnCON1),0x01);
#endif
	}
	
	_pwm_ch_flag |= (0x01 << ftm);		// set flag
#ifdef PWM_TEST
	Serial.print("_pwm_ch_flag =");
	Serial.println_long((long)_pwm_ch_flag, HEX);	
#endif
	return;
}


#ifdef PWM_TEST
void reg_log_out(unsigned short adr, unsigned short val)
{
	Serial.print_long(adr,HEX);
	Serial.print("\t");
	Serial.println_long(val,HEX);
	return;
}
#endif
