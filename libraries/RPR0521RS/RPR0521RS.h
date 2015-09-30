#ifndef _RPR0521RS_H_
#define _RPR0521RS_H_

#include "lazurite.h"

#define RPR0521RS_DEVICE_ADDRESS      (0x38)    // 7bit Addrss
#define RPR0521RS_PART_ID_VAL         (0x0A)
#define RPR0521RS_MANUFACT_ID_VAL     (0xE0)

#define RPR0521RS_SYSTEM_CONTROL      (0x40)
#define RPR0521RS_MODE_CONTROL        (0x41)
#define RPR0521RS_ALS_PS_CONTROL      (0x42)
#define RPR0521RS_PS_CONTROL          (0x43)
#define RPR0521RS_PS_DATA_LSB         (0x44)
#define RPR0521RS_ALS_DATA0_LSB       (0x46)
#define RPR0521RS_MANUFACT_ID         (0x92)

// ƒNƒ‰ƒX‚Ì’è‹`
typedef struct {
	byte (*init)(void) ;
    byte (*get_rawpsalsval)(unsigned char *data);
    byte (*get_psalsval)(unsigned short *ps, float *als);
    float (*convert_lux)(unsigned short *data);
    byte (*write)(unsigned char memory_address, unsigned char *data, unsigned char size);
    byte (*read)(unsigned char memory_address, unsigned char *data, int size); 
} t_RPR0521RS;

extern const t_RPR0521RS rpr0521rs;

#endif // _RPR0521RS_H_
