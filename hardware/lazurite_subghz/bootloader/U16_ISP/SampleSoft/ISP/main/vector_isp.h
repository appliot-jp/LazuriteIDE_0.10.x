/*****************************************************************************
    vector.h

    Copyright (C) 2011 LAPIS Semiconductor Co., Ltd.
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
    2014.04.21  ver.1.00
******************************************************************************/
#ifndef _VECTOR_H_
#define _VECTOR_H_



/*############################################################################*/
/*#                             Vector Table                                 #*/
/*############################################################################*/
#if defined(_ML620Q15X)
 #pragma INTERRUPT _intNullHdr		0x08	2
 #pragma INTERRUPT _intNullHdr		0x0A	1
 #pragma INTERRUPT _intNullHdr		0x0C	1
 #pragma INTERRUPT _intNullHdr		0x10	1
 #pragma INTERRUPT _intNullHdr		0x12	1
 #pragma INTERRUPT _intNullHdr		0x14	1
 #pragma INTERRUPT _intNullHdr		0x16	1
 #pragma INTERRUPT _intNullHdr		0x18	1
 #pragma INTERRUPT _intNullHdr		0x1A	1
 #pragma INTERRUPT _intNullHdr		0x20	1
 #pragma INTERRUPT _intNullHdr		0x24	1
 #pragma INTERRUPT _intNullHdr		0x2E	1
 #pragma INTERRUPT _intNullHdr		0x30	1
 #pragma INTERRUPT _intNullHdr		0x32	1
 #pragma INTERRUPT _intNullHdr		0x34	1
 #pragma INTERRUPT _intNullHdr		0x36	1
 #pragma INTERRUPT _intUA0INT		0x40	1
 #pragma INTERRUPT _intUA1INT		0x42	1
 #pragma INTERRUPT _intNullHdr		0x4E	1
 #pragma INTERRUPT _intNullHdr		0x58	1
 #pragma INTERRUPT _intNullHdr		0x5A	1
 #pragma INTERRUPT _intNullHdr		0x60	1
 #pragma INTERRUPT _intNullHdr		0x62	1
 #pragma INTERRUPT _intNullHdr		0x64	1
 #pragma INTERRUPT _intTBC128Hz		0x6A	1
 #pragma INTERRUPT _intNullHdr		0x6E	1
 #pragma INTERRUPT _intNullHdr		0x76	1
#elif defined(_ML620Q40X_50X)
 #pragma INTERRUPT _intNullHdr	0x08	2
 #pragma INTERRUPT _intNullHdr	0x10	1
 #pragma INTERRUPT _intNullHdr	0x12	1
 #pragma INTERRUPT _intNullHdr	0x14	1
 #pragma INTERRUPT _intNullHdr	0x16	1
 #pragma INTERRUPT _intNullHdr	0x18	1
 #pragma INTERRUPT _intNullHdr	0x1A	1
 #pragma INTERRUPT _intNullHdr	0x1C	1
 #pragma INTERRUPT _intNullHdr	0x1E	1
 #pragma INTERRUPT _intNullHdr	0x20	1
 #pragma INTERRUPT _intNullHdr	0x22	1
 #pragma INTERRUPT _intNullHdr	0x24	1
 #pragma INTERRUPT _intNullHdr	0x26	1
 #pragma INTERRUPT _intUA0INT	0x28	1
 #pragma INTERRUPT _intUA1INT	0x2A	1
 #pragma INTERRUPT _intNullHdr	0x2C	1
 #pragma INTERRUPT _intNullHdr	0x3A	1
 #pragma INTERRUPT _intNullHdr	0x3C	1
 #pragma INTERRUPT _intNullHdr	0x3E	1
 #pragma INTERRUPT _intNullHdr	0x40	1
 #pragma INTERRUPT _intNullHdr	0x42	1
 #pragma INTERRUPT _intNullHdr	0x48	1
 #pragma INTERRUPT _intNullHdr	0x4A	1
 #pragma INTERRUPT _intNullHdr	0x50	1
 #pragma INTERRUPT _intNullHdr	0x52	1
 #pragma INTERRUPT _intNullHdr	0x54	1
 #pragma INTERRUPT _intNullHdr	0x56	1
 #pragma INTERRUPT _intNullHdr	0x58	1
 #pragma INTERRUPT _intNullHdr	0x5A	1
 #pragma INTERRUPT _intNullHdr	0x5C	1
 #pragma INTERRUPT _intNullHdr	0x5E	1
 #pragma INTERRUPT _intNullHdr	0x60	1
 #pragma INTERRUPT _intNullHdr	0x62	1
 #pragma INTERRUPT _intNullHdr	0x64	1
 #pragma INTERRUPT _intNullHdr	0x66	1
 #pragma INTERRUPT _intTBC128Hz	0x70	1
 #pragma INTERRUPT _intNullHdr	0x72	1
 #pragma INTERRUPT _intNullHdr	0x74	1
#endif


#endif	/* _VECTOR_H_ */
