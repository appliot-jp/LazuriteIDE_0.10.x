/*****************************************************************************
    mcu.h

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
#ifndef _MCU_H_
#define _MCU_H_

#if defined(_ML620Q151)
 #define _ML620Q15X
 #include <ML620151.h>
#elif defined(_ML620Q152)
 #define _ML620Q15X
 #include <ML620152.h>
#elif defined(_ML620Q153)
 #define _ML620Q15X
 #include <ML620153.h>
#elif defined(_ML620Q154)
 #define _ML620Q15X
 #include <ML620154.h>
#elif defined(_ML620Q155)
 #define _ML620Q15X
 #include <ML620155.h>
#elif defined(_ML620Q156)
 #define _ML620Q15X
 #include <ML620156.h>
#elif defined(_ML620Q157)
 #define _ML620Q15X
 #include <ML620157.h>
#elif defined(_ML620Q158)
 #define _ML620Q15X
 #include <ML620158.h>
#elif defined(_ML620Q159)
 #define _ML620Q15X
 #include <ML620159.h>
#elif defined(_ML620Q404)
 #define _ML620Q40X_50X
 #include <ML620404.h>
#elif defined(_ML620Q503)
 #define _ML620Q40X_50X
 #include <ML620503.h>
#elif defined(_ML620Q504)
 #define _ML620Q40X_50X
 #include <ML620504.h>
#else
 #error "Unknown target MCU"
#endif

#endif /* _MCU_H */
