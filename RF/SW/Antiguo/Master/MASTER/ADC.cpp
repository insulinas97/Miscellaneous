#include "sam.h"

/*
 * Internal microcontroller register programming
 * to get Analog-Digital conversion
 */

void ADC_Init()
{
	// DO NOT USE CH6 AND CH7 IN ADC IF USING UART1
	// Enable ADC channel 4, channel 5 and channel 15
	REG_ADC_CHER |= ADC_CHER_CH4 | ADC_CHER_CH5;
	REG_ADC_CHER |= ADC_CHER_CH6 | ADC_CHER_CH7;
	
	// Set ADC clock to 1 MHz
	REG_ADC_ACR |= ADC_ACR_IBCTL(1);
	
	// Clock prescaler = (fcpu / (2 * adc_freq)) - 1
	uint8_t PRS = SystemCoreClock / (2 * 1E6) - 1;
	REG_ADC_MR |= ADC_MR_PRESCAL(PRS);
	
	// Enable ADC interrupts for end of conversion
	REG_ADC_IER |= ADC_IER_EOC4 | ADC_IER_EOC5 | ADC_IER_EOC6 | ADC_IER_EOC7;
	
	// Enable clock for ADC
	REG_PMC_PCER0|= PMC_PCER0_PID29;
	
	//ADC software reset
	//REG_ADC_CR |= ADC_CR_SWRST;
	//Autocalibration
	REG_ADC_CR |= ADC_CR_AUTOCAL;
	
	//enable ADC interrupts
	//NVIC_EnableIRQ(ADC_IRQn);
}
