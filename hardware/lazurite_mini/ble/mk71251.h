#ifndef _MK71251_H_
#define _MK71251_H_
#include <common.h>

struct BLE_LED_PARAM {
	uint8_t cycle;
	uint8_t pin;
	uint8_t on_time;
	uint8_t off_time;
};
struct BLE_LED_CONFIG {
	struct BLE_LED_PARAM sleep;
	struct BLE_LED_PARAM advertise;
	struct BLE_LED_PARAM connect;
};

struct BLE_INTERVAL_PARAM {
	uint16_t slow;
	uint16_t fast;
	uint16_t change_mode_time;
};
struct BLE_CONFIG {
	uint8_t pwr;
	uint8_t pairing;
	struct BLE_INTERVAL_PARAM advertise;
	struct BLE_INTERVAL_PARAM connect;
};
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
	volatile void (*advertise)(bool on);
	void (*setLedConfig)(struct BLE_LED_CONFIG *conf);
	void (*getLedConfig)(struct BLE_LED_CONFIG *conf);
	void (*setConfig)(struct BLE_CONFIG *conf);
	void (*setAdvertiseData)(uint8_t *data, uint8_t len);
	int (*setPasskey)(uint32_t key);
} BLUETOOTH;

extern const BLUETOOTH ble;

#endif
