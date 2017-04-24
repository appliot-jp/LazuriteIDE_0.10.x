
#define BLE_DEBUG
#define BLE_RSTB			7		// P11
#define BLE_GPIO3			2		// P02
#define BLE_GPIO2			32		// P54
#define BLE_GPIO1			33		// P55
#define BLE_GPIO0			21		// P41
#define PWR_LED				20		// P23
#define BLE_GPIO2_IRQNUM	3
#define ORANGE_LED			25
#define BLUE_LED			26

#define BLE_GPIO2_DET_TIME			5
#define BLE_CTS_WAIT_TIME			1000L
#define BLE_COMMAND_WAIT_TIME		20		//
#define BLE_COMMAND_WAIT_TIME_LONG	1000	//

#include <driver_uart.h>
#include <driver_irq.h>
#include <driver_extirq.h>
#include <driver_gpio.h>
#include <driver_timer.h>
#include <serial.h>
#include <lp_manage.h>
#include <print.h>
#include <string.h>
#include <wdt.h>

static void (*ble_callback_func)(int mode);

static uint8_t ble_tx_mode;				// 0 = normal, 1=cmd mode
static uint8_t ble_cmd_buf_index;		// cmd buffer index
static bool ble_flow_ctrl = true;
static uint8_t ble_rx_buf[BLE_RX_BUF_SIZE];
static uint8_t ble_tx_buf[BLE_TX_BUF_SIZE];
static char ble_tx_flag;							// true = UA1BUF is full, false = UA1BUF is empty
static char ble_tx_sending;							// true = sending (can not sleep), false = not sending (enable to sleep)
static char led_status;
static struct BLE_LED_PARAM *current_led_param;
static struct BLE_LED_CONFIG ble_led_conf = {
	{				// sleep
		1,			// cycle
		PWR_LED,	// pin
		1,			// on time
		100			// off time
	},
	{				// advertise
		3,			// cycle
		PWR_LED,	// pin
		1,		// on time
		100			// off time
	},
	{				// connect
		1,			// cycle
		PWR_LED,	// pin
		100,			// on time
		100			// off time
	},
	{				// serial_connect
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
static BLE_STATUS mode = NOT_START;
static uint8_t check_connect;
static uint8_t check_nocarrier;
static const char ble_msg_connect[] = "\r\nCONNECT\r\n";
static const char ble_msg_nocarrier[] = "\r\nNO CARRIER\r\n";
static const char ble_msg_ok[] = "\r\nOK\r\n";
static const char ble_msg_online_nocarrier[] = "\r\nOK\r\n\r\nNO CARRIER\r\n";


static void ble_serial_connect_check(void) {
	int data;
	if((mode == CONNECT) && (drv_digitalRead(BLE_GPIO2)==LOW)) {
		mode = SERIAL_CONNECT;
		if(ble_flow_ctrl) {
			if( ble_tx_sending == false) {
				data = uart_fifo_out(&ble_tx_fifo);
				if(data >= 0) {
					write_reg8(UA1BUF, (char)data);
					ble_tx_flag = true;
					ble_tx_sending = true;
				}
			}
		} else {
			if(ble_callback_func) ble_callback_func(mode);
		}
	}
}
static void ble_gpio2_isr(void) {
	if((mode == CONNECT) || (mode == SERIAL_CONNECT)) {
		if(drv_digitalRead(BLE_GPIO2)) {
			mode = CONNECT;
			if(!ble_flow_ctrl) {
				if(ble_callback_func) ble_callback_func(mode);
			}
		} else {
		    timer_8bit_set((uint8_t)TM_CH5, 0xb0, BLE_GPIO2_DET_TIME, ble_serial_connect_check);
		    timer_8bit_start(TM_CH5);
		}
	}
}

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
	case DEEPSLEEP:
		current_led_param=&ble_led_conf.sleep;
		break;
	case ADVERTISE:
		current_led_param=&ble_led_conf.advertise;
		break;
	case CONNECT:
		current_led_param=&ble_led_conf.connect;
		break;
	case SERIAL_CONNECT:
		current_led_param=&ble_led_conf.serial_connect;
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
	if((mode == SERIAL_CONNECT) && (ble_flow_ctrl) && (ble_tx_mode == 0)) {
		if( ble_tx_flag == false) {
			data = uart_fifo_out(&ble_tx_fifo);
			if(data < 0)
			{
				if(ble_tx_flag) {
					ble_tx_flag = false;
				} else {
					ble_tx_sending = false;
				}
			} else {
				write_reg8(UA1BUF, (char)data);
				ble_tx_flag = true;
			}
		} else {
			ble_tx_flag = false;
		}
	} else if((mode == CONNECT) && (ble_flow_ctrl) && (ble_tx_mode == 0)) {
		if(ble_tx_flag) ble_tx_flag = false;
		else ble_tx_sending = false;
	} else {
		data = uart_fifo_out(&ble_tx_fifo);
		digitalWrite(BLUE_LED,LOW);
		if(data < 0) {
			if(ble_tx_flag) {
				ble_tx_flag = false;
			} else {
				ble_tx_sending = false;
			}
		} else {
			write_reg8(UA1BUF, (char)data);
		}
		digitalWrite(BLUE_LED,HIGH);
	}
	return;
}

static void ble_rx_isr() {
	char data;
	int i=0;
	
	data = read_reg8(UA0BUF);
	if(ble_tx_mode == 0) {
		switch(mode) {
		case CONNECT:
		case ADVERTISE:
		case SERIAL_CONNECT:
			// check "NO CARRIER"
			if (data ==  ble_msg_nocarrier[check_nocarrier]) {
				check_nocarrier++;
			} else if (data == ble_msg_nocarrier[0]){
				check_nocarrier = 1;
			} else {
				// when data is not match to "no carrier", data is push to MCU
				if(mode == SERIAL_CONNECT) {
					while(check_nocarrier>0) {
						uart_fifo_in(&ble_rx_fifo,  ble_msg_nocarrier[i]);
						i++;
						check_nocarrier--;
					};
					uart_fifo_in(&ble_rx_fifo, data);
				}
			}
			// check "CONNECT"
			if (data == ble_msg_connect[check_connect]) {
				check_connect++;
			} else if(data == ble_msg_connect[0]){
				check_connect = 1;
			}
			
			// detect "NO CARRIER"
			if(check_nocarrier >= (sizeof(ble_msg_nocarrier)-1)) {
				drv_digitalWrite(BLE_GPIO1,HIGH);
				uart_fifo_init(&ble_tx_fifo);
				mode = DEEPSLEEP;
				check_nocarrier = 0;
				if(ble_callback_func) ble_callback_func(mode);
			}
			// detect "CONNECT"
			if(check_connect >= (sizeof(ble_msg_connect)-1)) {
				drv_digitalWrite(BLE_GPIO1,LOW);
				if(drv_digitalRead(BLE_GPIO2)) {
					uart_fifo_init(&ble_rx_fifo);
					uart_fifo_init(&ble_tx_fifo);
					mode = CONNECT;
				} else {
					uart_fifo_init(&ble_rx_fifo);
					uart_fifo_init(&ble_tx_fifo);
					mode = SERIAL_CONNECT;
				}
				check_connect = 0;
				if(ble_callback_func) ble_callback_func(mode);
			}
			break;
		case DEEPSLEEP:
		default:
			break;
		}
	} else {
		if(ble_cmd_buf_index < sizeof(ble_rx_buf)) {
			ble_rx_buf[ble_cmd_buf_index] = data;
			ble_cmd_buf_index++;
		}
	}
	return;
}
static void ble_reset(void) {
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
	
	check_nocarrier = 0;
	check_connect = 0;

	uart_fifo_init(&ble_rx_fifo);
	uart_fifo_init(&ble_tx_fifo);

	uart_gpio_init(0);
	uart_begin(57600,ble_rx_isr,ble_tx_isr);

	// release reset
	drv_digitalWrite(BLE_RSTB,HIGH);
	
	mode = DEEPSLEEP;
	digitalWrite(PWR_LED,HIGH);
	timer_8bit_set(TM_CH4, 0xb0, 10, ble_led_func);
	drv_attachInterrupt(BLE_GPIO2,BLE_GPIO2_IRQNUM,ble_gpio2_isr,CHANGE,false,false);

	delay(1000);
	// waiting callibration
	while(drv_digitalRead(BLE_GPIO0) == HIGH) {
	}
	ble_tx_flag = false;
	ble_tx_sending = false;
	ble_tx_mode = 0;
}
static void ble_begin(void (*callback)(int mode)){
	ble_callback_func = callback;
	ble_reset();
}

static void ble_end(void)
{
	uart_gpio_end(0);
	uart_end();
	mode = NOT_START;
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
	dis_interrupts(DI_UART);
	uart_fifo_init(&ble_rx_fifo);
	enb_interrupts(DI_UART);
	return;
}

static int ble_check_print_mode(void) {
	if(ble_flow_ctrl) {
		if((mode != SERIAL_CONNECT)&&(mode != CONNECT)) return(mode*-1);
	} else {
		if(mode != SERIAL_CONNECT) return(mode*-1);
	}
	return 0;
}

static int ble_tx_write_byte(char data)
{
	int result;
	
	if(ble_tx_mode == 0) {
		result = ble_check_print_mode();
		if(result) return result;
	} else {
		digitalWrite(BLUE_LED,LOW);
	}
	
	dis_interrupts(DI_UART);
	result = uart_fifo_in(&ble_tx_fifo,data);
	
	if(result == 1)
	{
		if(ble_tx_flag == false)
		{
			// waiting CTS flag
			data = (char)uart_fifo_out(&ble_tx_fifo);
			write_reg8(UA1BUF,data);
			ble_tx_flag = true;
			ble_tx_sending = true;
		}
	}
	enb_interrupts(DI_UART);
	
	return result;
}

static int ble_print_cmd(char* data)
{
	int n = 0;
	while(data[n] != NULL)
	{
		if(ble_tx_flag == false) {
			write_reg8(UA1BUF,data[n]);
			ble_tx_flag = true;
			ble_tx_sending = true;
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

static int ble_print(char* data)
{
	int n = 0;
	int result;
	
	result = ble_check_print_mode();
	if(result) return result;
	
	while(data[n] != NULL)
	{
		if(mode < CONNECT) return -1;
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
static int ble_println(char* data)
{
	int result;
	
	result = ble_check_print_mode();
	if(result) return result;
	
	ble_print(data);
	ble_print("\r\n");
	return (strlen(data)+2);
}
static int ble_print_long(long data, UCHAR fmt)
{
	char tmp_c[33];
	int result;
	
	result = ble_check_print_mode();
	if(result) return result;
	
	printNumber(tmp_c,data,fmt);
	ble_print(tmp_c);
	return strlen(tmp_c);
}
static int ble_println_long(long data, UCHAR fmt)
{
	char tmp_c[33];							// 33 is maximum length of charactor
	int result;
	
	result = ble_check_print_mode();
	if(result) return result;
	
	printNumber(tmp_c, data,fmt);			// convert long data to charactor
	ble_println(tmp_c);		// send data though uart
	return strlen(tmp_c);
}

static int ble_print_double(double data, UCHAR digit)
{
	char tmp_c[33];
	int result;
	
	result = ble_check_print_mode();
	if(result) return result;
	
	printFloat(tmp_c,data,digit);
	ble_print(tmp_c);
	return strlen(tmp_c);
}
static int ble_println_double(double data, UCHAR digit)
{
	char tmp_c[33];
	int result;
	
	result = ble_check_print_mode();
	if(result) return result;
	
	printFloat(tmp_c, data,digit);
	ble_println(tmp_c);
	return strlen(tmp_c);
}
static int ble_rx_available(void)
{
	return ble_rx_fifo.length;
}
static int ble_tx_write(char* data, int quantity)
{
	int n;
	int result;
	
	result = ble_check_print_mode();
	if(result) return result;
	
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

static bool ble_wait_cts(unsigned long wait_time) {
	unsigned long start_time,current_time;
	start_time = millis();
	current_time = start_time;
	while(drv_digitalRead(BLE_GPIO2) == HIGH) {
		current_time = millis();
		if((start_time + wait_time) < current_time) return false;
	}
	#ifdef BLE_DEBUG
		Serial3.print("gpio2 detect:\t");
		Serial3.println_long((long)(current_time-start_time), DEC);
	#endif
	return true;
}
static volatile int ble_advertise(bool on) {
	int result = 0;
	
	// mode check
	if(on && (mode>DEEPSLEEP)) return -100;
	else if ((on == false) && (mode == DEEPSLEEP)) return -101;
	
	ble_tx_mode = 1;
	switch(mode) {
	case NOT_START:
		break;
	case DEEPSLEEP:
		if(on) {
			drv_digitalWrite(BLE_GPIO1,LOW);
			if(ble_wait_cts(BLE_CTS_WAIT_TIME) == false) {
				result = -102;
				goto error;
			}
			ble_print_cmd("ATD\r");
			while(ble_tx_sending) {}
			drv_digitalWrite(BLE_GPIO1,HIGH);
			mode = ADVERTISE;
		}
		break;
	case ADVERTISE:
		drv_digitalWrite(BLE_GPIO1,LOW);
		if(ble_wait_cts(BLE_CTS_WAIT_TIME) == false) {
			result = -103;
			goto error;
		}
		ble_print_cmd("\r");
		while(ble_tx_sending) {}
		drv_digitalWrite(BLE_GPIO1,HIGH);
		mode = DEEPSLEEP;
		break;
	case CONNECT:
	case SERIAL_CONNECT:
		{
			volatile uint32_t current_time;
			volatile uint32_t throw_command_time;
			// flushing TX buffer
			dis_interrupts(DI_UART);
			uart_fifo_init(&ble_tx_fifo);
			enb_interrupts(DI_UART);
			
			// set tx mode to 1 (cmd mode)
			
			// reset cmd result buffer during sending
			memset(ble_rx_buf,0,sizeof(ble_rx_buf));
			ble_cmd_buf_index = 0;
			
			// set onnlie command mode
			ble_print_cmd("+++AT\r");
			
			// check data buffer
			throw_command_time = millis();
			while(strncmp(ble_msg_ok,ble_rx_buf,sizeof(ble_rx_buf)) != 0) {
				current_time = millis();
				if((throw_command_time + BLE_COMMAND_WAIT_TIME_LONG) < current_time) {
					result = -104;
					#ifdef BLE_DEBUG
						Serial3.print(ble_rx_buf);
						Serial3.print("\t");
					#endif
					goto error;
				}
			}
			// for test
			#ifdef BLE_DEBUG
			{
				int i;
				for(i=0;i<ble_cmd_buf_index;i++) {
					Serial3.print_long((long)ble_rx_buf[i],HEX);
					Serial3.print(" ");
				}
				Serial3.println("");
			}
			#endif
			
			// reset cmd result buffer during sending
			memset(ble_rx_buf,0,sizeof(ble_rx_buf));
			ble_cmd_buf_index = 0;
			// set onnlie command mode
			ble_print_cmd("ATH\r");
			
			// check data buffer
			throw_command_time = millis();
			while(strncmp(ble_msg_online_nocarrier,ble_rx_buf,sizeof(ble_rx_buf)) != 0) {
				current_time = millis();
				if((throw_command_time + BLE_COMMAND_WAIT_TIME_LONG) < current_time) {
					result = -105;
					#ifdef BLE_DEBUG
						Serial3.print(ble_rx_buf);
						Serial3.print("\t");
					#endif
					goto error;
				}
			}
			// for test
			#ifdef BLE_DEBUG
			{
				int i;
				for(i=0;i<ble_cmd_buf_index;i++) {
					Serial3.print_long((long)ble_rx_buf[i],HEX);
					Serial3.print(" ");
				}
				Serial3.println("");
			}
			#endif

			drv_digitalWrite(BLE_GPIO1,HIGH);
			mode = DEEPSLEEP;
		}
		break;
	default:
		break;
	}
	ble_tx_mode = 0;
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}
static int ble_setTxFlowCtrl(bool on) {
	ble_flow_ctrl = on;
	return 0;
}

static void ble_setLedConfig(struct BLE_LED_CONFIG *conf)
{
	dis_interrupts(DI_UART);
	memcpy(&ble_led_conf,conf,sizeof(struct BLE_LED_CONFIG));
	enb_interrupts(DI_UART);
}
static void ble_getLedConfig(struct BLE_LED_CONFIG *conf)
{
	dis_interrupts(DI_UART);
	memcpy(conf,&ble_led_conf,sizeof(struct BLE_LED_CONFIG));
	enb_interrupts(DI_UART);
}

static int ble_throw_message(char* message) {
	volatile uint32_t throw_command_time;
	volatile uint32_t current_time;
	// preparing
	drv_digitalWrite(BLE_GPIO1,LOW);

	// flushing TX buffer
	dis_interrupts(DI_UART);
	uart_fifo_init(&ble_tx_fifo);
	enb_interrupts(DI_UART);
	
	// wait untill enable to send command
	if(ble_wait_cts(BLE_CTS_WAIT_TIME) == false) return false;
	
	// set tx mode to 1 (cmd mode)
	ble_tx_mode = 1;
	
	// reset cmd result buffer during sending
	memset(ble_rx_buf,0,sizeof(ble_rx_buf));
	ble_cmd_buf_index = 0;
	// set onnlie command mode
	ble_print_cmd(message);
	
	// check data buffer
	throw_command_time = millis();
	while(1) {
		current_time = millis();
		if((throw_command_time + BLE_COMMAND_WAIT_TIME) < current_time) break;
	}
	return true;
}

static bool ble_close_message(void) {
	uint32_t start_time;
	uint32_t current_time;
	
	drv_digitalWrite(BLE_GPIO1,HIGH);
	
	start_time = millis();
	current_time = start_time;
	while(drv_digitalRead(BLE_GPIO2) == LOW){
		current_time = millis();
		if((start_time + BLE_COMMAND_WAIT_TIME_LONG) < current_time) {
			return false;
		}
	}
	delay(1);
	ble_tx_mode = 0;
	
	return true;
}

static bool ble_set_sreg(char* cmd, int val) {
	char *tp[2];
	volatile uint32_t throw_command_time;
	volatile uint32_t current_time;
	char msg[16];

	// buffer init
	Print.init(msg,sizeof(msg));
	
	// generate message
	Print.p(cmd);
	Print.l((long)val,DEC);
	Print.p("\r");
	
	if(ble_throw_message(msg) == false) return false;
	
	// check data buffer
	throw_command_time = millis();
	while(1) {
		current_time = millis();
		if((throw_command_time + BLE_COMMAND_WAIT_TIME) < current_time) break;
	}
	tp[0]=strtok(ble_rx_buf,"\r\n");
	tp[1]=strtok(NULL,"\r\n");
	
	if(strncmp(tp[0],"OK",sizeof("OK"))!=0) return false;
	return true;
}

static int ble_setPairingMode(char pairing) {
	int result;
	
	// error check
	if(mode != DEEPSLEEP) return (mode*-1);
	if((pairing > 2) || (pairing < 0)) return -100;
	
	if(ble_set_sreg("ATS101=",pairing) == false) {
		result = -200;
		goto error;
	}
	if (ble_close_message() == false) {
		result = -201;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}

static int ble_getPairingMode(char *pairing) {
	char *tp[2];
	int result;
	
	if(mode != DEEPSLEEP) return (mode*-1);
	
	if(ble_throw_message("ATS101?\r") == false) {
		result = -200;
		goto error;
	}
	if(pairing) {
		tp[0]=strtok(ble_rx_buf,"\r\n");
		tp[1]=strtok(NULL,"\r\n");
		#ifdef BLE_DEBUG
			Serial3.print(tp[0]);
			Serial3.print("\t");
			Serial3.println(tp[1]);
		#endif
		if(strncmp(tp[1],"OK",sizeof("OK"))==0) {
			*pairing = (char)strtol(tp[0],NULL,10);
		} else {
			result = -201;
			goto error;
		}
	}
	if (ble_close_message() == false) {
		result = -202;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}

static int ble_setTxPower(char pwr) {
	int result;
	
	// error check
	if(mode != DEEPSLEEP) return (mode*-1);
	if((pwr > 3) || (pwr < 0)) return -100;
	
	if(ble_set_sreg("ATS103=",pwr) == false){
		result = -200; 
		goto error;
	}
	if (ble_close_message() == false) {
		result = -201;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}

static int ble_getTxPower(char *pwr) {
	char *tp[2];
	int result;
	
	if(mode != DEEPSLEEP) return (mode*-1);
	
	if(ble_throw_message("ATS103?\r") == false) {
		result = -200;
		goto error;
	}
	if(pwr) {
		tp[0]=strtok(ble_rx_buf,"\r\n");
		tp[1]=strtok(NULL,"\r\n");
		#ifdef BLE_DEBUG
			Serial3.print(tp[0]);
			Serial3.print("\t");
			Serial3.println(tp[1]);
		#endif
		if(strncmp(tp[1],"OK",sizeof("OK"))==0) {
			*pwr = (char)strtol(tp[0],NULL,10);
		} else {
			result = -201;
			goto error;
		}
	}
	if (ble_close_message() == false) {
		result = -202;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}

static int ble_setAdvertiseInterval(uint16_t fast, uint16_t slow, uint16_t time) {
	int result;
	
	// error check
	if(mode != DEEPSLEEP) return (mode*-1);
	if((fast < 20) || (fast > 1000)) return -100;
	if((slow < 1000) || (slow > 10240)) return -101;
	if(time > 18000) return -102;
	
	if(ble_set_sreg("ATS112=",fast)==false) {
		result = -200;
		goto error;
	}
	if(ble_set_sreg("ATS113=",slow)==false)  {
		result = -201;
		goto error;
	}
	if(ble_set_sreg("ATS114=",time) == false) {
		result = -202;
		goto error;
	}
	if (ble_close_message() == false) {
		result = -203;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}

static int ble_getAdvertiseInterval(uint16_t *fast, uint16_t *slow, uint16_t *time) {
	char *tp[4];
	int result;
	
	if(mode != DEEPSLEEP) return (mode*-1);
	
	if(ble_throw_message("ATS112?S113?S114?\r") == false) {
		result = -200;
		goto error;
	}
	
	// check data buffer
	tp[0]=strtok(ble_rx_buf,"\r\n");
	tp[1]=strtok(NULL,"\r\n");
	tp[2]=strtok(NULL,"\r\n");
	tp[3]=strtok(NULL,"\r\n");
	#ifdef BLE_DEBUG
		Serial3.print(tp[0]);
		Serial3.print("\t");
		Serial3.print(tp[1]);
		Serial3.print("\t");
		Serial3.print(tp[2]);
		Serial3.print("\t");
		Serial3.println(tp[3]);
	#endif
	if(strncmp(tp[3],"OK",sizeof("OK"))==0) {
		if(fast) *fast = (uint16_t)strtol(tp[0],NULL,10);
		if(slow) *slow = (uint16_t)strtol(tp[1],NULL,10);
		if(time) *time = (uint16_t)strtol(tp[2],NULL,10);
	} else {
		result = -201;
		goto error;
	}
	if(ble_close_message() == false ) {
		result = -202;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}

static int ble_setConnectInterval(uint16_t fast, uint16_t slow, uint16_t time) {
	int result;
	
	// error check
	if(mode != DEEPSLEEP) return (mode*-1);
	if((fast < 8) || (fast > 4000)) return -100;
	if((slow < 10) || (slow > 4000)) return -101;
	if(((time < 500) || (time > 18000)) ||(time==0))  return -102;
	
	if(ble_set_sreg("ATS105=",fast) == false) {
		result = -200;
		goto error;
	}
	if(ble_set_sreg("ATS108=",slow) == false) {
		result = -201;
		goto error;
	} 
	if(ble_set_sreg("ATS104=",time) == false) {
		result = -202;
		goto error;
	}
	if(	ble_close_message() == false) {
		result = -203;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}
static int ble_getConnectInterval(uint16_t *fast, uint16_t *slow, uint16_t *time) {
	char *tp[4];
	int result;
	
	if(mode != DEEPSLEEP) return (mode*-1);
	
	if(ble_throw_message("ATS105?S108?S104?\r") == false) {
		result = -200;
		goto error;
	}
	
	// check data buffer
	tp[0]=strtok(ble_rx_buf,"\r\n");
	tp[1]=strtok(NULL,"\r\n");
	tp[2]=strtok(NULL,"\r\n");
	tp[3]=strtok(NULL,"\r\n");
	#ifdef BLE_DEBUG
		Serial3.print(tp[0]);
		Serial3.print("\t");
		Serial3.print(tp[1]);
		Serial3.print("\t");
		Serial3.print(tp[2]);
		Serial3.print("\t");
		Serial3.println(tp[3]);
	#endif
	if(strncmp(tp[3],"OK",sizeof("OK"))==0) {
		if(fast) *fast = (uint16_t)strtol(tp[0],NULL,10);
		if(slow) *slow = (uint16_t)strtol(tp[1],NULL,10);
		if(time) *time = (uint16_t)strtol(tp[2],NULL,10);
	} else {
		result = -201;
		goto error;
	}
	if(ble_close_message() == false) {
		result = -202;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}
static int ble_setAdvertiseData(uint8_t *data, uint8_t len)
{
	int result;
	int i;
	// error check
	if(mode != DEEPSLEEP) return (mode*-1);
	if(len > 31) return -100;
	
	Print.init(ble_tx_buf,sizeof(ble_tx_buf));
	Print.p("ATS150=");
	for(i=0;i<len;i++) {
		if(data[i] < 16) Print.p("0");
		Print.l(data[i],HEX);
	}
	Print.p("\r");
	
	#ifdef BLE_DEBUG
		Serial3.print("ble_tx_buf::\t");
		Serial3.println(ble_tx_buf);
	#endif
	
	if(ble_throw_message(ble_tx_buf) == false) {
		result = -200;
		goto error;
	}
	
	if(ble_close_message() == false) {
		result = -201;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}
static int ble_getAdvertiseData(uint8_t *data, uint8_t *len)
{
	char *tp[3];
	char tmp[3]={0,0,0};
	int i,j,result;
	
	if(mode != DEEPSLEEP) return (mode*-1);
	
	if(ble_throw_message("ATS150?S151?\r") == false) {
		result = -200;
		goto error;
	}
	
	// check data buffer
	if((data)&&(len)) {
		#ifdef BLE_DEBUG
			Serial3.print("ble_rx_buf::\t");
			Serial3.println(ble_rx_buf);
		#endif

		tp[0]=strtok(ble_rx_buf,"\r\n");
		tp[1]=strtok(NULL,"\r\n");
		tp[2]=strtok(NULL,"\r\n");
		#ifdef BLE_DEBUG
			if(tp[0]) {
				Serial3.print(tp[0]);
			} else {
				Serial3.print("tp[0]");
			}
			Serial3.print("\t");
			if(tp[1]) {
				Serial3.print(tp[1]);
			} else {
				Serial3.print("tp[1]");
			}
			Serial3.print("\t");
			if(tp[2]) {
				Serial3.print(tp[2]);
			} else {
				Serial3.print("tp[2]");
			}
			Serial3.println("");
		#endif
		if(strncmp(tp[2],"OK",sizeof("OK"))==0) {
			*len = (uint8_t)strtol(tp[1],NULL,10);
			j=0;
			for(i=0,j=0;i<*len;i++) {
				tmp[0] = tp[0][j],j++;
				tmp[1] = tp[0][j],j++;
				data[i] = (uint8_t)strtol(tmp,NULL,HEX);
			}
		} else {
			result = -201;
			goto error;
		}
	}
	if(ble_close_message() == false) {
		result = -202;
		goto error;
	}
	return 0;
error:
	ble_reset();
	if(ble_callback_func) ble_callback_func(mode);
	return result;
}
static int ble_setPasskey(uint32_t key) {
	if(mode != DEEPSLEEP) return (mode*-1);
	return 0;
}
static int ble_getPasskey(uint32_t *key) {
	if(mode != DEEPSLEEP) return (mode*-1);
	return 0;
}
const BLUETOOTH BLE = {
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
	ble_setTxFlowCtrl,
	ble_setLedConfig,
	ble_getLedConfig,
	ble_setPairingMode,
	ble_getPairingMode,
	ble_setTxPower,
	ble_getTxPower,
	ble_setAdvertiseInterval,
	ble_getAdvertiseInterval,
	ble_setConnectInterval,
	ble_getConnectInterval,
	ble_setAdvertiseData,
	ble_getAdvertiseData,
	ble_setPasskey,
	ble_getPasskey
};
