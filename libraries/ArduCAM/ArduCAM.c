/* ArduCAM.c - 
 *
 * Copyright (c) 2018  LAPIS Semiconductor Co.,Ltd.
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
 *
 * This program is based on the below program.
  ------------------------------------------------------------------
   ArduCAM.cpp - Arduino library support for CMOS Image Sensor
  Copyright (C)2011-2015 ArduCAM.com. All right reserved

  Basic functionality of this library are based on the demo-code provided by
  ArduCAM.com. You can find the latest version of the library at
  http://www.ArduCAM.com
  ------------------------------------------------------------------
 */

#include "memorysaver.h"
#include "ArduCAM.h"
#include <wire.h>
#include <spi.h>
#if defined OV7660_CAM	
	#include "ov7660_regs.h"
#endif

#if defined OV7725_CAM	
	#include "ov7725_regs.h"
#endif

#if defined OV7670_CAM	
	#include "ov7670_regs.h"
#endif

#if defined OV7675_CAM
	#include "ov7675_regs.h"
#endif

#if ( defined(OV5642_CAM) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP_PLUS) )	
	#include "ov5642_regs.h"
#endif

#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))	
	#include "ov3640_regs.h"
#endif

#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP) || defined(OV2640_MINI_2MP_PLUS))
	#include "ov2640_regs.h"
#endif

#if defined MT9D111A_CAM  || defined MT9D111B_CAM 	
	#include "mt9d111_regs.h"
#endif

#if defined MT9M112_CAM	
	#include "mt9m112_regs.h"
#endif

#if defined MT9V111_CAM	
	#include "mt9v111_regs.h"
#endif

#if ( defined(OV5640_CAM)	|| defined(OV5640_MINI_5MP_PLUS) )
	#include "ov5640_regs.h"
#endif

#if defined MT9M001_CAM	
	#include "mt9m001_regs.h"
#endif

#if defined MT9T112_CAM	
	#include "mt9t112_regs.h"
#endif

#if defined MT9D112_CAM	
	#include "mt9d112_regs.h"
#endif

#if defined MT9M034_CAM	
	#include "mt9m034_regs.h"
#endif

uint8_t ArduCAM_read_reg(uint8_t addr);
void ArduCAM_write_reg(uint8_t addr, uint8_t data);
uint8_t ArduCAM_bus_write(int address,int value);
uint8_t ArduCAM_bus_read(int address);
int ArduCAM_wrSensorRegs16_8(const struct sensor_reg reglist[]);
byte ArduCAM_wrSensorReg8_8(uint8_t regID, uint8_t regDat);
byte ArduCAM_rdSensorReg8_8(uint8_t regID, uint8_t* regDat);
byte ArduCAM_wrSensorReg8_16(uint8_t regID, int regDat);
byte ArduCAM_rdSensorReg8_16(uint8_t regID, int* regDat);
byte ArduCAM_wrSensorReg16_8(uint16_t regID, int regDat);
byte ArduCAM_rdSensorReg16_8(uint16_t regID, uint8_t* regDat);
byte ArduCAM_wrSensorReg16_16(uint16_t regID, int regDat);
byte ArduCAM_rdSensorReg16_16(uint16_t regID, uint16_t* regDat);

static byte sensor_model = OV7670;
static byte sensor_addr = 0x42;
static byte chip_sel;
static byte m_fmt;

void ArduCAM_init(byte model ,int CS)
{
	chip_sel = (uint8_t)CS;
	pinMode((uint8_t)CS, OUTPUT);
	digitalWrite((uint8_t)CS, HIGH);
	sensor_model = model;
	switch (sensor_model) {
		case OV7660:
		case OV7670:
		case OV7675:
		case OV7725:
			sensor_addr = 0x42;
			break;
		case MT9D111_A: //Standard MT9D111 module
			sensor_addr = 0xba;
			break;
		case MT9D111_B: //Flex MT9D111 AF module
			sensor_addr = 0x90;
			break;
		case MT9M112:
			sensor_addr = 0x90;
			break;
		case MT9M001:
			sensor_addr = 0xba;
			break;
		case MT9V034:
			sensor_addr = 0x90;
			break;
		case MT9M034:
			sensor_addr = 0x20;// 7 bits
			break;
		case OV3640:
		case OV5640:
		case OV5642:
		case MT9T112:
		case MT9D112:
			sensor_addr = 0x78;
			break;
		case OV2640:
		case OV9650:
		case OV9655:
			sensor_addr = 0x60;
			break;
		default:
			sensor_addr = 0x42;
			break;
	}
}

void ArduCAM_InitCAM(void)
{
	switch (sensor_model) {
		case OV7660:
		{
#if defined(OV7660_CAM)
			ArduCAM_wrSensorReg8_8(0x12, 0x80);
			delay(100);
			ArduCAM_wrSensorRegs8_8(OV7660_QVGA);
#endif
			break;
		}
		case OV7725:
		{
#if defined(OV7725_CAM)
			byte reg_val;
			ArduCAM_wrSensorReg8_8(0x12, 0x80);
			delay(100);
			ArduCAM_wrSensorRegs8_8(OV7725_QVGA);
			rdSensorReg8_8(0x15, &reg_val);
			ArduCAM_wrSensorReg8_8(0x15, (reg_val | 0x02));
#endif
			break;
		}
		case OV7670:
		{
#if defined(OV7670_CAM)
			ArduCAM_wrSensorReg8_8(0x12, 0x80);
			delay(100);
			ArduCAM_wrSensorRegs8_8(OV7670_QVGA);
#endif
			break;
		}
		case OV7675:
		{
#if defined(OV7675_CAM)
			ArduCAM_wrSensorReg8_8(0x12, 0x80);
			delay(100);
			ArduCAM_wrSensorRegs8_8(OV7675_QVGA);
#endif
			break;
		}
		case MT9D111_A:
		{
#if defined(MT9D111A_CAM)
			ArduCAM_wrSensorRegs8_16(MT9D111_QVGA_30fps);
			delay(1000);
			ArduCAM_wrSensorReg8_16(0x97, 0x0020);
			ArduCAM_wrSensorReg8_16(0xf0, 0x00);
			ArduCAM_wrSensorReg8_16(0x21, 0x8403); //Mirror Column
			ArduCAM_wrSensorReg8_16(0xC6, 0xA103); //SEQ_CMD
			ArduCAM_wrSensorReg8_16(0xC8, 0x0005); //SEQ_CMD
#endif
			break;
		}
		case MT9D111_B:
		{
#if defined(MT9D111B_CAM)
			ArduCAM_wrSensorRegs8_16(MT9D111_QVGA_30fps);
			delay(1000);
			ArduCAM_wrSensorReg8_16(0x97, 0x0020);
			ArduCAM_wrSensorReg8_16(0xf0, 0x00);
			ArduCAM_wrSensorReg8_16(0x21, 0x8403); //Mirror Column
			ArduCAM_wrSensorReg8_16(0xC6, 0xA103); //SEQ_CMD
			ArduCAM_wrSensorReg8_16(0xC8, 0x0005); //SEQ_CMD
#endif
			break;
		}
		case OV5642:
		{
#if (defined(OV5642_CAM) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP_PLUS))
			ArduCAM_wrSensorReg16_8(0x3008, 0x80);
			if (m_fmt == RAW){
				//Init and set 640x480;
				ArduCAM_wrSensorRegs16_8(OV5642_1280x960_RAW);	
				ArduCAM_wrSensorRegs16_8(OV5642_640x480_RAW);	
			} else {
				ArduCAM_wrSensorRegs16_8(OV5642_QVGA_Preview);
				if (m_fmt == JPEG){
					ArduCAM_wrSensorRegs16_8(OV5642_JPEG_Capture_QSXGA);
					ArduCAM_wrSensorRegs16_8(ov5642_320x240);
					ArduCAM_wrSensorReg16_8(0x3818, 0xa8);
					ArduCAM_wrSensorReg16_8(0x3621, 0x10);
					ArduCAM_wrSensorReg16_8(0x3801, 0xb0);
#if (defined(OV5642_MINI_5MP_PLUS) || defined(ARDUCAM_SHIELD_V2))
					ArduCAM_wrSensorReg16_8(0x4407, 0x04);
#else
					ArduCAM_wrSensorReg16_8(0x4407, 0x0C);
#endif
				} else {
					byte reg_val;
					ArduCAM_wrSensorReg16_8(0x4740, 0x21);
					ArduCAM_wrSensorReg16_8(0x501e, 0x2a);
					ArduCAM_wrSensorReg16_8(0x5002, 0xf8);
					ArduCAM_wrSensorReg16_8(0x501f, 0x01);
					ArduCAM_wrSensorReg16_8(0x4300, 0x61);
					ArduCAM_rdSensorReg16_8(0x3818, &reg_val);
					ArduCAM_wrSensorReg16_8(0x3818, (reg_val | 0x60) & 0xff);
					ArduCAM_rdSensorReg16_8(0x3621, &reg_val);
					ArduCAM_wrSensorReg16_8(0x3621, reg_val & 0xdf);
				}
			}
#endif
			break;
		}
		case OV5640:
		{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
			delay(100);
			if (m_fmt == JPEG) {
				ArduCAM_wrSensorReg16_8(0x3103, 0x11);
				ArduCAM_wrSensorReg16_8(0x3008, 0x82);
				delay(100);
				ArduCAM_wrSensorRegs16_8(OV5640YUV_Sensor_Dvp_Init);
				delay(500);
				ArduCAM_wrSensorRegs16_8(OV5640_JPEG_QSXGA);
				ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2QVGA);
#if (defined(OV5640_MINI_5MP_PLUS) || (defined(ARDUCAM_SHIELD_V2))
				ArduCAM_wrSensorReg16_8(0x4407, 0x04);
#else
				ArduCAM_wrSensorReg16_8(0x4407, 0x0C);
#endif
			} else {
				ArduCAM_wrSensorReg16_8(0x3103, 0x11);
				ArduCAM_wrSensorReg16_8(0x3008, 0x82);
				delay(500);
				ArduCAM_wrSensorRegs16_8(OV5640YUV_Sensor_Dvp_Init);
				ArduCAM_wrSensorRegs16_8(OV5640_RGB_QVGA);
			}
#endif
			break;
		}
		case OV3640:
		{
#if defined(OV3640_CAM)
			ArduCAM_wrSensorRegs16_8(OV3640_QVGA);
#endif
			break;
		}
		case OV2640:
		{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP) || defined(OV2640_MINI_2MP_PLUS))
			ArduCAM_wrSensorReg8_8(0xff, 0x01);
			ArduCAM_wrSensorReg8_8(0x12, 0x80);
			delay(100);
			if (m_fmt == JPEG) {
				ArduCAM_wrSensorRegs8_8(OV2640_JPEG_INIT);
				ArduCAM_wrSensorRegs8_8(OV2640_YUV422);
				ArduCAM_wrSensorRegs8_8(OV2640_JPEG);
				ArduCAM_wrSensorReg8_8(0xff, 0x01);
				ArduCAM_wrSensorReg8_8(0x15, 0x00);
				ArduCAM_wrSensorRegs8_8(OV2640_320x240_JPEG);
				//ArduCAM_wrSensorReg8_8(0xff, 0x00);
				//ArduCAM_wrSensorReg8_8(0x44, 0x32);
			} else {
				ArduCAM_wrSensorRegs8_8(OV2640_QVGA);
			}
#endif
			break;
		}
		case OV9655:
		{
			break;
		}
		case MT9M112:
		{
#if defined(MT9M112_CAM)
			ArduCAM_wrSensorRegs8_16(MT9M112_QVGA);
#endif
			break;
		}
		case MT9M034:
		{
#if defined(MT9M034_CAM)
			ArduCAM_wrSensorRegs16_16(MT9M034_RAW);
#endif
			break;
		}
		case MT9V111:
		{
#if defined(MT9V111_CAM)
			//Reset sensor core
			ArduCAM_wrSensorReg8_16(0x01, 0x04);
			ArduCAM_wrSensorReg8_16(0x0D, 0x01);
			ArduCAM_wrSensorReg8_16(0x0D, 0x00);
			//Reset IFP
			ArduCAM_wrSensorReg8_16(0x01, 0x01);
			ArduCAM_wrSensorReg8_16(0x07, 0x01);
			ArduCAM_wrSensorReg8_16(0x07, 0x00);
			delay(100);
			ArduCAM_wrSensorRegs8_16(MT9V111_QVGA);
			//delay(1000);
			ArduCAM_wrSensorReg8_16(0x97, 0x0020);
			ArduCAM_wrSensorReg8_16(0xf0, 0x00);
			ArduCAM_wrSensorReg8_16(0x21, 0x8403); //Mirror Column
			ArduCAM_wrSensorReg8_16(0xC6, 0xA103); //SEQ_CMD
			ArduCAM_wrSensorReg8_16(0xC8, 0x0005); //SEQ_CMD
#endif
			break;
		}
		case MT9M001:
		{
#if defined(MT9M001_CAM)
			ArduCAM_wrSensorRegs8_16(MT9M001_QVGA_30fps);
#endif
			break;
		}
		case MT9T112:
		{
#if defined(MT9T112_CAM)
			ArduCAM_wrSensorReg16_16(0x001a , 0x0219 );
			ArduCAM_wrSensorReg16_16(0x001a , 0x0018 );
			//reset camera
			ArduCAM_wrSensorReg16_16(0x0014 , 0x2425 );
			ArduCAM_wrSensorReg16_16(0x0014 , 0x2145 );
			ArduCAM_wrSensorReg16_16(0x0010 , 0x0110 );
			ArduCAM_wrSensorReg16_16(0x0012 , 0x00f0 );
			ArduCAM_wrSensorReg16_16(0x002a , 0x7f77 );
			ArduCAM_wrSensorReg16_16(0x0014 , 0x2545 );
			ArduCAM_wrSensorReg16_16(0x0014 , 0x2547 );
			ArduCAM_wrSensorReg16_16(0x0014 , 0x3447 );
			ArduCAM_wrSensorReg16_16(0x0014 , 0x3047 );
			delay(10);
			ArduCAM_wrSensorReg16_16(0x0014 , 0x3046 );
			ArduCAM_wrSensorReg16_16(0x0022 , 0x01f4 );
			ArduCAM_wrSensorReg16_16(0x001e , 0x0707 );
			ArduCAM_wrSensorReg16_16(0x3b84 , 0x01f4 );
			ArduCAM_wrSensorReg16_16(0x002e , 0x0500 );
			ArduCAM_wrSensorReg16_16(0x0018 , 0x402b );
			ArduCAM_wrSensorReg16_16(0x3b82 , 0x0004 );
			ArduCAM_wrSensorReg16_16(0x0018 , 0x402f );
			ArduCAM_wrSensorReg16_16(0x0018 , 0x402e );
			delay(50);
			ArduCAM_wrSensorReg16_16(0x0614 , 0x0001 );
			delay(1);
			ArduCAM_wrSensorReg16_16(0x0614 , 0x0001 );
			delay(1);
			ArduCAM_wrSensorReg16_16(0x0614 , 0x0001 );
			delay(1);
			ArduCAM_wrSensorReg16_16(0x0614 , 0x0001 );
			delay(1);
			ArduCAM_wrSensorReg16_16(0x0614 , 0x0001 );
			delay(1);
			ArduCAM_wrSensorReg16_16(0x0614 , 0x0001 );
			delay(1);
			delay(10);
			//init pll
			ArduCAM_wrSensorReg16_16(0x098e , 0x6800 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0140 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x6802 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x00f0 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x68a0 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x68a0 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x082d );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4802 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0000 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4804 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0000 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4806 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x060d );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4808 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x080d );
			ArduCAM_wrSensorReg16_16(0x098e , 0x480c );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x046c );
			ArduCAM_wrSensorReg16_16(0x098e , 0x480f );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x00cc );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4811 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0381 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4813 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x024f );
			ArduCAM_wrSensorReg16_16(0x098e , 0x481d );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0436 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x481f );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x05d0 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4825 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x1153 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x6ca0 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x6ca0 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x082d );
			ArduCAM_wrSensorReg16_16(0x098e , 0x484a );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0004 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x484c );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0004 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x484e );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x060b );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4850 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x080b );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4857 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x008c );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4859 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x01f1 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x485b );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x00ff );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4865 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0668 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4867 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0af0 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x486d );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0af0 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xa005 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0001 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x6c11 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0003 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x6811 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0003 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc8a5 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0025 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc8a6 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0028 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc8a7 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x002c );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc8a8 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x002f );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc844 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x00ba );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc92f );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0000 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc845 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x009b );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc92d );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0000 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc88c );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0082 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc930 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0000 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc88d );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x006d );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc92e );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0000 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xa002 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0010 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xa009 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0002 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xa00a );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0003 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xa00c );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x000a );
			ArduCAM_wrSensorReg16_16(0x098e , 0x4846 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0014 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x488e );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0014 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc844 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0085 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc845 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x006e );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc88c );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0082 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc88d );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x006c );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc8a5 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x001b );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc8a6 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x001e );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc8a7 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0020 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xc8a8 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0023 );
			//init setting
			ArduCAM_wrSensorReg16_16(0x0018 , 0x002a );
			ArduCAM_wrSensorReg16_16(0x3084 , 0x2409 );
			ArduCAM_wrSensorReg16_16(0x3092 , 0x0a49 );
			ArduCAM_wrSensorReg16_16(0x3094 , 0x4949 );
			ArduCAM_wrSensorReg16_16(0x3096 , 0x4950 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x68a0 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0a2e );
			ArduCAM_wrSensorReg16_16(0x098e , 0x6ca0 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0a2e );
			ArduCAM_wrSensorReg16_16(0x098e , 0x6c90 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0cb4 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x6807 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0004 );
			ArduCAM_wrSensorReg16_16(0x098e , 0xe88e );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0000 );
			ArduCAM_wrSensorReg16_16(0x316c , 0x350f );
			ArduCAM_wrSensorReg16_16(0x001e , 0x0777 );
			ArduCAM_wrSensorReg16_16(0x098e , 0x8400 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0001 );
			delay(100);
			ArduCAM_wrSensorReg16_16(0x098e , 0x8400 );
			ArduCAM_wrSensorReg16_16(0x0990 , 0x0006 );
#endif
			break;
		}
		case MT9D112:
		{
#if defined(MT9D112_CAM)
			ArduCAM_wrSensorReg16_16(0x301a , 0x0acc );
			ArduCAM_wrSensorReg16_16(0x3202 , 0x0008 );
			delay(100 );
			ArduCAM_wrSensorReg16_16(0x341e , 0x8f09 );
			ArduCAM_wrSensorReg16_16(0x341c , 0x020c );
			delay(100 );
			ArduCAM_wrSensorRegs16_16(MT9D112_default_setting);
			ArduCAM_wrSensorReg16_16(0x338c , 0xa103 );
			ArduCAM_wrSensorReg16_16(0x3390 , 0x0006 );
			delay(100 );
			ArduCAM_wrSensorReg16_16(0x338c , 0xa103 );
			ArduCAM_wrSensorReg16_16(0x3390 , 0x0005 );
			delay(100 );
			ArduCAM_wrSensorRegs16_16(MT9D112_soc_init);
			delay(200);
			ArduCAM_wrSensorReg16_16(0x332E, 0x0020); //RGB565

#endif
		}
		default:
			break;
	}
}

void ArduCAM_flush_fifo(void)
{
	ArduCAM_write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

void ArduCAM_start_capture(void)
{
	ArduCAM_write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

void ArduCAM_clear_fifo_flag(void)
{
	ArduCAM_write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

uint32_t ArduCAM_read_fifo_length(void)
{
	uint32_t len1,len2,len3,length=0;
	len1 = ArduCAM_read_reg(FIFO_SIZE1);
	len2 = ArduCAM_read_reg(FIFO_SIZE2);
	len3 = ArduCAM_read_reg(FIFO_SIZE3) & 0x7f;
	length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
	return length;	
}

void ArduCAM_set_fifo_burst(void)
{
	SPI.transfer(BURST_FIFO_READ);
}

void ArduCAM_CS_HIGH(void)
{
	digitalWrite(chip_sel, HIGH);
}

void ArduCAM_CS_LOW(void)
{
	digitalWrite(chip_sel, LOW);
}

uint8_t ArduCAM_read_fifo(void)
{
	uint8_t data;
	data = ArduCAM_bus_read(SINGLE_FIFO_READ);
	return data;
}

uint8_t ArduCAM_read_reg(uint8_t addr)
{
	uint8_t data;
	data = ArduCAM_bus_read(addr & 0x7F);
	return data;
}

void ArduCAM_write_reg(uint8_t addr, uint8_t data)
{
	ArduCAM_bus_write(addr | 0x80, data);
}

//Set corresponding bit
void ArduCAM_set_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = ArduCAM_read_reg(addr);
	ArduCAM_write_reg(addr, (uint8_t)(temp | bit));
}
//Clear corresponding bit 
void ArduCAM_clear_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = ArduCAM_read_reg(addr);
	ArduCAM_write_reg(addr, (uint8_t)(temp & (~bit)));
}

//Get corresponding bit status
uint8_t ArduCAM_get_bit(uint8_t addr, uint8_t bit)
{
	uint8_t temp;
	temp = ArduCAM_read_reg(addr);
	temp = (uint8_t)(temp & bit);
	return temp;
}

//Set ArduCAM working mode
//MCU2LCD_MODE: MCU writes the LCD screen GRAM
//CAM2LCD_MODE: Camera takes control of the LCD screen
//LCD2MCU_MODE: MCU read the LCD screen GRAM
void ArduCAM_set_mode(uint8_t mode)
{
	switch (mode) {
		case MCU2LCD_MODE:
			ArduCAM_write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
			break;
		case CAM2LCD_MODE:
			ArduCAM_write_reg(ARDUCHIP_MODE, CAM2LCD_MODE);
			break;
		case LCD2MCU_MODE:
			ArduCAM_write_reg(ARDUCHIP_MODE, LCD2MCU_MODE);
			break;
		default:
			ArduCAM_write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
			break;
	}
}

uint8_t ArduCAM_bus_write(int address,int value)
{	
	ArduCAM_CS_LOW();
	SPI.transfer((uint8_t)address);
	SPI.transfer((uint8_t)value);
	ArduCAM_CS_HIGH();
	return 1;
}

uint8_t ArduCAM_bus_read(int address)
{
	uint8_t value;
	ArduCAM_CS_LOW();
	SPI.transfer((uint8_t)address);
	value = SPI.transfer(0x00);
	// take the SS pin high to de-select the chip:
	ArduCAM_CS_HIGH();
	return value;
}

void ArduCAM_OV3640_set_JPEG_size(uint8_t size)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_2MP))
	switch(size) {
		case OV3640_176x144:
			ArduCAM_wrSensorRegs8_8(OV3640_176x144_JPEG);
			break;
		case OV3640_320x240:
			ArduCAM_wrSensorRegs16_8(OV3640_320x240_JPEG);
			break;
		case OV3640_352x288:
			ArduCAM_wrSensorRegs16_8(OV3640_352x288_JPEG);
			break;
		case OV3640_640x480:
			ArduCAM_wrSensorRegs16_8(OV3640_640x480_JPEG);
			break;
		case OV3640_800x600:
			ArduCAM_wrSensorRegs16_8(OV3640_800x600_JPEG);
			break;
		case OV3640_1024x768:
			ArduCAM_wrSensorRegs16_8(OV3640_1024x768_JPEG);
			break;
		case OV3640_1280x960:
			ArduCAM_wrSensorRegs16_8(OV3640_1280x960_JPEG);
			break;
		case OV3640_1600x1200:
			ArduCAM_wrSensorRegs16_8(OV3640_1600x1200_JPEG);
			break;
		case OV3640_2048x1536:
			ArduCAM_wrSensorRegs16_8(OV3640_2048x1536_JPEG);
			break;
		default:
			ArduCAM_wrSensorRegs16_8(OV3640_320x240_JPEG);
			break;
	}
#endif
}

void ArduCAM_OV2640_set_JPEG_size(uint8_t size)
{
#if (defined(OV2640_CAM)|| defined(OV2640_MINI_2MP) || defined(OV2640_MINI_2MP_PLUS))
	switch(size) {
		case OV2640_160x120:
			ArduCAM_wrSensorRegs8_8(OV2640_160x120_JPEG);
			break;
		case OV2640_176x144:
			ArduCAM_wrSensorRegs8_8(OV2640_176x144_JPEG);
			break;
		case OV2640_320x240:
			ArduCAM_wrSensorRegs8_8(OV2640_320x240_JPEG);
			break;
		case OV2640_352x288:
			ArduCAM_wrSensorRegs8_8(OV2640_352x288_JPEG);
			break;
		case OV2640_640x480:
			ArduCAM_wrSensorRegs8_8(OV2640_640x480_JPEG);
			break;
		case OV2640_800x600:
			ArduCAM_wrSensorRegs8_8(OV2640_800x600_JPEG);
			break;
		case OV2640_1024x768:
			ArduCAM_wrSensorRegs8_8(OV2640_1024x768_JPEG);
			break;
		case OV2640_1280x1024:
			ArduCAM_wrSensorRegs8_8(OV2640_1280x1024_JPEG);
			break;
		case OV2640_1600x1200:
			ArduCAM_wrSensorRegs8_8(OV2640_1600x1200_JPEG);
			break;
		default:
			ArduCAM_wrSensorRegs8_8(OV2640_320x240_JPEG);
			break;
	}
#endif
}

void ArduCAM_OV5642_set_RAW_size(uint8_t size)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch (size) {
		case OV5642_640x480:
			ArduCAM_wrSensorRegs16_8(OV5642_1280x960_RAW);	
			ArduCAM_wrSensorRegs16_8(OV5642_640x480_RAW);	
			break;
		case OV5642_1280x960:
			ArduCAM_wrSensorRegs16_8(OV5642_1280x960_RAW);	
			break;
		case OV5642_1920x1080:
			ArduCAM_wrSensorRegs16_8(ov5642_RAW);
			ArduCAM_wrSensorRegs16_8(OV5642_1920x1080_RAW);
			break;
		case OV5642_2592x1944:
			ArduCAM_wrSensorRegs16_8(ov5642_RAW);
			break;
		default:
			break;
	}
#endif			
}

void ArduCAM_OV5642_set_JPEG_size(uint8_t size)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch (size) {
		case OV5642_320x240:
			ArduCAM_wrSensorRegs16_8(ov5642_320x240);
			break;
		case OV5642_640x480:
			ArduCAM_wrSensorRegs16_8(ov5642_640x480);
			break;
		case OV5642_1024x768:
			ArduCAM_wrSensorRegs16_8(ov5642_1024x768);
			break;
		case OV5642_1280x960:
			ArduCAM_wrSensorRegs16_8(ov5642_1280x960);
			break;
		case OV5642_1600x1200:
			ArduCAM_wrSensorRegs16_8(ov5642_1600x1200);
			break;
		case OV5642_2048x1536:
			ArduCAM_wrSensorRegs16_8(ov5642_2048x1536);
			break;
		case OV5642_2592x1944:
			ArduCAM_wrSensorRegs16_8(ov5642_2592x1944);
			break;
		default:
			ArduCAM_wrSensorRegs16_8(ov5642_320x240);
			break;
	}
#endif
}

void ArduCAM_OV5640_set_JPEG_size(uint8_t size)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch (size) {
		case OV5640_320x240:
			ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2QVGA);
			break;
		case OV5640_352x288:
			ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2CIF);
			break;
		case OV5640_640x480:
			ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2VGA);
			break;
		case OV5640_800x480:
			ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2WVGA);
			break;
		case OV5640_1024x768:
			ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2XGA);
			break;
		case OV5640_1280x960:
			ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2SXGA);
			break;
		case OV5640_1600x1200:
			ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2UXGA);
			break;
		case OV5640_2048x1536:
			ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2QXGA);
			break;
		case OV5640_2592x1944:
			ArduCAM_wrSensorRegs16_8(OV5640_JPEG_QSXGA);
			break;
		default:
			//320x240
			ArduCAM_wrSensorRegs16_8(OV5640_QSXGA2QVGA);
			break;
	}
#endif
}

void ArduCAM_set_format(byte fmt)
{
	if (fmt == BMP)
		m_fmt = BMP;
	else if(fmt == RAW)
		m_fmt = RAW;
	else
		m_fmt = JPEG;
}

void ArduCAM_OV2640_set_Light_Mode(uint8_t Light_Mode)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP) || defined(OV2640_MINI_2MP_PLUS))
	switch(Light_Mode)
	{
		case Auto:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0xc7, 0x00); //AWB on
			break;
		case Sunny:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0xc7, 0x40); //AWB off
			ArduCAM_wrSensorReg8_8(0xcc, 0x5e);
			ArduCAM_wrSensorReg8_8(0xcd, 0x41);
			ArduCAM_wrSensorReg8_8(0xce, 0x54);
			break;
		case Cloudy:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0xc7, 0x40); //AWB off
			ArduCAM_wrSensorReg8_8(0xcc, 0x65);
			ArduCAM_wrSensorReg8_8(0xcd, 0x41);
			ArduCAM_wrSensorReg8_8(0xce, 0x4f);
			break;
		case Office:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0xc7, 0x40); //AWB off
			ArduCAM_wrSensorReg8_8(0xcc, 0x52);
			ArduCAM_wrSensorReg8_8(0xcd, 0x41);
			ArduCAM_wrSensorReg8_8(0xce, 0x66);
			break;
		case Home:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0xc7, 0x40); //AWB off
			ArduCAM_wrSensorReg8_8(0xcc, 0x42);
			ArduCAM_wrSensorReg8_8(0xcd, 0x3f);
			ArduCAM_wrSensorReg8_8(0xce, 0x71);
			break;
		default :
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0xc7, 0x00); //AWB on
			break; 
	}	
#endif
}

void ArduCAM_OV3640_set_Light_Mode(uint8_t Light_Mode)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch(Light_Mode)
	{
		case Auto:
			ArduCAM_wrSensorReg16_8(0x332b, 0x00); //AWB auto, bit[3]:0,auto
			break;
		case Sunny:
			ArduCAM_wrSensorReg16_8(0x332b, 0x08); //AWB off
			ArduCAM_wrSensorReg16_8(0x33a7, 0x5e);
			ArduCAM_wrSensorReg16_8(0x33a8, 0x40);
			ArduCAM_wrSensorReg16_8(0x33a9, 0x46);
			break;
		case Cloudy:
			ArduCAM_wrSensorReg16_8(0x332b, 0x08);
			ArduCAM_wrSensorReg16_8(0x33a7, 0x68);
			ArduCAM_wrSensorReg16_8(0x33a8, 0x40);
			ArduCAM_wrSensorReg16_8(0x33a9, 0x4e);	
			break;
		case Office:
			ArduCAM_wrSensorReg16_8(0x332b, 0x08);
			ArduCAM_wrSensorReg16_8(0x33a7, 0x52);
			ArduCAM_wrSensorReg16_8(0x33a8, 0x40);
			ArduCAM_wrSensorReg16_8(0x33a9, 0x58);
			break;
		case Home:
			ArduCAM_wrSensorReg16_8(0x332b, 0x08);
			ArduCAM_wrSensorReg16_8(0x33a7, 0x44);
			ArduCAM_wrSensorReg16_8(0x33a8, 0x40);
			ArduCAM_wrSensorReg16_8(0x33a9, 0x70);
			break;
	}
#endif
}

void ArduCAM_OV5642_set_Light_Mode(uint8_t Light_Mode)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch(Light_Mode)
	{
		case Advanced_AWB:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x5192 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5191 ,0xf8);
			ArduCAM_wrSensorReg16_8(0x518d ,0x26);
			ArduCAM_wrSensorReg16_8(0x518f ,0x42);
			ArduCAM_wrSensorReg16_8(0x518e ,0x2b);
			ArduCAM_wrSensorReg16_8(0x5190 ,0x42);
			ArduCAM_wrSensorReg16_8(0x518b ,0xd0);
			ArduCAM_wrSensorReg16_8(0x518c ,0xbd);
			ArduCAM_wrSensorReg16_8(0x5187 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5188 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5189 ,0x56);
			ArduCAM_wrSensorReg16_8(0x518a ,0x5c);
			ArduCAM_wrSensorReg16_8(0x5186 ,0x1c);
			ArduCAM_wrSensorReg16_8(0x5181 ,0x50);
			ArduCAM_wrSensorReg16_8(0x5184 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5182 ,0x11);
			ArduCAM_wrSensorReg16_8(0x5183 ,0x0 );	
			break;
		case Simple_AWB:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5183 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5191 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5192 ,0x00);
			break;
		case Manual_day:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x1 );
			ArduCAM_wrSensorReg16_8(0x3400 ,0x7 );
			ArduCAM_wrSensorReg16_8(0x3401 ,0x32);
			ArduCAM_wrSensorReg16_8(0x3402 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3403 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x3404 ,0x5 );
			ArduCAM_wrSensorReg16_8(0x3405 ,0x36);
			break;
		case Manual_A:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x1 );
			ArduCAM_wrSensorReg16_8(0x3400 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3401 ,0x88);
			ArduCAM_wrSensorReg16_8(0x3402 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3403 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x3404 ,0x8 );
			ArduCAM_wrSensorReg16_8(0x3405 ,0xb6);
			break;
		case Manual_cwf:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x1 );
			ArduCAM_wrSensorReg16_8(0x3400 ,0x6 );
			ArduCAM_wrSensorReg16_8(0x3401 ,0x13);
			ArduCAM_wrSensorReg16_8(0x3402 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3403 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x3404 ,0x7 );
			ArduCAM_wrSensorReg16_8(0x3405 ,0xe2);
			break;
		case Manual_cloudy:
			ArduCAM_wrSensorReg16_8(0x3406 ,0x1 );
			ArduCAM_wrSensorReg16_8(0x3400 ,0x7 );
			ArduCAM_wrSensorReg16_8(0x3401 ,0x88);
			ArduCAM_wrSensorReg16_8(0x3402 ,0x4 );
			ArduCAM_wrSensorReg16_8(0x3403 ,0x0 );
			ArduCAM_wrSensorReg16_8(0x3404 ,0x5 );
			ArduCAM_wrSensorReg16_8(0x3405 ,0x0 );
			break;
		default :
			break; 
	}	
#endif
}

void ArduCAM_OV5640_set_Light_Mode(uint8_t Light_Mode)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch(Light_Mode)
	{
		case Auto:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x3406, 0x00);
			ArduCAM_wrSensorReg16_8(0x3400, 0x04);
			ArduCAM_wrSensorReg16_8(0x3401, 0x00);
			ArduCAM_wrSensorReg16_8(0x3402, 0x04);
			ArduCAM_wrSensorReg16_8(0x3403, 0x00);
			ArduCAM_wrSensorReg16_8(0x3404, 0x04);
			ArduCAM_wrSensorReg16_8(0x3405, 0x00);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
			ArduCAM_wrSensorReg16_8(0x5183 ,0x0 );	
			break;
		case Sunny:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x3406, 0x01);
			ArduCAM_wrSensorReg16_8(0x3400, 0x06);
			ArduCAM_wrSensorReg16_8(0x3401, 0x1c);
			ArduCAM_wrSensorReg16_8(0x3402, 0x04);
			ArduCAM_wrSensorReg16_8(0x3403, 0x00);
			ArduCAM_wrSensorReg16_8(0x3404, 0x04);
			ArduCAM_wrSensorReg16_8(0x3405, 0xf3);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
			break;
		case Office:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x3406, 0x01);
			ArduCAM_wrSensorReg16_8(0x3400, 0x05);
			ArduCAM_wrSensorReg16_8(0x3401, 0x48);
			ArduCAM_wrSensorReg16_8(0x3402, 0x04);
			ArduCAM_wrSensorReg16_8(0x3403, 0x00);
			ArduCAM_wrSensorReg16_8(0x3404, 0x07);
			ArduCAM_wrSensorReg16_8(0x3405, 0xcf);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x3406, 0x01);
			ArduCAM_wrSensorReg16_8(0x3400, 0x06);
			ArduCAM_wrSensorReg16_8(0x3401, 0x48);
			ArduCAM_wrSensorReg16_8(0x3402, 0x04);
			ArduCAM_wrSensorReg16_8(0x3403, 0x00);
			ArduCAM_wrSensorReg16_8(0x3404, 0x04);
			ArduCAM_wrSensorReg16_8(0x3405, 0xd3);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
			break;
		case Cloudy:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x3406, 0x01);
			ArduCAM_wrSensorReg16_8(0x3400, 0x06);
			ArduCAM_wrSensorReg16_8(0x3401, 0x48);
			ArduCAM_wrSensorReg16_8(0x3402, 0x04);
			ArduCAM_wrSensorReg16_8(0x3403, 0x00);
			ArduCAM_wrSensorReg16_8(0x3404, 0x04);
			ArduCAM_wrSensorReg16_8(0x3405, 0xd3);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3	
			break;
		case Home:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x3406, 0x01);
			ArduCAM_wrSensorReg16_8(0x3400, 0x04);
			ArduCAM_wrSensorReg16_8(0x3401, 0x10);
			ArduCAM_wrSensorReg16_8(0x3402, 0x04);
			ArduCAM_wrSensorReg16_8(0x3403, 0x00);
			ArduCAM_wrSensorReg16_8(0x3404, 0x08);
			ArduCAM_wrSensorReg16_8(0x3405, 0x40);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // lanuch group 3
			break;
		default :
			break; 
	}
#endif
}

void ArduCAM_OV2640_set_Color_Saturation(uint8_t Color_Saturation)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP) || defined(OV2640_MINI_2MP_PLUS))
	switch(Color_Saturation)
	{
		case Saturation2:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x02);
			ArduCAM_wrSensorReg8_8(0x7c, 0x03);
			ArduCAM_wrSensorReg8_8(0x7d, 0x68);
			ArduCAM_wrSensorReg8_8(0x7d, 0x68);
			break;
		case Saturation1:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x02);
			ArduCAM_wrSensorReg8_8(0x7c, 0x03);
			ArduCAM_wrSensorReg8_8(0x7d, 0x58);
			ArduCAM_wrSensorReg8_8(0x7d, 0x58);
			break;
		case Saturation0:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x02);
			ArduCAM_wrSensorReg8_8(0x7c, 0x03);
			ArduCAM_wrSensorReg8_8(0x7d, 0x48);
			ArduCAM_wrSensorReg8_8(0x7d, 0x48);
			break;
		case Saturation_1:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x02);
			ArduCAM_wrSensorReg8_8(0x7c, 0x03);
			ArduCAM_wrSensorReg8_8(0x7d, 0x38);
			ArduCAM_wrSensorReg8_8(0x7d, 0x38);
			break;
		case Saturation_2:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x02);
			ArduCAM_wrSensorReg8_8(0x7c, 0x03);
			ArduCAM_wrSensorReg8_8(0x7d, 0x28);
			ArduCAM_wrSensorReg8_8(0x7d, 0x28);
			break;	
	}
#endif	
}

void ArduCAM_OV3640_set_Color_Saturation(uint8_t Color_Saturation)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch(Color_Saturation)
	{
		case Saturation2:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef); //bit[7]:1, enable SDE
			ArduCAM_wrSensorReg16_8(0x3355, 0x02); //enable color saturation
			ArduCAM_wrSensorReg16_8(0x3358, 0x70);
			ArduCAM_wrSensorReg16_8(0x3359, 0x70);
			break;
		case Saturation1:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x02);
			ArduCAM_wrSensorReg16_8(0x3358, 0x50);
			ArduCAM_wrSensorReg16_8(0x3359, 0x50);
			break;
		case Saturation0:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x02);
			ArduCAM_wrSensorReg16_8(0x3358, 0x40);
			ArduCAM_wrSensorReg16_8(0x3359, 0x40);
			break;
		case Saturation_1:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x02);
			ArduCAM_wrSensorReg16_8(0x3358, 0x30);
			ArduCAM_wrSensorReg16_8(0x3359, 0x30);
			break;
		case Saturation_2:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x02);
			ArduCAM_wrSensorReg16_8(0x3358, 0x20);
			ArduCAM_wrSensorReg16_8(0x3359, 0x20);
			break;
	}
#endif
}

void ArduCAM_OV5640_set_Color_Saturation(uint8_t Color_Saturation)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch(Color_Saturation)
	{
		case Saturation3:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5381, 0x1c);
			ArduCAM_wrSensorReg16_8(0x5382, 0x5a);
			ArduCAM_wrSensorReg16_8(0x5383, 0x06);
			ArduCAM_wrSensorReg16_8(0x5384, 0x2b);
			ArduCAM_wrSensorReg16_8(0x5385, 0xab);
			ArduCAM_wrSensorReg16_8(0x5386, 0xd6);
			ArduCAM_wrSensorReg16_8(0x5387, 0xda);
			ArduCAM_wrSensorReg16_8(0x5388, 0xd6);
			ArduCAM_wrSensorReg16_8(0x5389, 0x04);
			ArduCAM_wrSensorReg16_8(0x538b, 0x98);
			ArduCAM_wrSensorReg16_8(0x538a, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Saturation2:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5381, 0x1c);
			ArduCAM_wrSensorReg16_8(0x5382, 0x5a);
			ArduCAM_wrSensorReg16_8(0x5383, 0x06);
			ArduCAM_wrSensorReg16_8(0x5384, 0x24);
			ArduCAM_wrSensorReg16_8(0x5385, 0x8f);
			ArduCAM_wrSensorReg16_8(0x5386, 0xb3);
			ArduCAM_wrSensorReg16_8(0x5387, 0xb6);
			ArduCAM_wrSensorReg16_8(0x5388, 0xb3);
			ArduCAM_wrSensorReg16_8(0x5389, 0x03);
			ArduCAM_wrSensorReg16_8(0x538b, 0x98);
			ArduCAM_wrSensorReg16_8(0x538a, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Saturation1:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5381, 0x1c);
			ArduCAM_wrSensorReg16_8(0x5382, 0x5a);
			ArduCAM_wrSensorReg16_8(0x5383, 0x06);
			ArduCAM_wrSensorReg16_8(0x5384, 0x1f);
			ArduCAM_wrSensorReg16_8(0x5385, 0x7a);
			ArduCAM_wrSensorReg16_8(0x5386, 0x9a);
			ArduCAM_wrSensorReg16_8(0x5387, 0x9c);
			ArduCAM_wrSensorReg16_8(0x5388, 0x9a);
			ArduCAM_wrSensorReg16_8(0x5389, 0x02);
			ArduCAM_wrSensorReg16_8(0x538b, 0x98);
			ArduCAM_wrSensorReg16_8(0x538a, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Saturation0:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5381, 0x1c);
			ArduCAM_wrSensorReg16_8(0x5382, 0x5a);
			ArduCAM_wrSensorReg16_8(0x5383, 0x06);
			ArduCAM_wrSensorReg16_8(0x5384, 0x1a);
			ArduCAM_wrSensorReg16_8(0x5385, 0x66);
			ArduCAM_wrSensorReg16_8(0x5386, 0x80);
			ArduCAM_wrSensorReg16_8(0x5387, 0x82);
			ArduCAM_wrSensorReg16_8(0x5388, 0x80);
			ArduCAM_wrSensorReg16_8(0x5389, 0x02);
			ArduCAM_wrSensorReg16_8(0x538b, 0x98);
			ArduCAM_wrSensorReg16_8(0x538a, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;		
		case Saturation_1:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5381, 0x1c);
			ArduCAM_wrSensorReg16_8(0x5382, 0x5a);
			ArduCAM_wrSensorReg16_8(0x5383, 0x06);
			ArduCAM_wrSensorReg16_8(0x5384, 0x15);
			ArduCAM_wrSensorReg16_8(0x5385, 0x52);
			ArduCAM_wrSensorReg16_8(0x5386, 0x66);
			ArduCAM_wrSensorReg16_8(0x5387, 0x68);
			ArduCAM_wrSensorReg16_8(0x5388, 0x66);
			ArduCAM_wrSensorReg16_8(0x5389, 0x02);
			ArduCAM_wrSensorReg16_8(0x538b, 0x98);
			ArduCAM_wrSensorReg16_8(0x538a, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Saturation_2:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5381, 0x1c);
			ArduCAM_wrSensorReg16_8(0x5382, 0x5a);
			ArduCAM_wrSensorReg16_8(0x5383, 0x06);
			ArduCAM_wrSensorReg16_8(0x5384, 0x10);
			ArduCAM_wrSensorReg16_8(0x5385, 0x3d);
			ArduCAM_wrSensorReg16_8(0x5386, 0x4d);
			ArduCAM_wrSensorReg16_8(0x5387, 0x4e);
			ArduCAM_wrSensorReg16_8(0x5388, 0x4d);
			ArduCAM_wrSensorReg16_8(0x5389, 0x01);
			ArduCAM_wrSensorReg16_8(0x538b, 0x98);
			ArduCAM_wrSensorReg16_8(0x538a, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Saturation_3:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5381, 0x1c);
			ArduCAM_wrSensorReg16_8(0x5382, 0x5a);
			ArduCAM_wrSensorReg16_8(0x5383, 0x06);
			ArduCAM_wrSensorReg16_8(0x5384, 0x0c);
			ArduCAM_wrSensorReg16_8(0x5385, 0x30);
			ArduCAM_wrSensorReg16_8(0x5386, 0x3d);
			ArduCAM_wrSensorReg16_8(0x5387, 0x3e);
			ArduCAM_wrSensorReg16_8(0x5388, 0x3d);
			ArduCAM_wrSensorReg16_8(0x5389, 0x01);
			ArduCAM_wrSensorReg16_8(0x538b, 0x98);
			ArduCAM_wrSensorReg16_8(0x538a, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
	}
#endif
}

void ArduCAM_OV5642_set_Color_Saturation(uint8_t Color_Saturation)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch(Color_Saturation)
	{
		case Saturation4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
			break;
		case Saturation3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x70);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x70);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
			break;
		case Saturation2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x60);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x60);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
			break;
		case Saturation1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x50);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x50);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
			break;
		case Saturation0:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
			break;		
		case Saturation_1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
			break;
		case Saturation_2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
			break;
		case Saturation_3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
			break;
		case Saturation_4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5583 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5584 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x02);
			break;
		default:
			break;
	}
#endif	
}

void ArduCAM_OV2640_set_Brightness(uint8_t Brightness)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP) || defined(OV2640_MINI_2MP_PLUS))
	switch(Brightness)
	{
		case Brightness2:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x09);
			ArduCAM_wrSensorReg8_8(0x7d, 0x40);
			ArduCAM_wrSensorReg8_8(0x7d, 0x00);
			break;
		case Brightness1:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x09);
			ArduCAM_wrSensorReg8_8(0x7d, 0x30);
			ArduCAM_wrSensorReg8_8(0x7d, 0x00);
			break;	
		case Brightness0:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x09);
			ArduCAM_wrSensorReg8_8(0x7d, 0x20);
			ArduCAM_wrSensorReg8_8(0x7d, 0x00);
			break;
		case Brightness_1:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x09);
			ArduCAM_wrSensorReg8_8(0x7d, 0x10);
			ArduCAM_wrSensorReg8_8(0x7d, 0x00);
			break;
		case Brightness_2:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x09);
			ArduCAM_wrSensorReg8_8(0x7d, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x00);
			break;	
	}
#endif	

}
void ArduCAM_OV3640_set_Brightness(uint8_t Brightness)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))
	switch(Brightness)
	{
		case Brightness3:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04); //bit[2] enable
			ArduCAM_wrSensorReg16_8(0x3354, 0x01); //bit[3] sign of brightness
			ArduCAM_wrSensorReg16_8(0x335e, 0x30);
			break;
		case Brightness2:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x01);
			ArduCAM_wrSensorReg16_8(0x335e, 0x20);
			break;
		case Brightness1:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x01);
			ArduCAM_wrSensorReg16_8(0x335e, 0x10);
			break;
		case Brightness0:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x01);
			ArduCAM_wrSensorReg16_8(0x335e, 0x00);
			break;
		case Brightness_1:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x09);
			ArduCAM_wrSensorReg16_8(0x335e, 0x10);
			break;
		case Brightness_2:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x09);
			ArduCAM_wrSensorReg16_8(0x335e, 0x20);
			break;
		case Brightness_3:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x09);
			ArduCAM_wrSensorReg16_8(0x335e, 0x30);
			break;	
	}
#endif
}

void ArduCAM_OV5642_set_Brightness(uint8_t Brightness)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))

	switch(Brightness)
	{
		case Brightness4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Brightness3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;	
		case Brightness2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Brightness1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Brightness0:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;	
		case Brightness_1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x08);
			break;	
		case Brightness_2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x08);
			break;	
		case Brightness_3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x08);
			break;	
		case Brightness_4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5589 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x558a ,0x08);
			break;
		default:
			break;
	}
#endif	

}

void ArduCAM_OV5640_set_Brightness(uint8_t Brightness)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch(Brightness)
	{
		case Brightness4:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5587, 0x40);
			ArduCAM_wrSensorReg16_8(0x5588, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Brightness3:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5587, 0x30);
			ArduCAM_wrSensorReg16_8(0x5588, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Brightness2:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5587, 0x20);
			ArduCAM_wrSensorReg16_8(0x5588, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Brightness1:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5587, 0x10);
			ArduCAM_wrSensorReg16_8(0x5588, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3	
			break;
		case Brightness0:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5587, 0x00);
			ArduCAM_wrSensorReg16_8(0x5588, 0x01);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3	
			break;
		case Brightness_1:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5587, 0x10);
			ArduCAM_wrSensorReg16_8(0x5588, 0x09);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Brightness_2:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5587, 0x20);
			ArduCAM_wrSensorReg16_8(0x5588, 0x09);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Brightness_3:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5587, 0x30);
			ArduCAM_wrSensorReg16_8(0x5588, 0x09);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Brightness_4:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5587, 0x40);
			ArduCAM_wrSensorReg16_8(0x5588, 0x09);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;							
	}
#endif
}

void ArduCAM_OV2640_set_Contrast(uint8_t Contrast)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP) || defined(OV2640_MINI_2MP_PLUS))	
	switch(Contrast)
	{
		case Contrast2:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x07);
			ArduCAM_wrSensorReg8_8(0x7d, 0x20);
			ArduCAM_wrSensorReg8_8(0x7d, 0x28);
			ArduCAM_wrSensorReg8_8(0x7d, 0x0c);
			ArduCAM_wrSensorReg8_8(0x7d, 0x06);
			break;
		case Contrast1:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x07);
			ArduCAM_wrSensorReg8_8(0x7d, 0x20);
			ArduCAM_wrSensorReg8_8(0x7d, 0x24);
			ArduCAM_wrSensorReg8_8(0x7d, 0x16);
			ArduCAM_wrSensorReg8_8(0x7d, 0x06); 
			break;
		case Contrast0:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x07);
			ArduCAM_wrSensorReg8_8(0x7d, 0x20);
			ArduCAM_wrSensorReg8_8(0x7d, 0x20);
			ArduCAM_wrSensorReg8_8(0x7d, 0x20);
			ArduCAM_wrSensorReg8_8(0x7d, 0x06); 
			break;
		case Contrast_1:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x07);
			ArduCAM_wrSensorReg8_8(0x7d, 0x20);
			ArduCAM_wrSensorReg8_8(0x7d, 0x20);
			ArduCAM_wrSensorReg8_8(0x7d, 0x2a);
			ArduCAM_wrSensorReg8_8(0x7d, 0x06);	
			break;
		case Contrast_2:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x04);
			ArduCAM_wrSensorReg8_8(0x7c, 0x07);
			ArduCAM_wrSensorReg8_8(0x7d, 0x20);
			ArduCAM_wrSensorReg8_8(0x7d, 0x18);
			ArduCAM_wrSensorReg8_8(0x7d, 0x34);
			ArduCAM_wrSensorReg8_8(0x7d, 0x06);
			break;
	}
#endif		
}

void ArduCAM_OV3640_set_Contrast(uint8_t Contrast)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))	
	switch(Contrast)
	{
		case Contrast3:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04); //bit[2] enable contrast/brightness
			ArduCAM_wrSensorReg16_8(0x3354, 0x01); //bit[2] Yoffset sign
			ArduCAM_wrSensorReg16_8(0x335c, 0x2c);
			ArduCAM_wrSensorReg16_8(0x335d, 0x2c);
			break;
		case Contrast2:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x01);
			ArduCAM_wrSensorReg16_8(0x335c, 0x28);
			ArduCAM_wrSensorReg16_8(0x335d, 0x28);
			break;
		case Contrast1:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x01);
			ArduCAM_wrSensorReg16_8(0x335c, 0x24);
			ArduCAM_wrSensorReg16_8(0x335d, 0x24);
			break;
		case Contrast0:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x01);
			ArduCAM_wrSensorReg16_8(0x335c, 0x20);
			ArduCAM_wrSensorReg16_8(0x335d, 0x20);
			break;
		case Contrast_1:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x01);
			ArduCAM_wrSensorReg16_8(0x335c, 0x1c);
			ArduCAM_wrSensorReg16_8(0x335d, 0x1c);
			break;
		case Contrast_2:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x01);
			ArduCAM_wrSensorReg16_8(0x335c, 0x18);
			ArduCAM_wrSensorReg16_8(0x335d, 0x18);
			break;
		case Contrast_3:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x04);
			ArduCAM_wrSensorReg16_8(0x3354, 0x01);
			ArduCAM_wrSensorReg16_8(0x335c, 0x14);
			ArduCAM_wrSensorReg16_8(0x335d, 0x14);
			break;
	}
#endif		
}

void ArduCAM_OV5642_set_Contrast(uint8_t Contrast)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch(Contrast)
	{
		case Contrast4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x30);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x30);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Contrast3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x2c);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x2c);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Contrast2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x28);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x28);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Contrast1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x24);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x24);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Contrast0:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x20);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x20);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Contrast_1:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x1C);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x1C);
			ArduCAM_wrSensorReg16_8(0x558a ,0x1C);
			break;
		case Contrast_2:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x18);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Contrast_3:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x14);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x14);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		case Contrast_4:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x04);
			ArduCAM_wrSensorReg16_8(0x5587 ,0x10);
			ArduCAM_wrSensorReg16_8(0x5588 ,0x10);
			ArduCAM_wrSensorReg16_8(0x558a ,0x00);
			break;
		default:
			break;
	}
#endif		
}

void ArduCAM_OV5640_set_Contrast(uint8_t Contrast)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch(Contrast)
	{
		case Contrast3:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5586, 0x2c);
			ArduCAM_wrSensorReg16_8(0x5585, 0x1c);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Contrast2:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5586, 0x28);
			ArduCAM_wrSensorReg16_8(0x5585, 0x18);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Contrast1:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5586, 0x24);
			ArduCAM_wrSensorReg16_8(0x5585, 0x10);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Contrast0:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5586, 0x20);
			ArduCAM_wrSensorReg16_8(0x5585, 0x00);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Contrast_1:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5586, 0x1c);
			ArduCAM_wrSensorReg16_8(0x5585, 0x1c);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Contrast_2:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5586, 0x18);
			ArduCAM_wrSensorReg16_8(0x5585, 0x18);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Contrast_3:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5586, 0x14);
			ArduCAM_wrSensorReg16_8(0x5585, 0x14);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;	
	}
#endif
}

void ArduCAM_OV5642_set_hue(uint8_t degree)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch(degree)
	{
		case degree_180:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x00);
			ArduCAM_wrSensorReg16_8(0x558a ,0x32);
			break;
		case degree_150:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x40);
			ArduCAM_wrSensorReg16_8(0x558a ,0x32);
			break;
		case degree_120:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x558a ,0x32);
			break;
		case degree_90:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x80);
			ArduCAM_wrSensorReg16_8(0x558a ,0x02);
			break;
		case degree_60:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x558a ,0x02);
			break;
		case degree_30:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x40);
			ArduCAM_wrSensorReg16_8(0x558a ,0x02);
			break;
		case degree_0:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x00);
			ArduCAM_wrSensorReg16_8(0x558a ,0x01);
			break;
		case degree30:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x40);
			ArduCAM_wrSensorReg16_8(0x558a ,0x01);
			break;
		case degree60:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x558a ,0x01);
			break;
		case degree90:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x00);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x80);
			ArduCAM_wrSensorReg16_8(0x558a ,0x31);
			break;
		case degree120:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x558a ,0x31);
			break;
		case degree150:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x01);
			ArduCAM_wrSensorReg16_8(0x5581 ,0x6f);
			ArduCAM_wrSensorReg16_8(0x5582 ,0x40);
			ArduCAM_wrSensorReg16_8(0x558a ,0x31);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV2640_set_Special_effects(uint8_t Special_effect)
{
#if (defined(OV2640_CAM) || defined(OV2640_MINI_2MP) || defined(OV2640_MINI_2MP_PLUS))	
	switch(Special_effect)
	{
		case Antique:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x18);
			ArduCAM_wrSensorReg8_8(0x7c, 0x05);
			ArduCAM_wrSensorReg8_8(0x7d, 0x40);
			ArduCAM_wrSensorReg8_8(0x7d, 0xa6);
			break;
		case Bluish:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x18);
			ArduCAM_wrSensorReg8_8(0x7c, 0x05);
			ArduCAM_wrSensorReg8_8(0x7d, 0xa0);
			ArduCAM_wrSensorReg8_8(0x7d, 0x40);
			break;
		case Greenish:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x18);
			ArduCAM_wrSensorReg8_8(0x7c, 0x05);
			ArduCAM_wrSensorReg8_8(0x7d, 0x40);
			ArduCAM_wrSensorReg8_8(0x7d, 0x40);
			break;
		case Reddish:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x18);
			ArduCAM_wrSensorReg8_8(0x7c, 0x05);
			ArduCAM_wrSensorReg8_8(0x7d, 0x40);
			ArduCAM_wrSensorReg8_8(0x7d, 0xc0);
			break;
		case BW:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x18);
			ArduCAM_wrSensorReg8_8(0x7c, 0x05);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			break;
		case Negative:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x40);
			ArduCAM_wrSensorReg8_8(0x7c, 0x05);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			break;
		case BWnegative:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x58);
			ArduCAM_wrSensorReg8_8(0x7c, 0x05);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			break;
		case Normal:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x05);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV3640_set_Special_effects(uint8_t Special_effect)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))	
	switch(Special_effect)
	{
		case Antique:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x18);
			ArduCAM_wrSensorReg16_8(0x335a, 0x40);
			ArduCAM_wrSensorReg16_8(0x335b, 0xa6);
			break;
		case Bluish:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x18);
			ArduCAM_wrSensorReg16_8(0x335a, 0xa0);
			ArduCAM_wrSensorReg16_8(0x335b, 0x40);
			break;
		case Greenish:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x18);
			ArduCAM_wrSensorReg16_8(0x335a, 0x60);
			ArduCAM_wrSensorReg16_8(0x335b, 0x60);
			break;
		case Reddish:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x18);
			ArduCAM_wrSensorReg16_8(0x335a, 0x80);
			ArduCAM_wrSensorReg16_8(0x335b, 0xc0);
			break;
		case Yellowish:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x18);
			ArduCAM_wrSensorReg16_8(0x335a, 0x30);
			ArduCAM_wrSensorReg16_8(0x335b, 0x90);
			break;
		case BW:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x18);//bit[4]fix u enable, bit[3]fix v enable
			ArduCAM_wrSensorReg16_8(0x335a, 0x80);
			ArduCAM_wrSensorReg16_8(0x335b, 0x80);
			break;
		case Negative:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x40);//bit[6] negative
			break;
		case BWnegative:
			ArduCAM_wrSensorReg8_8(0xff, 0x00);
			ArduCAM_wrSensorReg8_8(0x7c, 0x00);
			ArduCAM_wrSensorReg8_8(0x7d, 0x58);
			ArduCAM_wrSensorReg8_8(0x7c, 0x05);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			ArduCAM_wrSensorReg8_8(0x7d, 0x80);
			break;
		case Normal:
			ArduCAM_wrSensorReg16_8(0x3302, 0xef);
			ArduCAM_wrSensorReg16_8(0x3355, 0x00);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV5642_set_Special_effects(uint8_t Special_effect)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch(Special_effect)
	{
		case Bluish:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0xa0);
			ArduCAM_wrSensorReg16_8(0x5586 ,0x40);
			break;
		case Greenish:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0x60);
			ArduCAM_wrSensorReg16_8(0x5586 ,0x60);
			break;
		case Reddish:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5586 ,0xc0);
			break;
		case BW:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0x80);
			ArduCAM_wrSensorReg16_8(0x5586 ,0x80);
			break;
		case Negative:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x40);
			break;
		case Sepia:
			ArduCAM_wrSensorReg16_8(0x5001 ,0xff);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x18);
			ArduCAM_wrSensorReg16_8(0x5585 ,0x40);
			ArduCAM_wrSensorReg16_8(0x5586 ,0xa0);
			break;
		case Normal:
			ArduCAM_wrSensorReg16_8(0x5001 ,0x7f);
			ArduCAM_wrSensorReg16_8(0x5580 ,0x00);		
			break;		
		default:
			break;
	}
#endif
}

void ArduCAM_OV5640_set_Special_effects(uint8_t Special_effect)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch(Special_effect)
	{
		case Normal:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5580, 0x06);
			ArduCAM_wrSensorReg16_8(0x5583, 0x40); // sat U
			ArduCAM_wrSensorReg16_8(0x5584, 0x10); // sat V
			ArduCAM_wrSensorReg16_8(0x5003, 0x08);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 
			break;
		case Blueish:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5580, 0x1e);
			ArduCAM_wrSensorReg16_8(0x5583, 0xa0);
			ArduCAM_wrSensorReg16_8(0x5584, 0x40);
			ArduCAM_wrSensorReg16_8(0x5003, 0x08);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Reddish:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5580, 0x1e);
			ArduCAM_wrSensorReg16_8(0x5583, 0x80);
			ArduCAM_wrSensorReg16_8(0x5584, 0xc0);
			ArduCAM_wrSensorReg16_8(0x5003, 0x08);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case BW:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5580, 0x1e);
			ArduCAM_wrSensorReg16_8(0x5583, 0x80);
			ArduCAM_wrSensorReg16_8(0x5584, 0x80);
			ArduCAM_wrSensorReg16_8(0x5003, 0x08);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Sepia:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5580, 0x1e);
			ArduCAM_wrSensorReg16_8(0x5583, 0x40);
			ArduCAM_wrSensorReg16_8(0x5584, 0xa0);
			ArduCAM_wrSensorReg16_8(0x5003, 0x08);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Negative:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5580, 0x40);
			ArduCAM_wrSensorReg16_8(0x5003, 0x08);
			ArduCAM_wrSensorReg16_8(0x5583, 0x40); // sat U
			ArduCAM_wrSensorReg16_8(0x5584, 0x10); // sat V
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3
			break;
		case Greenish:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5580, 0x1e);
			ArduCAM_wrSensorReg16_8(0x5583, 0x60);
			ArduCAM_wrSensorReg16_8(0x5584, 0x60);
			ArduCAM_wrSensorReg16_8(0x5003, 0x08);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3		
			break;		
		case Overexposure:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5580, 0x1e);
			ArduCAM_wrSensorReg16_8(0x5583, 0xf0);
			ArduCAM_wrSensorReg16_8(0x5584, 0xf0);
			ArduCAM_wrSensorReg16_8(0x5003, 0x08);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3	
			break;	
		case Solarize:
			ArduCAM_wrSensorReg16_8(0x3212, 0x03); // start group 3
			ArduCAM_wrSensorReg16_8(0x5580, 0x06);
			ArduCAM_wrSensorReg16_8(0x5583, 0x40); // sat U
			ArduCAM_wrSensorReg16_8(0x5584, 0x10); // sat V
			ArduCAM_wrSensorReg16_8(0x5003, 0x09);
			ArduCAM_wrSensorReg16_8(0x3212, 0x13); // end group 3
			ArduCAM_wrSensorReg16_8(0x3212, 0xa3); // launch group 3	
			break;
		default:
			break;
	}
#endif	
}

void ArduCAM_OV3640_set_Exposure_level(uint8_t level)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))	
	switch(level)
	{
		case Exposure_17_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x10);
			ArduCAM_wrSensorReg16_8(0x3019, 0x08);
			ArduCAM_wrSensorReg16_8(0x301a, 0x21);
			break;
		case Exposure_13_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x18);
			ArduCAM_wrSensorReg16_8(0x3019, 0x10);
			ArduCAM_wrSensorReg16_8(0x301a, 0x31);
			break;
		case Exposure_10_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x20);
			ArduCAM_wrSensorReg16_8(0x3019, 0x18);
			ArduCAM_wrSensorReg16_8(0x301a, 0x41);
			break;
		case Exposure_07_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x28);
			ArduCAM_wrSensorReg16_8(0x3019, 0x20);
			ArduCAM_wrSensorReg16_8(0x301a, 0x51);
			break;
		case Exposure_03_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x30);
			ArduCAM_wrSensorReg16_8(0x3019, 0x28);
			ArduCAM_wrSensorReg16_8(0x301a, 0x61);
			break;
		case Exposure_default:
			ArduCAM_wrSensorReg16_8(0x3018, 0x38);
			ArduCAM_wrSensorReg16_8(0x3019, 0x30);
			ArduCAM_wrSensorReg16_8(0x301a, 0x61);
			break;
		case Exposure03_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x40);
			ArduCAM_wrSensorReg16_8(0x3019, 0x38);
			ArduCAM_wrSensorReg16_8(0x301a, 0x71);
			break;
		case Exposure07_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x48);
			ArduCAM_wrSensorReg16_8(0x3019, 0x40);
			ArduCAM_wrSensorReg16_8(0x301a, 0x81);
			break;
		case Exposure10_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x50);
			ArduCAM_wrSensorReg16_8(0x3019, 0x48);
			ArduCAM_wrSensorReg16_8(0x301a, 0x91);
			break;
		case Exposure13_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x58);
			ArduCAM_wrSensorReg16_8(0x3019, 0x50);
			ArduCAM_wrSensorReg16_8(0x301a, 0x91);
			break;
		case Exposure17_EV:
			ArduCAM_wrSensorReg16_8(0x3018, 0x60);
			ArduCAM_wrSensorReg16_8(0x3019, 0x58);
			ArduCAM_wrSensorReg16_8(0x301a, 0xa1);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV5642_set_Exposure_level(uint8_t level)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch(level)
	{
		case Exposure_17_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x10);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x08);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x10);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x08);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
			break;
		case Exposure_13_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x18);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x10);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x18);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x10);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
			break;
		case Exposure_10_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x18);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x41);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x18);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
			break;
		case Exposure_07_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x28);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x51);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x28);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x20);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
			break;
		case Exposure_03_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x28);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x61);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x28);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
			break;
		case Exposure_default:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x38);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x61);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x38);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x30);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x40);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x38);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x71);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x40);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x38);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x10);
			break;
		case Exposure07_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x48);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x40);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x80);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x48);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x40);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x20);
			break;
		case Exposure10_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x50);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x48);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x90);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x50);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x48);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x20);
			break;
		case Exposure13_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x58);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x50);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0x91);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x58);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x50);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x20);
			break;
		case Exposure17_EV:
			ArduCAM_wrSensorReg16_8(0x3a0f ,0x60);
			ArduCAM_wrSensorReg16_8(0x3a10 ,0x58);
			ArduCAM_wrSensorReg16_8(0x3a11 ,0xa0);
			ArduCAM_wrSensorReg16_8(0x3a1b ,0x60);
			ArduCAM_wrSensorReg16_8(0x3a1e ,0x58);
			ArduCAM_wrSensorReg16_8(0x3a1f ,0x20);
			break;
		default:
			break;
	}
#endif	
}

void ArduCAM_OV3640_set_Sharpness(uint8_t Sharpness)
{	
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))	
	switch(Sharpness)
	{
		case Sharpness1:
			ArduCAM_wrSensorReg16_8(0x332d, 0x41);
			break;
		case Sharpness2:
			ArduCAM_wrSensorReg16_8(0x332d, 0x42);
			break;
		case Sharpness3:
			ArduCAM_wrSensorReg16_8(0x332d, 0x43);
			break;
		case Sharpness4:
			ArduCAM_wrSensorReg16_8(0x332d, 0x44);
			break;
		case Sharpness5:
			ArduCAM_wrSensorReg16_8(0x332d, 0x45);
			break;
		case Sharpness6:
			ArduCAM_wrSensorReg16_8(0x332d, 0x46);
			break;
		case Sharpness7:
			ArduCAM_wrSensorReg16_8(0x332d, 0x47);
			break;
		case Sharpness8:
			ArduCAM_wrSensorReg16_8(0x332d, 0x48);
			break;
		case Sharpness_auto:
			ArduCAM_wrSensorReg16_8(0x332d, 0x60);
			ArduCAM_wrSensorReg16_8(0x332f, 0x03);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV3640_set_Mirror_Flip(uint8_t Mirror_Flip)
{
#if (defined(OV3640_CAM) || defined(OV3640_MINI_3MP))	
	switch(Mirror_Flip)
	{
		case MIRROR:
			ArduCAM_wrSensorReg16_8(0x307c, 0x12);//mirror
			ArduCAM_wrSensorReg16_8(0x3090, 0xc8);
			ArduCAM_wrSensorReg16_8(0x3023, 0x0a);
			break;
		case FLIP:
			ArduCAM_wrSensorReg16_8(0x307c, 0x11);//flip
			ArduCAM_wrSensorReg16_8(0x3023, 0x09);
			ArduCAM_wrSensorReg16_8(0x3090, 0xc0);
			break;
		case MIRROR_FLIP:
			ArduCAM_wrSensorReg16_8(0x307c, 0x13);//flip/mirror
			ArduCAM_wrSensorReg16_8(0x3023, 0x09);
			ArduCAM_wrSensorReg16_8(0x3090, 0xc8);
			break;
		case Normal:
			ArduCAM_wrSensorReg16_8(0x307c, 0x10);//no mirror/flip
			ArduCAM_wrSensorReg16_8(0x3090, 0xc0);
			ArduCAM_wrSensorReg16_8(0x3023, 0x0a);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV5642_set_Sharpness(uint8_t Sharpness)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch(Sharpness)
	{
		case Auto_Sharpness_default:
			ArduCAM_wrSensorReg16_8(0x530A ,0x00);
			ArduCAM_wrSensorReg16_8(0x530c ,0x0 );
			ArduCAM_wrSensorReg16_8(0x530d ,0xc );
			ArduCAM_wrSensorReg16_8(0x5312 ,0x40);
			break;
		case Auto_Sharpness1:
			ArduCAM_wrSensorReg16_8(0x530A ,0x00);
			ArduCAM_wrSensorReg16_8(0x530c ,0x4 );
			ArduCAM_wrSensorReg16_8(0x530d ,0x18);
			ArduCAM_wrSensorReg16_8(0x5312 ,0x20);
			break;
		case Auto_Sharpness2:
			ArduCAM_wrSensorReg16_8(0x530A ,0x00);
			ArduCAM_wrSensorReg16_8(0x530c ,0x8 );
			ArduCAM_wrSensorReg16_8(0x530d ,0x30);
			ArduCAM_wrSensorReg16_8(0x5312 ,0x10);
			break;
		case Manual_Sharpnessoff:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x00);
			ArduCAM_wrSensorReg16_8(0x531f ,0x00);
			break;
		case Manual_Sharpness1:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x04);
			ArduCAM_wrSensorReg16_8(0x531f ,0x04);
			break;
		case Manual_Sharpness2:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x08);
			ArduCAM_wrSensorReg16_8(0x531f ,0x08);
			break;
		case Manual_Sharpness3:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x0c);
			ArduCAM_wrSensorReg16_8(0x531f ,0x0c);
			break;
		case Manual_Sharpness4:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x0f);
			ArduCAM_wrSensorReg16_8(0x531f ,0x0f);
			break;
		case Manual_Sharpness5:
			ArduCAM_wrSensorReg16_8(0x530A ,0x08);
			ArduCAM_wrSensorReg16_8(0x531e ,0x1f);
			ArduCAM_wrSensorReg16_8(0x531f ,0x1f);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV5642_set_Mirror_Flip(uint8_t Mirror_Flip)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	uint8_t reg_val;
	switch(Mirror_Flip)
	{
		case MIRROR:
			ArduCAM_rdSensorReg16_8(0x3818,&reg_val);
			reg_val = (uint8_t)(reg_val|0x00);
			reg_val = (uint8_t)(reg_val&0x9F);
			ArduCAM_wrSensorReg16_8(0x3818 ,reg_val);
			ArduCAM_rdSensorReg16_8(0x3621,&reg_val);
			reg_val = (uint8_t)(reg_val|0x20);
			ArduCAM_wrSensorReg16_8(0x3621, reg_val);
			break;
		case FLIP:
			ArduCAM_rdSensorReg16_8(0x3818,&reg_val);
			reg_val = (uint8_t)(reg_val|0x20);
			reg_val = (uint8_t)(reg_val&0xbF);
			ArduCAM_wrSensorReg16_8(0x3818 ,reg_val);
			ArduCAM_rdSensorReg16_8(0x3621,&reg_val);
			reg_val = (uint8_t)(reg_val|0x20);
			ArduCAM_wrSensorReg16_8(0x3621, reg_val);
			break;
		case MIRROR_FLIP:
			ArduCAM_rdSensorReg16_8(0x3818,&reg_val);
			reg_val = (uint8_t)(reg_val|0x60);
			reg_val = (uint8_t)(reg_val&0xFF);
			ArduCAM_wrSensorReg16_8(0x3818 ,reg_val);
			ArduCAM_rdSensorReg16_8(0x3621,&reg_val);
			reg_val = (uint8_t)(reg_val&0xdf);
			ArduCAM_wrSensorReg16_8(0x3621, reg_val);
			break;
		case Normal:
			ArduCAM_rdSensorReg16_8(0x3818,&reg_val);
			reg_val = (uint8_t)(reg_val|0x40);
			reg_val = (uint8_t)(reg_val&0xdF);
			ArduCAM_wrSensorReg16_8(0x3818 ,reg_val);
			ArduCAM_rdSensorReg16_8(0x3621,&reg_val);
			reg_val = (uint8_t)(reg_val&0xdf);
			ArduCAM_wrSensorReg16_8(0x3621, reg_val);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV5642_set_Compress_quality(uint8_t quality)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch(quality)
	{
		case high_quality:
			ArduCAM_wrSensorReg16_8(0x4407, 0x02);
			break;
		case default_quality:
			ArduCAM_wrSensorReg16_8(0x4407, 0x04);
			break;
		case low_quality:
			ArduCAM_wrSensorReg16_8(0x4407, 0x08);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV5642_Test_Pattern(uint8_t Pattern)
{
#if (defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_PLUS))
	switch(Pattern)
	{
		case Color_bar:
			ArduCAM_wrSensorReg16_8(0x503d , 0x80);
			ArduCAM_wrSensorReg16_8(0x503e, 0x00);
			break;
		case Color_square:
			ArduCAM_wrSensorReg16_8(0x503d , 0x85);
			ArduCAM_wrSensorReg16_8(0x503e, 0x12);
			break;
		case BW_square:
			ArduCAM_wrSensorReg16_8(0x503d , 0x85);
			ArduCAM_wrSensorReg16_8(0x503e, 0x1a);
			break;
		case DLI:
			ArduCAM_wrSensorReg16_8(0x4741 , 0x4);
			break;
		default:
			break;
	}
#endif
}

void ArduCAM_OV5640_set_Night_Mode(uint8_t Night_mode)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	uint8_t reg_val;
	switch(Night_mode)
	{
		case Night_Mode_On:
			ArduCAM_rdSensorReg16_8(0x3a00,&reg_val);
			reg_val = reg_val| 0x04;
			ArduCAM_wrSensorReg16_8(0x3a00, reg_val);
			break;
		case Night_Mode_Off:
			ArduCAM_rdSensorReg16_8(0x3a00,&reg_val);
			reg_val = reg_val& 0xfb;
			ArduCAM_wrSensorReg16_8(0x3a00, reg_val);
			break;
	}
#endif

}

void ArduCAM_OV5640_set_Banding_Filter(uint8_t Banding_Filter)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	uint8_t reg_val;
	switch(Banding_Filter)
	{
		case Off:
			ArduCAM_rdSensorReg16_8(0x3a00,&reg_val);
			reg_val = reg_val & 0xdf; // turn off banding filter
			ArduCAM_wrSensorReg16_8(0x3a00, reg_val);
			break;
		case Manual_50HZ:
			ArduCAM_wrSensorReg16_8(0x3c00, 04); // set to 50Hz
			ArduCAM_wrSensorReg16_8(0x3c01, 80); // manual banding filter
			ArduCAM_rdSensorReg16_8(0x3a00,&reg_val);
			reg_val = reg_val | 0x20; // turn on banding filter
			ArduCAM_wrSensorReg16_8(0x3a00, reg_val);
			break;
		case Manual_60HZ:
			ArduCAM_wrSensorReg16_8(0x3c00, 00); // set to 60Hz
			ArduCAM_wrSensorReg16_8(0x3c01, 80); // manual banding filter
			ArduCAM_rdSensorReg16_8(0x3a00, &reg_val);
			reg_val = reg_val | 0x20; // turn on banding filter
			ArduCAM_wrSensorReg16_8(0x3a00, reg_val);
			break;
		case Auto_Detection:
			ArduCAM_wrSensorReg16_8(0x3c01, 00); // auto banding filter
			ArduCAM_rdSensorReg16_8(0x3a00, &reg_val);
			reg_val = reg_val & 0xdf; // turn off banding filter
			ArduCAM_wrSensorReg16_8(0x3a00, reg_val);
			break;
	}
#endif
}

void ArduCAM_OV5640_set_EV(uint8_t EV)
{
#if (defined(OV5640_CAM) || defined(OV5640_MINI_5MP_PLUS))
	switch(EV)
	{
		case EV3:
			ArduCAM_wrSensorReg16_8(0x3a0f, 0x60);
			ArduCAM_wrSensorReg16_8(0x3a10, 0x58);
			ArduCAM_wrSensorReg16_8(0x3a11, 0xa0);
			ArduCAM_wrSensorReg16_8(0x3a1b, 0x60);
			ArduCAM_wrSensorReg16_8(0x3a1e, 0x58);
			ArduCAM_wrSensorReg16_8(0x3a1f, 0x20);
			break;
		case EV2:
			ArduCAM_wrSensorReg16_8(0x3a0f, 0x50);
			ArduCAM_wrSensorReg16_8(0x3a10, 0x48);
			ArduCAM_wrSensorReg16_8(0x3a11, 0x90);
			ArduCAM_wrSensorReg16_8(0x3a1b, 0x50);
			ArduCAM_wrSensorReg16_8(0x3a1e, 0x48);
			ArduCAM_wrSensorReg16_8(0x3a1f, 0x20);
			break;
		case EV1:
			ArduCAM_wrSensorReg16_8(0x3a0f, 0x40);
			ArduCAM_wrSensorReg16_8(0x3a10, 0x38);
			ArduCAM_wrSensorReg16_8(0x3a11, 0x71);
			ArduCAM_wrSensorReg16_8(0x3a1b, 0x40);
			ArduCAM_wrSensorReg16_8(0x3a1e, 0x38);
			ArduCAM_wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case EV0:
			ArduCAM_wrSensorReg16_8(0x3a0f, 0x38);
			ArduCAM_wrSensorReg16_8(0x3a10, 0x30);
			ArduCAM_wrSensorReg16_8(0x3a11, 0x61);
			ArduCAM_wrSensorReg16_8(0x3a1b, 0x38);
			ArduCAM_wrSensorReg16_8(0x3a1e, 0x30);
			ArduCAM_wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case EV_1:
			ArduCAM_wrSensorReg16_8(0x3a0f, 0x30);
			ArduCAM_wrSensorReg16_8(0x3a10, 0x28);
			ArduCAM_wrSensorReg16_8(0x3a11, 0x61);
			ArduCAM_wrSensorReg16_8(0x3a1b, 0x30);
			ArduCAM_wrSensorReg16_8(0x3a1e, 0x28);
			ArduCAM_wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case EV_2:
			ArduCAM_wrSensorReg16_8(0x3a0f, 0x20);
			ArduCAM_wrSensorReg16_8(0x3a10, 0x18);
			ArduCAM_wrSensorReg16_8(0x3a11, 0x41);
			ArduCAM_wrSensorReg16_8(0x3a1b, 0x20);
			ArduCAM_wrSensorReg16_8(0x3a1e, 0x18);
			ArduCAM_wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case EV_3:
			ArduCAM_wrSensorReg16_8(0x3a0f, 0x10);
			ArduCAM_wrSensorReg16_8(0x3a10, 0x08);
			ArduCAM_wrSensorReg16_8(0x3a1b, 0x10);
			ArduCAM_wrSensorReg16_8(0x3a1e, 0x08);
			ArduCAM_wrSensorReg16_8(0x3a11, 0x20);
			ArduCAM_wrSensorReg16_8(0x3a1f, 0x10);
			break;	
	}
#endif
}

// Write 8 bit values to 8 bit register address
int ArduCAM_wrSensorRegs8_8(const struct sensor_reg reglist[])
{
	int err = 0;
	uint8_t reg_addr = 0;
	uint8_t reg_val = 0;
	const struct sensor_reg *next = reglist;
	while ((reg_addr != 0xff) | (reg_val != 0xff))
	{
		reg_addr = (uint8_t)next->reg;
		reg_val = (uint8_t)next->val;
		err = ArduCAM_wrSensorReg8_8(reg_addr, reg_val);
		next++;
	}
	return 1;
}

// Write 16 bit values to 8 bit register address
int ArduCAM_wrSensorRegs8_16(const struct sensor_reg reglist[])
{
	int err = 0;
	uint8_t reg_addr = 0;
	uint16_t reg_val = 0;
	const struct sensor_reg *next = reglist;

	while ((reg_addr != 0xff) | (reg_val != 0xffff))
	{
		reg_addr = (uint8_t)next->reg;
		reg_val = next->val;
		err = ArduCAM_wrSensorReg8_16(reg_addr, reg_val);
		//if (!err)
		//return err;
		next++;
	}
	return 1;
}

// Write 8 bit values to 16 bit register address
int ArduCAM_wrSensorRegs16_8(const struct sensor_reg reglist[])
{
	int err = 0;
	uint16_t reg_addr = 0;
	uint8_t reg_val = 0;
	const struct sensor_reg *next = reglist;

	while ((reg_addr != 0xffff) | (reg_val != 0xff))
	{
		reg_addr = next->reg;
		reg_val = (uint8_t)next->val;
		err = ArduCAM_wrSensorReg16_8(reg_addr, reg_val);
		//if (!err)
		//return err;
		next++;
	}
	return 1;
}

//I2C Array Write 16bit address, 16bit data
int ArduCAM_wrSensorRegs16_16(const struct sensor_reg reglist[])
{
	int err = 0;
	uint16_t reg_addr = 0;
	uint16_t reg_val = 0;
	const struct sensor_reg *next = reglist;
	reg_addr = next->reg;
	reg_val = next->val;
	while ((reg_addr != 0xffff) | (reg_val != 0xffff))
	{
		err = ArduCAM_wrSensorReg16_16(reg_addr, reg_val);
		//if (!err)
		// return err;
		next++;
		reg_addr = next->reg;
		reg_val = next->val;
	}
	return 1;
}

// Read/write 8 bit value to/from 8 bit register address	
byte ArduCAM_wrSensorReg8_8(uint8_t regID, uint8_t regDat)
{
	Wire.beginTransmission((uint8_t)(sensor_addr >> 1));
	Wire.write_byte((uint8_t)(regID & 0xFF));
	Wire.write_byte((uint8_t)(regDat & 0xFF));
	if (Wire.endTransmission(true))
	{
		return 0;
	}
	delay(1);
	return 1;
}

byte ArduCAM_rdSensorReg8_8(uint8_t regID, uint8_t* regDat)
{	
	Wire.beginTransmission((uint8_t)(sensor_addr >> 1));
	Wire.write_byte((uint8_t)(regID & 0xFF));
	Wire.endTransmission(true);

	Wire.requestFrom((uint8_t)(sensor_addr >> 1), 1, true);
	if (Wire.available()) *regDat = (uint8_t)Wire.read();
	delay(1);
	return 1;
}

// Read/write 16 bit value to/from 8 bit register address
byte ArduCAM_wrSensorReg8_16(uint8_t regID, int regDat)
{
	Wire.beginTransmission((uint8_t)(sensor_addr >> 1));
	Wire.write_byte((uint8_t)(regID & 0xFF));

	Wire.write_byte((uint8_t)(regDat >> 8));			// sends data byte, MSB first
	Wire.write_byte((uint8_t)(regDat & 0x00FF));
	if (Wire.endTransmission(true)) return 0;
	delay(1);
	return 1;
}

byte ArduCAM_rdSensorReg8_16(uint8_t regID, int* regDat)
{
	uint8_t temp;
	Wire.beginTransmission((uint8_t)(sensor_addr >> 1));
	Wire.write_byte(regID);
	Wire.endTransmission(true);

	Wire.requestFrom((uint8_t)(sensor_addr >> 1), 2, true);
	if (Wire.available())
	{
		temp = (uint8_t)Wire.read();
		*regDat = (temp << 8) | Wire.read();
	}
	delay(1);
	return 1;
}

// Read/write 8 bit value to/from 16 bit register address
byte ArduCAM_wrSensorReg16_8(uint16_t regID, int regDat)
{
	Wire.beginTransmission((uint8_t)(sensor_addr >> 1));
	Wire.write_byte((uint8_t)(regID >> 8));			// sends instruction byte, MSB first
	Wire.write_byte((uint8_t)(regID & 0x00FF));
	Wire.write_byte((uint8_t)(regDat & 0x00FF));
	if (Wire.endTransmission(true)) return 0;
	delay(1);
	return 1;
}

byte ArduCAM_rdSensorReg16_8(uint16_t regID, uint8_t* regDat)
{
	Wire.beginTransmission((uint8_t)(sensor_addr >> 1));
	Wire.write_byte((uint8_t)(regID >> 8));
	Wire.write_byte((uint8_t)(regID & 0x00FF));
	Wire.endTransmission(true);
	Wire.requestFrom((uint8_t)(sensor_addr >> 1), 1, true);
	if (Wire.available()) *regDat = (uint8_t)Wire.read();
	delay(1);
	return 1;
}

//I2C Write 16bit address, 16bit data
byte ArduCAM_wrSensorReg16_16(uint16_t regID, int regDat)
{
	Wire.beginTransmission((uint8_t)(sensor_addr >> 1));
	Wire.write_byte((uint8_t)(regID >> 8));			// sends instruction byte, MSB first
	Wire.write_byte((uint8_t)(regID & 0x00FF));
	Wire.write_byte((uint8_t)(regDat >> 8));			// sends data byte, MSB first
	Wire.write_byte((uint8_t)(regDat & 0x00FF));
	if (Wire.endTransmission(true)) return 0;
	delay(1);
	return (1);
}

//I2C Read 16bit address, 16bit data
byte ArduCAM_rdSensorReg16_16(uint16_t regID, uint16_t* regDat)
{
	uint16_t temp;
	Wire.beginTransmission((uint8_t)(sensor_addr >> 1));
	Wire.write_byte((uint8_t)(regID >> 8));
	Wire.write_byte((uint8_t)(regID & 0x00FF));
	Wire.endTransmission(true);
	Wire.requestFrom((uint8_t)(sensor_addr >> 1), 2, true);
	if (Wire.available())
	{
		temp = Wire.read();
		*regDat = (temp << 8) | Wire.read();
	}
	delay(1);
	return (1);
}

const t_ArduCAM ArduCAM = {
	ArduCAM_init,
	ArduCAM_InitCAM,
	ArduCAM_flush_fifo,
	ArduCAM_start_capture,
	ArduCAM_clear_fifo_flag,
	ArduCAM_read_fifo_length,
	ArduCAM_set_fifo_burst,
	ArduCAM_CS_HIGH,
	ArduCAM_CS_LOW,
	ArduCAM_read_fifo,
	ArduCAM_read_reg,
	ArduCAM_write_reg,
	ArduCAM_set_bit,
	ArduCAM_clear_bit,
	ArduCAM_get_bit,
	ArduCAM_set_mode,
	ArduCAM_bus_write,
	ArduCAM_bus_read,
	ArduCAM_OV3640_set_JPEG_size,
	ArduCAM_OV2640_set_JPEG_size,
	ArduCAM_OV5642_set_RAW_size,
	ArduCAM_OV5642_set_JPEG_size,
	ArduCAM_OV5640_set_JPEG_size,
	ArduCAM_set_format,
	ArduCAM_OV2640_set_Light_Mode,
	ArduCAM_OV3640_set_Light_Mode,
	ArduCAM_OV5642_set_Light_Mode,
	ArduCAM_OV5640_set_Light_Mode,
	ArduCAM_OV2640_set_Color_Saturation,
	ArduCAM_OV3640_set_Color_Saturation,
	ArduCAM_OV5640_set_Color_Saturation,
	ArduCAM_OV5642_set_Color_Saturation,
	ArduCAM_OV2640_set_Brightness,
	ArduCAM_OV3640_set_Brightness,
	ArduCAM_OV5642_set_Brightness,
	ArduCAM_OV5640_set_Brightness,
	ArduCAM_OV2640_set_Contrast,
	ArduCAM_OV3640_set_Contrast,
	ArduCAM_OV5642_set_Contrast,
	ArduCAM_OV5640_set_Contrast,
	ArduCAM_OV5642_set_hue,
	ArduCAM_OV2640_set_Special_effects,
	ArduCAM_OV3640_set_Special_effects,
	ArduCAM_OV5642_set_Special_effects,
	ArduCAM_OV5640_set_Special_effects,
	ArduCAM_OV3640_set_Exposure_level,
	ArduCAM_OV5642_set_Exposure_level,
	ArduCAM_OV3640_set_Sharpness,
	ArduCAM_OV3640_set_Mirror_Flip,
	ArduCAM_OV5642_set_Sharpness,
	ArduCAM_OV5642_set_Mirror_Flip,
	ArduCAM_OV5642_set_Compress_quality,
	ArduCAM_OV5642_Test_Pattern,
	ArduCAM_OV5640_set_Night_Mode,
	ArduCAM_OV5640_set_Banding_Filter,
	ArduCAM_OV5640_set_EV,
	ArduCAM_wrSensorRegs8_8,
	ArduCAM_wrSensorRegs8_16,
	ArduCAM_wrSensorRegs16_8,
	ArduCAM_wrSensorRegs16_16,
	ArduCAM_wrSensorReg8_8,
	ArduCAM_rdSensorReg8_8,
	ArduCAM_wrSensorReg8_16,
	ArduCAM_rdSensorReg8_16,
	ArduCAM_wrSensorReg16_8,
	ArduCAM_rdSensorReg16_8,
	ArduCAM_wrSensorReg16_16,
	ArduCAM_rdSensorReg16_16
};