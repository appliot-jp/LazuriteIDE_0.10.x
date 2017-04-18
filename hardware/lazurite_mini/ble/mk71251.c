

#define BLE_RSTB			7		// P11
#define BLE_GPIO3			2		// P02
#define BLE_GPIO2			32		// P54
#define BLE_GPIO1			33		// P55
#define BLE_GPIO0			21		// P41
#define PWR_LED				11		// P55

#include <driver_uart.h>
#include <driver_irq.h>
#include <driver_gpio.h>
#include <driver_timer.h>
#include <serial.h>
#include <lp_manage.h>
#include <print.h>
#include <string.h>
#include <wdt.h>

static uint8_t ble_rx_buf[128];
static FIFO_CTRL ble_rx_fifo = {
	sizeof(ble_rx_buf),
	ble_rx_buf,
	0,
	0
};
static int mode = 0;
static uint8_t check_index;
static const char ble_connect[] = "\r\nCONNECT\r\n";
static const char ble_nocarrier[] = "\r\nNO CARRIER\r\n";


void ble_timer_func(void) {
	static bool ledstate = LOW;
	drv_digitalWrite(PWR_LED,ledstate);
	ledstate = ~ledstate;
}

static void ble_rx_callback() {
	char data;
	data = read_reg8(UA0BUF);
	if(mode == 3) {
		uart_fifo_in(&ble_rx_fifo, data);
		if(check_index < (sizeof(ble_nocarrier)-1)) {
			if (data == ble_nocarrier[check_index]) {
				check_index++;
			} else if (data == ble_nocarrier[0]){
				check_index = 1;
			} else {
				check_index = 0;				
			}
			if(check_index >= (sizeof(ble_nocarrier)-1)) {
				drv_digitalWrite(BLE_GPIO1,HIGH);
				mode = 1;
				check_index = 0;
				
			}
		}
	} else {
		uart_fifo_in(&ble_rx_fifo, data);
		if(check_index < (sizeof(ble_connect)-1)) {
			if (data == ble_connect[check_index]) {
				check_index++;
			} else if(data == ble_connect[0]){
				check_index = 1;
			}
			if(check_index >= (sizeof(ble_connect)-1)) {
				mode = 3;
				check_index = 0;
			}
		}
	}
	return;
}
static void ble_begin() {
	drv_pinMode(BLE_RSTB,OUTPUT);
	drv_digitalWrite(BLE_RSTB,LOW);
  
	drv_pinMode(BLE_GPIO2,INPUT);
	drv_pinMode(BLE_GPIO0,INPUT);

	drv_pinMode(BLE_GPIO1,OUTPUT);
	drv_digitalWrite(BLE_GPIO1,HIGH);

	drv_pinMode(BLE_GPIO3,OUTPUT);
	drv_digitalWrite(BLE_GPIO3,LOW);

	drv_pinMode(PWR_LED,OUTPUT);
	drv_digitalWrite(PWR_LED,LOW);

	delay(20);
	
	check_index = 0;

	uart_fifo_init(&ble_rx_fifo);

	uart_gpio_init(0);
	uart_begin(57600,ble_rx_callback,NULL);

	// release reset
	drv_digitalWrite(BLE_RSTB,HIGH);

	// waiting callibration
	while(drv_digitalRead(BLE_GPIO0) == HIGH) {
	}
	delay(1);
	mode = 1;
}
static void ble_end(void)
{
	uart_gpio_end(0);
	uart_end();
}

static int ble_rx_read(void)
{
	int res;
	dis_interrupts(DI_UART);
	res = uart_fifo_out(&ble_rx_fifo);		// get data
	enb_interrupts(DI_UART);

	return res;
}
static int ble_peek(void)
{
	return uart_fifo_out_peek(&ble_rx_fifo);
}

static void ble_flush(void)
{
	uart_fifo_init(&ble_rx_fifo);
	return;
}
static size_t ble_print(char* data)
{
	int n = 0;

	while(data[n] != NULL)
	{
		if(uart_tx_write(data[n]) == 1)
		{
			n++;
		}
		else
		{
			wdt_clear();
			lp_setHaltMode();
		}
	}
	return strlen(data);
}
static size_t ble_println(char* data)
{
	ble_print(data);
	ble_print("\r\n");
	return (strlen(data)+2);
}
static size_t ble_print_long(long data, UCHAR fmt)
{
	char tmp_c[33];
	printNumber(tmp_c,data,fmt);
	ble_print(tmp_c);
	return strlen(tmp_c);
}
static size_t ble_println_long(long data, UCHAR fmt)
{
	char tmp_c[33];							// 33 is maximum length of charactor
	printNumber(tmp_c, data,fmt);			// convert long data to charactor
	ble_println(tmp_c);		// send data though uart
	return strlen(tmp_c);
}

static size_t ble_print_double(double data, UCHAR digit)
{
	char tmp_c[33];
//	unsigned char n;
	printFloat(tmp_c,data,digit);
	ble_print(tmp_c);
	return strlen(tmp_c);
}
static size_t ble_println_double(double data, UCHAR digit)
{
	char tmp_c[33];
	printFloat(tmp_c, data,digit);
	ble_println(tmp_c);
	return strlen(tmp_c);
}
static int ble_rx_available(void)
{
	return ble_rx_fifo.length;
}
static size_t ble_tx_write(char* data, size_t quantity)
{
	size_t n;
	
	for(n=0;n<quantity;n++)
	{
		if(uart_tx_write(data[n]) != 1)
		{
			break;
		}
	}
	
	return n;
}

static volatile int ble_getStatus(void) {
	return mode;
}

static volatile void ble_advertising(bool on) {
	if(on) {
		drv_digitalWrite(BLE_GPIO1,LOW);
		// waitign cts
		while(drv_digitalRead(BLE_GPIO2) == HIGH) {
		}
		ble_print("ATD\r");
		mode = 2;
	} else {
		drv_digitalWrite(BLE_GPIO1,HIGH);
		mode = 1;
	}
	return;
}

const MK71251 mk71251 = {
	ble_begin,
	ble_end,
	ble_rx_available,
	ble_rx_read,
	ble_peek,
	ble_flush,
	ble_print,
	ble_println,
	ble_print_long,
	ble_println_long,
	ble_print_double,
	ble_println_double,
	ble_tx_write,
	uart_tx_write,
	uart_tx_available,
	ble_getStatus,
	ble_advertising
};
