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
int read_sda(uint8_t SDA)
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
int read_scl(uint8_t SDA)
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

void main()
{

  //Set the CFG Register to direct output instead of serial output
  CT_CFG.GPCFG0 = 0;
  //clear the R30 register
  __R30=0x0000;

  //clear the R31 register
  __R31=0x0000;
}

