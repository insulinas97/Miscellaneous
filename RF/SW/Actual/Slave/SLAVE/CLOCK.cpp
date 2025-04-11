#include "sam.h"

#define RC 0
#define XTAL 1
#define OSC 2

void CLK_Init(uint8_t source, uint8_t fsource, uint8_t num, uint8_t den)
{
	// Disable watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	//Embedded Flash Wait States for Worst-Case Conditions
	EFC0->EEFC_FMR = EEFC_FMR_FWS(7) | EEFC_FMR_CLOE;
	
	#if defined(ID_EFC1)
	EFC1->EEFC_FMR = EEFC_FMR_FWS(7) | EEFC_FMR_CLOE;
	#endif
	switch(source)
	{
		case RC:
		
		//Enable Fast RC oscillator
		REG_CKGR_MOR |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN;
		PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN;
		
		/* Wait the Fast RC to stabilize */
		while(!(REG_PMC_SR & PMC_SR_MOSCRCS));
		
		/* Change Fast RC oscillator frequency */
		switch(fsource)
		{
			case 4:
			REG_CKGR_MOR = (REG_CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk) |
			CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCF_4_MHz;
			break;
			
			case 8:
			REG_CKGR_MOR = (REG_CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk) |
			CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCF_8_MHz;
			break;
			
			default:
			REG_CKGR_MOR = (REG_CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk) |
			CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCF_12_MHz;
			break;
		}
		
		/* Wait the Fast RC to stabilize */
		while(!(REG_PMC_SR & PMC_SR_MOSCRCS));
		
		/* Switch to Fast RC */
		REG_CKGR_MOR = (REG_CKGR_MOR & ~CKGR_MOR_MOSCSEL) |
		CKGR_MOR_KEY_PASSWD;
		break;
		
		case XTAL:
		
		//Enable main crystal oscillator with the largest value of startup time
		REG_CKGR_MOR |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCXTST(0xff);
		while(!(REG_PMC_SR & PMC_SR_MOSCXTS));
		
		//Switch MAINCK to the main crystal oscillator
		REG_CKGR_MOR |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCSEL;
		while(!(REG_PMC_SR & PMC_SR_MOSCSELS));
		
		//Disable the on-chip fast RC oscillator
		REG_CKGR_MOR = (REG_CKGR_MOR & ~CKGR_MOR_MOSCRCEN) | CKGR_MOR_KEY_PASSWD;
		break;
		
		case OSC:
		
		//Bypass crystal and use oscillator
		REG_CKGR_MOR |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTBY;
		
		//Switch MAINCK to the main oscillator
		REG_CKGR_MOR |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCSEL;
		while(!(REG_PMC_SR & PMC_SR_MOSCSELS));
		
		//Disable the on-chip fast RC oscillator
		REG_CKGR_MOR = (REG_CKGR_MOR & ~CKGR_MOR_MOSCRCEN) | CKGR_MOR_KEY_PASSWD;
		break;
	}
	if(num == 0 || den == 0)
	{
		// Select MCK and HCLK
		// CSS: 0 SLOW_CLK; 1 MAIN_CLK; 2 PLLA_CLK; 3 PLLB_CLK
		REG_PMC_MCKR = PMC_MCKR_CSS_MAIN_CLK;
		SystemCoreClock = fsource*1000000;
	}
	else
	{
		//Config PLLA
		REG_CKGR_PLLAR |= CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(num-1) | CKGR_PLLAR_DIVA(den) | CKGR_PLLAR_PLLACOUNT(0x3ful);
		while(!(REG_PMC_SR & PMC_SR_LOCKA));
		
		//Select MCK and HCLK
		// CSS: 0 SLOW_CLK; 1 MAIN_CLK; 2 PLLA_CLK; 3 PLLB_CLK
		REG_PMC_MCKR = PMC_MCKR_CSS_PLLA_CLK;
		SystemCoreClock = (fsource*(num)/den)*1000000;
	}
	while(!(REG_PMC_SR & PMC_SR_MCKRDY));
}


void SAMBA_reboot(void)
{
	// Wait until Flash is ready
	while(!(EFC0->EEFC_FSR & EEFC_FSR_FRDY));

	//Send Clear GPNVM bit command with argument 1 (Clear GPNVM1)
	EFC0->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_CGPB |(1 << EEFC_FCR_FARG_Pos));
	while(!(EFC0->EEFC_FSR & EEFC_FSR_FRDY));

	// Perform Software reset
	RSTC->RSTC_CR |= (RSTC_CR_KEY_PASSWD | RSTC_CR_PROCRST | RSTC_CR_PERRST);

	//Infinite loop to avoid further code execution - chip should reset
	while(true);
}