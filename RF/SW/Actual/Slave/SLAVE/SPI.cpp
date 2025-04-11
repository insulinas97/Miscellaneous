#include "sam.h"

void SPI_setMode(uint8_t mode, uint8_t peripheral)
{
	switch(mode)
	{
		case 0:
		SPI->SPI_CSR[peripheral] = SPI_CSR_NCPHA;
		break;
		
		case 1:
		SPI->SPI_CSR[peripheral] = 0;
		break;
		
		case 2:
		SPI->SPI_CSR[peripheral] = SPI_CSR_CPOL | SPI_CSR_NCPHA;
		break;
		
		case 3:
		SPI->SPI_CSR[peripheral]  = SPI_CSR_CPOL;
		break;
	}
}

void SPI_selectPeripheral(uint8_t peripheral)
{
	// Chose peripheral
	// This only works if SPI_MR.PS = 0
	// if SPI_MR.PS = 1 then peripheral selection is done in SPI_THR.PCS
	if (peripheral == 0)
	{
		// Choose NPCS0
		REG_SPI_MR |= SPI_MR_PCS(0b1110);
		REG_PIOA_PDR |= PIO_PDR_P11; //NPCS0
	}
	else if(peripheral == 1)
	{
		// Choose NPCS1
		REG_SPI_MR |= SPI_MR_PCS(0b1101);
		REG_PIOA_PDR |= PIO_PDR_P31; //NPCS1
		REG_PIOB_PDR |= PIO_PDR_P14; //NPCS1
	}
	else if(peripheral == 2)
	{
		//Choose NPCS2
		REG_SPI_MR |= SPI_MR_PCS(0b1011);
	}
	else if (peripheral == 3)
	{
		//Choose NPCS3
		REG_SPI_MR |= SPI_MR_PCS(0b0111);
	}
}

void SPI_Init(uint8_t mode, uint8_t peripheral, uint8_t spibitrate)
{
	// Enable peripheral clock
	REG_PMC_PCER0 |= PMC_PCER0_PID21;
	
	// Give peripheral control of pins (Chip select pins are optional)
	REG_PIOA_PDR |= PIO_PDR_P12; //MISO
	REG_PIOA_PDR |= PIO_PDR_P13; //MOSI
	REG_PIOA_PDR |= PIO_PDR_P14; //SCK
	
	REG_SPI_CR = SPI_CR_SPIDIS;
	
	// Set SPI master mode
	REG_SPI_MR = SPI_MR_MSTR;
	
	// Set fixed peripheral select(peripheral chosen in SP_MR.PCS instead of SPI_THR.PCS)
	REG_SPI_MR &= ~SPI_MR_PS;
	
	//Peripheral select, default 0
	SPI_selectPeripheral(peripheral);
	
	//set polarity and clock phase
	SPI_setMode(mode, peripheral);
	
	// Set clock generator (1 = peripheral clock rate), otherwise a divisor
	// SCBR = fperipheral clock / SPCK Bit Rate ; chip select not active after transfer
	SPI->SPI_CSR[peripheral] |= SPI_CSR_SCBR(spibitrate) | SPI_CSR_CSNAAT;
	
	// Enable SPI
	REG_SPI_CR |= SPI_CR_SPIEN;
}

void SPI_transfer(uint8_t data)
{
	// Wait for transmit register to be empty
	while(!(REG_SPI_SR & SPI_SR_TDRE));
	
	// Send data to transmit register
	REG_SPI_TDR |= data;
}

uint8_t SPI_byteExchange(uint8_t data)
{
	// Wait for transmit register to be empty
	while(!(REG_SPI_SR & SPI_SR_TDRE));
	
	// Send data to transmit register
	REG_SPI_TDR |= data;
	
	// Wait for received data to be ready to be read
	while(!(REG_SPI_SR & SPI_SR_RDRF));
	
	// Read received data
	return REG_SPI_RDR;
}