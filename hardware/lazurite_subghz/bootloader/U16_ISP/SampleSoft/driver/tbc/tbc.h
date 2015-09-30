/*****************************************************************************
    tbc.h

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
#ifndef _TBC_H_
#define _TBC_H_


#define TBC_LTBINT_128	(0)
#define TBC_LTBINT_64	(1)
#define TBC_LTBINT_32	(2)
#define TBC_LTBINT_16	(3)
#define TBC_LTBINT_8	(4)
#define TBC_LTBINT_4	(5)
#define TBC_LTBINT_2	(6)
#define TBC_LTBINT_1	(7)


/*############################################################################*/
/*#                                  API                                     #*/
/*############################################################################*/
void tbc_setInt0(unsigned char ltb);
void tbc_wait15ms(void);


#endif	/* _TBC_H_ */
