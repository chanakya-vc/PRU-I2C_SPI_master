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
struct i2c_adapter adapter;
void *slave_address;
void *sda;
void *sda_flag_write;
void *sda_flag_read;
void *read_or_write;
void *stop_bit;
};
static int pru0_i2c_xfer_one_message(struct i2c_adapter *adapter, struct i2c_msg *msg,int stop)
{
	struct pru0_i2c *pru0= i2c_get_adapdata(adapter);
	//int flags;
	uint8_t sda_transfer = *(msg->buf);
	uint8_t slave_address_value = msg->addr;
	uint8_t read_or_write_value= msg->flags ;
	uint8_t sda_flag_write_val=1;
	uint8_t sda_flag_read_val=0;
	uint8_t stop_bit_val=1;
	void *address =pru0->slave_address;
	void *sda= pru0->sda;
	void *read_or_write=pru0->read_or_write;
	void *sda_write_flag =pru0->sda_flag_write;
	void *sda_read_flag =pru0->sda_flag_read;
	void *stop_bit_flag=pru0->stop_bit;
	if(msg->addr)
	{
		iowrite8(slave_address_value,address);
	}
	if(msg->len!=0 && (read_or_write_value!=1))
	{
		iowrite8(sda_transfer,sda);
		iowrite8(read_or_write_value,read_or_write);
		iowrite8(sda_flag_write_val,sda_write_flag);
	}
	else if(msg->len!=0 && read_or_write_value)
	{
		iowrite8(read_or_write_value,read_or_write);
		while(!(ioread8(sda_read_flag)));
		ioread8(sda);
		msg->buf=sda;
		iowrite8(sda_flag_read_val, sda_read_flag);
			
	}
	if(stop && (read_or_write_value!=1))
	{
		iowrite8(stop_bit_val,stop_bit_flag);
	}
	return 0;
}
static int pru0_i2c_xfer(struct i2c_adapter *adapter, struct i2c_msg msgs[],int num)
{
	int i;
	int err;
	for(i=0;i<num;i++)
	{
		err=pru0_i2c_xfer_one_message(adapter,&msgs[i],i==(num-1));
		if(err)
		{
			return err;
		}
	}
return i;
}
static u32 pru0_i2c_functionality(struct i2c_adapter *adap) //advertises functionality
{
	return I2C_FUNC_I2C;
}

static const struct i2c_algorithm pru0_i2c_algo={
	.master_xfer = pru0_i2c_xfer,
	.functionality=pru0_i2c_functionality,
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
	
	request_mem_region(0x4a310000, 48, "Data");
	pru0->slave_address = ioremap(0x4a310000,8);
	pru0->sda = ioremap(0x4a310000+8,8);
	pru0->sda_flag_write = ioremap(0x4a310000 + 16,8);
	pru0->sda_flag_read  = ioremap(0x4a310000 + 24,8);
	pru0->read_or_write  = ioremap(0x4a310000 + 32,8);
	pru0->stop_bit       = ioremap(0x4a310000 + 40,8);
	platform_set_drvdata(pdev,pru0);
	adapter = &pru0->adapter;
	strlcpy(adapter->name, "PRU0-I2C", sizeof(adapter->name));
	adapter->owner = THIS_MODULE;
	adapter->algo = &pru0_i2c_algo;
    adapter->dev.of_node = pdev->dev.of_node;
    i2c_set_adapdata(adapter, pru0);
    err=i2c_add_adapter(adapter);
    if(!(err))
    {
    	pr_info("I2C data not set");
    	return err;
    }

return 0;
}
static int pru_i2c_remove(struct platform_device *pdev)
{
	struct pru0_i2c *pru0 = platform_get_drvdata(pdev);
	i2c_del_adapter(&pru0->adapter);
	pr_info("pru0_i2c_remove called\n");
	release_mem_region(0x4a310000, 48);
	iounmap(pru0->slave_address);
	iounmap(pru0->sda);
	iounmap(pru0->sda_flag_write);
	iounmap(pru0->sda_flag_read);
	iounmap(pru0->read_or_write);
	iounmap(pru0->stop_bit);

	return 0;
}
static const struct of_device_id pru0_i2c_ids[] = {
	{.compatible = "beagle,pru0-i2c",.data = NULL},
	{},
};
static struct platform_driver pru0_i2c_driver={
	.probe=pru0_i2c_probe,
	.remove=pru_i2c_remove,
	.driver={
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = pru0_i2c_ids,
	}
};
module_platform_driver(pru0_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV CHOUDHARY");
MODULE_DESCRIPTION("Driver for Bitbanging I2C on the PRU");