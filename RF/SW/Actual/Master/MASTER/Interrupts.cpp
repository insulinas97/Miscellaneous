#include "sam.h"
#include "RTT.h"
#include "interrupts.h"
#include "systemLib.h"

extern bool isTelemetriaActiva;

volatile uint8_t RTT_ISR = 0;

void rtt_telemetria(void)
{
	//uint32_t ul_previous_time;
	
	/* Clear SR */
	rtt_get_status(RTT);
	
	
	/* Configure RTT */
	rtt_sel_source(RTT, false);
	rtt_init(RTT, RETARDO_TELEMETRIA / 1000.0 * 0x8000u);
	
	//ul_previous_time = rtt_read_timer_value(RTT);
	//while (ul_previous_time == rtt_read_timer_value(RTT));

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_RTTINCIEN);
	rtt_enable(RTT);
	
	RTT_ISR |= RTT_TELEMETRIA;
}

void RTT_Handler(void)
{
	uint32_t ul_status;
	
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN);
	rtt_disable(RTT);	

	/* Get RTT status */
	ul_status = rtt_get_status(RTT);

	/* Time has changed, refresh display */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) 
	{
		if(RTT_ISR & RTT_TELEMETRIA)
		{
			isTelemetriaActiva = true;
			RTT_ISR &= (~RTT_TELEMETRIA);
		}
	}

	/* Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) 
	{
		// No hacemos nada con las alarmas en este codigo
	}
}