#include "main.h"

int main(void)
{
	uartInit();
	
	while(1)
	{
		uartStrTransmit("Hello Word!");
		if(rx_done)
		{
			uartHandleRX();
		}
	}	
}