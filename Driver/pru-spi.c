/*
 *Written by Vaibhav Choudhary under GSOC-2016 for BeagleBoard.org
 *Copyright (C) 2016 Vaibhav Choudhary 
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <asm/io.h>
uint8_t *mosi;
uint8_t *miso;
static void * Data_pointer; 
//struct dev_t stores the major and minor numbers
static dev_t device1;              
//To register as a character device in the kernel
static struct cdev spi_pru;    
//udev daemon uses this to create device files
static struct class *cl; 		   
static int openchardevice(struct inode *i, struct file *f) 	
{
    printk(KERN_INFO "Device has succesfully been opened\n");
    return 0;
}
static int closechardevice(struct inode *i, struct file *f) 
{
    printk(KERN_INFO "Device has successfully been closed\n");
    return 0;
}
/*void allocate_mem_ioremap(void)
{	//Allocate memory for I/O.
	struct resource *request_mem_region(0x4a310000 0x3000, 8,"Data");
	//Ioremap returns a virtual address in Data_pointer.
	Data_pointer=ioremap(0x4a310000 0x3000, 0X8);
	//Allocate memeory to *mosi
	mosi=kmalloc(sizeof(uint8_t), GFP_KERNEL);
}*/
static ssize_t spi_write(struct file *filp,const char __user *buf, size_t count,loff_t *f_pos)
{
	copy_from_user(mosi,buf,count);
	uint8_t mosi_transfer=*mosi;
	iowrite8(mosi_transfer,Data_pointer);
}
static ssize_t spi_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos)
{
	uint8_t miso_transfer=ioread8(Data_pointer);
	*miso=miso_transfer;
	int len =sizeof(miso_transfer);
	copy_to_user(buf,miso,len);
}

//fileoperations structure defines the char driver's method
static struct file_operations file_ops =			
{
    .owner = THIS_MODULE,
    .open = openchardevice,
    .release = closechardevice,
    .read=spi_read,
    .write=spi_write,
};

static int __init spi_init(void)
{
	struct device *dev_ret;
	//Allot Major and Minor number to the device file
	if ( alloc_chrdev_region(&device1, 0, 1, "spi_pru") < 0)
    {
        printk(KERN_INFO "No major and minor numbers were assigned");
        return -1;
    }
 	//Initialize the cdev structure with file operation structure
    cdev_init(&spi_pru, &file_ops);
 	//Tell the kernel about the char device structure
    if (cdev_add(&spi_pru, device1, 1) < 0)
    {
        printk(KERN_INFO "Device has not been succesfully registered in the kernel");
        return -1;
    }
    //Creates class cl that will be populated by Kernel 
    cl = class_create(THIS_MODULE, "char1"); 
    //cl is populated for udev daemon to create the device file
    dev_ret = device_create(cl, NULL, device1, NULL, "spi_pru_device");
    //Allocate memory for I/O.
	request_mem_region(0x4a310000, 8,"Data");
	//Ioremap returns a virtual address in Data_pointer.
	Data_pointer=ioremap(0x4a310000, 8);
	//Allocate memeory to *mosi
	mosi=kmalloc(sizeof(uint8_t), GFP_KERNEL);
//   allocate_mem_ioremap();
// 	write_to_mem_ioremap()
 	return 0;
}
static void __exit spi_exit(void)
{
    device_destroy(cl, device1);
    class_destroy(cl);
    cdev_del(&spi_pru);
    unregister_chrdev_region(device1, 1);
    release_mem_region(0x4a310000, 8);
    iounmap(Data_pointer);
    Data_pointer=NULL;
    mosi=NULL;
    miso=NULL;
}
 
module_init(spi_init);
module_exit(spi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV CHOUDHARY");
MODULE_DESCRIPTION("Driver for Bitbanging SPI on the PRU");
