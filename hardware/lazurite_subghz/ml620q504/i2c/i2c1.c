/* FILE NAME: i2c1.c
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

#include "mcu.h"
#include "rdwr_reg.h"
#include "i2c1.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/

/*--- Operation status ---*/
#define I2C_COMMUNICATION_END       ( 0 )           /**< Operation status : communication completion              */
#define I2C_TRANS_SLAVE_ADDRESS     ( 1 )           /**< Operation status : slave address send mode               */
#define I2C_TRANS_ADDRESS           ( 2 )           /**< Operation status : write/read address send mode          */
#define I2C_WRITE_DATA              ( 3 )           /**< Operation status : data write mode                       */
#define I2C_READ_DATA               ( 4 )           /**< Operation status : data read mode                        */

/**
 * Control parameter for I2C
 */
typedef struct {
    unsigned char   mode;                           /**< transfer mode(0:write, 1:read) after send slave address  */
    unsigned char   *addr;                          /**< pointer to area where the send/receive address is stored */
    unsigned int    addrSize;                       /**< size of send address                                     */
    unsigned char   *data;                          /**< pointer to area where the write/read data is stored      */
    unsigned int    dataSize;                       /**< size of write/read data                                  */
    unsigned int    cnt;                            /**< size of data which is writing/reading                    */
    cbfI2c_t        callBack;                       /**< callback function                                        */
    unsigned char   errStat;                        /**< error status                                             */
    unsigned char   status;                         /**< writing/reading operation status                         */
} I2cCtrlParam_t;

/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
static I2cCtrlParam_t       s_ctrlParam;            /**< control parameter for I2C    */

/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/

/**
 * Initialize I2C
 *
 * @param[in]   i2cnmod     Configuration of I2C mode , and so on               <br>
 *                          (Setting value of I2C1MOD register)                 <br>
 *                          Specify the logical sum of the following items.
 * @arg                         Transmission mode     : I2C_MOD_STD, or I2C_MOD_FST
 * @arg                         Attenuation rate      : I2C_DW_ATTENUAT_NONE, or I2C_DW_ATTENUAT_10, or I2C_DW_ATTENUAT_20, or I2C_DW_ATTENUAT_30
 * @arg                         Clock division        : I2C_CD_OSCLK, or I2C_CD_OSCLK2, or I2C_CD_OSCLK4
 *
 * @return      None
 *
 * @note
 *              This is set so that the communication speed becomes 100 kbps/400 kbps when the operating frequency of I2C is 4MHz. <br>
 *              Set the operating frequency of I2C in I2nCD0 and I2nCD1.
 */
void i2c1_init( unsigned short i2cnmod ) {
	/*=== Register setting ===*/
	/*--- i2c communication action stop ---*/
	i2c1_stop();
	/*--- Communication setting ---*/
	write_reg16( I2C1MOD, (unsigned short)I2CnMOD_I2nEN | i2cnmod );
}

/**
 * Start to send writing data
 *
 * @param[in]   slaveAddr   Slave address                                       <br>
 *                          (Setting I2C1SA register value)                     <br>
 *                          Bit0(=I21RW) means read/write mode.
 *
 * @param[in]   *addr       Pointer to area where the adress data is stored
 *
 * @param[in]   addrSize    Size of data to adress (unit is byte)               <br>
 *                          Exclude the size of slave address.                  <br>
 *                          If you not need to transmit address data, Set this parameter '0'.
 *
 * @param[in]   *buf        Pointer to area where the write data is stored
 *
 * @param[in]   size        Size of data to write (unit is byte)                <br>
 *                          Exclude the size of slave address.
 *
 * @param[in]   func        Pointer to callback function
 *
 * @retval      I2C_R_OK(=0)               : I2C communication is started.      (success)
 */
int i2c1_write( unsigned char slaveAddr, unsigned char *addr, unsigned int addrSize, unsigned char *buf, unsigned short size, cbfI2c_t func )
{
	/*=== Transmission of a message system order parameter setting. ===*/
	s_ctrlParam.mode       = 0;    /* write */
	s_ctrlParam.addr       = addr;
	s_ctrlParam.addrSize   = addrSize;
	s_ctrlParam.data       = buf;
	s_ctrlParam.dataSize   = size;
	s_ctrlParam.cnt        = 0;
	s_ctrlParam.callBack   = func;
	s_ctrlParam.errStat    = 0;
	s_ctrlParam.status     = 0;

	/*=== transmit a adddress, and it is worked to start. ===*/
	s_ctrlParam.status = I2C_TRANS_SLAVE_ADDRESS;

	slaveAddr &= ~I2CnSA_I2nRW;
	i2c1_setSlaveAdrs( slaveAddr | I2C_RW_WRITE );  /* Data transmit mode (data write) */
	i2c1_trigStart();                               /* Start condition */
	return ( I2C_R_OK );
}

/**
 * Start to receive reading data
 *
 * @param[in]   slaveAddr   Slave address                                       <br>
 *                          (Setting I2C1SA register value)                     <br>
 *                          Bit0(=I21RW) means read/write mode.
 *
 * @param[in]   *addr       Pointer to area where the adress data is stored
 *
 * @param[in]   addrSize    Size of data to adress (unit is byte)               <br>
 *                          Exclude the size of slave address.                  <br>
 *                          If you not need to transmit address data, Set this parameter '0'.
 *
 * @param[in]   *buf        Pointer to area where the receive data is stored
 *
 * @param[in]   size        Size of data to read (unit is byte)                 <br>
 *                          Exclude the size of slave address.
 *
 * @param[in]   func        Pointer to callback function
 *
 * @retval      I2C_R_OK(=0)               : I2C communication is started.      (success)
 */
int i2c1_read( unsigned char slaveAddr, unsigned char *addr, unsigned int addrSize, unsigned char *buf, unsigned short size, cbfI2c_t func )
{
	/*=== Reception mode setting ===*/
	/*=== Transmission of a message system order parameter setting. ===*/
	s_ctrlParam.mode        = 1;    /* read */
	s_ctrlParam.addr        = addr;
	s_ctrlParam.addrSize    = addrSize;
	s_ctrlParam.data        = buf;
	s_ctrlParam.dataSize    = size;
	s_ctrlParam.cnt         = 0;
	s_ctrlParam.callBack    = func;
	s_ctrlParam.errStat     = 0;
	s_ctrlParam.status      = 0;

	/*=== I receive it, and it is worked to start. ===*/
	s_ctrlParam.status = I2C_TRANS_SLAVE_ADDRESS;

	slaveAddr &= ~I2CnSA_I2nRW;
	if( addrSize == 0 ){
		i2c1_setSlaveAdrs( slaveAddr | I2C_RW_READ );   /* Data receive mode (data read) */
	}
	else{
		i2c1_setSlaveAdrs( slaveAddr | I2C_RW_WRITE );  /* Data transmit mode (data write) */
	}
	i2c1_trigStart();                                   /* Start condition */
	return ( I2C_R_OK );
}

/**
 * Continue to communicate of writing/reading data
 *
 * @param       -
 *
 * @retval      I2C_R_TRANS_FIN(=1)        : Communication of writing/reading data is finished  (success/failure) <br>
 *                                           The callback function parameter has the causes of this failure .
 * @retval      I2C_R_TRANS_CONT_OK(=0)    : Communication of writing/reading data is continued (success)
 */

int i2c1_continue( void )
{
	switch( s_ctrlParam.status ){
		case I2C_TRANS_SLAVE_ADDRESS:
			/* transmission error check */
			if( i2c1_checkError() == 1 ){
				s_ctrlParam.status = I2C_COMMUNICATION_END;
				s_ctrlParam.errStat = I2C_ERR_SEND_ERR;
				write_reg16( I2C1CON, 0x0000 ); /* Stop operation of I2C */
				if( s_ctrlParam.callBack != (void *)0 ){
					s_ctrlParam.callBack( s_ctrlParam.cnt, s_ctrlParam.errStat );
				}
				return ( I2C_R_TRANS_FIN );
			}
			/* NACK reception check */
			else if( i2c1_getReceivedAck() == 1 ){
				s_ctrlParam.status = I2C_COMMUNICATION_END;
				s_ctrlParam.errStat = I2C_ERR_ACR;
				i2c1_trigStopCondition();       /* Stop condition */
			}
			/* Slave address transmission success */
			else{
				if( s_ctrlParam.addrSize > 0 ){
					s_ctrlParam.status = I2C_TRANS_ADDRESS;
					i2c1_putc( (unsigned char)*s_ctrlParam.addr );
					s_ctrlParam.addr++;
					s_ctrlParam.addrSize--;
					i2c1_trigStart();               /* Next operation start */
				}
				else{
					if( s_ctrlParam.mode != 0 ){
						s_ctrlParam.status = I2C_READ_DATA;
						if( ( s_ctrlParam.dataSize - 1) == s_ctrlParam.cnt ){
							set_bit( I21ACT );
						}
						i2c1_trigStart();           /* Next operation start */
					}
					else{
						s_ctrlParam.status = I2C_WRITE_DATA;
						i2c1_putc( (unsigned char)*s_ctrlParam.data );
						s_ctrlParam.data++;
						s_ctrlParam.cnt++;
						i2c1_trigStart();           /* Next operation start */
					}
				}
			}
			return(I2C_R_TRANS_CONT_OK);

		case I2C_TRANS_ADDRESS:
			if( i2c1_getReceivedAck() == 1 ){
				s_ctrlParam.status = I2C_COMMUNICATION_END;
				s_ctrlParam.errStat = I2C_ERR_ACR;
				i2c1_trigStopCondition();       /* Stop condition */
			}
			else{
				if( s_ctrlParam.addrSize > 0 ){
					i2c1_putc( (unsigned char)*s_ctrlParam.addr );
					s_ctrlParam.addr++;
					s_ctrlParam.addrSize--;
					i2c1_trigStart();           /* Next operation start */
				}
				/* Address transmission success */
				else{
					if( s_ctrlParam.dataSize == 0 ){
						s_ctrlParam.status = I2C_COMMUNICATION_END;
						i2c1_trigStopCondition();       /* Stop condition */
					}
					else{
						if( s_ctrlParam.mode != 0 ){    /* Read mode */
							s_ctrlParam.status = I2C_TRANS_SLAVE_ADDRESS;
							set_bit( I21RW );           /* Read mode */
							i2c1_trigRestartCondition();/* Restart condition */
						}
						else{/* send mode */
							s_ctrlParam.status = I2C_WRITE_DATA;
							i2c1_putc( (unsigned char)*s_ctrlParam.data );
							s_ctrlParam.data++;
							s_ctrlParam.cnt++;
							i2c1_trigStart();           /* Next operation start */
						}
					}
				}
			}
			return ( I2C_R_TRANS_CONT_OK );

		case I2C_WRITE_DATA:
			/* NACK reception check */
			if( i2c1_getReceivedAck() == 1 ){
				s_ctrlParam.status = I2C_COMMUNICATION_END;
				s_ctrlParam.errStat = I2C_ERR_ACR;
				i2c1_trigStopCondition();       /* Stop condition */
			}
			/* Check to communication finish */
			else if( s_ctrlParam.dataSize > s_ctrlParam.cnt ){
				i2c1_putc( (unsigned char)*s_ctrlParam.data );
				s_ctrlParam.data++;
				s_ctrlParam.cnt++;
				i2c1_trigStart();               /* Next operation start */
			}
			else{
				s_ctrlParam.status = (unsigned char)I2C_COMMUNICATION_END;
				/*--- Stop condition send ? ---*/
				i2c1_trigStopCondition();       /* Stop condition */
			}
			return ( I2C_R_TRANS_CONT_OK );

		case I2C_READ_DATA:
			*s_ctrlParam.data = i2c1_getc();
			s_ctrlParam.data++;
			s_ctrlParam.cnt++;
			/* Check to communication finish */
			if( s_ctrlParam.dataSize > s_ctrlParam.cnt ){
				if( ( s_ctrlParam.dataSize - 1) == s_ctrlParam.cnt ){
					set_bit( I21ACT );
				}
				i2c1_trigStart();               /* Next operation start */
			}
			else{
				s_ctrlParam.status = (unsigned char)I2C_COMMUNICATION_END;
				i2c1_trigStopCondition();       /* Stop condition */
			}
			return ( I2C_R_TRANS_CONT_OK );

		case I2C_COMMUNICATION_END:
		default:
			/* End of communication */
			if( s_ctrlParam.callBack != (void *)0 ){
				s_ctrlParam.callBack( s_ctrlParam.cnt, s_ctrlParam.errStat );
			}
			break;
	}
	return ( I2C_R_TRANS_FIN );
}

