
#include <AltSoftSerial.h>
// Configure build so that the HW uart is accessible through this mini library.
#include <debug_uart0.h>

AltSoftSerial serial2;
int main()
{
	sei();
	UART0_Init();
	serial2.begin(9600, 7, 1, 2 );
	while(1)
	{
		if(UART0_available())
		{
			uint8_t ch=UART0_read();
			serial2.write(ch);
		}
		while(serial2.available())
		{
			UART0_Send(serial2.read());
		}
	}
	return 0;
}
