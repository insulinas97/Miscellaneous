#include "sam.h"
#include "systemLib.h"
#include "interrupts.h"
#include <math.h>
#include <string.h>

/*	==========================================
	==========================================
	·Atenuaciones acoplos direccionales maleta
	==========================================
	==========================================
	
		-Modulos 1 y 2 = 50 dB
		-Modulo 3  = 2x25 = 50 dB
		-Modulo 4 = 2x20 = 40 dB
		-Modulo 5 = 2x20 = 40 dB

	=================
	=================
	-Detector ZX47-50
	=================
	=================
	
		-24 mV / dBm * [Input Power [dBm]] + 800 mV = [Sensor Voltage [mV]]
		
		=============================================================
		([Sensor Voltage [mV]] - 800 [mV]) / (-24 [mV / dBm]) = [Sensor Input Power [dBm]]
		=============================================================
		
		-Potencia en EP [dBm] = Potencia en detector [dBm] + Atenuación cadena acoplos [dB]
	
		([Sensor Voltage [mV]] - 800 [mV]) / (-24) = P_EP [dBm] - A_acoplos [dB]
	
		======================================================================
		([Sensor Voltage [mV]] - 800 [mV]) / (-24) + A_acoplos [dB] = P_EP [dBm]
		======================================================================
	
	=========================================================================
	=========================================================================
	Modelos de etapa de potencia maleta:
	-Empower 1094: 20 MHz - 520 MHz	Gen. 1
	-Empower 1119: 500 MHz - 2500 MHz Gen. 2 y 3
	=========================================================================
	=========================================================================
	
		Datasheet:
		·Current Monitoring
		   50 mV / 100 mA * [Current [mA]] = [Sensor Voltage [mV]] =>
	   
		   ==========================================
		   [Sensor Voltage [mV]] * 2 = [Current [mV]]
		   ==========================================
	   
			   Min = 0V -> 0A
			   Max = 3.3V -> 6.6A
		   Nota: La etapa puede consumir más de 6.6A pero como estamos usando un ADC con v_ref = 3.3V, sólo
			   podremos medir con la telemetría hasta 6.6A
		   
			Margen útil de salida del sensor: 0V ~ 3.3V
			-------------------------------------------
		
		·Temp Monitoring
		   10 mV / ºC * [Temperature [ºC]] = [Sensor Voltage [mV]] =>
	   
		   ===============================================
		   [Sensor Voltage [mV]] / 10 = [Temperature [ºC]]
		   ===============================================
	   	   
			   Min = 0V -> 0ºC
			   Max = 3.3V -> 330ºC
			   Degradacion termica en 85ºC -> .850 mV
			   Si Max escala = 100ºC -> 1V
			   Si Max escala = 110ºC -> 1.1V
		
			Margen útil de salida del sensor: 0V ~ 1.1V
			-------------------------------------------	

	=========================================================================
	=========================================================================
	Modelo de etapa de potencia maleta:
	-Empower 1212: 2000 MHz - 6000 MHz Gen. 4 y 5
	=========================================================================
	=========================================================================

		Datasheet:
		·Current Monitoring
			1000 mV / 10000 mA * [Current [mA]] = [Sensor Voltage [mV]] =>

			==========================================
			[Sensor Voltage [mV]] * 10 = [Current [mA]]
			==========================================

			Min = 0V -> 0A
			Max = 3.3V -> 33A
			Max corriente soportada: 9A => Max voltaje en sensor: 0.9V

			Margen útil de salida del sensor: 0V ~ 0.9V
			-------------------------------------------

		·Temp Monitoring
			10 mV / ºC * [Temperature [ºC]] + 500mV = [Sensor Voltage [mV]] =>

			===========================================================
			([Sensor Voltage [mV]] - 500 mV) / 10 = [Temperature [ºC]]
			===========================================================

			Min = 500mV -> 0ºC
			Max = 3.3V -> 280ºC
			No hay informacion sobre degradacion termica
			Si Max escala = 100ºC -> 1.5V
			Si Max escala = 110ºC -> 1.6V

			Margen útil de salida del sensor: 0.5V ~ 1.6V											
*/

extern uint16_t thROE, thTemp, thIdc;
extern uint8_t alarmTest;

// PD = Potencia directa; PR = Potencia reflejada
float floatMediaPD = 0, floatMediaPR = 0, floatMediaIdc = 0, floatMediaTemp = 0;
float floatROE = 0, floatROEtemp = 0, floatMediaROE = 0;
bool flagROE = false, flagTemp = false, flagIdc = false;
volatile bool isTelemetriaActiva = false;
uint8_t contadorAlarmasTemp = 0, contadorAlarmasIdc = 0, contadorAlarmasROE = 0;
uint8_t regAlarm = 0;

float bufferPD[BUFFER_SIZE] = {};
float bufferPR[BUFFER_SIZE] = {};
float bufferIdc[BUFFER_SIZE] = {};
float bufferTemp[BUFFER_SIZE] = {};
float bufferROE[BUFFER_SIZE] = {};

void encenderEtapaPotencia()
{
#if defined(MALETA_1) || defined(MALETA_2) || defined(MALETA_3)
	DPIN->PIO_CODR |= ENA;
#else
	DPIN->PIO_SODR |= ENA;
#endif
	DPIN->PIO_SODR |= END;

	// Reseteo de alarmas
	flagROE	= false;
	flagTemp = false;
	flagIdc = false;
	regAlarm = 0;
	
	contadorAlarmasTemp = 0; 
	contadorAlarmasIdc = 0;
	contadorAlarmasROE = 0;
	
	// Limpieza de bufferes
	memset(bufferPD, 0.0, sizeof bufferPD);
	memset(bufferPR, 0.0, sizeof bufferPR);
	memset(bufferIdc, 0.0, sizeof bufferIdc);
	memset(bufferTemp, 0.0, sizeof bufferTemp);
	memset(bufferROE, 0.0, sizeof bufferROE);
	
	// Activacion de timer de telemetria
	rtt_telemetria();	
}

void apagarEtapaPotencia()
{
#if defined(MALETA_1) || defined(MALETA_2) || defined(MALETA_3)
	DPIN->PIO_SODR |= ENA;
#else
	DPIN->PIO_CODR |= ENA;
#endif

	// Desactivacion de telemetria
	isTelemetriaActiva = false;
}

/*
 * Comprobacion de alarmas
 */
void alarmSet()
{	
	if(alarmTest == 0)	//Alarm test not enabled
	{
		if(floatMediaROE > thROE)
		{
			contadorAlarmasROE++;
			if(contadorAlarmasROE >= UMBRAL_ALARMAS)
			{
				flagROE = true;					
			}			
		}
		if(floatMediaTemp > thTemp)
		{
			contadorAlarmasTemp++;
			if(contadorAlarmasTemp >= UMBRAL_ALARMAS)
			{
				flagTemp = true;
			}
		}
		if(floatMediaIdc > thIdc)
		{
			contadorAlarmasIdc++;
			if(contadorAlarmasIdc >= UMBRAL_ALARMAS)
			{
				flagIdc = true;
			}
		}
	}
	else	//Alarm test enabled
	{
		flagROE = (alarmTest >> 7) & 1;
		flagTemp = (alarmTest >> 6) & 1;
		flagIdc = (alarmTest >> 5) & 1;
		regAlarm = flagROE << 2 | flagTemp << 1 | flagIdc;
		alarmTest = 0;
	}
	if(flagROE || flagTemp || flagIdc)
	{
		// En los estandares actuales de C, se entiende que un bool true es 1
		regAlarm = flagROE << 2 | flagTemp << 1 | flagIdc;
		DPIN->PIO_CODR |= END;
		apagarEtapaPotencia();
	}
}

/*
	El buffer no se debe llevar como una lista enlazada por el overhead de memoria y la propension a errores del
	manejo dinamico de memoria con punteros. La clase vector no parece que vaya a funcionar en este entorno.
	Tampoco se debe usar un array e ir moviendo elementos porque exigiria copias constantes de N - 1 elementos 
	en un array de N elementos si se quiere	eliminar el elemento mas antiguo que hay introducido en el buffer 
	e introducir el nuevo.
	Lo que se hace es coger un puntero al elemento mas antiguo que hay en el buffer (que se implementara como un
	array de C, es decir estatico). Cuando llegue el momento de eliminar un elemento del buffer para añadir uno
	nuevo, se deberá eliminar el que es apuntado por el puntero, añadir en su lugar el elemento nuevo y mover a
	la derecha el puntero (con wrap up una vez que se llegue al elemento mas a la derecha (con el indice mayor)
	De esta manera nos evitamos movimientos costosos de memoria y el unico overhead en la memoria es la de un
	puntero. (O quizá, más que usar un puntero valga con guardar el valor de indice)
*/
void insertarTelemetriaEnBuffer(float buffer[], float telemetria, int * bufferBottom)
{
	buffer[*bufferBottom] = telemetria;
	(*bufferBottom)++;
	if((*bufferBottom) >= BUFFER_SIZE)
	{
		*bufferBottom = 0;
	}
}

float mediaMovil(float buffer[])
{
	float acumulador = 0;
	
	for(int i = 0; i < BUFFER_SIZE; i++)
	{
		acumulador += buffer[i];
	}
	
	return acumulador / BUFFER_SIZE;
}

#ifdef TEST_TELEMETRIA
void insertarTelemetriaEnBufferUint(uint16_t buffer[], uint16_t telemetria, int * bufferBottom)
{
	buffer[*bufferBottom] = telemetria;
	(*bufferBottom)++;
	if((*bufferBottom) >= BUFFER_SIZE)
	{
		*bufferBottom = 0;
	}
}

uint16_t mediaMovilUint(uint16_t buffer[])
{
	uint16_t acumulador = 0;
	
	for(int i = 0; i < BUFFER_SIZE; i++)
	{
		acumulador += buffer[i];
	}
	
	return acumulador / BUFFER_SIZE;
}
#endif

/*
	Mejora a implementar es el envio de toda la telemetria
	aprovechando todo el margen de la linea (16 bits)
	Esto ya esta implementado con el ROE, pero se puede hacer
	tambien con la corriente, la temperatura y las potencias
	acordando un margen util de estas magnitudes y haciendo que los limites
	de este margen coincidan con el margen 0-65535 de los 16 bits
*/
void procesarTelemetria()
{	
	static int bufferPDBottom = 0;
	static int bufferPRBottom = 0;
	static int bufferIdcBottom = 0;
	static int bufferTempBottom = 0;
	static int bufferROEBottom = 0;

#ifdef TEST_TELEMETRIA
	static uint16_t uintMediaPD = 0, uintMediaPR = 0, uintMediaIdc = 0, uintMediaTemp = 0;
	static uint16_t bufferUintPD[BUFFER_SIZE] = {};
	static uint16_t bufferUintPR[BUFFER_SIZE] = {};
	static uint16_t bufferUintIdc[BUFFER_SIZE] = {};
	static uint16_t bufferUintTemp[BUFFER_SIZE] = {};

	static int bufferUintPDBottom = 0;
	static int bufferUintPRBottom = 0;
	static int bufferUintIdcBottom = 0;
	static int bufferUintTempBottom = 0;
#endif

	uint16_t uintPD = 65535, uintPR = 65535, uintIdc = 65535, uintTemp = 65535;
	
	// PD = Potencia directa; PR = Potencia reflejada
	float floatPD = 0, floatPR = 0, floatIdc = 0, floatTemp = 0;
	
	REG_ADC_CR |= ADC_CR_START;
	while (!(REG_ADC_ISR & ADC_IMR_EOC4));
	while (!(REG_ADC_ISR & ADC_IMR_EOC5));
	while (!(REG_ADC_ISR & ADC_IMR_EOC6));
	while (!(REG_ADC_ISR & ADC_IMR_EOC7));
					
	uintPD = ADC->ADC_CDR[4];
	uintPR = ADC->ADC_CDR[5];
	uintIdc = ADC->ADC_CDR[6];
	uintTemp = ADC->ADC_CDR[7];
					
	floatPD = uintPD * ANALOG_REF / 4096; // 12 bit ADC, 806 uV resolucion
	floatPR = uintPR * ANALOG_REF / 4096;
	floatIdc = uintIdc * ANALOG_REF / 4096;
	floatTemp = uintTemp * ANALOG_REF / 4096;
					
#if defined(MALETA_1) || defined(MALETA_2) || defined(MALETA_3)

	//([Sensor Voltage [mV]] - 800 [mV]) / (-24 [mv / dBm]) + A_acoplos [dB] = P_EP [dBm]
	//([Sensor Voltage [mV]] - 800 [mV]) / (-24 [mv / dBm]) + 50 [dB] = P_EP [dBm]
	floatPD = (-floatPD * 41.7e-3 + 83.3);
					
	//([Sensor Voltage [mV]] - 800 [mV]) / (-24 [mv / dBm]) + A_acoplos [dB] = P_EP [dBm]
	//([Sensor Voltage [mV]] - 800 [mV]) / (-24 [mv / dBm]) + 50 [dB] = P_EP [dBm]
	floatPR = (-floatPR * 41.7e-3 + 83.3);
					
	// [Sensor Voltage [mV]] * 2 = [Current [mA]]
	floatIdc = 2 * floatIdc;
					
	// [Sensor Voltage [mV]] / 10 = [Temperature [ºC]]
	floatTemp = floatTemp / 10;
					
#elif defined(MALETA_4A) || defined(MALETA_5A)

	//([Sensor Voltage [mV]] - 800 [mV]) / (-24 [mv / dBm]) + A_acoplos [dB] = P_EP [dBm]
	//([Sensor Voltage [mV]] - 800 [mV]) / (-24 [mv / dBm]) + 40 [dB] = P_EP [dBm]
	floatPD = (-floatPD * 41.7e-3 + 73.3);

	//([Sensor Voltage [mV]] - 800 [mV]) / (-24 [mv / dBm]) + A_acoplos [dB] = P_EP [dBm]
	//([Sensor Voltage [mV]] - 800 [mV]) / (-24 [mv / dBm]) + 40 [dB] = P_EP [dBm]
	floatPR = (-floatPR * 41.7e-3 + 73.3);

	// [Sensor Voltage [mV]] * 10 = [Current [mV]]
	floatIdc = 10 * floatIdc;

	// ([Sensor Voltage [mV]] - 500 mV) / 10 = [Temperature [ºC]]
	floatTemp = (floatTemp - 500) / 10;

	/*
		Telemetrias L-TEL

		1. REVERSE OUTPUT POWER ANALOG 0 to 3,3V
			0V MIN POWER 3,3V MAX POWER
		2. TRANSMITTER OUTPUT POWER
			ANALOG 0 to 3,3V 0V MIN POWER 3,3V MAX POWER
		3. ENABLE / DISABLE TTL 0V DISABLE 3,3V	ENABLE
		4. TEMPERATURE SENSOR ANALOG 0 to 3,3V
			0 to 85ºC
		5. CURRENT MONITOR ANALOG 0 to 3,3V 0V = 0A	3,3V MAX A
		
		-ETAPA 1: 20 MHz-500 MHz
			MAX CURRENT = 6 A
		-ETAPA 2: 500 MHz-1000 MHz
			MAX CURRENT = 7 A
		-ETAPA 3: 1000 MHz-2000 MHz
			MAX CURRENT = 7 A
		-ETAPA 4: 2000 MHz-4000 MHz
			MAX CURRENT = 6.5 A
		-ETAPA 5: 4000 MHz-6000 MHz
			MAX CURRENT = 8 A
	*/	

#elif defined(ENRACKABLE_1)

	// Potencia [W]
	floatPD = floatPD * 50 / ANALOG_REF;
	floatPR = floatPR * 50 / ANALOG_REF;
	
	floatTemp = floatTemp * 85 / ANALOG_REF;

	floatIdc = floatIdc * 6000 / ANALOG_REF;
#elif defined(ENRACKABLE_2) || defined(ENRACKABLE_3)

	// Potencia [W]
	floatPD = floatPD * 50 / ANALOG_REF;
	floatPR = floatPR * 50 / ANALOG_REF;

	floatTemp = floatTemp * 85 / ANALOG_REF;
	
	floatIdc = floatIdc * 7000 / ANALOG_REF;
#elif defined(ENRACKABLE_4A)

	// Potencia [W]
	floatPD = floatPD * 50 / ANALOG_REF;
	floatPR = floatPR * 50 / ANALOG_REF;

	floatTemp = floatTemp * 85 / ANALOG_REF;

	floatIdc = floatIdc * 6500 / ANALOG_REF;
#elif defined(ENRACKABLE_5A)

	// Potencia [W]
	floatPD = floatPD * 50 / ANALOG_REF;
	floatPR = floatPR * 50 / ANALOG_REF;

	floatTemp = floatTemp * 85 / ANALOG_REF;

	floatIdc = floatIdc * 8000 / ANALOG_REF;
#endif
	
	insertarTelemetriaEnBuffer(bufferTemp, floatTemp, &bufferTempBottom);
	insertarTelemetriaEnBuffer(bufferIdc, floatIdc, &bufferIdcBottom);

#if defined(MALETA_1) || defined(MALETA_2) || defined(MALETA_3) || \
	defined(MALETA_4A) || defined(MALETA_5A)
	
	// Al guardar en el buffer, se pasa la potencia a [W] para hacer una media con sentido fisico
	insertarTelemetriaEnBuffer(bufferPD, pow(10, floatPD / 10) / 1000, &bufferPDBottom);
	insertarTelemetriaEnBuffer(bufferPR, pow(10, floatPR / 10) / 1000, &bufferPRBottom);
#elif defined(ENRACKABLE_1) || defined(ENRACKABLE_2) || defined(ENRACKABLE_3) || \
	defined(ENRACKABLE_4A) || defined(ENRACKABLE_5A)
	
	// Potencia en [mW]
	insertarTelemetriaEnBuffer(bufferPD, floatPD, &bufferPDBottom);
	insertarTelemetriaEnBuffer(bufferPR, floatPR, &bufferPRBottom);
#endif

	floatMediaTemp = mediaMovil(bufferTemp);
	floatMediaIdc = mediaMovil(bufferIdc);
	floatMediaPD = mediaMovil(bufferPD);
	floatMediaPR = mediaMovil(bufferPR);

#ifdef TEST_TELEMETRIA
	insertarTelemetriaEnBufferUint(bufferUintTemp, uintTemp, &bufferUintTempBottom);
	insertarTelemetriaEnBufferUint(bufferUintIdc, uintIdc, &bufferUintIdcBottom);
	insertarTelemetriaEnBufferUint(bufferUintPD, uintTemp, &bufferUintPDBottom);
	insertarTelemetriaEnBufferUint(bufferUintPR, uintIdc, &bufferUintIdcBottom);
	uintMediaTemp = mediaMovilUint(bufferUintTemp);
	uintMediaIdc = mediaMovilUint(bufferUintIdc);
	uintMediaPD = mediaMovilUint(bufferUintPD);
	uintMediaPR = mediaMovilUint(bufferUintPR);
#endif

#if !defined(MALETA_4B) && !defined(ENRACKABLE_4B) && \
	!defined(MALETA_5B) && !defined(ENRACKABLE_5B)
	
	// El valor maximo de ROE es ROE_MAX
	// En esta ecuacion la potencia es en dimensiones lineales, no en dB
	floatROEtemp = (1 + sqrt(floatMediaPR / floatMediaPD)) / (1 - sqrt(floatMediaPR / floatMediaPD));
	floatROE = floatROEtemp <= ROE_MAX ? floatROEtemp : ROE_MAX;
	insertarTelemetriaEnBuffer(bufferROE, floatROE, &bufferROEBottom);
	floatMediaROE = mediaMovil(bufferROE);
	
	alarmSet();
#endif
}