#ifndef _UART1
#define _UART1

/*
	Baudrate validos
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
void UART1_Init(uint32_t baudrate);

void UART1_print(char data[]);

void UART1_println(char data[]);

char* itoa(int value, char* result, int base);

char* ftoa(float x, char *p,int buffsize);

#endif