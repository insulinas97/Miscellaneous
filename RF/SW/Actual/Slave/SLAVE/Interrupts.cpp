#include "sam.h"
#include "RTT.h"
#include "interrupts.h"
//#include "systemLib.h"

void conmutarVentanas(void);

// tiempoBarrido: Tiempo de barridos por ventana [us]
void rtt_ventana(uint32_t tiempoBarrido)
{
	//uint32_t ul_previous_time;
	
	/* Clear SR */
	rtt_get_status(RTT);	
	
	/* Configure RTT */
	rtt_sel_source(RTT, false);
	rtt_init(RTT, tiempoBarrido / 1e6 * 0x8000u); // 0x8000 -> 32768 Hz crystal oscilator (28.4.2 pag 508 datasheet ATMEL)
	
	//ul_previous_time = rtt_read_timer_value(RTT);
	//while (ul_previous_time == rtt_read_timer_value(RTT));

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_RTTINCIEN);
	rtt_enable(RTT);
}

void RTT_Handler(void)
{
	uint32_t ul_status;
	
	//rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN);
	/* Get RTT status*/
	ul_status = rtt_get_status(RTT);

	/* Time has changed */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) 
	{
		conmutarVentanas();
	}

	/* Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) 
	{
		// No hacemos nada con las alarmas en este codigo
	}
	//rtt_enable_interrupt(RTT, RTT_MR_RTTINCIEN);
}