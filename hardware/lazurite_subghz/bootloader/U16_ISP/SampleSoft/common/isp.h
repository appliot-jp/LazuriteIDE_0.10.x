/*****************************************************************************
    isp.h

    Copyright (C) 2014 LAPIS Semiconductor Co., Ltd.
    All rights reserved.

    This software is provided "as is" and any expressed or implied
    warranties, including, but not limited to, the implied warranties of
    merchantability and fitness for a particular purpose are disclaimed.
    LAPIS Semiconductor shall not be liable for any direct, indirect,
    consequential or incidental damages arising from using or modifying
    this software.
    You (customer) can modify and use this software in whole or part on
    your own responsibility, only for the purpose of developing the software
    for use with microcontroller manufactured by LAPIS Semiconductor.

    History
    2014.04.14  ver.1.00
******************************************************************************/
#ifndef _ISP_H_
#define _ISP_H_



/*############################################################################*/
/*#                                  Macro                                   #*/
/*############################################################################*/
#if defined(_ML620Q15X)
 #if (defined(_ML620Q151) || defined(_ML620Q154) || defined(_ML620Q157))
  #define FLASH_SIZE					( 0x08000 )
  #define ISP_AREA_START_ADDR			( 0x07000 )
  #define TEST_AREA_END_ADDR			( 0x07FFF )
  #define FLASH_MIN_SECTOR_ERASE_ADRS	( 0x06000 )
 #elif (defined(_ML620Q152) || defined(_ML620Q155) || defined(_ML620Q158))
  #define FLASH_SIZE					( 0x0C000 )
  #define ISP_AREA_START_ADDR			( 0x0B000 )
  #define TEST_AREA_END_ADDR			( 0x0BFFF )
  #define FLASH_MIN_SECTOR_ERASE_ADRS	( 0x0A000 )
 #elif (defined(_ML620Q153) || defined(_ML620Q156) || defined(_ML620Q159))
  #define FLASH_SIZE					( 0x10000 )
  #define ISP_AREA_START_ADDR			( 0x0F000 )
  #define TEST_AREA_END_ADDR			( 0x0FFFF )
  #define FLASH_MIN_SECTOR_ERASE_ADRS	( 0x0E000 )
 #endif
 #define FLASH_BLOCK_SIZE				( 8192 )
 #define FLASH_SECTOR_SIZE				( 1024 )
#elif defined(_ML620Q40X_50X)
 #if defined(_ML620Q503)
  #define FLASH_SIZE					( 0x08000 )
  #define ISP_AREA_START_ADDR			( 0x07000 )
  #define TEST_AREA_END_ADDR			( 0x07FFF )
  #define FLASH_MIN_SECTOR_ERASE_ADRS	( 0x06000 )
 #elif (defined(_ML620Q404) || defined(_ML620Q504))
  #define FLASH_SIZE					( 0x10000 )
  #define ISP_AREA_START_ADDR			( 0x0F000 )
  #define TEST_AREA_END_ADDR			( 0x0FFFF )
  #define FLASH_MIN_SECTOR_ERASE_ADRS	( 0x0E000 )
 #elif (defined(_ML620Q406) || defined(_ML620Q506))
  #define FLASH_SIZE					( 0x20000 )
  #define ISP_AREA_START_ADDR			( 0x0F000 )
  #define TEST_AREA_END_ADDR			( 0x0FFFF )
  #define FLASH_MIN_SECTOR_ERASE_ADRS	( 0x0E000 )
 #endif
 #define FLASH_BLOCK_SIZE				( 8192 )
 #define FLASH_SECTOR_SIZE				( 1024 )
#endif

#ifdef USE_DATA_FALSH
 #define USER_AREA_ADDR				(0x70000)	/* for debug */
#else
 #define USER_AREA_ADDR				(0x00000)
#endif

#ifdef _ML620Q40X_50X
 #define WDT_TIMEOUT		(0x82)
#else
 #define WDT_TIMEOUT		(0x02)
#endif


#endif	/* _ISP_H_ */
