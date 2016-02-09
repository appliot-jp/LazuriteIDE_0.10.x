/* FILE NAME: BD1020.h
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

#include "BD1020.h"
#include "lazurite.h"

static unsigned char bd1020_pin;
static double bd1020_vdd;

void bd1020_init(unsigned char ana_pin,double vdd)
{
	bd1020_pin  = ana_pin;
	bd1020_vdd = vdd;
}

unsigned short bd1020_get_rawval()
{
  return analogRead(bd1020_pin);
}

double bd1020_convert_degree(unsigned short data)
{
  double tmp, temp;
	
	tmp = (double)data * bd1020_vdd / 1024;
	temp = -(tmp * 1000  - 1546) / 8.2;
  return (temp);
}

double bd1020_get_val()
{
  return bd1020_convert_degree(bd1020_get_rawval());
}

// Calculations for Temp Sensor - BD1020
// Math:  ADC_Voltage = sensorValue * (5V/1024)
//        Tempurature = - (1000 * ADC_Voltage - 1546) / 8.2


const t_BD1020 bd1020 = {
	bd1020_init,
	bd1020_get_val
};
