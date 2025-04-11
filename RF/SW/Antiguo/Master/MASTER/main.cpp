/*
* main.cpp
* Author : J.Ramos P.Vazquez M.Vallina
* Agile frequency synthesizer using AD9914 and ADF4350
* Master uC
*/

#include "sam.h"
#include "clock.h"
#include "UART0.h"
#include "I2C.h"
#include "ADC.h"
#include "EP.h"
#include "systemLib.h"
#include "main.h"

/* 
	Nota preliminar de uso con la telemetria: 
	=========================================
	
	La telemetria solo se recoge mientras las etapas de potencia esten encendidas.

	En esta version del firmware, las generadoras tienen que recibir los umbrales
	directamente sin procesar en [ºC] [W] y ROE directamente como enteros sin signo.
	Las generadoras envian las telemetrias de estas magnitudes directamente como enteros sin signo, 
	salvo en el caso de la ROE, que las envía como un entero sin signo * ROE_MAX / 65535
	
	El DC-DC se activa en cuanto la generadora es alimentada.
	Cuando se activa una alarma, la etapa de potencia y el DC-DC se desactivan automaticamente.
	Sin embargo la generadora sigue emitiendo RF (por el momento)
	Para resetear la alarma, basta con volver a enviar un comando de encender etapa de potencia.
	
	Actualmente, cuando se envia un comando 7 de apagar la generadora, se apagan tambien 
	las etapas de potencia.
*/

/*
	El comando 2 esta estructurado de forma que se tienen que cumplir los siguientes requisitos
	para que la placa no se quede bloqueada y haya que resetearle la alimentacion:
	
	1-. Numero par de parametros de frecuencias (freq_inicio, freq_final). NO enviar numero impar
			puesto que no tiene sentido y se pierde la alineacion de los parametros para el parseo.
	2-. El numero de atenuaciones es el numero par de frecuencias que se acaba de mencionar / 2
	3-. El numero de parametros de frecuencias, y por lo tanto el numero de atenuaciones no puede
			ser 0 o la placa se quedara bloqueada
	4-. Los umbrales deben enviarse tal y como se menciona en la nota preliminar anterior
*/

extern float floatMediaPD, floatMediaPR, floatMediaIdc, floatMediaTemp, floatMediaROE;
extern uint8_t regAlarm;
extern bool isTelemetriaActiva;

#ifdef TEST_TELEMETRIA
extern uint16_t uintMediaPD, uintMediaPR, uintMediaIdc, uintMediaTemp;
#endif

bool lockRS232 = false;
bool lockI2C = false;

// Codigo de operacion de 3 bits del comando en procesamiento
// El siguiente comando se lee en la ISR UART solo cuando el cerrojo UART está desactivado
uint8_t command;

// Codigo de operacion de 5 bits de subcomando del comando 6.
// Este sistema esta ideado para aumentar la cantidad de comandos usables (31 comandos mas, concretamente)
// con respecto a la idea inicial de Jesus y mantener la compatibilidad de este firmware 
// con la version actual del programa de Miguel Angel. Sirva como ñapa hasta la implementacion de una
// API seria para estas generadoras
uint8_t subcommand;

// Byte leido desde UART
uint8_t readByte;

// Contador de bytes del comando en procesamiento
uint8_t commandByte = 0;

// Numero de ventanas que contiene el comando 2 que se esta procesando (si es el caso)
uint8_t W = 0;

#define FREQ_MEM_SIZE sizeof(float)
#define ATT_MEM_SIZE sizeof(uint8_t)
#define WINDOW_MEM_SIZE (2 * FREQ_MEM_SIZE + ATT_MEM_SIZE) // Frecuencias inicial y final y atenuacion
#define THRESOLDS_MEM_SIZE 3 * sizeof(uint16_t) // 3 valores de umbral
#define REG_SIZE (MAX_VENTANAS_PLACA * WINDOW_MEM_SIZE + THRESOLDS_MEM_SIZE)

// Parametros de inhibicion. En el maestro no se van a usar, son solo datos de paso que llegan
// por UART y se envian por I2C, pero se declaran aqui para ilustrar directamente el tamaño del buffer
// que los almacenara en el maestro. En un futuro, cuando se ponga el codigo que hay en comun entre
// el maestro y el esclavo, estas declaraciones se podran poner alli
//uint16_t fclock;
//uint16_t tiempoRampa;
//uint32_t tiempoBarrido;
//uint16_t parametroDwelling;
//uint8_t sentidoBarrido;

#define SIZE_PARAMS (sizeof(uint32_t) + 3 * sizeof(uint16_t) + sizeof(uint8_t))

// Buffer de almacenamiento de ventanas (Freqs inicial y final, atenuacion y umbrales)
#ifdef PARAMETROS_INHIBICION
uint8_t bufferWindows[REG_SIZE + SIZE_PARAMS];
#else
uint8_t bufferWindows[REG_SIZE];
#endif

// Buffer de almacenamiento de parametros de inhibicion (fclock, tiempoRampa, etc)
uint8_t bufferParams[SIZE_PARAMS];

// Registro de tests de alarmas para el comando 5
uint8_t alarmTest = 0;

// Registros para el comando 4, terminan siempre en 010 (el codigo de operacion) y el resto son bits
// que marcan si las ventanas estan activadas o no
uint8_t r1 = 0;
uint8_t r2 = 0;

// Umbrales. Temperatura en grados Celsius e Idc en mA
// Los umbrales por defecto estan en el maximo: 0xFFFF y no se cambian hasta que se envia el primer
// comando de enviar ventanas
uint16_t thROE = UINT16_MAX, thTemp = UINT16_MAX, thIdc = UINT16_MAX;		

int main(void)
{
	int contadorSaltosTelemetria = 0;

	// Inicializaciones
	CLK_Init(OSC, FOSC, 15, 2);
	I2C_master_Init(I2CCLOCK);
	ADC_Init();
	UART0_fullInit(BAUDRATE);

	DPIN->PIO_OER |= ENA | END; // Output
	
	// Se enciende el DC-DC al encender el sistema
	DPIN->PIO_SODR |= END; // high
	
	apagarEtapaPotencia();
	
    // Las iteraciones del bucle principal consisten solo en enviar datos por I2C al esclavo,
    // cosa que ocurre cuando han llegado comandos 2, 4 o 7
    // Cuando esto ocurre no se atiende a lo que entre por el UART
	while(true)
	{

#if !defined(MALETA_4B) && !defined(ENRACKABLE_4B) && !defined(MALETA_5B) && !defined(ENRACKABLE_5B)
		if(isTelemetriaActiva)
		{
			if(contadorSaltosTelemetria < SKIP_TELEMETRIA)
			{
				contadorSaltosTelemetria++;
			}
			else
			{
				contadorSaltosTelemetria = 0;
				procesarTelemetria();
			}
		}
#endif
		// Cerrojo i2c:
		//
		// Este cerrojo domina el bucle principal, el codigo completo solo se ejecuta
		// si el cerrojo esta activado.
		// Basicamente activar el cerrojo equivale a hacerle una iteracion al bucle principal
		//
		// Condiciones activacion:
		//		se ha terminado de recibir comando 2 completo (configurar parametros inhibicion) o
		//		se ha terminado de recibir comando 4 completo (activar modo reactivo)
		//		se ha recibido un comando 7 (modo bajo consumo)
		// Condiciones desactivacion:
		//		Tras completar una iteracion a este bucle principal del uC
		//		Es decir, tras transmitirle al esclavo cualquiera de esos 2 comandos y sus parametros
		
		if(lockI2C)
		{
			NVIC_DisableIRQ(UART0_IRQn);
			// Ultimo comando recibido desde PC o tester por puerto COM
			switch(command)
			{	
				//Set windows and/or frequencies; Jammer parameters
				case CONFIGURAR_VENTANAS:					
	
					// Envio de configuracion de parametros de inhibicion al esclavo
					I2C_beginTransmission(IDR1);
					I2C_write(W << 3 | CONFIGURAR_VENTANAS);
					for(uint8_t i = 0; i < W; i++)
					{
						for(uint8_t u = WINDOW_MEM_SIZE * i; u < WINDOW_MEM_SIZE * (i + 1); u++)
						{
							I2C_write(bufferWindows[u]);
						}
					}					
									
#ifdef PARAMETROS_INHIBICION

					// Nos saltamos los bytes de los umbrales
					for(uint8_t i = WINDOW_MEM_SIZE * W + THRESOLDS_MEM_SIZE; 
						i < WINDOW_MEM_SIZE * W + THRESOLDS_MEM_SIZE + SIZE_PARAMS; i++)
					{
						I2C_write(bufferWindows[i]);
					}
#endif
					// Fin
					I2C_endTransmission();
					
					thROE = bufferWindows[WINDOW_MEM_SIZE * W] << 8 | bufferWindows[WINDOW_MEM_SIZE * W + 1];
					thTemp = bufferWindows[WINDOW_MEM_SIZE * W + 2] << 8 | bufferWindows[WINDOW_MEM_SIZE * W + 3];
					thIdc = bufferWindows[WINDOW_MEM_SIZE * W + 4] << 8 | bufferWindows[WINDOW_MEM_SIZE * W + 5];
					
				break;
				case CONMUTAR_VENTANAS: // Envio de Activacion/Desactivacion ventanas al esclavo
					I2C_beginTransmission(IDR1);
					I2C_write(r1);
					I2C_write(r2);
					I2C_endTransmission();
				break;
				case MISC:
					switch(subcommand)
					{
						case JAMMING_PARAMETERS_OP:
							I2C_beginTransmission(IDR1);
							I2C_write(JAMMING_PARAMETERS_OP << 3 | MISC);
							for(uint8_t i = 0; i < SIZE_PARAMS; i++)
							{
								I2C_write(bufferParams[i]);
							}
							I2C_endTransmission();
						break;
					}
					
				break;
				case APAGAR_PLACA: // Envio de señal de modo bajo consumo al esclavo
					I2C_beginTransmission(IDR1);
					I2C_write(APAGAR_PLACA);
					I2C_endTransmission();
				break;
			}
			lockI2C = false;
			NVIC_EnableIRQ(UART0_IRQn);
		}
		else
		{
			asm("nop");
		} // FUNCTION TO AVOID ESCAPE WHILE(TRUE) LOOP ??
	}
}

/**
 *  \brief UART0 Interrupt handler.
 */
void UART0_Handler(void) 
{
	// when we receive a byte, transmit that byte back
	uint32_t status = REG_UART0_SR;
	if(status & UART_SR_RXRDY)
	{
		// read receive holding register
		readByte = REG_UART0_RHR;
		
		// Cerrojo UART:
		//
		// Como hay comandos con 1 byte y otros comandos (2 y 4) que tienen mas de un byte porque
		// tienen parametros, el proposito de este cerrojo es evitar que se parseen los parametros
		// como un comando
		//
		// Los nuevos comandos a ejecutar solo se leen en esta ISR
		//		cuando el cerrojo UART esta desactivado
		//
		// Condiciones activacion:
		//		1-. No está bloqueado el cerrojo UART (comms con el PC/Tester a traves de puerto COM)
		//			Y
		//				se recibe el primer byte de un comando 2 (configurar parametros inhibicion)
		//				O
		//				se recibe el primer byte de un comando 4 (activar modo reactivo)
		// Condiciones desactivacion:
		//		1-. Está bloqueado el cerrojo UART (esto es, hay un comando 2 o 4 en proceso de parseo)
		//			Y
		//				se ha terminado de recibir comando 2 completo (configurar parametros inhibicion)
		//				O
		//				se ha terminado de recibir comando 4 completo (activar modo reactivo)
		//
		
		if(lockRS232) // Segundo byte y siguientes de alguno de los comandos compuestos de varios bytes 
		{
			switch(command)
			{
				case CONFIGURAR_VENTANAS:
				
					// Configurar parametros de inhibicion
					bufferWindows[commandByte] = readByte;
					commandByte++;
					
#ifdef PARAMETROS_INHIBICION
					if(commandByte >= WINDOW_MEM_SIZE * W + THRESOLDS_MEM_SIZE + SIZE_PARAMS)
#else
					if(commandByte >= WINDOW_MEM_SIZE * W + THRESOLDS_MEM_SIZE)
#endif
					{
						commandByte = 0;
						lockRS232 = false;
						lockI2C = true; // <= Hace que se ejecute una iteracion del bucle principal
						regAlarm = 0;
					}
				break;
				case CONMUTAR_VENTANAS:
				
					// Activacion/desactivacion de tonos/ventanas de barrido
					r2 = readByte;
					lockRS232 = false;
					lockI2C = true;	// <= Hace que se ejecute una iteracion del bucle principal
				break;
				case MISC:
					switch(subcommand)
					{
						case JAMMING_PARAMETERS_OP:
							bufferParams[commandByte] = readByte;
							commandByte++;
							
							if(commandByte >= SIZE_PARAMS)
							{
								commandByte = 0;
								lockRS232 = false;
								lockI2C = true;
							}
						break;
					}
				break;
			}
		}
		else // Primer byte de cualquier comando
		{
			command = readByte & 7; // Se lee el siguiente comando y nos quedamos con los 3 lsb
			switch(command) 
			{
				case ENCENDER_ETAPA_POTENCIA: // Encender etapa de potencia del modulo
					encenderEtapaPotencia();
					UART0_byte(readByte);     // Se vuelve a enviar el comando por el COM - Eco
				break;
				case APAGAR_ETAPA_POTENCIA: // Apagar etapa de potencia del modulo
					apagarEtapaPotencia();
					UART0_byte(readByte);   // Se vuelve a enviar el comando por el COM - Eco
				break;
				case CONFIGURAR_VENTANAS: // 1er byte de configuracion de parametros de inhibicion
					W = readByte >> 3;	//N = NUMBER OF WINDOWS
					lockRS232 = true;
					UART0_byte(readByte);
				break;
				case ENVIAR_TELEMETRIA: // Solicitud de telemetria
					/* 
						Datos recuperacion de telemetria en software PC:
						ROE =
							Entero sin signo * ROE_MAX / 65535
						Temperatura =
							Entero sin signo directamente. ºC
						IDC
							Entero sin signo directamente. mA
						PD
							Entero sin signo directamente. W
						PR
							Entero sin signo directamente. W
					*/
					
					// Resolucion ROE: 0.003
					UART0_byte((uint16_t(floatMediaROE * (65535.0 / ROE_MAX)) >> 8) & 0xFF);
					UART0_byte(uint16_t(floatMediaROE * (65535.0 / ROE_MAX)) & 0xFF);
					UART0_byte((uint16_t(floatMediaTemp) >> 8) & 0xFF);
					UART0_byte(uint16_t(floatMediaTemp) & 0xFF);
					UART0_byte((uint16_t(floatMediaIdc) >> 8) & 0xFF);
					UART0_byte(uint16_t(floatMediaIdc) & 0xFF);
					UART0_byte((uint16_t(floatMediaPD) >> 8) & 0xFF);
					UART0_byte(uint16_t(floatMediaPD) & 0xFF);
					UART0_byte((uint16_t(floatMediaPR) >> 8) & 0xFF);
					UART0_byte(uint16_t(floatMediaPR) & 0xFF);
					UART0_byte(regAlarm);
					
					/* 
						El codigo que sigue solo se debe activar para hacer pruebas con el tester de telemetria. 
					*/
#ifdef TEST_TELEMETRIA
					UART0_byte((uintMediaTemp >> 8) & 0xFF);
					UART0_byte(uintMediaTemp & 0xFF);
					UART0_byte((uintMediaIdc >> 8) & 0xFF);
					UART0_byte(uintMediaIdc & 0xFF);
					UART0_byte((uintMediaPD >> 8) & 0xFF);
					UART0_byte(uintMediaPD & 0xFF);
					UART0_byte((uintMediaPR >> 8) & 0xFF);
					UART0_byte(uintMediaPR & 0xFF);
#endif
				break;
				case CONMUTAR_VENTANAS: // Activacion/Desactivacion ventanas
					lockRS232 = true;
					r1 = readByte;
				break;	
				case TEST_ALARMA: // Test alarma
					alarmTest = readByte;
				break;
				case MISC: // Antes era el comando de comprobar ID. Ahora es un comando general con subcomandos
					subcommand = readByte >> 3;
					switch(subcommand)
					{
						case ID_OP:
							UART0_byte(IDG);
						break;
						case JAMMING_PARAMETERS_OP:
							lockRS232 = true;
						break;
					}
				break;
				case APAGAR_PLACA: // Apagado de la generadora (modo bajo consumo)
					apagarEtapaPotencia(); // Apagamos ademas la etapa de potencia
					lockI2C = true;
					UART0_byte(readByte);
				break;
			}
		}
	}
}
