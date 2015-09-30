/* FILE NAME: ssiof0.c
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
#include "ssiof0.h"

/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#define SSIOF_SFnINTC_SFnRFIC0_BIT_NO ( 12 )                                      /**< SFnINTC Register SFnRFIC0 bit No.  */

#define SSIOF_WR_FIFO_MAX             ( 4 )                                       /**< Size of FIFO (Write)             */
#define SSIOF_DUMMY_DATA              ( 0x0000 )                                  /**< dummy data                       */
#define SSIOF_RD_ERR_STATUS           ( SFnSRR_SFnORF | SFnSRR_SFnMDF )           /**< Mask of error status (for Read)  */
#define SSIOF_WR_ERR_STATUS           ( SFnSRR_SFnMDF | SFnSRR_SFnWOF )           /**< Mask of error status (for Write) */

/*--- Operation status(internal) ---*/
#define SSIOF_COMMUNICATION_END       ( 0 )   /**< Operation status : Communication completion            */
#define SSIOF_READ_DATA               ( 1 )   /**< Operation status : Data read operation mode            */
#define SSIOF_WRITE_DATA              ( 2 )   /**< Operation status : Data write operation mode           */
#define SSIOF_READ_WRITE_DATA         ( 3 )   /**< Operation status : Data read / write operation mode    */

/**
 * Control parameters for SSIOF
 */
typedef struct {
	void            *rxData;                /**< Pointer to area where the receive address is stored        */
	void            *txData;                /**< Pointer to area where the send address is stored           */
	unsigned int    dataSize;               /**< Size of writing/reading data                               */
	unsigned int    cnt;                    /**< Size of data which is writing/reading                      */
	cbfSsiof_t      callBack;               /**< Callback function                                          */
	unsigned short  errStat;                /**< error status (for callback)                                */
	unsigned char   status;                 /**< Read/write operation status                                */
	unsigned char   blockSize;              /**< size of data which is one-operation writing/reading size   */
} ssiofCtrlParam_t;


/*############################################################################*/
/*#                                Variable                                  #*/
/*############################################################################*/
static ssiofCtrlParam_t   s_ctrlParam;        /**< control parameter for SSIOF    */

/*############################################################################*/
/*#                               Prototype                                  #*/
/*############################################################################*/

static void s_ssiof_writeSingleData_8( ssiofCtrlParam_t *param );
static void s_ssiof_writeDummyData_8( ssiofCtrlParam_t *param );
static void s_ssiof_writeSingleData_16( ssiofCtrlParam_t *param );
static void s_ssiof_writeDummyData_16( ssiofCtrlParam_t *param );
static void s_ssiof_writeFifo( ssiofCtrlParam_t *param, unsigned short (*wrFunc)( ssiofCtrlParam_t *prm ), unsigned char wrSize );

static void s_ssiof_readSingleData_8( ssiofCtrlParam_t *param );
static void s_ssiof_readSingleData_16( ssiofCtrlParam_t *param );
static void s_ssiof_readSingleDataNotCnt_8( ssiofCtrlParam_t *param );
static void s_ssiof_readSingleDataNotCnt_16( ssiofCtrlParam_t *param );
static void s_ssiof_readFifo( ssiofCtrlParam_t *param, unsigned short (*rdFunc)( ssiofCtrlParam_t *prm ) );

/*############################################################################*/
/*#                              Subroutine                                  #*/
/*############################################################################*/

/**
 * Writing a data
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @return          None
 */
static void s_ssiof_writeSingleData_8( ssiofCtrlParam_t *param )
{
	ssiof0_putcByte( *((unsigned char *)param->txData) );
	param->txData = ( (unsigned char *)s_ctrlParam.txData ) + 1;
	param->cnt++;
}

/**
 * Writing a data(Dummy)
 *
 * @param[in,out]   *param  Pointer to control parameters <br>
 *                          (Not used)
 * @return          None
 */
static void s_ssiof_writeDummyData_8( ssiofCtrlParam_t *param )
{
	ssiof0_putcByte( (unsigned char)SSIOF_DUMMY_DATA );

	param = param;  /* Warning removal */
}

/**
 * Writing a data
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @return          None
 */
static void s_ssiof_writeSingleData_16( ssiofCtrlParam_t *param )
{
	ssiof0_putcWord( *((unsigned short *)param->txData) );
	/* pointing next write data */
	param->txData = (void*)(( (unsigned short *)s_ctrlParam.txData ) + 1);
	param->cnt++;
}

/**
 * Writing a data(Dummy)
 *
 * @param[in,out]   *param  Pointer to control parameters <br>
 *                          (Not used)
 * @return          None
 */
static void s_ssiof_writeDummyData_16( ssiofCtrlParam_t *param )
{
	ssiof0_putcWord( (unsigned short)SSIOF_DUMMY_DATA );

	param = param;  /* Warning removal */
}

/**
 * Writing data to FIFO
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @param[in]       *wrFunc Pointer to write data function
 * @param[in]       wrSize  The size of next writing
 * @return          None
 */
#pragma INLINE s_ssiof_writeFifo
static void s_ssiof_writeFifo( ssiofCtrlParam_t *param, void (*wrFunc)( ssiofCtrlParam_t *prm ), unsigned char wrSize )
{
	if( (param->cnt + wrSize) > param->dataSize ) {
		wrSize = (unsigned char)(param->dataSize - param->cnt);
	}
	/* Write data (to FIFO) */
	for( ; wrSize != 0; wrSize-- ) {
		wrFunc( param );
	}
}

/**
 * Reading a data
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @return          None
 */
static void s_ssiof_readSingleData_8( ssiofCtrlParam_t *param )
{
	*( (unsigned char *)param->rxData ) = (unsigned char)ssiof0_getcByte();
	param->rxData = ( (unsigned char *)param->rxData ) + 1;
	param->cnt++;
}

/**
 * Reading a data
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @return          None
 */
static void s_ssiof_readSingleData_16( ssiofCtrlParam_t *param )
{
	*( (unsigned short *)param->rxData ) = (unsigned short)ssiof0_getcWord();
	param->rxData = (void*)(( (unsigned short *)param->rxData ) + 1);
	param->cnt++;
}

/**
 * Reading a data (not count)
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @return          None
 */
static void s_ssiof_readSingleDataNotCnt_8( ssiofCtrlParam_t *param )
{
	*( (unsigned char *)param->rxData ) = (unsigned char)ssiof0_getcByte();
	param->rxData = ( (unsigned char *)param->rxData ) + 1;
}

/**
 * Reading a data (not count)
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @return          None
 */
static void s_ssiof_readSingleDataNotCnt_16( ssiofCtrlParam_t *param )
{
	*( (unsigned short *)param->rxData ) = (unsigned short)ssiof0_getcWord();
	param->rxData = (void*)(( (unsigned short *)param->rxData ) + 1);
}

/**
 * Reading data from FIFO
 *
 * @param[in,out]   *param  Pointer to control parameters
 * @param[in]       *rdFunc Pointer to read data function
 * @return          None
 */
#pragma INLINE s_ssiof_readFifo
static void s_ssiof_readFifo( ssiofCtrlParam_t *param, void (*rdFunc)( ssiofCtrlParam_t *prm ) )
{
	unsigned char   rdSize;

	rdSize = ssiof0_getReadFifoSize();        /* size of reading */
	/* Read data (from FIFO) */
	for( ; rdSize != 0; rdSize-- ) {
		rdFunc( param );
	}
}

/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/

/**
 * Crear FIFO
 *
 * @param       -
 * @return      None
 */
void ssiof0_clearFifo(){
	unsigned char stat = 0;

	stat = SF0SPE;

	clear_bit(SF0SPE);
	set_bit( SF0FICL );
	clear_bit( SF0FICL );

	write_bit(SF0SPE, stat);
}


/**
 * Start to read/write data
 *
 * @param[in]   mode        Operation of SSIOF mode                                                 <br>
 *
 * @arg                         SSIOF_MODE_RD : Reading (receive)
 * @arg                         SSIOF_MODE_WR : Writing (send)
 * @arg                         SSIOF_MODE_RD_WR : Reading / Writing(receive/send)
 *
 * @param[in]   *rxData     Pointer to area where the read data is stored                           <br>
 *                              If bit-length is 16bit(SFnSIZ=1), this parameter must be WORD address.
 *
 * @param[in]   *txData     Pointer to area where the write data is stored                          <br>
 *                              If bit-length is 16bit(SFnSIZ=1), this parameter must be WORD address.
 *
 * @param[in]   dataCnt     Count of reading/writing operation (unit is BYTE or WORD)               <br>
 *                          If bit-length is 8bit(SFnSIZ=0), set BYTE value to this parameter.      <br>
 *                          If bit-length is 16bit(SFnSIZ=1), set WORD value to this parameter.     <br>
 *                          ex. 4word data writing operation                                        <br>
 * @code
                unsigned short writeData[4] = { 0x0000,0x0001,0x0002,0x0003 };
                ssiof0_start( SSIOF_MODE_WR, (void*)0, writeData, 4, anyFunction );
   @endcode
 *
 * @param[in]   *func       Pointer to callback function
 *
 * @retval      SSIOF_R_OK(=0)               : SSIOF communication is started.       (success)
 * @retval      SSIOF_R_ERR(=-1)             : A parameter 'mode' is incorrect.      (error)
 *
 * @note        Transmitting data via SSIOF makes receiving dummy data at FIFO.                      <br>
 *              Therefore receive FIFO shall be cleared before receiving data when data was sent.
 */
int ssiof0_start( unsigned char mode, void *rxData, void *txData, unsigned int dataCnt, cbfSsiof_t func )
{
	void             (*wrFunc)( ssiofCtrlParam_t *param );
	unsigned short   status=0;
	unsigned char    opUnit;

	/* Unit of a reading/writing operation  */
	opUnit = ( unsigned char )get_bit( SF0SIZ );

	/*=== Parameters setting ===*/
	s_ctrlParam.rxData      = rxData;
	s_ctrlParam.txData      = txData;
	s_ctrlParam.dataSize    = dataCnt;
	s_ctrlParam.cnt         = 0;
	s_ctrlParam.callBack    = func;
	s_ctrlParam.errStat     = 0;
	s_ctrlParam.status      = 0;

	switch( mode ) {
		/* Read(data receive) mode      */
		case SSIOF_MODE_RD:
			s_ctrlParam.blockSize = ( unsigned char )(((get_bit( SF0RFIC1 )<<1) | get_bit( SF0RFIC0 )) + 1);
			/* Master mode */
			if( get_bit(SF0MST) == 1 ) {
				if( opUnit == 0 ) {
					s_ssiof_writeDummyData_8( &s_ctrlParam );
				}
				else {
					s_ssiof_writeDummyData_16( &s_ctrlParam );
				}
				/* write dummy data to FIFO         */
				/* bacause of output reading clock  */
				s_ctrlParam.errStat |= ( status & SSIOF_WR_ERR_STATUS );
			}
			/* setting operation status */
			s_ctrlParam.status = ( unsigned char )SSIOF_READ_DATA;
			/* intruppt enable */
			write_bit( SF0INTCL, (unsigned char)0xA );

			break;

		/* Write(data send) mode        */
		case SSIOF_MODE_WR:
			s_ctrlParam.blockSize = ( unsigned char )( SSIOF_WR_FIFO_MAX - ((get_bit( SF0TFIC1 )<<1) | get_bit( SF0TFIC0 )) );
			if( opUnit == 0 ) {
				wrFunc = s_ssiof_writeSingleData_8;
			}
			else {
				wrFunc = s_ssiof_writeSingleData_16;
			}
			/* write to FIFO */
			s_ssiof_writeFifo( &s_ctrlParam, wrFunc, SSIOF_WR_FIFO_MAX );
			s_ctrlParam.errStat |= ( status & SSIOF_WR_ERR_STATUS );
			/* setting operation status */
			s_ctrlParam.status = SSIOF_WRITE_DATA;
			/* intruppt enable */
			write_bit( SF0INTCL, (unsigned char)0x5 );

			break;

		/* bi-direction (data receive / send) mode  */
		case SSIOF_MODE_RD_WR:
			/* [Note]writing size equals reading size */
			s_ctrlParam.blockSize = ( unsigned char )( SSIOF_WR_FIFO_MAX - ((get_bit( SF0TFIC1 )<<1) | get_bit( SF0TFIC0 )) );
			
			if( opUnit == 0 ) {
				wrFunc = s_ssiof_writeSingleData_8;
			}
			else {
				wrFunc = s_ssiof_writeSingleData_16;
			}
			/* write to FIFO */
			/* [Note]dummy data is not required */
			s_ssiof_writeFifo( &s_ctrlParam, wrFunc, SSIOF_WR_FIFO_MAX );
			s_ctrlParam.errStat |= ( status & SSIOF_WR_ERR_STATUS );

			/* setting operation status */
			s_ctrlParam.status = SSIOF_READ_WRITE_DATA;
			/* intruppt enable */
			write_bit( SF0INTCL, (unsigned char)0xF );

			break;

		default:
			/* can not start */
			return ( SSIOF_R_ERR );
	}

	/* --- Communication setting --- */
	/* Communication start */
	set_bit( SF0SPE );

	return ( SSIOF_R_OK );
}


/**
 * Process to continue communication
 *
 * @param   -
 * @retval  SSIOF_R_TRANS_FIN(=1)      : reading/writing communication is finished
 * @retval  SSIOF_R_TRANS_CONT_OK(=0)  : reading/writing communication is continued (success)
 */
int ssiof0_continue( void )
{
	int             ret;
	void            (*rwFunc)( ssiofCtrlParam_t *param );
	unsigned short  status=0;
	unsigned char   flgTransEnd=0;
	unsigned char   opUnit;

	/* communication status */
	status = ssiof0_getStatus();
	ssiof0_clearStatus( status );
	/* Unit of a reading/writing operation  */
	opUnit = ( unsigned char )get_bit( SF0SIZ );

	/* operation status */
	switch( s_ctrlParam.status ) {
		/* --- Write(data send) mode ---    */
		case SSIOF_WRITE_DATA:
			if( s_ctrlParam.dataSize > s_ctrlParam.cnt ) {
				if( (status & (SFnSRR_SFnTFI|SFnSRR_SFnFI)) != 0 ){
					/* 8bit mode */
					if( opUnit == 0 ) {
						rwFunc = s_ssiof_writeSingleData_8;
					}
					/* 16bit mode */
					else {
						rwFunc = s_ssiof_writeSingleData_16;
					}
					/* write to FIFO */
					s_ssiof_writeFifo( &s_ctrlParam, rwFunc, ( unsigned char )( SSIOF_WR_FIFO_MAX - ssiof0_getWriteFifoSize() ) );
					s_ctrlParam.errStat |= ( status & SSIOF_WR_ERR_STATUS );
				}
			}
			/* End of communication */
			else {
				if( (status & SFnSRR_SFnFI) != 0 ){
					s_ctrlParam.status = SSIOF_COMMUNICATION_END;
					flgTransEnd = ( unsigned char )0x1;
				}
			}
			break;

		/* --- Read(data receive) mode ---  */
		case SSIOF_READ_DATA:
			if( s_ctrlParam.dataSize > s_ctrlParam.cnt ) {
				if( (status & SFnSRR_SFnRFI) != 0 ){
					/* 8bit mode */
					if( opUnit == 0 ) {
						rwFunc = s_ssiof_readSingleData_8;
					}
					/* 16bit mode */
					else {
						rwFunc = s_ssiof_readSingleData_16;
					}
					/* read from FIFO */
					s_ssiof_readFifo( &s_ctrlParam, rwFunc );
					s_ctrlParam.errStat |= ( status & SSIOF_RD_ERR_STATUS );
					
					/* End of communication */
					if( s_ctrlParam.cnt >= s_ctrlParam.dataSize  ) {
						s_ctrlParam.status = SSIOF_COMMUNICATION_END;
						flgTransEnd = ( unsigned char )0x1;
						break;
					}

					/* Master mode */
					if( get_bit(SF0MST) == 1 ) {
						if( opUnit == 0 ) {
							s_ssiof_writeDummyData_8(&s_ctrlParam);
						}
						else {
							s_ssiof_writeDummyData_16(&s_ctrlParam);
						}
						/* write dummy data to FIFO         */
						/* bacause of output reading clock  */
						s_ctrlParam.errStat |= ( status & SSIOF_WR_ERR_STATUS );
					}
				}
			}
			/* End of communication */
			else {
				if( (status & SFnSRR_SFnSSIOFF) != 0 ){
					s_ctrlParam.status = SSIOF_COMMUNICATION_END;
					flgTransEnd = ( unsigned char )0x1;
				}
			}
			break;

		/* --- bi-direction (data receive / send) mode ---  */
		case SSIOF_READ_WRITE_DATA:
			/* receive interrupt */
			if( (status & SFnSRR_SFnRFI) != 0 ){
				/* 8bit mode */
				if( opUnit == 0 ) {
					rwFunc = s_ssiof_readSingleDataNotCnt_8;
				}
				/* 16bit mode */
				else {
					rwFunc = s_ssiof_readSingleDataNotCnt_16;
				}
				/* read from FIFO */
				s_ssiof_readFifo( &s_ctrlParam, rwFunc );
				s_ctrlParam.errStat |= ( status & SSIOF_RD_ERR_STATUS );
			}
			/* send interrupt */
			if( s_ctrlParam.dataSize > s_ctrlParam.cnt ) {
				if( (status & (SFnSRR_SFnTFI|SFnSRR_SFnFI)) != 0 ){
					/* 8bit mode */
					if( opUnit == 0 ) {
						rwFunc = s_ssiof_writeSingleData_8;
					}
					/* 16bit mode */
					else {
						rwFunc = s_ssiof_writeSingleData_16;
					}
					/* write to FIFO */
					s_ssiof_writeFifo( &s_ctrlParam, rwFunc, ( unsigned char )( SSIOF_WR_FIFO_MAX - ssiof0_getWriteFifoSize() ) );
					s_ctrlParam.errStat |= ( status & SSIOF_WR_ERR_STATUS );
				}
			}
			/* End of communication */
			else {
				if( (status & SFnSRR_SFnFI) != 0 ){
					s_ctrlParam.status = SSIOF_COMMUNICATION_END;
					flgTransEnd = ( unsigned char )0x1;
				}
			}
			break;

		/* Communication stop */
		case SSIOF_COMMUNICATION_END:
		default:
			flgTransEnd = ( unsigned char )0x1;
			break;
	}

	ret = (int)( SSIOF_R_TRANS_CONT_OK );
	/* If communication is completed, then callback function is called. */
	if( flgTransEnd != 0 ) {
		write_bit( SF0INTCL, (unsigned char)0x0 );
		if( s_ctrlParam.callBack != (void *)0 ) {
			s_ctrlParam.callBack( s_ctrlParam.cnt, s_ctrlParam.errStat );
		}
		ret = (int)( SSIOF_R_TRANS_FIN );
	}
	ssiof0_trigIRQ();
	return ret;
}
