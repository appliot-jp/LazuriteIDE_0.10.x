/* FILE NAME: SdInfo.h
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

#ifndef SdInfo_h
#define SdInfo_h

#include <lazurite.h>

// Based on the document:
//
// SD Specifications
// Part 1
// Physical Layer
// Simplified Specification
// Version 2.00
// September 25, 2006
//
// www.sdcard.org/developers/tech/sdcard/pls/Simplified_Physical_Layer_Spec.pdf
//------------------------------------------------------------------------------
// SD card commands
/** GO_IDLE_STATE - init card in spi mode if CS low */
#define  CMD0 (0x00)
/** SEND_IF_COND - verify SD Memory Card interface operating condition.*/
#define  CMD8 (0x08)
/** SEND_CSD - read the Card Specific Data (CSD register) */
#define  CMD9 (0x09)
/** SEND_CID - read the card identification information (CID register) */
#define  CMD10 (0x0A)
/** SEND_STATUS - read the card status register */
#define  CMD13 (0x0D)
/** READ_BLOCK - read a single data block from the card */
#define  CMD17 (0x11)
/** WRITE_BLOCK - write a single data block to the card */
#define  CMD24 (0x18)
/** WRITE_MULTIPLE_BLOCK - write blocks of data until a STOP_TRANSMISSION */
#define  CMD25 (0x19)
/** ERASE_WR_BLK_START - sets the address of the first block to be erased */
#define  CMD32 (0x20)
/** ERASE_WR_BLK_END - sets the address of the last block of the continuous
    range to be erased*/
#define  CMD33 (0x21)
/** ERASE - erase all previously selected blocks */
#define  CMD38 (0x26)
/** APP_CMD - escape for application specific command */
#define  CMD55 (0x37)
/** READ_OCR - read the OCR register of a card */
#define  CMD58 (0x3A)
/** SET_WR_BLK_ERASE_COUNT - Set the number of write blocks to be
    pre-erased before writing */
#define  ACMD23 (0x17)
/** SD_SEND_OP_COMD - Sends host capacity support information and
    activates the card's initialization process */
#define  ACMD41 (0x29)
//------------------------------------------------------------------------------
/** status for card in the ready state */
#define  R1_READY_STATE (0x00)
/** status for card in the idle state */
#define  R1_IDLE_STATE (0x01)
/** status bit for illegal command */
#define  R1_ILLEGAL_COMMAND (0x04)
/** start data token for read or write single block*/
#define  DATA_START_BLOCK (0xFE)
/** stop token for write multiple blocks*/
#define  STOP_TRAN_TOKEN (0xFD)
/** start data token for write multiple blocks*/
#define  WRITE_MULTIPLE_TOKEN (0xFC)
/** mask for data response tokens after a write block operation */
#define  DATA_RES_MASK (0x1F)
/** write data accepted token */
#define  DATA_RES_ACCEPTED (0x05)

#endif  // SdInfo_h
