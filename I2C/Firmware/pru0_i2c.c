/*
 *Written by Vaibhav Choudhary under GSOC-2016 for BeagleBoard.org
 *Copyright (C) 2016 Vaibhav Choudhary -www.vaibhavchoudhary.com
 *PRU-I2C FIRMWARE
 *This code may be copied and/or modified freely according to GNU General Public  
 *License version 2 as published by the Free Software Foundation, provided   
 *the following conditions are also met:
 *1) Redistributions/adaptions of source code must retain this copyright
 *   notice on the top, giving credit to the original author, along with 
 *   this list of conditions.
 *(
  *2) Redistributions in binary form, compiled from this source code and/or 
  *   modified/adapted versions of this source code, must include this copyright 
  *   notice giving credit to the original author, along with this list of conditions 
  *   in the documentation and other materials provided with the
  *   distribution.
  *
  *3) The original author shall not held for any loss arising from using this code.
  *   This program is distributed in the hope that it will be useful,
  *   but WITHOUT ANY WARRANTY; without even the implied warranty of
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
  */
#include <stdint.h>
#include "resource_table_empty.h"
#define SDA 15 //P8_11 
#define SCL 14 //P8_12
#define SDA_READ  5 //P9_27
#define SCL_READ  3 //P9_28
volatile register uint32_t __R30;  
volatile register uint32_t __R31;
volatile uint16_t *slave_address = (volatile uint16_t *)(0x00010000);
volatile uint8_t *sda = (volatile uint8_t *)(0x00010000 +16);
volatile uint8_t *sda_flag_write = (volatile uint8_t *)(0x00010000 +24);
volatile uint8_t *sda_flag_read  = (volatile uint8_t *)(0x00010000 +32);
volatile uint8_t *read_or_write  = (volatile uint8_t *)(0x00010000 +40);
volatile uint8_t *stop_bit = (volatile uint8_t *)(0x00010000 +48);
bool started= false;
void set_scl(void);
{
  __R30 & = ~(1 << SCL); //Apply zero voltage to the base so as to turn off the transistor
}
void set_sda(void)
{
  __R30 & = ~(1 << SDA); //Apply zero voltage to the base so as to turn off the transistor
}
void clear_sda(void)
{
  __R30 |= (1 << SDA); //Apply high voltage to the base so as to pull sda line low.
}
void clear_scl(void)
{
  __R30 |= (1 << SCL); //Apply high voltage to the base so as to pull scl line low.
}
bool read_sda(void)
{
  if(__R31 & (1 << SDA_READ ))
  {
    return 1;
  }
  else
  {
    return 0;
  }     
}
bool read_scl(void)
{
  if(__R31 & (1 << SCL_READ ))
  {
    return 1;
  }
  else
  {
    return 0;
  }     
}

void start_condition(void)
{
  if(started)
  {
    set_sda();
    set_scl();
    while(read_scl()==0)
    {
      //Clock stretching
    }
  I2C_delay();
  }
  //SCL is High, pull SDA low in order to give the start bit
  clear_sda;
  clear_scl;
  I2C_delay();
  started = true;
}
void I2C_delay(void)
{
  for (int i=0;i<1000;i++)
  {
    //Added Delay to achieve minimum high and low period for SCL in Standard Mode
  }
}
void bitbang_address(uint16_t address,uint8_t read_or_write)
{
  
  start_condition();
  for (int i =0;i<7;i++)
  {
    if((address<<i) && 0x40)
    {
      set_sda();
    }
    else
    {
      clear_sda();  
    }  
    set_scl();
    while(read_scl()==0)
    {
      //Clock stretching
    }
  //Hold the SCL until the value is sampled by the slave
  I2C_delay();
  clear_scl();
  }
if(read_or_write ==1)
{
  clear_sda();
  set_scl();
  I2C_delay();
}
else
{  
  set_sda();
  set_scl();
  I2C_delay();
}

clear_scl();
clear_sda();
I2C_delay();
set_scl();
I2C_delay();
if(read_sda())
{
  bitbang_address(address,read_or_write);
}
}
int write_byte()
{
  int ack;
  start_condition();
  uint8_t sda_val=*sda;
  for(int i =0;i<8;i++)
  {
      if((sda << i) & 0x80)
        set_sda();
      else
        clear_sda();
      I2C_delay(); //Delay to match the propagation delay in sda
      set_scl();
      I2C_delay(); //Slave should read the value of changed sda
      while(read_scl()==0)
      {
        //Clock stretching
      }
      clear_scl();
  }
  set_scl();
  I2C_delay(); //Let the slave register the change in the clock
  if(read sda()==0)
    return 0;
  else
    return 1;
}
uint8_t read_byte()
{
    uint8_t sda_dat=0;
}
void main()
{

  //Set the CFG Register to direct output instead of serial output
  CT_CFG.GPCFG0 = 0;
  //clear the R30 register
  __R30=0x0000;
  //clear the R31 register
  __R31=0x0000;
  while(1)
  {
    *read_or_write=0;
    *stop_bit=0;
    *sda_flag_write=0;
    while(!(*read_or_write)); //Block until address has been written and the read/write bit has been written
  
    /*Bitbang the Address along with the read/write bit
    Currently this firmware only supports 7 bit addressing
    Only bitbanging bits 0-6 of the address and the read/
    write bit
    */
    volatile uint16_t slave_address= (*slave_address);
    volatile uint8_t  read_or_write =(*read_or_write);
    bitbang_address(slave_address,read_or_write);

  
  while((*stop_bit)==0)
  {
    if(read_or_write==1) //Master wants to write Data
    {
      int ack=1;
      while(!(*sda_flag_write)); //Block until the sda value has been written to the memory  
      while(ack)                 //Continuously write the data until the slave acknowledges it
      {
        ack=write_byte();
      }
      *sda_flag_write=0;
    }
    else                //Master wants to read data
    {

      

    }
  }

  }
}
