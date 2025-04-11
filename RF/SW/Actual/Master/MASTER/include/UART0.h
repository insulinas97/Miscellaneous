#ifndef _UART0
#define _UART0

/*
	Baudrate validos:
	2400
	4800
	9600
	14400
	19200
	28800
	38400
	57600
	76800
	115200
	230400
*/
void UART0_Init(uint32_t baudrate);

void UART0_fullInit(uint32_t baudrate);

void UART0_byte(uint8_t data);

void UART0_print(char data[]);

void UART0_println(char data[]);

char* itoa(int value, char* result, int base);

char* ftoa(float x, char *p, int buffsize);

#endif