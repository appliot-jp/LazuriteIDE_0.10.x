/* subghz_api.c
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
#include "subghz_api.h"
#include "bp3596.h"
#include "hal.h"
#include "string.h"
#include "lp_manage.h"
#include "driver_irq.h"

#define INIT_SLEEP
//#define TEST_SEND_INTERVAL

// local function
static SUBGHZ_MSG subghz_init(void);
static SUBGHZ_MSG subghz_begin(uint8_t ch, uint16_t panid, SUBGHZ_RATE rate, SUBGHZ_POWER txPower);
static SUBGHZ_MSG subghz_close(void);
static SUBGHZ_MSG subghz_tx(uint16_t panid, uint16_t dstAddr, uint8_t *data, uint16_t len, void (*callback)(uint8_t rssi, uint8_t status));
static bool subghz_rxStatus(void);
static SUBGHZ_MSG subghz_rxEnable(void (*callback)(uint8_t rssi, int status, uint16_t size));
static SUBGHZ_MSG subghz_rxDisable(void);
//static SUBGHZ_MSG subghz_setPANID(uint16_t panid);
static uint16_t subghz_getMyAddress(void);
static void subghz_msgOut(SUBGHZ_MSG msg);
static SUBGHZ_MSG subghz_halt_until_complete(void);
static void subghz_getStatus(SUBGHZ_STATUS *tx, SUBGHZ_STATUS *rx);
static void subghz_txdone(uint8_t rssi, int status);
static void subghz_rxdone(uint8_t *data, uint8_t rssi, int status);
static short subghz_readData(uint8_t *data, uint16_t max_size);
static SUBGHZ_MSG subghz_setSendMode(SUBGHZ_PARAM *param);
static SUBGHZ_MSG subghz_getSendMode(SUBGHZ_PARAM *param);

// setting of function
const SubGHz_CTRL SubGHz = {
	subghz_init,
	subghz_begin,
	subghz_close,
	subghz_tx,
	subghz_rxEnable,
	subghz_rxDisable,
	subghz_readData,
	subghz_getMyAddress,
	subghz_getStatus,
	subghz_msgOut,
	subghz_setSendMode,
	subghz_getSendMode
};


// local parameters
static struct {
	SUBGHZ_STATUS tx_stat;
	SUBGHZ_STATUS rx_stat;
	volatile bool sending;
	volatile bool open;
	void (*rx_callback)(uint8_t *data, uint8_t rssi, uint16_t size);
	void (*tx_callback)(uint8_t rssi, int status);
	uint8_t *rx_buf;
	bool read;
	uint16_t myAddress;
	uint8_t addrType;
	uint8_t senseTime;
	uint8_t txRetry;
	uint8_t ch;
	SUBGHZ_RATE rate;
	uint16_t txInterval;
	// 2015.10.26 Eiichi Saito   addition random backoff
	uint16_t ccaWait;
} subghz_param;

#define DEBUG

#ifndef DEBUG
static struct {
#else
struct {
#endif
	unsigned long start_time;
	unsigned long last_send_time;
	unsigned long total_send_bytes;
} arib = {0,0x80000000,0};

// local function

static SUBGHZ_MSG subghz_init(void)
{
	SUBGHZ_MSG msg;
	int result;
	
	// clear parameter memory
	memset(&subghz_param,0,sizeof(subghz_param));
	// setting default value
	subghz_param.addrType = 6;
	subghz_param.senseTime = 20;
	subghz_param.txRetry = 0;
	subghz_param.txInterval = 500;
	// 2015.10.26 Eiichi Saito   addition random backoff
	subghz_param.ccaWait = 4;
	
	// reset
	result = BP3596_reset();
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_RESET_FAIL;
		goto error;
	}
	
	// 2015.10.26 Eiichi Saito   addition random backoff
	result = BP3596_setup(33,	 (uint8_t)SUBGHZ_50KBPS, (uint8_t)SUBGHZ_PWR_1MW, subghz_param.senseTime, subghz_param.txRetry,subghz_param.txInterval, subghz_param.ccaWait );
	if( result != BP3596_STATUS_OK )
	{
		msg = SUBGHZ_SETUP_FAIL;
		goto error;
	}
	
	// get my address for setting address filter
	result = BP3596_getMyAddr(&subghz_param.myAddress);
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_MYADDR_FAIL;
		goto error;
	}
#ifdef	INIT_SLEEP
	result = BP3596_sleep();
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_SLEEP_FAIL;
		goto error;
	}
#endif // INIT_SLEEP
	
	subghz_param.sending = false;
	subghz_param.read = false;
#ifdef INIT_SLEEP
	subghz_param.open = false;
#else  // INIT_SLEEP
	subghz_param.open = true;
#endif // INIT_SLPPE
	msg =  SUBGHZ_OK;
	
	arib.start_time = millis();

error:
	subghz_param.tx_stat.status = result;
	return msg;
}

static SUBGHZ_MSG subghz_begin(uint8_t ch, uint16_t panid, SUBGHZ_RATE rate, SUBGHZ_POWER txPower)
{
	SUBGHZ_MSG msg;
	int result;
	
	if(subghz_param.open == false)
	{
		subghz_param.open = true;
		// start clock of ml7386
		result = BP3596_wakeup();
		if(result != BP3596_STATUS_OK)
		{
			msg = SUBGHZ_WAKEUP_FAIL;
			goto error;
		}
	}
	
	subghz_param.ch = ch;
	
	// 2015.10.26 Eiichi Saito   addition random backoff
	result = BP3596_setup(ch, (uint8_t)rate, (uint8_t)txPower, subghz_param.senseTime, subghz_param.txRetry,subghz_param.txInterval,subghz_param.ccaWait );
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_SETUP_FAIL;
		goto error;
	}
	
	subghz_param.rate = rate;
	
	// data to myaddress and grobal address in specified PANID can be received.
	#ifndef SNIFFER
	result = BP3596_setFilter(panid, subghz_param.myAddress, 0xffff);
	#endif
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_SETFIL_FAIL;
		goto error;
	}
	
	result = BP3596_setMyPANID(panid);
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_PANID;
		goto error;
	}
	
	msg = SUBGHZ_OK;
	
error:
	subghz_param.tx_stat.status = result;
	return msg;
}

static SUBGHZ_MSG subghz_close(void)
{
	SUBGHZ_MSG msg;
	int result;
	result = BP3596_sleep();
	if( result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_SLEEP_FAIL;
		goto error;
	}
	subghz_param.open = false;
	
	msg = SUBGHZ_OK;
error:
	subghz_param.tx_stat.status = result;
	return msg;
}

static SUBGHZ_MSG subghz_tx(uint16_t panid, uint16_t dstAddr, uint8_t *data, uint16_t len, void (*callback)(uint8_t rssi, uint8_t status))
{
	SUBGHZ_MSG msg;
	int result;
	
	unsigned long current_time;
	unsigned long tmp_ttl_byte;
	unsigned long duration;
	
	// setting tx callback
	result = BP3596_setFuncSendComplete(subghz_txdone);
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_TX_COMP_FAIL;
		goto error_not_send;
	}
	
	// check total send bytes in an hours -- an hours
	current_time = millis();
	duration = current_time - arib.start_time;
	if(duration > 3600000L)
	{
		arib.total_send_bytes = 0;
		arib.start_time = current_time;
	}
	
	// check total send bytes in an hours -- calcurate total send bytes
	tmp_ttl_byte = arib.total_send_bytes;
	switch(subghz_param.rate)
	{
#define SUBGHZ_HEADER_SIZE	11
	case SUBGHZ_50KBPS:
		tmp_ttl_byte += len + SUBGHZ_HEADER_SIZE;
	case SUBGHZ_100KBPS:
		tmp_ttl_byte += len + SUBGHZ_HEADER_SIZE;
		if(tmp_ttl_byte>45000000)
		{
			msg = SUBGHZ_TTL_SEND_OVR;
			goto error_not_send;
		}
		break;
	default:
		msg = SUBGHZ_SETUP_FAIL;
		goto error_not_send;
		break;
	}
	
	// CHECK ARIB condition -- interval --
	duration = current_time - arib.last_send_time;
	
	if((subghz_param.ch >= 24)&&(subghz_param.ch<=32))
	{
		if(duration < 50)
		{
			sleep(50-duration);
		}
	}
	
//	BP3596_send(data, len, addrType, dstAddr, dstPANID);
	
	result = BP3596_send(data, len, subghz_param.addrType,        dstAddr, panid);
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_TX_FAIL;
		goto error_not_send;
	}
	subghz_param.sending = true;
	
	subghz_param.tx_callback = callback;
	if(subghz_param.tx_callback == NULL)
	{
		msg = subghz_halt_until_complete();
//	#ifdef DEBUG
//		Serial.print("msg=");
//		Serial.println_long(msg,DEC);
//	#endif
		if(msg == SUBGHZ_TX_CCA_FAIL)
		{
			goto error_not_send;
		}
	}
	
	arib.last_send_time = millis();
	arib.total_send_bytes = tmp_ttl_byte;
error_not_send:
	subghz_param.tx_stat.status = result;
	return msg;
}

SUBGHZ_MSG subghz_halt_until_complete(void)
{
	SUBGHZ_MSG msg;
	
	while(subghz_param.sending == true)
	{
		lp_setHaltMode();
	}
	if(subghz_param.tx_stat.status > 0)
	{
		msg = SUBGHZ_OK;
	}
	else if(subghz_param.tx_stat.status == BP3596_STATUS_ERROR_CCA)
	{
		msg = SUBGHZ_TX_CCA_FAIL;
	}
	else
	{
		msg = SUBGHZ_TX_ACK_FAIL;
	}

	return msg;
}

static void subghz_txdone(uint8_t rssi, int status)
{
	subghz_param.sending = false;
	subghz_param.tx_stat.rssi = rssi;
	subghz_param.tx_stat.status = status;
//#ifdef DEBUG
//	Serial.print("status=");
//	Serial.println_long(status,DEC);
//#endif
	if(subghz_param.tx_callback != NULL)
	{
		subghz_param.tx_callback(rssi, status);
	}
}

static void subghz_rxdone(uint8_t *data, uint8_t rssi, int status)
{
//	static long cycle = 0;
//	Serial.print_long(++cycle, DEC);				// for test
//	Serial.print(" ");								// for test
//	Serial.print_long(rssi, DEC);					// for test
//	Serial.print(" ");								// for test
//	Serial.print_long(status, DEC);					// for test
//	Serial.println("");

//	__DI();

	if(subghz_param.rx_callback != NULL)
	{
		subghz_param.rx_callback(data, rssi, status);
	}
	else
	{
		subghz_param.rx_buf = data;	
		subghz_param.rx_stat.rssi = rssi;
//		subghz_param.rx_stat.rssi = 1;					// for test
		subghz_param.rx_stat.status = status;
	}
//	__EI();
	return;
}

static short subghz_readData(uint8_t *data, uint16_t max_size)
{
	short result = 0;
	
//	__DI();
	dis_interrupts(DI_SUBGHZ);
	if(subghz_param.rx_buf == NULL)
	{
		result = 0;
		goto end;
	}
	result = subghz_param.rx_stat.status;
	
	if(result > 0) 
	{
		if(max_size > result)
		{
			max_size = result;
		}
		memcpy(data, subghz_param.rx_buf,max_size+1);
		subghz_param.rx_buf = NULL;
	}
	
end:
//	__EI();
	enb_interrupts(DI_SUBGHZ);
	return result;
}
static SUBGHZ_MSG subghz_rxEnable(void (*callback)(uint8_t rssi, int status, uint16_t size))
{
	int result;
	SUBGHZ_MSG msg = SUBGHZ_OK;
	
	subghz_param.rx_callback = callback;
	if(subghz_param.read == false)
	{
		result = BP3596_setFuncRecvComplete(subghz_rxdone);
		if(result != BP3596_STATUS_OK)
		{
			msg = SUBGHZ_RX_COMP_FAIL;
			subghz_param.rx_stat.status = result;
			goto error;
		}
		result = BP3596_recvEnable();
		if(result != BP3596_STATUS_OK)
		{
			msg = SUBGHZ_RX_ENB_FAIL;
			subghz_param.rx_stat.status = result;
			goto error;
		}
	}
	subghz_param.rx_buf = NULL;
	subghz_param.read = true;
	
error:
	return msg;
}

static SUBGHZ_MSG subghz_rxDisable(void)
{
	SUBGHZ_MSG msg;
	int result;
	
	result = BP3596_recvDisable();
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_RX_DIS_FAIL;
	}
	subghz_param.read = false;
	msg = SUBGHZ_OK;
error:
	subghz_param.rx_stat.status = result;
	return msg;
}

/*static SUBGHZ_MSG subghz_setPANID(uint16_t panid)
{
	SUBGHZ_MSG msg;
	int result;
	
	result = BP3596_setMyPANID(panid);
	if(result != BP3596_STATUS_OK)
	{
		msg = SUBGHZ_PANID;
		goto error;
	}
	msg = SUBGHZ_OK;
error:
	subghz_param.tx_stat.status = result;
	return msg;
}
*/
static void subghz_getStatus(SUBGHZ_STATUS *tx, SUBGHZ_STATUS *rx)
{
	if (tx != NULL)
	{
		memcpy(tx,&subghz_param.tx_stat, sizeof(subghz_param.tx_stat));
	}
	if (rx != NULL)
	{
		memcpy(rx,&subghz_param.rx_stat, sizeof(subghz_param.rx_stat));
	}
	return;
}

static uint16_t subghz_getMyAddress(void)
{
	return subghz_param.myAddress;
}

static void subghz_msgOut(SUBGHZ_MSG msg)
{
	char message[32];
	
	switch (msg)
	{
	case SUBGHZ_RESET_FAIL:
		strncpy(message,"RESET FAIL\t",31);
		break;
	case SUBGHZ_SETUP_FAIL:
		strncpy(message,"SETUP FAIL\t",31);
		break;
	case SUBGHZ_SLEEP_FAIL:
		strncpy(message,"SLEEP FAIL\t",31);
		break;
	case SUBGHZ_WAKEUP_FAIL:
		strncpy(message,"WAKEUP FAIL\t",31);
		break;
	case SUBGHZ_MYADDR_FAIL:
		strncpy(message,"get My Address FAIL\t",31);
		break;
	case SUBGHZ_SETFIL_FAIL:
		strncpy(message,"set filter FAIL\t",31);
		break;
	case SUBGHZ_TX_COMP_FAIL:
		strncpy(message,"Send Complete FAIL\t",31);
		break;
	case SUBGHZ_TX_FAIL:
		strncpy(message,"Send FAIL\t",31);
		break;
	case SUBGHZ_TX_ACK_FAIL:
		strncpy(message,"ACK of send FAIL\t",31);
		break;
	case SUBGHZ_RX_ENB_FAIL:
		strncpy(message,"Recv Enable FAIL\t",31);
		break;
	case SUBGHZ_RX_DIS_FAIL:
		strncpy(message,"Recv Disable FAIL\t",31);
		break;
	case SUBGHZ_PANID:
		strncpy(message,"set PANID FAIL\t",31);
		break;
	default:
		goto no_error;
		break;
	}
	Serial.print(message);

no_error:
	Serial.print("RSSI=");
	Serial.print_long((long)subghz_param.tx_stat.rssi,DEC);
	Serial.print("\tSTATUS=");
	Serial.println_long((long)subghz_param.tx_stat.status,DEC);
	return;
}

static SUBGHZ_MSG subghz_getSendMode(SUBGHZ_PARAM *param)
{
	param->addrType = subghz_param.addrType;
	param->senseTime = subghz_param.senseTime;
	param->txRetry = subghz_param.txRetry;
	param->txInterval = subghz_param.txInterval;
	// 2015.10.26 Eiichi Saito   addition random backoff
	param->ccaWait = subghz_param.ccaWait;
	param->myAddress = subghz_param.myAddress; 

	return SUBGHZ_OK;
}

static SUBGHZ_MSG subghz_setSendMode(SUBGHZ_PARAM *param)
{
	// check addrType
	if( param->addrType > 7 )
	{
		return SUBGHZ_ERR_ADDRTYPE;
	}
	subghz_param.addrType = param->addrType;
	subghz_param.senseTime = param->senseTime;
	subghz_param.txRetry = param->txRetry;
	subghz_param.txInterval = param->txInterval;
	// 2015.10.26 Eiichi Saito   addition random backoff
	subghz_param.ccaWait = param->ccaWait;
	subghz_param.myAddress = param->myAddress;
	
	return SUBGHZ_OK;
}

