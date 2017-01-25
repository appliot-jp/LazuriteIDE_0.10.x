/* FILE NAME: Sd2Card.h
 *
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This Library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef Sd2Card_h
#define Sd2Card_h

#include "SdInfo.h"

//------------------------------------------------------------------------------
/** Protect block zero from write if nonzero */
#define SD_PROTECT_BLOCK_ZERO (1)
/** init timeout ms */
#define SD_INIT_TIMEOUT (2000)
/** erase timeout ms */
#define SD_ERASE_TIMEOUT (10000)
/** read timeout ms */
#define SD_READ_TIMEOUT (300)
/** write time out ms */
#define SD_WRITE_TIMEOUT (600)
//------------------------------------------------------------------------------
// SD card errors
/** timeout error for command CMD0 */
#define SD_CARD_ERROR_CMD0 (0x1)
/** CMD8 was not accepted - not a valid SD card*/
#define SD_CARD_ERROR_CMD8 (0x2)
/** card returned an error response for CMD17 (read block) */
#define SD_CARD_ERROR_CMD17 (0x3)
/** card returned an error response for CMD24 (write block) */
#define SD_CARD_ERROR_CMD24 (0x4)
/**  WRITE_MULTIPLE_BLOCKS command failed */
#define SD_CARD_ERROR_CMD25 (0x05)
/** card returned an error response for CMD58 (read OCR) */
#define SD_CARD_ERROR_CMD58 (0x06)
/** SET_WR_BLK_ERASE_COUNT failed */
#define SD_CARD_ERROR_ACMD23 (0x07)
/** card's ACMD41 initialization process timeout */
#define SD_CARD_ERROR_ACMD41 (0x08)
/** card returned a bad CSR version field */
#define SD_CARD_ERROR_BAD_CSD (0x09)
/** erase block group command failed */
#define SD_CARD_ERROR_ERASE (0x0A)
/** card not capable of single block erase */
#define SD_CARD_ERROR_ERASE_SINGLE_BLOCK (0x0B)
/** Erase sequence timed out */
#define SD_CARD_ERROR_ERASE_TIMEOUT (0x0C)
/** card returned an error token instead of read data */
#define SD_CARD_ERROR_READ (0x0D)
/** read CID or CSD failed */
#define SD_CARD_ERROR_READ_REG (0x0E)
/** timeout while waiting for start of read data */
#define SD_CARD_ERROR_READ_TIMEOUT (0x0F)
/** card did not accept STOP_TRAN_TOKEN */
#define SD_CARD_ERROR_STOP_TRAN (0x10)
/** card returned an error token as a response to a write operation */
#define SD_CARD_ERROR_WRITE (0x11)
/** attempt to write protected block zero */
#define SD_CARD_ERROR_WRITE_BLOCK_ZERO (0x12)
/** card did not go ready for a multiple block write */
#define SD_CARD_ERROR_WRITE_MULTIPLE (0x13)
/** card returned an error to a CMD13 status check after a write */
#define SD_CARD_ERROR_WRITE_PROGRAMMING (0x14)
/** timeout occurred during write programming */
#define SD_CARD_ERROR_WRITE_TIMEOUT (0x15)
/** incorrect rate selected */
#define SD_CARD_ERROR_SCK_RATE (0x16)
//------------------------------------------------------------------------------
// SD card types
/** Standard capacity V1 SD card */
#define SD_CARD_TYPE_SD1 (1)
/** Standard capacity V2 SD card */
#define SD_CARD_TYPE_SD2 (2)
/** High Capacity SD card */
#define SD_CARD_TYPE_SDHC (3)
//------------------------------------------------------------------------------

typedef struct {
  uint32_t block_;
  uint8_t  chipSelectPin_;
  uint8_t  errorCode_;
  uint8_t  inBlock_;
  uint16_t offset_;
  uint8_t  partialBlockRead_;
  uint8_t  status_;
  uint8_t  type_;
} st_Sd2Card_v;

uint8_t Sd2Card_cardAcmd(st_Sd2Card_v *v, uint8_t cmd, uint32_t arg);
uint8_t Sd2Card_cardCommand(st_Sd2Card_v *v, uint8_t cmd, uint32_t arg);
void 	Sd2Card_chipSelectHigh(st_Sd2Card_v *v);
void 	Sd2Card_chipSelectLow(st_Sd2Card_v *v);
void 	Sd2Card_error(st_Sd2Card_v *v, uint8_t code);
boolean Sd2Card_init(st_Sd2Card_v *v, uint8_t sckRateID, uint8_t chipSelectPin);
uint8_t Sd2Card_readBlock(st_Sd2Card_v *v, uint32_t block, uint8_t* dst);
uint8_t Sd2Card_readData(st_Sd2Card_v *v, uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);
void	Sd2Card_readEnd(st_Sd2Card_v *v);
uint8_t Sd2Card_waitNotBusy(uint16_t timeoutMillis);
uint8_t Sd2Card_waitStartBlock(st_Sd2Card_v *v);
uint8_t Sd2Card_writeBlock(st_Sd2Card_v *v, uint32_t blockNumber, const uint8_t* src);
uint8_t Sd2Card_writeData(st_Sd2Card_v *v, uint8_t token, const uint8_t* src);
uint8_t Sd2Card_writeStart(st_Sd2Card_v *v, uint32_t blockNumber, uint32_t eraseCount);
uint8_t Sd2Card_writeStop(st_Sd2Card_v *v);
void	Sd2Card_type(st_Sd2Card_v *v, uint8_t value);

#endif  // Sd2Card_h
