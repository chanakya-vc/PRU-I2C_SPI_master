#include <stdint.h>
#include <pru_cfg.h>
#define   P8_11  15
#define   CLK  14
volatile register uint32_t _R30;

 void main()
{
	volatile uint32_t _MOSI;
	//Set the CFG Register to direct output
	CT_CFG.GPCFG0 = 0;
	//clear the r30 register
	_R30=0x0000;
	//set MOSI to test data
	_MOSI=0X6;
	//shifting this data to the 15 place
	_MOSI<<=P8_11;

	while(1)
	{
		//set clk 1
		_R30^=(1<<CLK);
		//write data to it
		_R30=_MOSI| _R30;
		//set clk 0
		_R30^=(0<<CLK);
		//Reset the register
		_R30=0x0000;
		//shift data in Mosi
		_MOSI<<=1;

    }              
	
}