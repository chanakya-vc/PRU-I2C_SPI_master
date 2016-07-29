/*
 * Written by Vaibhav Choudhary under GSOC-2016 for BeagleBoard.org
 * Copyright (C) 2016 Vaibhav Choudhary -www.vaibhavchoudhary.com
 *
 * This code may be copied and/or modified freely according to GNU General Public  
 * License version 2 as published by the Free Software Foundation, provided   
 * the following conditions are also met:
 * 1) Redistributions/adaptions of source code must retain this copyright
 *    notice on the top, giving credit to the original author, along with 
 *    this list of conditions.
 *
 * 2) Redistributions in binary form, compiled from this source code and/or 
 *    modified/adapted versions of this source code, must include this copyright 
 *    notice giving credit to the original author, along with this list of conditions 
 *    in the documentation and other materials provided with the
 *    distribution.
 *
 * 3) The original author shall not held for any loss arising from using this code.
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>			//for ioremap
#include <linux/ioport.h>
#include <linux/slab.h>			//for allocating memory
#include <linux/spi/spi.h>		//defines all the structures used by the spi-subsytem
#include <linux/platform_device.h>	//Defines structure for using the platform-bus
#include <linux/err.h>

#define DRIVER_NAME "pru0_spi_vc"

struct pru0_spi {
	struct spi_master *master;
	void *Data_pointer_mosi;
	void *Data_pointer_miso;
	void *flag_mosi;
	void *flag_miso;
	void *spi_cs;
	void *spi_lsb_first;
	void *spi_cpol;
	void *spi_cpha;
};

static int pru0_spi_setup(struct spi_device *spi)
{
	struct pru0_spi *pru0 = spi_master_get_devdata(spi->master);
	uint8_t spi_cs_val;
	uint8_t spi_lsb_first_val;
	uint8_t spi_cpol_val;
	uint8_t spi_cpha_val;

	if (!(spi->mode & SPI_CS_HIGH)) {
		spi_cs_val = 0;
		iowrite8(spi_cs_val, pru0->spi_cs);	//normal cs ,i.e low on active
	} else {
		spi_cs_val = 0x1;
		iowrite8(spi_cs_val, pru0->spi_cs);
	}

	if (spi->mode & SPI_LSB_FIRST) {
		spi_lsb_first_val = 1;
		iowrite8(spi_lsb_first_val, pru0->spi_lsb_first);	//lsb first tranfer will take place
	} else {
		spi_lsb_first_val = 0;
		iowrite8(spi_lsb_first_val, pru0->spi_lsb_first);	//msb first tranfer will take place
	}
	if (spi->mode & SPI_CPOL) {
		spi_cpol_val = 0x1;
		iowrite8(spi_cpol_val, pru0->spi_cpol);
	} else {
		spi_cpol_val = 0;
		iowrite8(spi_cpol_val, pru0->spi_cpol);
	}
	if (spi->mode & SPI_CPHA) {
		spi_cpha_val = 0x1;
		iowrite8(spi_cpha_val, pru0->spi_cpha);
	} else {
		spi_cpha_val = 0;
		iowrite8(spi_cpha_val, pru0->spi_cpha);
	}

	return 0;
}

static int pru0_spi_transfer_one(struct spi_master *master,
				 struct spi_device *spi, struct spi_transfer *t)
{
	const uint8_t *tx_buf = t->tx_buf;
	uint8_t *rx_buf = t->rx_buf;
	struct pru0_spi *pru0 = spi_master_get_devdata(master);
	uint8_t mosi_transfer = *tx_buf;
	uint8_t mosi_flag_val = 0x1;
	uint8_t miso_flag_val = 0;
	uint8_t miso_flag_val_ret;
	void *mosi = pru0->Data_pointer_mosi;
	void *miso = pru0->Data_pointer_miso;
	void *mosi_flag = pru0->flag_mosi;
	void *miso_flag = pru0->flag_miso;
	miso_flag_val_ret=ioread8(miso_flag);
	if (tx_buf != NULL) {
		iowrite8(mosi_transfer, mosi);
		iowrite8(mosi_flag_val, mosi_flag);	//set value for the flag to 1 
	}
	while(!miso_flag_val_ret);
	if (miso != NULL) {
		*rx_buf = ioread8(miso);
		 iowrite8(miso_flag_val, miso_flag);	//set value for the flag back to 0 
	}

	return 0;
}

static int pru0_spi_remove(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct pru0_spi *pru0 = spi_master_get_devdata(master);

	pr_info("pru0_spi_remove called\n");
	release_mem_region(0x4a310000, 56);
	pr_info("pru0_spi_remove called\n");

	iounmap(pru0->Data_pointer_mosi);
	iounmap(pru0->Data_pointer_miso);
	iounmap(pru0->flag_mosi);
	iounmap(pru0->flag_miso);
	iounmap(pru0->spi_cs);
	iounmap(pru0->spi_lsb_first);
	iounmap(pru0->spi_cpol);
	iounmap(pru0->spi_cpha);

	pr_info("pru0_spi_remove called\n");
	return 0;
}

static int pru0_spi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct pru0_spi *pru0;
	int status;

	pr_info("pru0_spi_probe called\n");

	master = spi_alloc_master(&pdev->dev, sizeof(struct pru0_spi));
	if (!master) {
		printk(KERN_INFO "Master Allocation Failed\n");
		return -ENODEV;
	}

	master->bus_num = 3;
	master->num_chipselect = 1;
	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH | SPI_LSB_FIRST;	//MODE BITS understood by this driver
	master->bits_per_word_mask = SPI_BPW_RANGE_MASK(8, 8);
	master->max_speed_hz = 50000000;
	master->setup = pru0_spi_setup;
	master->dev.of_node = pdev->dev.of_node;
	master->transfer_one = pru0_spi_transfer_one;

	platform_set_drvdata(pdev, master);

	pru0 = spi_master_get_devdata(master);

	request_mem_region(0x4a310000, 56, "Data");
	pru0->Data_pointer_mosi = ioremap(0x4a310000, 8);
	pru0->Data_pointer_miso = ioremap(0x4a310000 + 8, 8);
	pru0->flag_mosi = ioremap(0x4a310000 + 16, 8);
	pru0->flag_miso = ioremap(0x4a310000 + 24, 8);
	pru0->spi_cs = ioremap(0x4a310000 + 32, 8);
	pru0->spi_lsb_first = ioremap(0x4a310000 + 40, 8);
	pru0->spi_cpol = ioremap(0x4a310000 + 48, 8);
	pru0->spi_cpha = ioremap(0x4a310000 + 56, 8);

	status = devm_spi_register_master(&pdev->dev, master);
	if (status < 0) {
		printk(KERN_INFO "Master Registration Failed\n");
	}

	return status;
}

static const struct of_device_id pru0_spi_ids[] = {
	{.compatible = "beagle,pru0-spi",.data = NULL},
	{},
};

//struct to register with the platform bus
static struct platform_driver pru0_spi_driver = {
	.probe = pru0_spi_probe,
	.remove = pru0_spi_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = pru0_spi_ids,
	},
};

module_platform_driver(pru0_spi_driver);	//Macro to register the Driver With the Platform bus

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV CHOUDHARY");
MODULE_DESCRIPTION("Driver for Bitbanging SPI on the PRU");
