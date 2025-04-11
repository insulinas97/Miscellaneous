#include "sam.h"

void I2C_Init()
{
	//enable i2c peripheral in PMC
	REG_PMC_PCER0 |= PMC_PCER0_PID19;
	
	//disable PIO control of pins to enable peripheral control
	REG_PIOA_PDR  |= PIO_PDR_P3;
	REG_PIOA_PDR  |= PIO_PDR_P4;
}

void I2C_Stop()
{
	REG_TWI0_CR |= TWI_CR_STOP;
}

void I2C_start_slave(uint8_t IDL)
{
	//WAIT UNTIL THE TXCOMP FLAG IS AT 1
	while(!(REG_TWI0_SR & TWI_SR_TXCOMP));
	
	//Enter Slave address
	REG_TWI0_SMR |= TWI_SMR_SADR(IDL);
	
	//disable master mode
	REG_TWI0_CR |= TWI_CR_MSDIS;
	
	//enable slave mode
	REG_TWI0_CR |= TWI_CR_SVEN;
	
	//Interrupt enabled by SCL_WS
	REG_TWI0_IER |= TWI_IER_RXRDY | TWI_IER_SCL_WS;
	
	//Enable TWI0 Interrupts
	NVIC_EnableIRQ(TWI0_IRQn);
	
	//send start
	REG_TWI0_CR |= TWI_CR_START;
}


void I2C_write(uint8_t data)
{
	//write data or slave register to THR
	REG_TWI0_THR |= data;
	
	//wait for ack
	while(!(REG_TWI0_SR & TWI_SR_TXRDY));
}

void I2C_start_master(uint8_t IDR, uint8_t mread, uint8_t cldiv, uint8_t ckdiv)
{
	//enter slave address
	REG_TWI0_MMR |= TWI_MMR_DADR(IDR);
	
	// Set clock frecuency
	// tlow = ((CLDIV x 2^CKDIV+4))+Tmclk;
	// CLDIV = ((fmclk/fclk) -4)/(2^CKDIV)
	// for 165 MHz, fclk = 400 KHz; ckdiv = 3, cldiv = 51
	REG_TWI0_CWGR |= (TWI_CWGR_CLDIV(cldiv) | TWI_CWGR_CHDIV(cldiv)| TWI_CWGR_CKDIV(ckdiv));
	
	//disable slave mode
	REG_TWI0_CR |= TWI_CR_SVDIS;
	
	//enable master mode
	REG_TWI0_CR |= TWI_CR_MSEN;
	
	if(mread == 1) //RECEIVE MODE
	{
		REG_TWI0_MMR &= ~TWI_MMR_MREAD;
	}
	else //REQUEST MODE
	{
		REG_TWI0_MMR |= TWI_MMR_MREAD;
	}
	
	//send start
	REG_TWI0_CR |= TWI_CR_START;
}

// UPDATED LIBRARIES
void I2C_master_Init(uint16_t i2cclock)
{
	//enable i2c peripheral in PMC
	REG_PMC_PCER0 |= PMC_PCER0_PID19;
	
	//disable PIO control of pins to enable peripheral control
	REG_PIOA_PDR  |= PIO_PDR_P3 | PIO_PDR_P4;
	
	// Configure Clock Waveform Generator Register
	// CLDIV = ((fmclk/fclk) -4)/(2^CKDIV); for 120 MHz, fclk = 400 KHz; ckdiv = 3, cldiv = 37
	uint8_t cldiv = ((SystemCoreClock / (i2cclock*1000))-4)/8;
	REG_TWI0_CWGR = TWI_CWGR_CLDIV(cldiv) | TWI_CWGR_CHDIV(cldiv)| TWI_CWGR_CKDIV(3);
	
	// Configure Control Register (enable/disable master/slave)
	REG_TWI0_CR = TWI_CR_SVDIS | TWI_CR_MSEN;
}

void I2C_beginTransmission(uint8_t ID)
{
	// Configure Master Mode Register
	REG_TWI0_MMR = TWI_MMR_DADR(ID);
	
	// Send start
	REG_TWI0_CR |= TWI_CR_START;
}

void I2C_beginRequestTransmission(uint8_t ID)
{
	// Configure Master Mode Register
	REG_TWI0_MMR = TWI_MMR_DADR(ID) | TWI_MMR_MREAD;
	
	// Send start
	REG_TWI0_CR |= TWI_CR_START;
}

void I2C_endTransmission()
{
	REG_TWI0_CR |= TWI_CR_STOP;
}

void I2C_endRequestTransmission(uint8_t ID)
{
	REG_TWI0_CR |= TWI_CR_STOP;
}


void I2C_slave_Init(uint8_t ID)
{
	//enable i2c peripheral in PMC
	REG_PMC_PCER0 |= PMC_PCER0_PID19;
	
	//disable PIO control of pins to enable peripheral control
	REG_PIOA_PDR  |= PIO_PDR_P3 | PIO_PDR_P4;
	
	// Configure Slave Mode Register
	REG_TWI0_SMR = TWI_SMR_SADR(ID);
	
	// Configure Control Register (enable/disable master/slave)
	REG_TWI0_CR = TWI_CR_MSDIS | TWI_CR_SVEN;

	// Interrupt enabled by SCL_WS
	REG_TWI0_IER |= TWI_IER_RXRDY | TWI_IER_SCL_WS;
	
	// Enable TWI0 Interrupts
	NVIC_EnableIRQ(TWI0_IRQn);
	
	// Send start
	REG_TWI0_CR |= TWI_CR_START;
}
