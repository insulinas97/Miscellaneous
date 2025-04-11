#define MS 0
#define S 1

void delaySetUnit(uint8_t unit);

void delayStart();

void delayStop();

void delay(uint32_t sec);

/**
 *  \brief TC1 Interrupt handler.
 */
void TC1_Handler(void);