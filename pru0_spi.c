/************************************************************************************




*************************************************************************************/
#include <stdint.h>
#include <pru_cfg.h>
#define   P8_11  15
#define   CLK  14
volatile register uint32_t __R30;
//volatile register uint32_t __R31
int CPOL=0,CPHA=0;

 void main()
{
	volatile uint32_t mosi ,miso;
	//Set the CFG Register to direct output
	CT_CFG.GPCFG0 = 0;
	//clear the r30 register
	__R30=0x0000;
	//setmosi to test data
	mosi=0X6;
	//shifting this data to the 15 place
	mosi<<=P8_11;
	scanf("%d %d",&CPOL,&CPHA);
	//set value of the choice variable between 0 and 3
	int choice=CPOL*2+CPHA;
	if(choice>=2)
	{
		//set the clock 1 to be in its idle state
		__R30^=(1<<CLK);
	}

	switch(choice){
		case 0:	

			for (int i=0;i<8;i++)
			{
				//set clk 1
				__R30^=(1<<CLK);
				//write data to it
				__R30=mosi| __R30;
				//set clk 0
				__R30^=(1<<CLK);
				//Reset the register
				__R30=0x0000;
				//shift data in Mosi
				mosi<<=1;

		    }

		case 1:
			for (int i=0;i<8;i++)
			{
				//set clk 1
				__R30^=(1<<CLK);
				//set clk 0
				__R30^=(1<<CLK);
				//write data on the falling edge
				__R30=mosi| __R30;
				
				//Reset the register
				__R30=0x0000;
				//shift data in Mosi
				mosi<<=1;

		    }
			                  
		case 2:
			for (int i=0;i<8;i++)
			{
				//set clk 0 to set it in active state
				__R30^=(1<<CLK);
				//write data on the falling edge
				__R30=mosi| __R30;
				//set clk 1 to idle state
				__R30^=(1<<CLK);
				
				//Reset the data pin in the register to 0
				__R30&=(0<<P8_11);
				//shift data in Mosi
				mosi<<=1;

		    }

		case 3:
			for (int i=0;i<8;i++)
			{
				//set clk 0 to set it in active state
				__R30^=(1<<CLK);
				//set clk 1 to idle state
				__R30^=(1<<CLK);
				//write data on the rising edge
				__R30=mosi| __R30;
				//Reset the data pin in the register to 0
				__R30&=(0<<P8_11);
				//shift data in Mosi
				mosi<<=1;

		    }


}

}