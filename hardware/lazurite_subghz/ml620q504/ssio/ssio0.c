/* FILE NAME: ssio0.c
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
#include "ssio0.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/

/*--- Operation status(internal) ---*/
#define SSIO_COMMUNICATION_END      ( 0 )   /**< Operation status : Communication completion            */
#define SSIO_READ_DATA              ( 1 )   /**< Operation status : Data read operation mode            */
#define SSIO_WRITE_DATA             ( 2 )   /**< Operation status : Data write operation mode           */
#define SSIO_READ_WRITE_DATA        ( 3 )   /**< Operation status : Data read and write operation mode  */

/**
 * Control parameters for SSIO
 */
typedef struct {
	void            *rxData;               /**< Pointer to area where the receive address is stored    */
	void            *txData;               /**< Pointer to area where the send address is stored       */
	unsigned int    dataSize;              /**< Size of writing/reading data                           */
	unsigned int    cnt;                    /**< Size of data which is writing/reading                  */
	cbfSsio_t       callBack;               /**< Callback function                                      */
	unsigned char   status;                 /**< Read/write operation status                            */
} ssioCtrlParam_t;


/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
static ssioCtrlParam_t  s_ctrlParam;        /**< control parameter for SSIO   */

/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/

/**
 * Start to read/write data
 *
 * @param[in]   sionmod     Operation of SSIO mode                                                  <br>
 *                          (Setting SIOnMOD register (SnMD0-1))
 * @arg                         SSIO_MODE1  : Reading (receive)
 * @arg                         SSIO_MODE2  : Writing (send)
 * @arg                         SSIO_MODE3  : Reading and writing (receive/send)
 *
 * @param[in]   *rxData    Pointer to area where the read data is stored                           <br>
 *                              If bit-length is 16bit(SnLG=1), this parameter must be WORD address.
 *
 * @param[in]   *txData    Pointer to area where the write data is stored                          <br>
 *                              If bit-length is 16bit(SnLG=1), this parameter must be WORD address.
 *
 * @param[in]   dataSize   Size of data                                                            <br>
 *                          If bit-length is 8bit(SnLG=0),  unit is BYTE(=8bit).                    <br>
 *                          If bit-length is 16bit(SnLG=1), unit is WORD(=16bit).
 *
 * @param[in]   *func       Pointer to callback function
 *
 * @retval      SSIO_R_OK(=0)               : SSIO communication is started.        (success)
 * @retval      SSIO_R_ERR(=-1)             : A parameter 'mode' is incorrect.      (error)
 * @retval      SSIO_R_DATA_SIZE_NG(=-2)    : A parameter 'dataSize' is incorrect. (error)
 */
int ssio0_start( unsigned short sionmod, void *rxData, void *txData, unsigned int dataSize, cbfSsio_t func )
{
	unsigned short   op_mode;
	unsigned char    lg_mode;

	/* Operation of SSIO */
	op_mode = sionmod & (unsigned short)( SIOnMOD_SnMD1 | SIOnMOD_SnMD0 );
	/* Unit of a reading/writing operation  */
	lg_mode = ( unsigned char )get_bit( S0LG );
	/* If buffer mode is 16bit mode and transmission data size is odd value, then this function return to error. */
	if( (lg_mode != 0) && (( dataSize & (unsigned int)0x1 ) != 0) ) {
		return( SSIO_R_DATA_SIZE_NG );
	}

	/*=== Parameters setting ===*/
	s_ctrlParam.rxData     = rxData;
	s_ctrlParam.txData     = txData;
	s_ctrlParam.dataSize   = dataSize;
	s_ctrlParam.cnt         = 0;
	s_ctrlParam.callBack    = func;
	s_ctrlParam.status      = 0;

	switch( op_mode ) {
		/* Read(data receive) mode      */
		case SSIO_MODE1:
			/* setting operation status */
			s_ctrlParam.status = ( unsigned char )SSIO_READ_DATA;
			break;

		/* Write(data send) mode        */
		case SSIO_MODE2:
		/* Read and write mode          */
		case SSIO_MODE3:
			if( lg_mode == 0 ) {
				ssio0_putcByte( *((unsigned char *)txData) );
				s_ctrlParam.txData = ( (unsigned char *)s_ctrlParam.txData ) + 1;
				s_ctrlParam.cnt++;
			}
			else {
				ssio0_putcWord( *((unsigned short *)txData) );
				s_ctrlParam.txData = (void*)(( (unsigned short *)s_ctrlParam.txData ) + 1);
				/* add plus "2" because of 16bit transmission */
				s_ctrlParam.cnt += ( unsigned int )2;
			}
			/* setting operation status */
			if( op_mode == SSIO_MODE2 ) {
				s_ctrlParam.status = SSIO_WRITE_DATA;
			}
			else {
				s_ctrlParam.status = SSIO_READ_WRITE_DATA;
			}
			break;

		default:
			/* can not start */
			return ( SSIO_R_ERR );
	}

	/* --- Communication setting --- */
	/* Operation mode */
	clear_bit( S0MD0 );
	clear_bit( S0MD1 );
	set_reg8( SIO0MOD0, sionmod );
	/* Communication start */
	set_bit( S0EN );

	return ( SSIO_R_OK );
}


/**
 * Process to continue communication
 *
 * @param   -
 * @retval  SSIO_R_TRANS_FIN(=1)      : reading/writing communication is finished
 * @retval  SSIO_R_TRANS_CONT_OK(=0)  : reading/writing communication is continued (success)
 */
int ssio0_continue( void )
{
	unsigned char   flgTransEnd=0;
	unsigned char   lg_mode;

	/* Unit of a reading/writing operation  */
	lg_mode = ( unsigned char )get_bit( S0LG );

	/* operation status */
	switch( ( unsigned char )s_ctrlParam.status ) {
		/* --- Write(data send) mode ---    */
		case ( ( unsigned char )SSIO_WRITE_DATA ):
			if( s_ctrlParam.dataSize > s_ctrlParam.cnt ) {
				/* 8bit mode */
				if( lg_mode == 0 ) {
					ssio0_putcByte( *((unsigned char *)s_ctrlParam.txData) );
					/* pointing next write data */
					s_ctrlParam.txData = ( (unsigned char *)s_ctrlParam.txData ) + 1;
					s_ctrlParam.cnt++;
				}
				/* 16bit mode */
				else {
					ssio0_putcWord( *((unsigned short *)s_ctrlParam.txData) );
					/* pointing next write data */
					s_ctrlParam.txData = (void*)(( (unsigned short *)s_ctrlParam.txData ) + 1);
					/* counting value added "+2" because of 16bit transmission */
					s_ctrlParam.cnt += ( unsigned int )2;
				}
				/* Continue to writing */
				set_bit( S0EN );
			}
			/* End of communication */
			else {
				s_ctrlParam.status = SSIO_COMMUNICATION_END;
				flgTransEnd = ( unsigned char )0x1u;
			}
			break;

		/* --- Read(data receive) mode ---  */
		case ( ( unsigned char )SSIO_READ_DATA ):
			/* 8bit mode */
			if( lg_mode == 0 ) {
				*( (unsigned char *)s_ctrlParam.rxData ) = ssio0_getcByte();
				s_ctrlParam.rxData = ( (unsigned char *)s_ctrlParam.rxData ) + 1;
				s_ctrlParam.cnt++;
			}
			/* 16bit mode */
			else {
				*( (unsigned short *)s_ctrlParam.rxData ) = ssio0_getcWord();
				s_ctrlParam.rxData = (void*)( ( (unsigned short *)s_ctrlParam.rxData ) + 1 );
				/* counting value added "+2" because of 16bit transmission */
				s_ctrlParam.cnt += ( unsigned int )2;
			}
			if( s_ctrlParam.dataSize > s_ctrlParam.cnt ) {
				/* Continue to reading */
				set_bit( S0EN );
			}
			/* End of communication */
			else {
				s_ctrlParam.status = SSIO_COMMUNICATION_END;
				flgTransEnd = ( unsigned char )0x1u;
			}
			break;

		/* --- Read and write mode ---  */
		case ( ( unsigned char )SSIO_READ_WRITE_DATA ):
			/*--- Data read ---*/
			/* 8bit mode */
			if( lg_mode == 0 ) {
				*( (unsigned char *)s_ctrlParam.rxData ) = ssio0_getcByte();
				s_ctrlParam.rxData = ( (unsigned char *)s_ctrlParam.rxData ) + 1;
			}
			/* 16bit mode */
			else {
				*( (unsigned short *)s_ctrlParam.rxData ) = ssio0_getcWord();
				s_ctrlParam.rxData = (void*)( ( (unsigned short *)s_ctrlParam.rxData ) + 1 );
			}
			if( s_ctrlParam.dataSize > s_ctrlParam.cnt ) {
				/*--- Next data write ---*/
				/* 8bit mode */
				if( lg_mode == 0 ) {
					ssio0_putcByte( *((unsigned char *)s_ctrlParam.txData) );
					/* pointing next send data */
					s_ctrlParam.txData = ( (unsigned char *)s_ctrlParam.txData ) + 1;
					s_ctrlParam.cnt++;
				}
				/* 16bit mode */
				else {
					ssio0_putcWord( *((unsigned short *)s_ctrlParam.txData) );
					/* pointing next send data */
					s_ctrlParam.txData = (void*)( ( (unsigned short *)s_ctrlParam.txData ) + 1 );
					/* counting value added "+2" because of 16bit transmission */
					s_ctrlParam.cnt += ( unsigned int )2;
				}
				/* Continue to writing */
				set_bit( S0EN );
			}
			/* End of communication */
			else {
				s_ctrlParam.status = SSIO_COMMUNICATION_END;
				flgTransEnd = ( unsigned char )0x1u;
			}
			break;

		/* Communication stop */
		case ( ( unsigned char )SSIO_COMMUNICATION_END ):
		default:
			return ( SSIO_R_TRANS_FIN );
	}

	/* If communication is completed, then callback function is called. */
	if( flgTransEnd != 0 ) {
		if( s_ctrlParam.callBack != (void *)0 ) {
			s_ctrlParam.callBack( s_ctrlParam.cnt );
		}
		return ( SSIO_R_TRANS_FIN );
	}
	return ( SSIO_R_TRANS_CONT_OK );
}

