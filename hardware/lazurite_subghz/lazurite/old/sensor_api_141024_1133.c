/* FILE NAME: sensor_api_141024_1133.c
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
#include "driver_api.h"
#include "lazurite.h"
#include "spi.h"
#include "rtc.h"
//#include "led_ctrl.h"
//#include "led_ctrl_def.h"

#ifdef    _SENSOR_API_H_
#include "sensor_api.h"
#include "spi.h"
#endif // _SENSOR_API_H_


//********************************************************************************
//   local definition
//********************************************************************************
// command between host
#define	CMD_GET_DEVICE			0x10
#define	CMD_GET_NUM_SENSOR		0x11
#define	CMD_GET_SENSOR_LIST		0x12
#define	CMD_SET_TIME			0x13
#define	CMD_GET_TIME			0x14
#define	CMD_SET_ENA_DIS			0x15
#define	CMD_GET_ENA_DIS			0x16
#define	CMD_SET_MODE			0x17
#define	CMD_GET_MODE			0x18
#define	CMD_ERROR				0x00

//  length of receive
#define	LEN_RX_GET_DEVICE		2
#define	LEN_RX_GET_NUM_SENSOR	2
#define	LEN_RX_GET_SENSOR_LIST	3
#define	LEN_RX_SET_TIME			9
#define	LEN_RX_GET_TIME			2
#define	LEN_RX_SET_ENA_DIS		2		// actual data length = LEN_RX_SET_ENA_DIS + sensor_amount
#define	LEN_RX_GET_ENA_DIS		2
#define	LEN_RX_SET_MODE			3
#define	LEN_RX_GET_MODE			2

// length of transfer (return)
#define	LEN_TX_GET_DEVICE		19
#define	LEN_TX_GET_NUM_SENSOR	2
#define	LEN_TX_GET_SENSOR_LIST	20
#define	LEN_TX_GET_TIME			8
#define	LEN_TX_GET_ENA_DIS		2
#define	LEN_TX_GET_MODE			2
#define	LEN_TX_ERROR			18
//********************************************************************************
//   local function
//********************************************************************************
static void (*_loopHs)(void);
void  WriteCommand(void);
void  NotifyCommand(void);
void  TxErrorStatus(UCHAR error_mode);
//********************************************************************************
//   local parameters
//********************************************************************************
const char BOARD_ID[] = "LAZULITE";
const char BOARD_VERH   = 	0x01;
const char BOARD_VERL   = 	0x00;

static UCHAR _sensor_data[4];
static UCHAR _sensor_data_len;


static UCHAR _sensor_mode = 0;
static BOOL _sensor_ena[16];
static CHAR _current_sensor_num;

typedef struct {
	char message[16];
} _error_message;

const _error_message ERROR_MESSAGE[] ={
	{"command"},			// 0
	{"length"},				// 1
	{"sensor num"},			// 2
	{"RTC error"},			// 3
	{"ena/dis data"},		// 4
	{"mode data"}			// 5
};
//*******************************************************************************/
//      void set_LoopHS_callback(unsigned short loophs_interval,void (*callback_func)(void));
//      function: timer interrupt of ms order.
//                On this BLE application, main loop function is called per about 1 second.
//                Some of sensors require many of actual data from sensor.
//                The function can be used to get data from sensor for short interval.
//                After compless process, data can be sent per 1 sec.
//      parameters:
//           loophs_interval =  interval of function.
//                              interval = 1/32768 x n [s] = about 30us x n
//                              minimum value is 33 ( about 1ms ).
//           callback_func   =  call back function.
//********************************************************************************/
void _handler_LoopHS_callback(void)
{
	// HSCLK setting
	// HALT setting
	// execute call back setting
	_loopHs();
	
	
}
void set_LoopHS_callback(unsigned short loophs_interval,void (*callback_func)(void))
{
	_loopHs = callback_func;
	// BLKCON Enable
	// set interval
	// use timer 4-5
	// time base clock = 1LSCLK = 
//	irq_sethandler(IRQ_NO,_loopHs);
	return;
}

void init_LoopHS_callback(void)
{
//	_loopHs = _intNullHdr;
//	irq_sethandler(IRQ_NO,_loopHs);
	return;
}

void start_LoopHS_callback(void)
{
	return;
}

void stop_LoopHS_callback(void)
{
	return;
}

// start up function
void arduino_setup(void)
{
	char n;
	for(n=0;n<sensor_amount;n++)
	{
		_sensor_ena[n] = TRUE;
	}
	_current_sensor_num = sensor_amount ;
	setup();
	return;
}

// main routine (called when anything is requested from host)
void arduino_main(void)
{
	if(ble_info.event & EVENT_VSP_APP_READ_IND)		// read request
	{
		// TX data is prepared in Write command process.
	}
	
	if(ble_info.event & EVENT_VSP_APP_WRITE_IND)	// write command
	{
		WriteCommand();
	}
	if(ble_info.event & EVENT_VSP_APP_NOTFY_IND)	// Notification
	{
		NotifyCommand();
	}
	if(ble_info.event & EVENT_VSP_APP_INDY_IND)		// Indication
	{
		// Indication is not in use.
	}
	return;
}

// Write command process
void WriteCommand(void)
{
	tRtcTime t_time;
	int res;
	UCHAR n;
	BT_memset(&ble_info.tx_data[0],0,ble_info.tx_len);			// initialize output memory
	ble_info.tx_len = 20;										//
	switch(ble_info.rx_data[0])		// command
	{
	case 	CMD_GET_DEVICE:
		if(ble_info.rx_len != LEN_RX_GET_DEVICE) 
		{
			TxErrorStatus(1);						// Output Error Status
			break;
		}
		else
		{
			ble_info.tx_data[0] = ble_info.rx_data[1];					// Unique ID
			BT_memcpy(&ble_info.tx_data[1],BOARD_ID,sizeof(BOARD_ID));	// Boad Name
			ble_info.tx_data[17] = BOARD_VERH;							// Board Version H
			ble_info.tx_data[18] = BOARD_VERL;							// Board Version H
			break;
		}
	case 	CMD_GET_NUM_SENSOR:
		if(ble_info.rx_len != LEN_RX_GET_NUM_SENSOR) 
		{
			TxErrorStatus(1);						// Output Error Status
			break;
		}
		else
		{
			ble_info.tx_data[0] = ble_info.rx_data[1];					// Unique ID
			ble_info.tx_data[1] = sensor_amount;							// sensor_num
			break;
		}
		break;
	case 	CMD_GET_SENSOR_LIST:
		if(ble_info.rx_len != LEN_RX_GET_SENSOR_LIST) 
		{
			TxErrorStatus(1);						// Output Error Status
			break;
		}
		else if (ble_info.rx_data[2] >= sensor_amount )
		{
			TxErrorStatus(2);						// Output Error Status
			break;
		}
		else
		{
			ble_info.tx_data[0] = ble_info.rx_data[1];					// Unique ID
			ble_info.tx_data[1] = ble_info.rx_data[2];					// Sensor Num
			BT_memcpy(&ble_info.tx_data[2],&sensor_param[ble_info.rx_data[2]].sensor_name[0],len_sensor_name);
			break;
		}
		break;
	case 	CMD_SET_TIME:
		if(ble_info.rx_len != LEN_RX_SET_TIME)
		{
			TxErrorStatus(1);						// Output Error Status
			break;
		}
		else
		{
			BT_memcpy(&t_time,&ble_info.rx_data[2],7);
			res = rtc_setTime(&t_time);
			if(res == RTC_R_OK)
			{
				ble_info.tx_data[0] = ble_info.rx_data[1];	 	// output Unique ID
				ble_info.tx_len = 1;						// output Unique ID
			}
			else
			{
				TxErrorStatus(3);
			}
		}
		break;
	case 	CMD_GET_TIME:
		if(ble_info.rx_len != LEN_RX_GET_TIME)
		{
			TxErrorStatus(1);						// Output Error Status
			break;
		}
		else
		{
			res = rtc_getTime(&t_time);
			if(res == RTC_R_OK)
			{
				ble_info.tx_data[0] = ble_info.rx_data[1];					// Unique ID
				BT_memcpy(&ble_info.tx_data[1],&t_time,7);					// set RTC time to output buffer
			}
			else
			{
				TxErrorStatus(3);
			}
		}
		break;
	case 	CMD_SET_ENA_DIS:
		if(ble_info.rx_len != LEN_RX_SET_ENA_DIS + sensor_amount)								// check data length
		{
			TxErrorStatus(1);						// Output Error Status
			break;
		}
		else
		{
			res = TRUE;
			for(n=0; n < sensor_amount ;n++) 				// enable/disenable data check data
			{
				if ((ble_info.rx_data[n+2] != TRUE) &&
					(ble_info.rx_data[n+2] != FALSE))
				{
					res = FALSE;
					TxErrorStatus(4);						// Output ena/dis data error status
					break;
				}
			}
			if(res == TRUE)									// set enable/disenable
			{
				for(n=0; n < sensor_amount ;n++) // set data
				{
					_sensor_ena[n] = ble_info.rx_data[n+2];
				}
				ble_info.tx_data[0] = ble_info.rx_data[1];	 	// output Unique ID
				ble_info.tx_len = 1;						// output Unique ID
				_current_sensor_num = sensor_amount;
			}
		}
		break;
	case 	CMD_GET_ENA_DIS:
		if(ble_info.rx_len != LEN_RX_GET_ENA_DIS)			// check data length
		{
			TxErrorStatus(1);						// Output Error Status
			break;
		}
		else
		{
			ble_info.tx_data[0] = ble_info.rx_data[1];					// Unique ID
			BT_memcpy(&ble_info.tx_data[1],_sensor_ena,sensor_amount);
		}
		break;
	case 	CMD_SET_MODE:
		if(ble_info.rx_len != LEN_RX_SET_MODE)
		{
			TxErrorStatus(1);						// Output Error Status
			break;
		}
		else
		{
			_sensor_mode = ble_info.rx_data[2];
			ble_info.tx_data[0] = ble_info.rx_data[1]; 	// output Unique ID
			ble_info.tx_len = 1;						// output Unique ID
			
		}
		break;
	case 	CMD_GET_MODE:
		if(ble_info.rx_len != LEN_RX_GET_MODE)
		{
			TxErrorStatus(1);						// Output Error Status
			break;
		}
		else
		{
			ble_info.tx_data[0] = ble_info.rx_data[1];					// Unique ID
			ble_info.tx_data[1] = _sensor_mode;
		}
		break;
	default:
		TxErrorStatus(0);
		break;
	}
	return;
}

// Error process in Write command
void TxErrorStatus(UCHAR error_mode)
{
	ble_info.tx_len = LEN_TX_ERROR;
	ble_info.tx_data[0] = CMD_ERROR;
	ble_info.tx_data[1] = ble_info.rx_data[1];
	BT_memcpy(&ble_info.tx_data[2],&ERROR_MESSAGE[error_mode].message,sizeof(_error_message));
	return;
}

// Notification
void NotifyCommand(void)
{
	tRtcTime t_time;
	UCHAR n;
	for(n=0;n<sensor_amount;n++)					// calcurate next sensor number
	{
		_current_sensor_num++;
		if(_current_sensor_num >= sensor_amount)
		{
			_current_sensor_num = 0;
		}
		if(_sensor_ena[_current_sensor_num] == TRUE)// check enable/disenable
		{
			break;
		}
	}
	if(n>=sensor_amount)							// if any sensor is not avaiable, set -1 to _current_sensor_num.
	{
		_current_sensor_num = -1;
	}
	loop();											// Prepare sensor data
	if(_current_sensor_num>=0)						// prepare data packet of notification
	{
		rtc_getTime(&t_time);
		ble_info.notify_data[0] = _current_sensor_num;					// set current sensor number
		BT_memcpy(&ble_info.notify_data[1],&t_time,7);					// set RTC data
		BT_memcpy(&ble_info.notify_data[8],_sensor_data,4);				// set sensor data;
		ble_info.notify_len = (UCHAR)(_sensor_data_len + 8);			// set length
	}
	else											//if any sensor is not available, data is not sent
	{
		ble_info.notify_len = 0;		

	}
}

BOOL set_OutputSensorData(void *SensorData)
{
	UCHAR n;
	BOOL res = TRUE;
	for(n=0;n<sizeof(_sensor_data);n++)
	{
		_sensor_data[n] = 0;
	}
	switch(sensor_param[_current_sensor_num].datatype)
	{
	case TYPE_CHAR:
	case TYPE_UCHAR:
		_sensor_data_len = 1;
		BT_memcpy(_sensor_data,SensorData,1);
		break;
	case TYPE_INT16:
	case TYPE_UINT16:
		_sensor_data_len = 2;
		BT_memcpy(_sensor_data,SensorData,2);
		break;
	case TYPE_INT32:
	case TYPE_FLOAT:
	case TYPE_UINT32:
		_sensor_data_len = 4;
		BT_memcpy(_sensor_data,SensorData,4);
		break;
	default :
		res = FALSE;
		_sensor_data_len = 0;
		break;
	}
	return res;
}

CHAR get_SensorNum(void)
{
	return _current_sensor_num;
}
