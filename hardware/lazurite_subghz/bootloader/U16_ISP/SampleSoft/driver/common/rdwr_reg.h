/*****************************************************************************
 rdwr_reg.h

 Copyright (C) 2014 LAPIS Semiconductor Co., LTD.
 All rights reserved.

 This software is provided "as is" and any expressed or implied
  warranties, including, but not limited to, the implied warranties of
  merchantability and fitness for a particular purpose are disclaimed.
 LAPIS SEMICONDUCTOR shall not be liable for any direct, indirect,
 consequential or incidental damages arising from using or modifying
 this software.
 You (customer) can modify and use this software in whole or part on
 your own responsibility, only for the purpose of developing the software
 for use with microcontroller manufactured by LAPIS SEMICONDUCTOR.

 History
    2014.03.31 ver.0.90

******************************************************************************/
/**
 * @file    rdwr_reg.h
 * 
 * This file is common definition to read/write SFR registers.
 *
 */

#ifndef _RDWR_REG_H_
#define _RDWR_REG_H_

#ifndef __ICCARM__
#define read_reg8(a)        ((unsigned char)(a))                       /**< 1Byte read  */
#define read_reg16(a)       ((unsigned short)(a))                      /**< 2Byte read  */
#define read_reg32(a)       ((unsigned long)(a))                       /**< 4Byte read  */
#define write_reg8(a,d)     ((a)=(unsigned char)(d))                   /**< 1Byte write */
#define write_reg16(a,d)    ((a)=(unsigned short)(d))                  /**< 2Byte write */
#define write_reg32(a,d)    ((a)=(unsigned long)(d))                   /**< 4Byte write */
#define set_reg8(a,m)       ((a)|=(unsigned char)(m))                  /**< 1Byte(any bit) set   */
#define set_reg16(a,m)      ((a)|=(unsigned short)(m))                 /**< 2Byte(any bit) set   */
#define set_reg32(a,m)      ((a)|=(unsigned long)(m))                  /**< 4Byte(any bit) set   */
#define clear_reg8(a,m)     ((a)&=~(unsigned char)(m))                 /**< 1Byte(any bit) clear */
#define clear_reg16(a,m)    ((a)&=~(unsigned short)(m))                /**< 2Byte(any bit) clear */
#define clear_reg32(a,m)    ((a)&=~(unsigned long)(m))                 /**< 4Byte(any bit) clear */

#define set_bit(n)          ((n) = 1)                                                           /**< bit set                                             */
                                                                                                /**< @note 'n' : A bit-field simbol defined 'ML620***.H' */
#define clear_bit(n)        ((n) = 0)                                                           /**< bit clear                                           */
                                                                                                /**< @note 'n' : A bit-field simbol defined 'ML620***.H' */
#define get_bit(n)          (n)                                                                 /**< bit read                                            */
                                                                                                /**< @note 'n' : A bit-field simbol defined 'ML620***.H' */
#define write_bit(n,d)      ((n) = (d))                                                         /**< bit write                                           */
                                                                                                /**< @note 'n' : A bit-field simbol defined 'ML620***.H' */

#else
#define read_reg8(a)        (*((volatile unsigned char  *)(a)))                                 /* 1Byte read  */
#define read_reg16(a)       (*((volatile unsigned short *)(a)))                                 /* 2Byte read  */
#define read_reg32(a)       (*((volatile unsigned long  *)(a)))                                 /* 4Byte read  */
#define write_reg8(a,d)     (*((volatile unsigned char  *)(a))=(volatile unsigned char)(d))     /* 1Byte write */
#define write_reg16(a,d)    (*((volatile unsigned short *)(a))=(volatile unsigned short)(d))    /* 2Byte write */
#define write_reg32(a,d)    (*((volatile unsigned long  *)(a))=(volatile unsigned long)(d))     /* 4Byte write */
#define set_reg8(a,m)       (*((volatile unsigned char  *)(a))|=(unsigned char)(m))             /* 1Byte(any bit) set */
#define set_reg16(a,m)      (*((volatile unsigned short *)(a))|=(unsigned short)(m))            /* 2Byte(any bit) set */
#define set_reg32(a,m)      (*((volatile unsigned long  *)(a))|=(unsigned long)(m))             /* 4Byte(any bit) set */
#define clear_reg8(a,m)     (*((volatile unsigned char  *)(a))&=~(unsigned char)(m))            /* 1Byte(any bit) clear */
#define clear_reg16(a,m)    (*((volatile unsigned short *)(a))&=~(unsigned short)(m))           /* 2Byte(any bit) clear */
#define clear_reg32(a,m)    (*((volatile unsigned long  *)(a))&=~(unsigned long)(m))            /* 4Byte(any bit) clear */

#define set_bit(n,c)        (set_reg8  (n,c))                                                   /* byte bit set   */
#define clear_bit(n,c)      (clear_reg8(n,c))                                                   /* byte bit clear */
#define get_bit(n,c)        ((read_reg8(n)) & (c)) != 0)                                        /* byte bit read  */
#define write_bit(n,c,d)    (do{ clear_bit(n,c);set_bit(n,d); }while(0))                        /* byte bit write */
#endif /*__ICCARM__*/

#endif /* _RDWR_REG_H_ */
