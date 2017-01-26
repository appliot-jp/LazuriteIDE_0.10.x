/* FILE NAME: Sd2Card.c
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

#include "lazurite.h"
#include "Sd2Card.h"
#include "SPI.h"
#include "wdt.h"

#ifndef SDCARD_SPI
#define SDCARD_SPI SPI
#endif

// functions for hardware SPI
//------------------------------------------------------------------------------
/** Send a byte to the card */
static void Sd2Card_spiSend(uint8_t b) {
  SDCARD_SPI.transfer(b);
}
//------------------------------------------------------------------------------
/** Receive a byte from the card */
static uint8_t Sd2Card_spiRec(void) {
  return SDCARD_SPI.transfer(0xFF);
}
//------------------------------------------------------------------------------
uint8_t Sd2Card_cardAcmd(st_Sd2Card_v *v, uint8_t cmd, uint32_t arg) {
  Sd2Card_cardCommand(v, CMD55, (uint32_t)0);
  return Sd2Card_cardCommand(v, cmd, arg);
}
//------------------------------------------------------------------------------
// send command and return error code.  Return zero for OK
uint8_t Sd2Card_cardCommand(st_Sd2Card_v *v, uint8_t cmd, uint32_t arg) {
  int8_t crc = 0xFF;
  int8_t s, i;

  // end read if in partialBlockRead mode
  Sd2Card_readEnd(v);

  // select card
  Sd2Card_chipSelectLow(v);

  // wait up to 300 ms if busy
  Sd2Card_waitNotBusy(300);

  // send command
  Sd2Card_spiSend((uint8_t)(cmd | 0x40));

  // send argument
  for (s = 24; s >= 0; s -= 8) Sd2Card_spiSend((uint8_t)(arg >> s));

  // send CRC
  if (cmd == CMD0) crc = 0x95;  // correct crc for CMD0 with arg 0
  if (cmd == CMD8) crc = 0x87;  // correct crc for CMD8 with arg 0x1AA
  Sd2Card_spiSend(crc);

  // wait for response
  for (i = 0; ((v->status_ = Sd2Card_spiRec()) & 0x80) && (i != 0xFF); i++);

  return v->status_;
}

//------------------------------------------------------------------------------
#pragma INLINE Sd2Card_chipSelectHigh
void Sd2Card_chipSelectHigh(st_Sd2Card_v *v) {
  digitalWrite(v->chipSelectPin_, HIGH);
}
//------------------------------------------------------------------------------
#pragma INLINE Sd2Card_chipSelectLow
void Sd2Card_chipSelectLow(st_Sd2Card_v *v) {
  digitalWrite(v->chipSelectPin_, LOW);
}
//------------------------------------------------------------------------------
#pragma INLINE Sd2Card_error
void Sd2Card_error(st_Sd2Card_v *v, uint8_t code) {v->errorCode_ = code;}
//------------------------------------------------------------------------------
/**
 * Initialize an SD flash memory card.
 *
 * \param[in] sckRateID SPI clock rate selector. See setSckRate().
 * \param[in] chipSelectPin SD chip select pin number.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.  The reason for failure
 * can be determined by calling errorCode() and errorData().
 */
boolean Sd2Card_init(st_Sd2Card_v *v, uint8_t sckRateID, uint8_t chipSelectPin) {
  uint8_t i;
  uint16_t t0;
  uint32_t arg;

  v->errorCode_ = v->inBlock_ = v->partialBlockRead_ = v->type_ = 0;
  v->chipSelectPin_ = chipSelectPin;
  // 16-bit init start time allows over a minute
  t0 = (uint16_t)millis();

  // set pin modes
  pinMode(v->chipSelectPin_, OUTPUT);
  Sd2Card_chipSelectHigh(v);

  SDCARD_SPI.setClockDivider(sckRateID);
  SDCARD_SPI.setBitOrder(SPI_MSBFIRST);
  SDCARD_SPI.setDataMode(SPI_MODE0);
  SDCARD_SPI.begin();

  Sd2Card_chipSelectLow(v);

  // command to go idle in SPI mode
  while ((v->status_ = Sd2Card_cardCommand(v, CMD0, (uint32_t)0)) != R1_IDLE_STATE) {
    if (((uint16_t)(millis() - t0)) > SD_INIT_TIMEOUT) {
      Sd2Card_error(v, SD_CARD_ERROR_CMD0);
      goto fail;
    }
	wdt_clear();
  }
  // check SD version
  if ((Sd2Card_cardCommand(v, CMD8, (uint32_t)0x1AA) & R1_ILLEGAL_COMMAND)) {
    Sd2Card_type(v, SD_CARD_TYPE_SD1);
  } else {
    // only need last byte of r7 response
    for (i = 0; i < 4; i++) v->status_ = Sd2Card_spiRec();
    if (v->status_ != 0xAA) {
      Sd2Card_error(v, SD_CARD_ERROR_CMD8);
      goto fail;
    }
    Sd2Card_type(v, SD_CARD_TYPE_SD2);
  }
  // initialize card and send host supports SDHC if SD2
  arg = v->type_ == SD_CARD_TYPE_SD2 ? 0x40000000 : 0;

  while ((v->status_ = Sd2Card_cardAcmd(v, ACMD41, arg)) != R1_READY_STATE) {
    // check for timeout
    if (((uint16_t)(millis() - t0)) > SD_INIT_TIMEOUT) {
      Sd2Card_error(v, SD_CARD_ERROR_ACMD41);
      goto fail;
    }
	wdt_clear();
  }
  // if SD2 read OCR register to check for SDHC card
  if (v->type_ == SD_CARD_TYPE_SD2) {
    if (Sd2Card_cardCommand(v, CMD58, (uint32_t)0)) {
      Sd2Card_error(v, SD_CARD_ERROR_CMD58);
      goto fail;
    }
    if ((Sd2Card_spiRec() & 0xC0) == 0xC0) Sd2Card_type(v, SD_CARD_TYPE_SDHC);
    // discard rest of ocr - contains allowed voltage range
    for (i = 0; i < 3; i++) Sd2Card_spiRec();
  }
  wdt_clear();
  Sd2Card_chipSelectHigh(v);
  return true;

 fail:
  wdt_clear();
  Sd2Card_chipSelectHigh(v);
  return false;
}
//------------------------------------------------------------------------------
/**
 * Read a 512 byte block from an SD card device.
 *
 * \param[in] block Logical block to be read.
 * \param[out] dst Pointer to the location that will receive the data.

 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
uint8_t Sd2Card_readBlock(st_Sd2Card_v *v, uint32_t block, uint8_t* dst) {
  return Sd2Card_readData(v, block, 0, 512, dst);
}
//------------------------------------------------------------------------------
/**
 * Read part of a 512 byte block from an SD card.
 *
 * \param[in] block Logical block to be read.
 * \param[in] offset Number of bytes to skip at start of block
 * \param[out] dst Pointer to the location that will receive the data.
 * \param[in] count Number of bytes to read
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
uint8_t Sd2Card_readData(st_Sd2Card_v *v, uint32_t block,
        uint16_t offset, uint16_t count, uint8_t* dst) {
  uint16_t i;

  if (count == 0) return true;
  if ((count + offset) > 512) {
    goto fail;
  }
  if (!v->inBlock_ || block != v->block_ || offset < v->offset_) {
    v->block_ = block;
    // use address if not SDHC card
    if (v->type_ != SD_CARD_TYPE_SDHC) block <<= 9;
    if (Sd2Card_cardCommand(v, CMD17, block)) {
      Sd2Card_error(v, SD_CARD_ERROR_CMD17);
      goto fail;
    }
    if (!Sd2Card_waitStartBlock(v)) {
      goto fail;
    }
    v->offset_ = 0;
    v->inBlock_ = 1;
  }

  // skip data before offset
  for (;v->offset_ < offset; v->offset_++) {
    Sd2Card_spiRec();
  }
  // transfer data
  for (i = 0; i < count; i++) {
    dst[i] = Sd2Card_spiRec();
  }

  v->offset_ += count;
  if (!v->partialBlockRead_ || v->offset_ >= 512) {
    // read rest of data, checksum and set chip select high
    Sd2Card_readEnd(v);
  }
  return true;

 fail:
  Sd2Card_chipSelectHigh(v);
  return false;
}
//------------------------------------------------------------------------------
/** Skip remaining data in a block when in partial block read mode. */
void Sd2Card_readEnd(st_Sd2Card_v *v) {
  if (v->inBlock_) {
      // skip data and crc
    while (v->offset_++ < 514) Sd2Card_spiRec();
    Sd2Card_chipSelectHigh(v);
    v->inBlock_ = 0;
  }
}
//------------------------------------------------------------------------------
  /** Return the card type: SD V1, SD V2 or SDHC */
//  uint8_t Sd2Card_type(st_Sd2Card_v *v) {return v->type_;}
//------------------------------------------------------------------------------
#pragma INLINE Sd2Card_type
void Sd2Card_type(st_Sd2Card_v *v, uint8_t value) {v->type_ = value;}
//------------------------------------------------------------------------------
// wait for card to go not busy
uint8_t Sd2Card_waitNotBusy(uint16_t timeoutMillis) {
  uint16_t t0 = (uint16_t)millis();
  do {
    if (Sd2Card_spiRec() == 0xFF) return true;
	wdt_clear();
  }
  while (((uint16_t)millis() - t0) < timeoutMillis);
  return false;
}
//------------------------------------------------------------------------------
/** Wait for start block token */
uint8_t Sd2Card_waitStartBlock(st_Sd2Card_v *v) {
  uint16_t t0 = (uint16_t)millis();
  while ((v->status_ = Sd2Card_spiRec()) == 0xFF) {
    if (((uint16_t)millis() - t0) > SD_READ_TIMEOUT) {
      Sd2Card_error(v, SD_CARD_ERROR_READ_TIMEOUT);
      goto fail;
    }
	wdt_clear();
  }
  if (v->status_ != DATA_START_BLOCK) {
    Sd2Card_error(v, SD_CARD_ERROR_READ);
    goto fail;
  }
  return true;

 fail:
  Sd2Card_chipSelectHigh(v);
  return false;
}
//------------------------------------------------------------------------------
/**
 * Writes a 512 byte block to an SD card.
 *
 * \param[in] blockNumber Logical block to be written.
 * \param[in] src Pointer to the location of the data to be written.
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
uint8_t Sd2Card_writeBlock(st_Sd2Card_v *v, uint32_t blockNumber, const uint8_t* src) {
#if SD_PROTECT_BLOCK_ZERO
  // don't allow write to first block
  if (blockNumber == 0) {
    Sd2Card_error(v, SD_CARD_ERROR_WRITE_BLOCK_ZERO);
    goto fail;
  }
#endif  // SD_PROTECT_BLOCK_ZERO

  // use address if not SDHC card
  if (v->type_ != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (Sd2Card_cardCommand(v, CMD24, blockNumber)) {
    Sd2Card_error(v, SD_CARD_ERROR_CMD24);
    goto fail;
  }
  if (!Sd2Card_writeData(v, DATA_START_BLOCK, src)) goto fail;

  // wait for flash programming to complete
  if (!Sd2Card_waitNotBusy(SD_WRITE_TIMEOUT)) {
    Sd2Card_error(v, SD_CARD_ERROR_WRITE_TIMEOUT);
    goto fail;
  }
  // response is r2 so get and check two bytes for nonzero
  if (Sd2Card_cardCommand(v, CMD13, (uint32_t)0) || Sd2Card_spiRec()) {
    Sd2Card_error(v, SD_CARD_ERROR_WRITE_PROGRAMMING);
    goto fail;
  }
  Sd2Card_chipSelectHigh(v);
  return true;

 fail:
  Sd2Card_chipSelectHigh(v);
  return false;
}
//------------------------------------------------------------------------------
// send one block of data for write block or write multiple blocks
uint8_t Sd2Card_writeData(st_Sd2Card_v *v, uint8_t token, const uint8_t* src) {
  uint16_t i;

  Sd2Card_spiSend(token);
  for (i = 0; i < 512; i++) {
    Sd2Card_spiSend(src[i]);
  }

  Sd2Card_spiSend(0xff);  // dummy crc
  Sd2Card_spiSend(0xff);  // dummy crc

  v->status_ = Sd2Card_spiRec();
  if ((v->status_ & DATA_RES_MASK) != DATA_RES_ACCEPTED) {
    Sd2Card_error(v, SD_CARD_ERROR_WRITE);
    Sd2Card_chipSelectHigh(v);
    return false;
  }
  return true;
}
