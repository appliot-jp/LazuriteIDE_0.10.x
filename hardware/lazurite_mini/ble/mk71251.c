

#define BLE_RSTB			7		// P11
#define BLE_GPIO3			2		// P02
#define BLE_GPIO2			32		// P54
#define BLE_GPIO1			33		// P55
#define BLE_GPIO0			21		// P41
#define PWR_LED				20		// P23

#include <driver_uart.h>
#include <driver_irq.h>
#include <driver_gpio.h>
#include <driver_timer.h>
#include <serial.h>
#include <lp_manage.h>
#include <print.h>
#include <string.h>
#include <wdt.h>

static uint8_t ble_tx_mode;				// 0 = normal, 1=cmd mode
static uint8_t ble_cmd_buf_index;		// cmd buffer index
static uint8_t ble_cmd_buf[40];			// data buffer for cmd mode

static uint8_t ble_rx_buf[128];
static uint8_t ble_tx_buf[128];
static char ble_tx_flag;							// true = send, false = not send
static char ble_tx_sending=false;
static char led_status;
static struct BLE_LED_PARAM *current_led_param;
static struct BLE_LED_CONFIG ble_led_conf = {
	{				// sleep
		3,			// cycle
		PWR_LED,	// pin
		10,			// on time
		100			// off time
	},
	{				// advertise
		1,			// cycle
		PWR_LED,	// pin
		10,			// on time
		100			// off time
	},
	{				// connect
		0,			// cycle
		PWR_LED,	// pin
		10,			// on time
		100			// off time
	}
};

static FIFO_CTRL ble_tx_fifo = {
	sizeof(ble_tx_buf),
	ble_tx_buf,
	0,
	0
};
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

static void ble_led_func(void) {
	if(led_status==0) digitalWrite(current_led_param->pin,HIGH);
	else {
		led_status--;
		if(led_status&0x01) {
		    timer_8bit_set((uint8_t)TM_CH4, 0xb0, current_led_param->on_time, ble_led_func);
		    timer_8bit_start(TM_CH4);
			digitalWrite(current_led_param->pin,LOW);
		} else {
		    timer_8bit_set((uint8_t)TM_CH4, 0xb0, current_led_param->off_time, ble_led_func);
		    timer_8bit_start(TM_CH4);
			digitalWrite(current_led_param->pin,HIGH);
		}
	}
}

void ble_timer_func(void) {
	switch(mode) {
	case 1:
		current_led_param=&ble_led_conf.sleep;
		break;
	case 2:
		current_led_param=&ble_led_conf.advertise;
		break;
	case 3:
		current_led_param=&ble_led_conf.connect;
		break;
	default:
		return;
	}
	if(current_led_param->cycle == 0) digitalWrite(current_led_param->pin,HIGH);
	else digitalWrite(current_led_param->pin,LOW);
    timer_8bit_set(TM_CH4, 0xb0, current_led_param->on_time, ble_led_func);
    timer_8bit_start(TM_CH4);
    led_status = current_led_param->cycle;
}

void ble_tx_isr(void)
{
	int data;
	if(ble_tx_flag == false)
	{
		ble_tx_sending = false;
	}
	data = uart_fifo_out(&ble_tx_fifo);
	if(data < 0)
	{
		if(mode != 3) {
			if(ble_tx_sending==false) {
				drv_digitalWrite(BLE_GPIO1,HIGH);
			}
		}
		ble_tx_flag = false;
		return;
	}
	write_reg8(UA1BUF, (char)data);
	return;
}

static void ble_rx_isr() {
	char data;
	int i=0;
	
	data = read_reg8(UA0BUF);
	if(mode == 3) {
//		uart_fifo_in(&ble_rx_fifo, data);
		if(check_index < (sizeof(ble_nocarrier)-1)) {
			if (data == ble_nocarrier[check_index]) {
				check_index++;
			} else if (data == ble_nocarrier[0]){
				check_index = 1;
			} else {
				while(check_index>0) {
					uart_fifo_in(&ble_rx_fifo, ble_nocarrier[i]);
					i++;
					check_index--;
				};
				uart_fifo_in(&ble_rx_fifo, data);
				check_index = 0;				
			}
			if(check_index >= (sizeof(ble_nocarrier)-1)) {
				drv_digitalWrite(BLE_GPIO1,HIGH);
				mode = 1;
				check_index = 0;
				
			}
		}
	} else {
//		uart_fifo_in(&ble_rx_fifo, data);
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
static void ble_begin(void) {
	drv_pinMode(BLE_RSTB,OUTPUT);
	drv_digitalWrite(BLE_RSTB,LOW);
  
	drv_pinMode(BLE_GPIO2,INPUT);
	drv_pinMode(BLE_GPIO0,INPUT);

	drv_pinMode(BLE_GPIO1,OUTPUT);
	drv_digitalWrite(BLE_GPIO1,HIGH);

	drv_pinMode(BLE_GPIO3,OUTPUT);
	drv_digitalWrite(BLE_GPIO3,LOW);

	pinMode(PWR_LED,OUTPUT);
	digitalWrite(PWR_LED,LOW);

	delay(20);
	
	check_index = 0;

	uart_fifo_init(&ble_rx_fifo);
	uart_fifo_init(&ble_tx_fifo);

	uart_gpio_init(0);
	uart_begin(57600,ble_rx_isr,ble_tx_isr);

	// release reset
	drv_digitalWrite(BLE_RSTB,HIGH);
	delay(100);
	// waiting callibration
	while(drv_digitalRead(BLE_GPIO0) == HIGH) {
	}
	delay(10);
	mode = 1;
	digitalWrite(PWR_LED,HIGH);
	timer_8bit_set(TM_CH4, 0xb0, 10, ble_led_func);
	ble_tx_flag = false;
	ble_tx_sending = false;
}
static void ble_end(void)
{
	uart_gpio_end(0);
	uart_end();
	mode = 0;
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

static size_t ble_print_cmd(char* data)
{
	int n = 0;

	while(data[n] != NULL)
	{
		if(ble_tx_write_byte(data[n]) == 1)
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
static size_t ble_print(char* data)
{
	int n = 0;
	if(mode != 3) return(mode*-1);
	while(data[n] != NULL)
	{
		if(ble_tx_write_byte(data[n]) == 1)
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
	if(mode != 3) return(mode*-1);
	ble_print(data);
	ble_print("\r\n");
	return (strlen(data)+2);
}
static size_t ble_print_long(long data, UCHAR fmt)
{
	char tmp_c[33];
	if(mode != 3) return(mode*-1);
	printNumber(tmp_c,data,fmt);
	ble_print(tmp_c);
	return strlen(tmp_c);
}
static size_t ble_println_long(long data, UCHAR fmt)
{
	char tmp_c[33];							// 33 is maximum length of charactor
	if(mode != 3) return(mode*-1);
	printNumber(tmp_c, data,fmt);			// convert long data to charactor
	ble_println(tmp_c);		// send data though uart
	return strlen(tmp_c);
}

static size_t ble_print_double(double data, UCHAR digit)
{
	char tmp_c[33];
//	unsigned char n;
	if(mode != 3) return(mode*-1);
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
size_t ble_tx_write_byte(char data)
{
	size_t res;
	
//	__DI();										// disenable interrupt
	dis_interrupts(DI_UART);
	res = uart_fifo_in(&ble_tx_fifo,data);
	
	if(res == 1)
	{
		if(ble_tx_flag == false)
		{
			// waiting CTS flag
			drv_digitalWrite(BLE_GPIO1,LOW);
			while(drv_digitalRead(BLE_GPIO2) == HIGH) {
			}
			data = (char)uart_fifo_out(&ble_tx_fifo);
			write_reg8(UA1BUF,data);
			ble_tx_flag = true;
			ble_tx_sending = true;
		}
	}
//	__EI();										// enable interrupt
	enb_interrupts(DI_UART);
	return res;
}
static size_t ble_tx_write(char* data, size_t quantity)
{
	size_t n;
	
	for(n=0;n<quantity;n++)
	{
		if(ble_tx_write_byte(data[n]) != 1)
		{
			break;
		}
	}
	
	return n;
}
static int ble_tx_available(void)
{
	return (ble_tx_fifo.max_length-ble_tx_fifo.length);
}


static volatile int ble_getStatus(void) {
	return mode;
}

static volatile void ble_advertise(bool on) {
	switch(mode) {
	case 0:
		break;
	case 1:
		if(on) {
			ble_print_cmd("ATD\r");
			mode = 2;
		}
		break;
	case 2:
		if(!on) {
			ble_print_cmd("\r");
			mode = 1;
		}
		break;
	case 3:
		if(!on) {
			ble_print_cmd("+++AT\rATH");
			mode = 1;
		}
		break;
	default:
		break;
	}
	return;
}

static void ble_setLedConfig(struct BLE_LED_CONFIG *conf)
{
	memcpy(&ble_led_conf,conf,sizeof(struct BLE_LED_CONFIG));
}
static void ble_getLedConfig(struct BLE_LED_CONFIG *conf)
{
	memcpy(conf,&ble_led_conf,sizeof(struct BLE_LED_CONFIG));
}
static void ble_setConfig(struct BLE_CONFIG *conf)
{
	return;
}
static void ble_setAdvertiseData(uint8_t *data, uint8_t len)
{
	return;
}
static int ble_setPasskey(uint32_t key) {
	return 0;
}

const BLUETOOTH ble = {
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
	ble_tx_write_byte,
	ble_tx_available,
	ble_getStatus,
	ble_advertise,
	ble_setLedConfig,
	ble_getLedConfig,
	ble_setConfig,
	ble_setAdvertiseData,
	ble_setPasskey
};
