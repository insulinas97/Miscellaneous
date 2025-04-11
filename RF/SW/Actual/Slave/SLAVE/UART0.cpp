#include "sam.h"

void UART0_Init(uint32_t baudrate)
{
	// Disable watchdog
	REG_WDT_MR |= WDT_MR_WDDIS;
	
	//configure PIO controller A  - disable means enable peripheral on pins
	//disable PIOA control of PA9 and enable peripheral on pin
	REG_PIOA_PDR |= PIO_PDR_P9;
	
	//disable PIOA control of PA10 and enable peripheral on pin
	REG_PIOA_PDR |= PIO_PDR_P10;
	REG_PIOA_ABCDSR &=  ~(PIO_ABCDSR_P9);
	REG_PIOA_ABCDSR &=  ~(PIO_ABCDSR_P10);
	
	//configure PMC UART Clock
	//enable UART0 clock
	REG_PMC_PCER0 |= PMC_PCER0_PID8;
	
	//configure baud rate
	REG_UART0_BRGR |= SystemCoreClock / (baudrate * 16);
	
	//parity
	REG_UART0_MR |= UART_MR_PAR_NO;
	
	//mode
	//normal mode default
	REG_UART0_MR |= UART_MR_CHMODE_NORMAL;
	//enable transmit
	REG_UART0_CR |= UART_CR_TXEN;
	
	//enable receive
	//REG_UART0_CR |= UART_CR_RXEN;
	
	//enable interrupt on receive
	//REG_UART0_IER |= UART_IER_RXRDY;
}

void UART0_fullInit(uint32_t baudrate)
{
	// Disable watchdog
	REG_WDT_MR |= WDT_MR_WDDIS;
	
	// Configure PIO controller A
	REG_PIOA_PDR |= PIO_PDR_P9 | PIO_PDR_P10;
	
	// Configure PMC UART Clock
	REG_PMC_PCER0 |= PMC_PCER0_PID8;
	
	// Configure baud rate
	REG_UART0_BRGR = SystemCoreClock / (baudrate * 16);
	
	// Configure Mode Register (Parity and Channel mode)
	REG_UART0_MR = UART_MR_PAR_NO | UART_MR_CHMODE_NORMAL;

	// Configure Control Register (enable/disable TX/RX)
	REG_UART0_CR |= UART_CR_TXEN | UART_CR_RXEN;

	// Enable interrupt on receive
	REG_UART0_IER |= UART_IER_RXRDY;
	
	//Enable TWI0 Interrupts
	NVIC_EnableIRQ(UART0_IRQn);
}

void UART0_byte(uint8_t data)
{
		while(!(REG_UART0_SR & UART_SR_TXRDY));	// Wait for transmitter to be ready
		REG_UART0_THR |= data;						// Put data into buffer, sends the data
}

void UART0_print(char data[])
{
	uint16_t i = 0;
	while(data[i]) 
	{
		while(!(REG_UART0_SR & UART_SR_TXRDY));	// Wait for transmitter to be ready
		REG_UART0_THR |= data[i];					// Put data into buffer, sends the data
		i++;										// increment counter
	}
}

void UART0_println(char data[])
{
	uint16_t i = 0;
	while(data[i])
	{
		while(!(REG_UART0_SR & UART_SR_TXRDY));	// Wait for transmitter to be ready
		REG_UART0_THR |= data[i];					// Put data into buffer, sends the data
		i++;										// increment counter
	}
	i = 0;
	char c[] = "\r\n";
	while(c[i])
	{
		while(!(REG_UART0_SR & UART_SR_TXRDY));	// Wait for transmitter to be ready
		REG_UART0_THR |= c[i];					// Put data into buffer, sends the data
		i++;										// increment counter
	}
}

char* itoa(int value, char* result, int base) 
{	
	// check that the base if valid
	if(base < 2 || base > 36) 
	{ 
		*result = '\0'; return result; 
	}

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while(value);

	// Apply negative sign
	if(tmp_value < 0) {
		*ptr++ = '-';
	}
	*ptr-- = '\0';
	while(ptr1 < ptr) 
	{
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

char* ftoa(float x, char *p,int buffsize) 
{
	char *s = p + buffsize; // go to end of buffer
	uint16_t decimals;  // variable to store the decimals
	int units;  // variable to store the units (part to left of decimal place)
	if(x < 0) // take care of negative numbers
	{ 
		decimals = (int)(x * -100) % 100; // make 1000 for 3 decimals etc.
		units = (int)(-1 * x);
	} 
	else 
	{ // positive numbers
		decimals = (int)(x * 100) % 100;
		units = (int)x;
	}

	*--s = (decimals % 10) + '0';
	decimals /= 10; // repeat for as many decimal places as you need
	*--s = (decimals % 10) + '0';
	*--s = '.';

	while(units > 0) 
	{
		*--s = (units % 10) + '0';
		units /= 10;
	}
	if(x < 0) 
	{
		*--s = '-';
	} // unary minus sign for negative numbers
	return s;
}