#include "sam.h"

#define MS 0
#define S 1

uint32_t CS = 0;
uint32_t US = 0;

void delaySetUnit(uint8_t unit)
{
	// PMC setup: Enable peripheral clock for timer counter channel1
	PMC->PMC_PCER0 |= 1 << ID_TC1;
	
	// Disable internal clocking while setup regs
	TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	
	// Interrupt Setup SCLK and will be trigger compare RC
	TC0->TC_CHANNEL[1].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK5 | TC_CMR_CPCTRG;
	
	// Enable interrupts in NVIC for TC0
	NVIC_EnableIRQ(TC1_IRQn);
	
	//Enable RC compare interrupt
	TC0->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;
	
	/*
	ftimer = SCLK = 32768 Hz
	ttimer = 1/ftimer  = 3.05175e-5 s
	*/
	// Set number of ttimer to get the timer
	// 1 second/3.05175e-5 = 32768
	switch(unit)
	{
		case MS:
			// REG_TC0_RC1 = 33;
			TC0->TC_CHANNEL[1].TC_RC = 33;
		break;
		case S:
			//REG_TC0_RC1 = 32768;
			TC0->TC_CHANNEL[1].TC_RC = 32768;
		break;
	}
}

void delayStart()
{
	// Enable tc clock and start timer
	TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void delayStop()
{
	// Disable tc clock
	TC0->TC_CHANNEL[1].TC_CCR |= TC_CCR_CLKDIS;
}


void delay(uint32_t sec)
{
	US = sec;
	delayStart();
	while(CS < US);
	delayStop();
	CS = 0;
}

/**
 *  \brief TC1 Interrupt handler.
 */

void TC1_Handler(void)
{
	//read status register - this clears interrupt flags
	uint32_t status = REG_TC0_SR1;
	if (status & TC_SR_CPCS)
	{
		// Increment counter
		CS++;
	}
}
