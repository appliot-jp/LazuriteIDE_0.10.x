#ifndef _MK71251_H_
#define _MK71251_H_

typedef struct {
	void (*begin)(void);
	void (*end)(void);
	int (*available)(void);
	int (*read)(void);
	int (*peek)(void);
	void (*flush)(void);
	volatile size_t (*print)(char* data);
	volatile size_t (*println)(char* data);
	volatile size_t (*print_long)(long data, UCHAR fmt);
	volatile size_t (*println_long)(long data, UCHAR fmt);
	volatile size_t (*print_double)(double data, UCHAR digit);
	volatile size_t (*println_double)(double data, UCHAR digit);
	volatile size_t (*write)(char* data, size_t quantity);
	volatile size_t (*write_byte)(char data);
	int (*tx_available)(void);								// add 
	volatile int (*getStatus)(void);
	volatile void (*advertising)(bool on);
} MK71251;

extern const MK71251 mk71251;

#endif
