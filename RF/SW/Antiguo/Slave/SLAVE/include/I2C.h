void I2C_Init();

void I2C_Stop();

void I2C_start_slave(uint8_t IDL);

void I2C_write(uint8_t data);

void I2C_start_master(uint8_t IDR, uint8_t mread, uint8_t cldiv, uint8_t ckdiv);

// UPDATED LIBRARIES
void I2C_master_Init(uint16_t i2cclock);

void I2C_beginTransmission(uint8_t ID);

void I2C_beginRequestTransmission(uint8_t ID);

void I2C_endTransmission();

void I2C_endRequestTransmission(uint8_t ID);

void I2C_slave_Init(uint8_t ID);