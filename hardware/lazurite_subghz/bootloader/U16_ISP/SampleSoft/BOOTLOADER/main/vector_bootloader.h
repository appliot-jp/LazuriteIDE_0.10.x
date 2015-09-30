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
 #pragma INTERRUPT _intNullHdr	0x08	2
 #pragma INTERRUPT _intNullHdr	0x0C	2
#elif defined(_ML620Q40X_50X)
 #pragma INTERRUPT _intNullHdr	0x08	2
#endif


#endif	/* _VECTOR_H_ */
