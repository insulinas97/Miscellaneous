#ifndef _SPI
#define _SPI

void SPI_setMode(uint8_t mode, uint8_t peripheral);

void SPI_selectPeripheral(uint8_t peripheral);

void SPI_Init(uint8_t mode, uint8_t peripheral, uint8_t spibitrate);

void SPI_transfer(uint8_t data);

uint8_t SPI_byteExchange(uint8_t data);

#endif