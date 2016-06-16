/*
 * Lazurite Pi Gateway I2C Driver
 * 
 * File:  i2c-lzpi.c
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
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include "i2c-lzpi.h"
#ifndef LAZURITE_IDE
#include "../../common-lzpi.h"
#endif

#define I2C_BUFFERSIZE 16

#define I2C_NOT_PROBED	NULL
static struct i2c_client *g_client=I2C_NOT_PROBED;
static uint8_t lzpi_i2c_addr;

// local function
int lzpi_i2c_write(const uint8_t *data, uint8_t length) {
	return i2c_master_send(g_client, data, length);
}

int lzpi_i2c_read(uint8_t *data, uint8_t length) {
	return i2c_master_recv(g_client, data, length);
}
// Driver Interface
static const struct i2c_device_id lzpi_i2c_id[] = {
	{ "lzpi_i2c", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, lzpi_i2c_id);

//static unsigned short normal_i2c[] = { lzpi_i2_addr, I2C_CLIENT_END};

static int lzpi_i2c_probe(struct i2c_client *client,
			 const struct i2c_device_id *did) {
	if(i2c_get_clientdata(client))
	{
		return -EBUSY;
	}
	g_client = client;
	return 0;
}

static int lzpi_i2c_remove(struct i2c_client *client) {
	if (!client->adapter)
		printk(KERN_INFO "[I2C] client is not attached\n");
		return -ENODEV;	/* our client isn't attached */
	return 0;
}
static struct i2c_driver lzpi_i2c_driver = {
	.driver	= {
		.name = "lzpi_i2c",
	},
	.probe	= lzpi_i2c_probe,
	.remove	= lzpi_i2c_remove,
	.id_table = lzpi_i2c_id,
	//.address_list = normal_i2c,
};

int lzpi_i2c_init(void)
{
	return i2c_add_driver(&lzpi_i2c_driver);
}

int lzpi_i2c_adapter_init(uint8_t i2c_addr)
{
	//int status;
	struct i2c_client *client;
	static struct i2c_adapter *adapter = NULL;
	static struct i2c_board_info info;

	// 
	lzpi_i2c_addr = i2c_addr;

	/* I2C adapter init */
	if(g_client == I2C_NOT_PROBED)
	{
		adapter = i2c_get_adapter(1);
		if(adapter == NULL)
		{
			i2c_del_driver(&lzpi_i2c_driver);
			printk("[I2C] i2c adapter fail\n");
			return -2;
		}
		memset(&info, 0, sizeof(struct i2c_board_info));
		info.addr = i2c_addr;
		memcpy(info.type, "lzpi_i2c", strlen("lzpi_i2c"));
		client = i2c_new_device(adapter,&info);
		if (client == NULL)
		{
			i2c_del_driver(&lzpi_i2c_driver);
			printk("[I2C] i2c new device fail\n");
			return -3;
		}
		i2c_put_adapter(adapter); 
	}
	else
	{
		printk("[I2C] i2c adapter already probed\n");
	}

	return 0;
}

int lzpi_i2c_del_driver(void)
{
	i2c_unregister_device(g_client);
	i2c_del_driver(&lzpi_i2c_driver);
	return 0;
}

