/*
 * IEEE802.15.4e driver IO
 * 
 * File:  drv-802154e.c
 * 
 * Copyright 2015 Lapis Semiconductor Co.,Ltd.
 * Author: Naotaka Saito
 * 
 * The program is based on BP3596 driver by Communication Technology
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/ioctl.h>
#include <linux/parport.h>
#include <linux/ctype.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/time.h>

#include "common-lzpi.h"
#include "subghz_api.h"
#include "drv-lazurite.h"
#include "CTI/hwif/hal.h"

#define DATA_SIZE		256+16
#define DRV_NAME		"lzgw"

wait_queue_head_t tx_done;
extern int que_th2ex;

struct list_data {				
	uint8_t	data[DATA_SIZE];
	int		len;
	struct list_head list;
};
struct list_data head;				// char dev list head
static struct s_CHAR_DEV {
	char name[32];
	struct class *dev_class;		// device class
	int major;						// device major number
	int access_num;					// open count
	struct mutex	lock;				// chardev spin lock
	wait_queue_head_t	read_q;		// polling wait for char dev
} chrdev =
{
	.name = DRV_NAME,
	.access_num = 0
};
int listed_packet = 0;

static struct {
	unsigned short drv_mode;
	unsigned char ch;
	unsigned char pwr;
	unsigned char bps;
	unsigned char addr_type;
	unsigned short my_panid;
	unsigned short tx_panid;
	unsigned char my_addr[8];
	unsigned char tx_addr[8];
	unsigned char rx_rssi;
	unsigned char tx_rssi;
	unsigned char senseTime;
	unsigned char txRetry;
	unsigned short txInterval;
	unsigned short ccaWait;
	struct timespec rx_time;
	int rx_status;
	int tx_status;
} p = {
	0,		// drv_mode
	36,		// default ch
	20,		// default pwr
	100,		// default bps
	6,
	0xABCD,		// default my panid
	0xABCD,		// default tx panid
	{0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef},		// my addr
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},		// tx addr
};
// *****************************************************************
//			transfer process (input from chrdev)
// *****************************************************************
int write_list_data(const uint8_t* raw,int len,uint8_t rssi){
	int offset = 0;
	struct list_data *new_data;
	struct timespec rx_time;
	const uint8_t *in;
	uint8_t *out;

	new_data = kmalloc(sizeof(struct list_data), GFP_KERNEL);
	if (new_data == NULL) {
		printk(KERN_ERR "[DRV-Lazurite] kmalloc (list_data) GFP_KERNEL no memory\n");
		return -ENOMEM;
	}

	// copy data to list
	if((len < DATA_SIZE) & (len>0))
	{
		// get time stamp
		getnstimeofday(&rx_time);
		// copy memory
		in = raw;
		out = new_data->data;
		memcpy(out+offset,&rx_time,sizeof(rx_time)),offset+=sizeof(rx_time);
		memcpy(out+offset,&rssi,sizeof(uint8_t)),offset+=sizeof(uint8_t);
		memcpy(out+offset,in,len);
		// new_data=>len is just length of raw data. total data size = raw + time + rssi
		new_data->len = len;
		// list add 
		list_add_tail(&new_data->list, &head.list);
		listed_packet++;

		//check number of list. if the number is  over, delete list.
		while(listed_packet >4) {
			struct list_data *data;
			data = list_entry(head.list.next, struct list_data, list);
			list_del(head.list.next);
			listed_packet--;
			kfree(data);
		}
		// poll wait cancell 
		wake_up_interruptible(&chrdev.read_q);	
	}
	else
	{
		printk(KERN_ERR "[DRV-802154E] add_list PHY Size error\n");
		kfree(new_data);
		return -1;
	}

	return 0;
}
void rx_callback(const uint8_t *data, uint8_t rssi, int status)
{
	if(status > 0) {
		EXT_rx_led_flash(2);
		write_list_data(data,status,rssi);
	}
	return;

}

// *****************************************************************
//			char dev function
// *****************************************************************

static long chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	unsigned int command = cmd & 0xF000;
	unsigned int param = cmd & 0x0FFF;
	long ret=-EFAULT;
	mutex_lock( &chrdev.lock );

	switch(command) {
		case IOCTL_CMD:
			switch(param) {
				case IOCTL_SET_BEGIN:
					ret = SubGHz.begin(p.ch,p.my_panid,p.bps,p.pwr);
					if(ret != SUBGHZ_OK) ret *= -1;
					break;
				case IOCTL_SET_RXON:
					ret = SubGHz.rxEnable(rx_callback);
					if(ret != SUBGHZ_OK) ret *=-1;
					break;
				case IOCTL_SET_RXOFF:
					ret = SubGHz.rxDisable();
					if(ret != SUBGHZ_OK) ret *=-1;
					break;
				case IOCTL_SET_CLOSE:
					ret = SubGHz.close();
					if(ret != SUBGHZ_OK) ret = EFAULT;
					break;
				case IOCTL_GET_SEND_MODE:
					{
						SUBGHZ_PARAM param;
						ret = SubGHz.getSendMode(&param);
						if(ret != SUBGHZ_OK) ret *=-1;
						p.addr_type = param.addrType;
						p.senseTime = param.senseTime;
						p.txRetry = param.txRetry;
						p.txInterval = param.txInterval;
						p.my_addr[0] = (param.myAddress >> 0 ) & 0x00ff;
						p.my_addr[1] = (param.myAddress >> 8 ) & 0x00ff;
						p.ccaWait = param.ccaWait;
						break;
					}
				case IOCTL_SET_SEND_MODE:
					{
						SUBGHZ_PARAM param;
						param.addrType = p.addr_type;
						param.senseTime = p.senseTime;
						param.txRetry = p.txRetry;
						param.txInterval = p.txInterval;
						param.myAddress = p.my_addr[1];
						param.myAddress = (param.myAddress << 8 ) | p.my_addr[0];
						p.ccaWait = param.ccaWait;
						ret = SubGHz.setSendMode(&param);
						if(ret != SUBGHZ_OK) ret *=-1;
						break;
					}
				default:
					ret = -ENOTTY;
					break;
			}
			break;
		case IOCTL_PARAM:
			switch(param) {
				case IOCTL_GET_CH:			// get ch
					ret = p.ch;
					break;
				case IOCTL_SET_CH:			// set ch
					if(p.drv_mode == 0xFFFF) {
						p.ch = arg;
						ret = p.ch;
					} else if(p.bps==50) {
						if((arg>=24) && (arg<=61)) {
							p.ch = arg;
							ret = p.ch;
						} else {
							printk(KERN_ERR"ch at 50kbps = %ld error!! must be 24-61\n",arg);
							ret = -EINVAL;
						}
					}else if(p.bps==100) {
						if((arg>=24) && (arg<=60) &&(arg != 32 )) {
							p.ch = arg;
							ret = p.ch;
						} else {
							printk(KERN_ERR"ch at 100kbps = %ld error!! must be 24-31, 33-60\n",arg);
							ret = -EINVAL;
						}
					}
					break;
				case IOCTL_GET_PWR:			// get pwr
					ret = p.pwr;
					break;
				case IOCTL_SET_PWR:			// set pwr
					if(p.drv_mode == 0xFFFF) {
						p.pwr = arg;
						ret = p.pwr;
					} else if((arg==1) || (arg==20)) {
						p.pwr = arg;
						ret = p.pwr;
					} else {
						printk(KERN_ERR"pwr = %lx error!! must be 1 or 20\n",arg);
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_BPS:			// get bps
					ret = p.bps;
					break;
				case IOCTL_SET_BPS:			// set bps
					if(p.drv_mode == 0xFFFF) {
						p.bps = arg;
						ret = p.bps;
					} else if((arg==50) || (arg==100)) {
						p.bps = arg;
						ret = p.bps;
					} else {
						printk(KERN_ERR"bps = %lx error!! must be 50 or 100\n",arg);
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_MY_PANID:			// get panid
					ret = p.my_panid;
					break;
				case IOCTL_SET_MY_PANID:			// set panid
					if((arg >= 0) && (arg <= 0xffff)) {
						p.my_panid = arg;
						ret = p.my_panid;
					} else {
						printk(KERN_ERR"bps = %lx error!! must be 50 or 100\n",arg);
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_RX_PANID:			// get panid
					ret = p.tx_panid;
					break;
				case IOCTL_SET_RX_PANID:			// set panid
					if((arg >= 0) && (arg <= 0xffff)) {
						p.tx_panid = arg;
						ret = p.tx_panid;
					} else {
						printk(KERN_ERR"bps = %lx error!! must be 50 or 100\n",arg);
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_MY_ADDR0:			// get panid
					ret = SubGHz.getMyAddress();
					p.my_addr[1] = (ret >> 8) & 0x00ff;
					p.my_addr[0] = ret  & 0x000000ff;
					break;
				case IOCTL_GET_MY_ADDR1:			// get panid
				case IOCTL_GET_MY_ADDR2:			// get panid
				case IOCTL_GET_MY_ADDR3:			// get panid
					break;
				case IOCTL_SET_MY_ADDR0:			// set panid
					if((arg >= 0) && (arg <= 0xffff)&&(p.drv_mode==0x0000FFFF)) {
						p.my_addr[(param-IOCTL_SET_MY_ADDR0)+1] = (arg >> 8) & 0x000000ff;
						p.my_addr[(param-IOCTL_SET_MY_ADDR0)+0] = arg  & 0x000000ff;
						ret = arg;
					} else {
						ret = -EINVAL;
					}
				case IOCTL_SET_MY_ADDR1:			// set panid
				case IOCTL_SET_MY_ADDR2:			// set panid
				case IOCTL_SET_MY_ADDR3:			// set panid
					break;
				case IOCTL_GET_RX_ADDR0:			// get panid
				case IOCTL_GET_RX_ADDR1:			// get panid
				case IOCTL_GET_RX_ADDR2:			// get panid
				case IOCTL_GET_RX_ADDR3:			// get panid
					ret = p.tx_addr[(param-IOCTL_GET_RX_ADDR0)+1];
					ret <<= 8;
					ret += p.tx_addr[(param-IOCTL_GET_RX_ADDR0)+0];
					break;
				case IOCTL_SET_RX_ADDR0:			// set panid
				case IOCTL_SET_RX_ADDR1:			// set panid
				case IOCTL_SET_RX_ADDR2:			// set panid
				case IOCTL_SET_RX_ADDR3:			// set panid
					if((arg >= 0) && (arg <= 0xffff)) {
						p.tx_addr[(param-IOCTL_SET_RX_ADDR0)+1] = (arg >> 8) & 0x000000ff;
						p.tx_addr[(param-IOCTL_SET_RX_ADDR0)+0] = arg  & 0x000000ff;
						ret = arg;
					} else {
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_ADDR_TYPE:			// get panid
					ret = p.addr_type;
					break;
				case IOCTL_SET_ADDR_TYPE:			// set panid
					if(p.drv_mode == 0xFFFF) {
						p.addr_type = arg;
						ret = p.addr_type;
					} else if((arg >= 0) && (arg <= 7)) {
						p.addr_type = arg;
						ret = p.addr_type;
					} else {
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_DRV_MODE:			// get panid
					ret = p.drv_mode;
					break;
				case IOCTL_SET_DRV_MODE:			// set panid
					if((arg >= 0) && (arg <= 0xFFFF)) {
						p.drv_mode = arg;
						ret = p.drv_mode;
					} else {
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_SENSE_TIME:			// get panid
					ret = p.senseTime;
					break;
				case IOCTL_SET_SENSE_TIME:			// get panid
					if((arg >= 0) && (arg <= 0x00FF)) {
						ret = p.senseTime;
					} else {
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_TX_RETRY:			// get panid
					ret = p.txRetry;
					break;
				case IOCTL_SET_TX_RETRY:			// get panid
					if((arg >= 0) && (arg <= 0x00FF)) {
						ret = p.txRetry;
					} else {
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_TX_INTERVAL:			// get panid
					ret = p.txInterval;
					break;
				case IOCTL_SET_TX_INTERVAL:			// get panid
					if((arg >= 0) && (arg <= 0x00FFFF)) {
						ret = p.txInterval;
					} else {
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_CCA_WAIT:			// get panid
					ret = p.ccaWait;
					break;
				case IOCTL_SET_CCA_WAIT:			// get panid
					if((arg >= 0) && (arg <= 0x00FFFF)) {
						ret = p.ccaWait;
					} else {
						ret = -EINVAL;
					}
					break;
				case IOCTL_GET_RX_SEC0:
					ret = (p.rx_time.tv_sec >> 0) & 0x0000ffff;
					break;
				case IOCTL_GET_RX_SEC1:
					ret = (p.rx_time.tv_sec >> 16) & 0x0000ffff;
					break;
				case IOCTL_GET_RX_NSEC0:
					ret = (p.rx_time.tv_nsec >> 0) & 0x0000ffff;
					break;
				case IOCTL_GET_RX_NSEC1:
					ret = (p.rx_time.tv_nsec >> 16) & 0x0000ffff;
					break;
				case IOCTL_GET_RX_RSSI:
					ret = p.rx_rssi;
					break;
				case IOCTL_GET_TX_RSSI:
					ret = p.tx_rssi;
					break;
				default:
					ret = -ENOTTY;
					break;
			}
			break;
		case IOCTL_RF:
			if(param<0x7f)	// read
			{
				uint8_t rdata[1];
				uint8_t wdata[1];
				wdata[0] = (param<<1);
				EXT_SPI_transfer(wdata,1,rdata,1);
				ret = rdata[0];
			}
			else			// write
			{
				uint8_t rdata;
				uint8_t wdata[2];
				wdata[0]=((param&0x7f)<<1)+1;
				wdata[1]=arg;
				EXT_SPI_transfer(wdata,2,&rdata,0);
				ret = 0;
			}
			break;
		case IOCTL_EEPROM:
			{
				uint8_t data;
				EXT_I2C_read(param,&data,1);
				ret = data;
				break;
			}
		case IOCTL_LED:
			if(param == 0x000) {
				EXT_rx_led_flash(arg);
			} else {
				EXT_tx_led_flash(arg);
			}
			ret = 0;
			break;
		default:
			ret = -ENOTTY;
			break;
	}
	mutex_unlock( &chrdev.lock );
	return ret;
}

static int chardev_open(struct inode* inode, struct file* filp) {
	mutex_lock( &chrdev.lock );
	if (chrdev.access_num > 2) {
		mutex_unlock(&chrdev.lock );
		return -EBUSY;
	}
	chrdev.access_num ++;
	mutex_unlock( &chrdev.lock );
	return 0;
}

static int chardev_release(struct inode* inode, struct file* filp)
{
	mutex_lock( &chrdev.lock );
	chrdev.access_num --;
	mutex_unlock( &chrdev.lock );
	return 0;
}

static ssize_t chardev_read (struct file * file, char __user * buf, size_t count, loff_t * ppos) {
	ssize_t bytes_read = 0;
	struct list_data *ptr = NULL;

	mutex_lock( &chrdev.lock );

	// list empty
	if (list_empty(&head.list) != 0) {
		goto end;
	}
	// error
	ptr = list_entry(head.list.next, struct list_data, list);
	if (ptr == NULL) {
		printk( KERN_ERR "%s : list_entry failed\n", chrdev.name);
		bytes_read = 0;
		goto end;
	}
	// 2byte read,then return size
	if (count == sizeof(unsigned short)) {
		bytes_read = count;
		if (bytes_read > 0 && copy_to_user (buf, &(ptr->len), count)) {
			printk( KERN_ERR "%s : copy_to_user failed\n", chrdev.name);
			bytes_read = 0;
			goto end;
		}
	} else {
		int offset=0;
		memcpy(&p.rx_time,&ptr->data[offset],sizeof(p.rx_time)),offset+=sizeof(p.rx_time);
		memcpy(&p.rx_rssi,&ptr->data[offset],sizeof(uint8_t)),offset+=sizeof(uint8_t);
		// return data
		bytes_read = count;

		if (bytes_read > 0 && copy_to_user (buf, &ptr->data[offset], bytes_read)) {
			printk( KERN_ERR "%s : copy_to_user failed\n", chrdev.name);
			bytes_read = 0;
			goto end;
		}
		list_del(head.list.next);
		listed_packet--;


		kfree(ptr);
	}
end:
	mutex_unlock( &chrdev.lock );
	return bytes_read;
}
static void tx_callback(uint8_t rssi,int status) {
	p.tx_rssi = rssi;
	p.tx_status = status;

	if(que_th2ex == 0) {
		que_th2ex = 1;
		wake_up_interruptible(&tx_done);
	}
	return;
}
static ssize_t chardev_write (struct file * file, const char __user * buf,
		size_t count, loff_t * ppos) {
	int status = 0;
	uint8_t payload[DATA_SIZE];

	mutex_lock( &chrdev.lock );

	if(count<DATA_SIZE)
	{
		uint16_t tx_addr;
		tx_addr = p.tx_addr[1];
		tx_addr = (tx_addr << 8 ) | p.tx_addr[0];
		if(copy_from_user(payload,buf,count))
		{
			status = -EFAULT;
			goto error;
		}
		EXT_set_tx_led(0);
		status = SubGHz.send(p.tx_panid,tx_addr,payload,count,tx_callback);
		p.tx_status = status;
		if(status == SUBGHZ_OK)
		{
			status = count;
		} else if(p.tx_status == SUBGHZ_TX_CCA_FAIL) {
			status = -EBUSY;
		} else if (p.tx_status == SUBGHZ_TX_ACK_FAIL) {
			status = -ENODEV;
		} else {
			status = -EFAULT;
		}
	} else {
		status = -E2BIG;
		goto error;
	}

error:
	EXT_set_tx_led(1);
	mutex_unlock( &chrdev.lock );
	return status;
}

static unsigned int chardev_poll(struct file* filp, poll_table* wait) {
	unsigned int retmask = 0;
	poll_wait( filp, &chrdev.read_q,  wait );
	if (list_empty(&head.list) == 0) {
		retmask |= ( POLLIN  | POLLRDNORM );
	}
	return retmask;
}

static const struct file_operations chardev_fops = {
	.owner		= THIS_MODULE,
	.read		= chardev_read,
	.write		= chardev_write,
	.open		= chardev_open,
	.poll		= chardev_poll,
	.unlocked_ioctl	= chardev_ioctl,
	.release    = chardev_release,
};

// *****************************************************************
//			driver initialization
// *****************************************************************
static int __init drv_param_init(void) {
	//int drv_param_init(void) {
	int status = 0;
	int err;
	struct device *dev;
	// create char device
	if((chrdev.major = register_chrdev(0, DRV_NAME, &chardev_fops)) < 0)
	{
		printk(KERN_ERR "[drv-lazurite] unable to get major =%d\n",
				chrdev.major);
		goto error;
	}
	chrdev.dev_class = class_create(THIS_MODULE, chrdev.name);
	if(IS_ERR(chrdev.dev_class))
	{
		err = PTR_ERR(chrdev.dev_class);
		printk(KERN_ERR"[drv-lazurite] class_create error %d\n", err);
		goto error_class_create;
	}
	dev = device_create(chrdev.dev_class, NULL, MKDEV(chrdev.major, 0), NULL, chrdev.name);
	if (IS_ERR(dev)) {
		err = PTR_ERR(dev);
		printk(KERN_ERR"[drv-lazurite]device_create error %d\n", err);
		goto error_device_create;
	}

	//initializing list head
	INIT_LIST_HEAD(&head.list);

	// initializing wait queue
	init_waitqueue_head( &chrdev.read_q );
	init_waitqueue_head( &tx_done );

	status = SubGHz.init();
	if(status != SUBGHZ_OK) goto error_device_create;

	// get address
	/*
	   EXT_I2C_read(0x20,p.my_addr[7],1);
	   EXT_I2C_read(0x21,p.my_addr[6],1);
	   EXT_I2C_read(0x22,p.my_addr[5],1);
	   EXT_I2C_read(0x23,p.my_addr[4],1);
	   EXT_I2C_read(0x24,p.my_addr[3],1);
	   EXT_I2C_read(0x25,p.my_addr[2],1);
	   EXT_I2C_read(0x26,p.my_addr[1],1);
	   EXT_I2C_read(0x27,p.my_addr[0],1);
	 */

	printk(KERN_INFO "[drv-lazurite] End of init\n");
	mutex_init( &chrdev.lock );

	return status;

error_device_create:
	class_destroy(chrdev.dev_class);
error_class_create:
	unregister_chrdev(chrdev.major, chrdev.name);
error:
	printk(KERN_ERR "[drv-lazurite] Init Error\n");
	return status;
}

// *****************************************************************
//			unload driver 
// *****************************************************************
static void __exit drv_param_exit(void) {
	//void drv_param_exit(void) {
	//	mac_802154e_exit();
	//list delete 
	while (!list_empty(&head.list)) {
		struct list_data *data;
		data = list_entry(head.list.next, struct list_data, list);
		list_del(head.list.next);
		listed_packet--;
		kfree(data);
	}

	// char dev destroy
	device_destroy(chrdev.dev_class, MKDEV(chrdev.major, 0));
	class_destroy(chrdev.dev_class);
	unregister_chrdev(chrdev.major, chrdev.name);
	// mac remove
	SubGHz.remove();
	printk(KERN_INFO "[drv-lazurite] exit remove\n");
	return;
}


module_init(drv_param_init);
module_exit(drv_param_exit);

MODULE_AUTHOR("Lapis Semiconductor.");
MODULE_DESCRIPTION("Lazurite Kernel Driver");
MODULE_LICENSE("Dual BSD/GPL");

