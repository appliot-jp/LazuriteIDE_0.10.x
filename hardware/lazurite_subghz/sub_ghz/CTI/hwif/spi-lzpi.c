/*
 * Lazurite Pi Gateway SPI Driver
 * 
 * File:  spi-lzpi.c
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
#include <linux/spi/spi.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

//	SPI API
struct lzpi_spi_dev {
	struct spi_device *spi;
	char name[SPI_NAME_SIZE];
	u16 irq;
	struct mutex		lock;
};

struct lzpi_spi_dev *m_lzpi_spi;
int (*probe_callback)(void);
// SPI probe
static int lzpi_spi_probe(struct spi_device *spi)
{
	int err;
	struct lzpi_spi_dev *lzpi_spi;

	// initializing SPI
	lzpi_spi = kzalloc(sizeof(*lzpi_spi), GFP_KERNEL);
	if (!lzpi_spi) {
	        return -ENOMEM;
	}

	spi->bits_per_word = 8;
	spi->max_speed_hz = 5000000;
	err = spi_setup(spi);
	if (err < 0) {
		printk(KERN_ERR"spi_setup error %d\n", err);
		goto error_spi_setup;
	}

	mutex_init(&lzpi_spi->lock);

	lzpi_spi->spi = spi;
	strcpy(lzpi_spi->name, spi->modalias);
	lzpi_spi->irq = (u16)spi->irq;

	m_lzpi_spi = lzpi_spi;
	printk(KERN_INFO "spi_probe name[%s]]\n", lzpi_spi->name);

	spi_set_drvdata(spi, lzpi_spi);

	return probe_callback();

error_spi_setup:
	kfree(lzpi_spi);
	return err;
}

static int lzpi_spi_remove(struct spi_device *dev)
{
	struct lzpi_spi_dev *lzpi_spi = spi_get_drvdata(dev);

//	finish_flag = 1;

	spi_set_drvdata(lzpi_spi->spi, NULL);
	if (!lzpi_spi)
		return 0;
	kfree(lzpi_spi);
	
	/* gpio uninit */
	//gpio_free(GPIO_SINTN);

	printk(KERN_INFO "spi_remove\n");
	return 0;
}

static struct spi_driver lzpi_spi_driver = {
	.driver = {
		.name   = "bp3596_spi",
		.owner  = THIS_MODULE,
	},
	.probe          = lzpi_spi_probe,
	.remove         = lzpi_spi_remove,
};

int lzpi_spi_init(int (*callback)(void))
{
	probe_callback = callback;
	return spi_register_driver(&lzpi_spi_driver);
}


int lzpi_spi_del_driver(void)
{
	int status = 0;
	spi_unregister_driver(&lzpi_spi_driver);
	return status;
}

int lzpi_spi_transfer(const uint8_t* wdata, uint16_t wsize, uint8_t* rdata, uint16_t rsize) {
    int status;
	status = spi_write_then_read(m_lzpi_spi->spi,wdata, wsize, rdata, rsize);
	if(status != 0) printk("[LZPI] SPI Unknow Error\r\n");
    return status;
}

