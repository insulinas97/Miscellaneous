#ifndef _INTERRUPTS
#define _INTERRUPTS

// Definiciones de posibles origenes de las interrupciones para ser procesadas en las ISR
#define RTT_TELEMETRIA (0x01u << 0)

void rtt_telemetria(void);

#endif