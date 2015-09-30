
/* FILE NAME: rdwr_reg.h
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
