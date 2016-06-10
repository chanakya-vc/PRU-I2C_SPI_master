#include <stdint.h>
#include "resource_table_empty.h"
#include <pru_cfg.h>
#define   P8_11  15
#define   CLK  14
volatile register uint32_t __R30;
//volatile register uuint8_t32_t __R31
uint8_t CPOL=0,CPHA=0;
uint8_t i=0;


 void main()
{
	volatile uint8_t mosi ,miso;
	//Set the CFG Register to direct output
	CT_CFG.GPCFG0 = 0;
	//clear the r30 register
	__R30=0x0000;
	//setmosi to test data
	mosi=0X6;
	CPOL=1;
	CPHA=1;
	//set value of the clock_pol_pha variable between 0 and 3
	uint8_t clock_pol_pha=(CPOL*2)+CPHA;
	if (clock_pol_pha>=2)
	{
		//set the clock 1 to be in its idle state
		__R30^=(1<<CLK);
	}

	switch(clock_pol_pha){
		case 0:	

			for ( i=0;i<8;i++)
			{
				//set clk 1
				__R30^=(1<<CLK);
				//write data to it
				if ((mosi<<(7-i))&0x80)
				{
					__R30|=(1<<P8_11);
				}
				//set clk 0
				__R30^=(1<<CLK);
				//Reset the register
				__R30=0x0000;
			}
		    break;
		case 1:
			for ( i=0;i<8;i++)
			{
				//set clk 1
				__R30^=(1<<CLK);
				//set clk 0
				__R30^=(1<<CLK);
				//write data on the falling edge
				if ((mosi<<(7-i))&0x80)
				{
					__R30|=(1<<P8_11);
				}
				//Reset the registers
				__R30=0x0000;

		    }
		    break;
			                  
		case 2:
			for ( i=0;i<8;i++)
			{
				//set clk 0 to set it in active state
				__R30^=(1<<CLK);
				//write data on the falling edge
				if ((mosi<<(7-i))&0x80)
				{
					__R30|=(1<<P8_11);
				}
				//set clk 1 to idle state
				__R30^=(1<<CLK);
				
				//Reset the data pin in the register to 0
				__R30&=(0<<P8_11);
				//shift data in Mosi


		    }
		    break;
		case 3:
			for ( i=0;i<8;i++)
			{
				//set clk 0 to set it in active state
				__R30^=(1<<CLK);
				//set clk 1 to idle state
				__R30^=(1<<CLK);
				//write data on the rising edge
				if ((mosi<<(7-i))&0x80)
				{
					__R30|=(1<<P8_11);
				}
				//Reset the data pin in the register to 0
				__R30&=(0<<P8_11);
				//shift data in Mosi
			}

		    break;
 		default:
 			break;


 }

__halt();
}
