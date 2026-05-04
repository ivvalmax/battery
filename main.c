#include "main.h"
#include "uart.h"

int main(void)
{
	uartInit();
	
	while(1)
	{
		uartStrTransmit("Hello Word!");
	}	
}