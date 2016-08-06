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
#include <asm/io.h>		//for ioremap
#include <linux/ioport.h>
#include <linux/slab.h>		//for allocating memory
#include <linux/i2c.h>	//defines all the structures used by the spi-subsytem
#include <linux/platform_device.h>	//Defines structure for using the platform-bus
#include <linux/err.h>

#define DRIVER_NAME "pru0_I2C_vc"

struct pru0_i2c {
struct i2c_adapter *adapter;
void *slave_address;
void *sda;
void *sda_flag_write;
void *sda_flag_read;
void *read_or_write;
};
static int pru0_i2c_probe(struct platform_device *pdev)
{
	struct pru0_i2c *pru0;
	struct i2c_adapter *adapter;
	int err;
	pr_info("pru0_i2c_probe called\n");
	pru0 = devm_kzalloc(&pdev->dev, sizeof(*pru0), GFP_KERNEL);
	if(!pru0)
	{
		return -ENOMEM;
	}
	
	request_mem_region(0x4a310000, 40, "Data");
	pru0->slave_address = ioremap(0x4a310000,8);
	pru0->sda = ioremap(0x4a310000+8,8);
	pru0->sda_flag_write = ioremap(0x4a310000 + 16,8);
	pru0->sda_flag_read  = ioremap(0x4a310000 + 24,8);
	pru0->read_or_write  = ioremap(0x4a310000 + 32,8);
	platform_set_drvdata(pdev,pru0);
	adapter=&pru0->adapter;
	strlcpy(adapter->name, "PRU0-I2C", sizeof(adapter->name));
	adapter->owner = THIS_MODULE;
	adapter->algo = &pru0_i2c_algo;
    adapter->dev.of_node = pdev->dev.of_node;
    err=i2c_set_adapdata(adapter, pru0);
    if(err)
    {
    	pr_info("I2C data not set")
    	return err;
    }

return 0;
}
static struct platform_driver pru0_i2c_driver={
	.probe=pru0_i2c_probe,
	.remove=pru_i2c_remove,
	.driver={
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = pru0_i2c_ids,
	}
}

module_platform_driver(pru0_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV CHOUDHARY");
MODULE_DESCRIPTION("Driver for Bitbanging I2C on the PRU");