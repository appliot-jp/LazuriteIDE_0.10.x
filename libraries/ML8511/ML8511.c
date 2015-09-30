/* FILE NAME: ml8511.c
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

#include "ML8511.h"

static uint8_t analog_pin;
static void ml8511_init(int ana_pin)
{
	analog_pin  = (uint8_t)ana_pin;
}



// Calculations for UV Sensor - ML8511
// Known Point = 2.2V @ 10mW/cm2
// Rate = 0.129
static void ml8511_get_rawval(unsigned short *data)
{
  *data = analogRead(analog_pin);
}


// Math:  ADC_Voltage = (sensorValue / 670) * 3.3V              //Convert to V
//        ADC_Voltage = sensorValue * (3.3V/670)
//        ADC_Voltage = sensorValue * 0.004925
//        UV Intensity = (ADC_Voltage - 2.2)/(0.129) + 10   //Conver to UV in DegC
static float ml8511_convert_uv(unsigned short data)
{
  float uv;

  uv = (float)data * 0.004925f;
  uv = (uv - 2.2f) / (0.129f) + 10;
  
  return (uv);
}
// Notes on Arduino ADC
// Arduino Uno uses an 5V ADC Reference Voltage; thus, we will need to scale this 
// to 3.3V Levels to safely operate the sensors
// 5V = 1024, 3.3V = 670   
static void ml8511_get_val(float *data)
{
  unsigned short rawdata;
  
  ml8511_get_rawval(&rawdata);
  *data = ml8511_convert_uv(rawdata);
}

const t_ML8511 ml8511 =
{
	ml8511_init,
	ml8511_get_val,
};

