#define RC 0
#define XTAL 1
#define OSC 2

void CLK_Init(uint8_t source, uint8_t fsource, uint8_t num, uint8_t den);

void SAMBA_reboot(void);