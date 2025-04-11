/*
* slave.cpp
* Author : J.Ramos P.Vazquez M.Vallina G.Izquierdo
* Agile frequency synthesizer using AD9914 and ADF4350
* Slave uC
*
* PIN AT1	 -------- pin PA6
* PIN AT2    -------- pin PB4
* PIN AT4    -------- pin PA7
* PIN AT8    -------- pin PA8
* PIN AT16   -------- pin PA5

* PIN CSLO	 -------- pin PA9
* PIN CSCLK  -------- pin PA10
* PIN SYNCIO -------- pin PA15
* PIN EN33	 -------- pin PA19
* PIN IOUPD  -------- pin PA20
* PIN EN18   -------- pin PA22
* PIN EN50   -------- pin PA23
*/


#include <math.h>
#include "sam.h"
#include "ADF4350.h"
#include "AD9914.h"
#include "CLOCK.h"
#include "UART0.h"
#include "I2C.h"
#include "SPI.h"
#include "interrupts.h"

enum status {
	OFF = 0,
	ON = 1
};

enum status alimentacion_AD4350; // Esta conectado al rail desactivable de 3V3

// Comandos
enum comando {
	ENCENDER_ETAPA_POTENCIA = 0,
	APAGAR_ETAPA_POTENCIA = 1,
	CONFIGURAR_VENTANAS = 2,
	ENVIAR_TELEMETRIA = 3,
	CONMUTAR_VENTANAS = 4,      // Comando para activar y desactivar ventanas configuadas con el comando 2
	TEST_ALARMA = 5,
	MISC = 6, // Comprobar ID   // Comando con 32 posibles subcomandos. Inicialmente usado solo para identificar las placas
	APAGAR_PLACA = 7            // Se apaga la RF y se pone la placa en modo de bajo consumo
};

// Subcomandos del comando 6
enum subcomando {
	ID_OP = 0,
	JAMMING_PARAMETERS_OP = 1
};

// Codigo de operacion de 3 bits del comando en procesamiento
uint8_t command;

// Codigo de operacion de 5 bits de subcomando del comando 6.
//	Este sistema esta ideado para aumentar la cantidad de comandos usables (31 comandos mas, concretamente)
//	con respecto a la idea inicial de Jesus y mantener la compatibilidad de este firmware
//	con la version actual del programa de Miguel Angel. Sirva como ñapa hasta la implementacion de una
//	API seria para estas generadoras
uint8_t subcommand;

#define MAX_VENTANAS_PLACA 10 

// Parametros extra para control interno y tests de inhibicion (parametro de dwelling, tiempo por barrido
// y tiempo en cada ventana.
// Tengase en cuenta con el parametro de tiempo de barrido por ventana que en cambiar de una ventana a otra
// se tardan unos 32 us.
//#define PARAMETROS_INHIBICION

// Silencio periodico en la inhibicion. Inhibe peor, se debe evitar, pero sirve para hacer
// algunas pruebas. Además el atenuador programable funciona mejor pese a no estar sincronizado
//#define SILENCIO

// Rutina para hacer pruebas de diferentes secuencias de comandos en el DDS.
// Se sustituye el comando 7 por el lanzamiento de la secuencia de prueba.
// Esta macro NO debe estar definida nunca en despliege
//#define TEST_DDS

#define BARRIDO_IDA_VUELTA 0
#define BARRIDO_SOLO_IDA 1

#define IDL 1

#define FOSC 16

#if defined(MALETA_1) || defined(ENRACKABLE_1)
#define FLOCAL 0
#endif
#if defined(MALETA_2) || defined(ENRACKABLE_2)
#define FLOCAL 0
#endif
#if defined(MALETA_3) || defined(ENRACKABLE_3)
#define FLOCAL 0
#endif
#if defined(MALETA_4A) || defined(ENRACKABLE_4A)
#define FLOCAL 4000
#endif
#if defined(MALETA_4B) || defined(ENRACKABLE_4B)
#define FLOCAL 2000
#endif
#if defined(MALETA_5A) || defined(ENRACKABLE_5A)
#define FLOCAL 3000
#endif
#if defined(MALETA_5B) || defined(ENRACKABLE_5B)
#define FLOCAL 4000
#endif

#define PCLOCK 3
#define FSTEP 1

#define ADFDEL 25      // Minimun 20ns  
#define DACCAL 100000  // 50000 ||| max = 135 us (page 6 datasheet ad9914)  
#define GPIODEL 100000 // 50000
#define DDSDEL 10

#define DPIN PIOA  // Puerto A
#define DPIN2 PIOB // Puerto B

#define AT1 PIO_PA6
#define AT2	PIO_PB4
#define AT4	PIO_PA7
#define AT8	PIO_PA8
#define AT16 PIO_PA5

#define CSLO PIO_PA9
#define CSCLK PIO_PA10

#define SYNCIO PIO_PA15
#define IOUPD PIO_PA20

#define EN18 PIO_PA22
#define EN33 PIO_PA19
#define EN50 PIO_PA23

bool lockI2C = false;
bool finProg = false;
bool off = true;

// Numero de ventanas que contiene el comando 2 que se esta procesando (si es el caso)
uint8_t W = 0;

// Byte del comando 2 que se esta procesando
uint8_t commandByte = 0;

/*
	Frecuencias en MHz
	Margen aceptable por datasheet de frecuencias de referencia de reloj: (500, 3500) [MHz]
	Frecuencia max salida del DDS f_DDS_max = fclock * 0.4 (Pag. 6 datasheet AD9914)
	Nosotros usamos un minimo de 1 GHz de f_DDS_max en todas las placas menos la 3 => 
		fclock minima = 2000 / 0.4 = 5000 para placa 3. Estamos poniendo 4200 ¿?¿?
		fclock minima = 1000 / 0.4 = 2500 para resto de placas
	Al margen de la placa 3, que habra que corregir introduciendo un LO, nos queda por lo tanto
		un margen útil de fclock = (2500, 3500) [MHz]. Hay que tener en cuenta que cuanto menor
		fclock, mayor el dwelling time a igualdad de parametro de dwelling y mas se tarda en barrer
		un margen de frecuencias a igualdad de salto (o mayor es el salto a igualdad de tiempo de barrido)
*/
uint16_t fclock = 4200; // Esta es la frecuencia que se ha venido usando anteriormente, OVERCLOCK del DDS
//uint16_t fclock = 3500;
float f1 = 0;
float f2 = 0;
uint8_t *ptrf1 = (uint8_t *)&f1;
uint8_t *ptrf2 = (uint8_t *)&f2;

uint16_t tiempoRampa = 5;	  // Tiempo de barrido de rampa [us]
uint32_t tiempoBarrido = 200; // Tiempo de barrido por ventana [us] => tiempoBarrido/tiempoRampa = 200/5 = 40 veces

// Parametro de dwelling de rampa (parametro P o N en datasheet DDS)
// Tiempo dwelling = fclock / 24 * [Parametro dwelling]
uint16_t parametroDwelling = 1;

// barrido solo de ida o barrido de ida y vuelta
uint8_t sentidoBarrido = BARRIDO_SOLO_IDA;

#define FREQ_MEM_SIZE sizeof(float) // float = 32 bits, 4 bytes => 4 
#define ATT_MEM_SIZE sizeof(uint8_t) // uint8_t = 8 bits, 1 byte => 1 
#define WINDOW_MEM_SIZE (2 * FREQ_MEM_SIZE + ATT_MEM_SIZE) // Frecuencia inicial y final
#define THRESOLDS_MEM_SIZE 3 * sizeof(uint16_t) // 3 valores de umbral
#define REG_SIZE (MAX_VENTANAS_PLACA * WINDOW_MEM_SIZE + THRESOLDS_MEM_SIZE)

#define SIZE_PARAMS (sizeof fclock + sizeof tiempoRampa + sizeof tiempoBarrido + \
	sizeof parametroDwelling + sizeof sentidoBarrido)

#ifdef PARAMETROS_INHIBICION
uint8_t bufferWindows[REG_SIZE + SIZE_PARAMS];
#else
uint8_t bufferWindows[REG_SIZE];
#endif

// Buffer de almacenamiento de parametros de inhibicion (fclock, tiempoRampa, etc)
uint8_t bufferParams[SIZE_PARAMS];

// Umbrales. Temperatura en grados Celsius e Idc en mA
// Los umbrales por defecto estan en el maximo: 0xFFFF y no se cambian hasta que se envia el primer
// comando de enviar ventanas
uint16_t thROE = UINT16_MAX, thTEMP = UINT16_MAX, thIDC = UINT16_MAX;

bool activeWindows[MAX_VENTANAS_PLACA] = {true, true, true, true, true, true, true, true, true, true};
float frequencies[MAX_VENTANAS_PLACA * 2] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t att[MAX_VENTANAS_PLACA] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


/**
*  \brief delay16b
*/
void delay16b(uint16_t val)
{
	for(uint16_t i = 0; i < val; i++)
	{
		asm("nop"); // No Operation Performed
	}
}

/**
*  \brief delay32b
*/
void delay32b(uint32_t val)
{
	for(uint32_t i = 0; i < val; i++)
	{
		asm("nop");
	}
}

/*
* ADF4350_synthetizer: This function generate a frequency between 138 MHz and 4400 MHz using ADF4350 PLL Synthetizer.
* The input parameters are defined as:
* fref : Input frequency of ADF4350 in MHz. Normally 16. Example 16
* freq : Output desired frequency in MHz. Values between 138 MHz and 4400 MHz. Example 2000
* power: Output power. 0 = -4dB ; 1 = -1dB  ; 2 = +2dB ; 3 = +5dB. Example 3
* LE   : Chip select pin of ADF4350. Example: PIO_PA7
*/
void ADF4350_synthetizer(float fref, float freq, uint8_t power, uint32_t LE) // 16, 4200
{
	// ADFDEL = SPI delay between LE and transmissions
	uint16_t MOD = 4000;	// 4000. Fractional modulus. Ratio of the PFD frequency to the channel step resolution on the RF output
	
	uint8_t twoexp = pow(2, (uint8_t)(log(4400.0 / freq) / log(2))); // Dos elevado a log en base 2 de 4400/freq 
	float fvco = freq * twoexp;
	float division = fvco / fref;
	
	uint16_t INT = division;					// Integer device of the feedback division factor. 23 to 65,535 are allowed for 4/5 prescaler. For 8/9 prescaler, the minimum integer value is 75. Range[137 for 2200MHz ; 274 for 4400MHz]
	uint16_t FRAC = (division - INT) * MOD;		// Numerator of the fraction that is input to the ?-? modulator. Values from 0 to MOD ? 1
	uint8_t DIVIDER_SELECT = log(twoexp) / log(2); // logaritmo en base 2 de twoexp				// 0 = /1 ; 1 = /2 ; 2 = /4 ; 3 = /8 ; 4 = /16
	
	// Inicializacion por secuencia de registros del sintetizador de frecuencia

	// Formulas del sintetizador: (obtenido de su datasheet pagina 11)
	// ==========================
	// f_PFD = REFin * [(1 + bit_duplicador_ref)/(factor_division_R * (1 + bit_divisor_por_2_ref))]
	// RF_out = (INT + FRAC/MOD) * f_PFD/divisor_RF
	
	// Valores que se cargan en la inicializacion en esta subrutina:
	// =============================================================
	// bit_duplicador_ref = 0;	factor_division_R = 1;	bit_divisor_por_2_ref = 0;
	// Sustituyendo esos valores en las formulas del sintetizador de frecuencia:
	// f_PFD = REFin = fref
	// REFin = fref;
	// RF_out = (INT + FRAC/MOD) * fref/DIVIDER_SELECT
	
	// Caso particular de esta generadora: (obtenido a partir de los parametros en la llamada de la interrupcion UART)
	// ===================================
	// fref = 16;		freq = 4400;		power = 3 = +5dB;		Nota: Los 16 MHz de referencia provienen de un oscilador ECS-3225SMV-160
	// fvco = 4400;		division = 275;		MOD = 4000;
	// INT = 275,		FRAC = 0;			DIVIDER_SELECT = ÷1;
	// Por tanto: RF_out = fref * INT = 4400 MHz
	// Por otra parte:  MOD = 4000. Como MOD es la relacion f_PFD/Channel_step_res, la resolucion de canal es de 4 KHz
	
	
	// Loss PLL lock REGISTER 5 
	DPIN->PIO_CODR |= LE;
	delay16b(ADFDEL);   // 20 ns minimum 
	SPI_transfer(RESERVED << 7 | RESERVED << 6 | RESERVED << 5 | \
				RESERVED << 4 | RESERVED << 3 | RESERVED << 2 | \
				RESERVED << 1 | RESERVED); // RESERVADO
	SPI_transfer(ADF4350_LD_PIN_MODE(2) << 6 | RESERVED << 5 | \
				ADF4350_RESERVED_HIGH << 4 | ADF4350_RESERVED_HIGH << 3 | \
				RESERVED << 2| RESERVED << 1 | RESERVED); // LD (Lock Detect) pin mode (1 0) -> Lock Detect  - Low Lock detect pin en low + reservado
	SPI_transfer(RESERVED << 7 | RESERVED << 6 | RESERVED << 5 | \
				RESERVED << 4 | RESERVED << 3 | RESERVED << 2 | \
				RESERVED << 1 | RESERVED); // RESERVADO
	SPI_transfer(RESERVED << 7 | RESERVED << 6 | RESERVED << 5 | \
				RESERVED << 4 | RESERVED << 3 | ADF4350_CTRL_REG5); // Register 5 - Control bits 
	delay16b(ADFDEL);   // 10 ns minimum 
	DPIN->PIO_SODR |= LE;
	
	// Configure clock divider REGISTER 4 
	DPIN->PIO_CODR |= LE;
	delay16b(ADFDEL);
	SPI_transfer(RESERVED << 7 | RESERVED << 6 | RESERVED << 5 | \
				RESERVED << 4 | RESERVED << 3 | RESERVED << 2 | \
				RESERVED << 1 | RESERVED); // RESERVADO
	SPI_transfer(ADF4350_FBCK_FUNDAMENTAL << 7 | ADF4350_DIV_SEL(DIVIDER_SELECT) << 4 | \
				(ADF4350_SEL_CLK_DIV_VALUE(255) & 0xF0) >> 4); // DIVIDER SELECT & MSB CLOCK DIVIDER VALUE: (1 1 1 1-> DB19 DB18 DB17 DB16) Clock divider: 255 
											 // Desplazamiento 4 posiciones a izquierda o multiplicacion por 16; OR con 0b10001111 0x8F
											 // Settings:	4 bits mas significativos del valor de divisor de reloj selector de banda puestos a 1: Resultado total del selector de banda: 0xFF que equivale a 255
											 //				divider select: segun lo calculado
											 //				feedback select: fundamental (es decir, no dividido)
	SPI_transfer((ADF4350_SEL_CLK_DIV_VALUE(255) & 0x0F) << 4 | \
				ADF4350_VCO_PWR_UP << 3 | ADF4350_MTLD_DISABLE << 2 | \
				ADF4350_AUX_OUT_DIVIDED << 1 | ADF4350_AUX_OUT_DISABLE); // Select LSB clock divider value: (1 1 1 1 -> DB15 DB14 DB13 DB12) Clock divider: 255 
						// Settings:	salida auxiliar desactivada
						//				salida auxiliar dividida
						//				mute till lock detect: desactivado
						//				VCO activado
						//				los 4 bits menos significativos del valor de divisor de reloj selector de banda puestos a 1: Resultado total del selector de banda: 0xFF que equivale a 255
	SPI_transfer(ADF4350_AUX_OUT_PWR(0) << 6 | ADF4350_RF_OUT_ENABLE << 5 | \
				ADF4350_OUT_PWR(3) << 3 | ADF4350_CTRL_REG4); // Power: +5 (1 1) 
								   // Desplazamiento 3 posiciones a izquierda o multiplicacion por 8; OR con 0b100100.
								   // Settings:	control bits para programacion
								   //				potencia de salida fijada en parametro
								   //				activacion de salida RF
								   //				potencia salida auxiliar RF a -4 dBm (Es indiferente porque la salida aux esta desactivada. Este valor simplemente es asignado con los bits a 0)
	delay16b(ADFDEL);
	DPIN->PIO_SODR |= LE;
	
	// Configure Clock divider REGISTER 3 
	DPIN->PIO_CODR |= LE;
	delay16b(ADFDEL);
	SPI_transfer(RESERVED << 7 | RESERVED << 6 | RESERVED << 5 | \
				RESERVED << 4 | RESERVED << 3 | RESERVED << 2 | \
				RESERVED << 1 | RESERVED); // RESERVADO
	SPI_transfer(RESERVED << 7 | RESERVED << 6 | RESERVED << 5 | \
				RESERVED << 4 | RESERVED << 3  | ADF4350_CSR_DISABLE << 2 | \
				RESERVED << 1 | (ADF4350_CLK_DIV_MODE(0) & 0b10) >> 1); // Clock divider off  - bit mas significativo del modo divisor de reloj: 0b0. Resultado total. 0b00. Equivale a divisor de reloj desactivado
						// Cycle slip reduction desactivado
	SPI_transfer((ADF4350_CLK_DIV_MODE(0) & 0b01) << 7 | (ADF4350_CLK_DIV_VALUE(150) & 0xFE0) >> 5); // 7 MSB clock divider value: 0 0 0 0 1 0 0 -> Clock divider: 150  - los 7 bits mas significantes del valor de divisor de reloj de 12-bit: 0b0000100. Resultado total: 0b000010010110 0x096 150
						// bit menos significativo del modo divisor de reloj: 0b0. Resultado total. 0b00. Equivale a divisor de reloj desactivado
	SPI_transfer((ADF4350_CLK_DIV_VALUE(150) & 0x01F) << 3 | ADF4350_CTRL_REG3); // 5 LSB clock divider value: 1 0 1 1 0 -> Clock divider: 150 
						// control bits para programacion
						// los 5 bits menos significantivos del valor de divisor de reloj de 12-bit: 0b10110. Resultado total: 0b000010010110 0x096 150
	delay16b(ADFDEL);
	DPIN->PIO_SODR |= LE;
	
	// Configure REGISTER 2 
	DPIN->PIO_CODR |= LE;
	delay16b(ADFDEL);
	SPI_transfer(RESERVED << 7 | ADF4350_LOW_NOISE_AND_SPUR_MODES(0) << 5 | \
				ADF4350_MUX_OUT(0) << 2 | \
				ADF4350_REF_DOUBLER_DISABLE << 1 | ADF4350_RDIV2_DISABLE); // Low noise mode 
						// Modo de bajo ruido (y no minimizando espureas)
						// Salida tri estado
						// Duplicador de la frecuencia referencia desactivado
	SPI_transfer((ADF4350_R_COUNTER(1) & 0b1111111100) << 2); // 8 bits mas significativos del contador R: 0x00. Resultado total: 0b0000000001. Equivale a divisor R = 1
	SPI_transfer((ADF4350_R_COUNTER(1) & 0b0000000011) << 6 | \
				ADF4350_DOUBLE_BUFF_DISABLE << 5 | ADF4350_CP_CURRENT_SET(0x7) << 1 | \
				ADF4350_LDF_FRACN); // LDF FRAC-N, 2.5mA  ajuste de corriente de la bomba de carga, doble buffer desactivado, divisor R = 1 
						// 2 bits menos significativos del contador R: 0b01. Resultado total: 0b0000000001. Equivale a divisor R = 1
						// Doble buffer desactivado
						// Bit mas significativo del ajuste de corriente de la bomba de carga: 0b0
						// 3 bits menos significativos del ajuste de corriente de la bomba de carga: 0b111. Resultado total 0b0111. Equivale a Icp = 2.5 mA (¿sobre 5K1? ¡Comprobarlo! -ver pag 16 datasheet ADF4350-)
						// LDF = FRAC-N (En vez de INT-N)
	SPI_transfer(ADF4350_LDP_10NS << 7 | ADF4350_PD_POLARIRY_POSITIVE << 6 | \
				ADF4350_PWR_DWN_DISABLE << 5 | ADF4350_CP_THREE_STATE_DISABLE << 4 | \
				ADF4350_COUNTER_RST_DISABLE << 3 | ADF4350_CTRL_REG2); // Counter reset and power-down disable, CP tri-estadp desactivada, counter reset disable, PD polarity positive, LDP = 10 ns 
						// LDP = 10 ns (En vez de 6 ns)
						// PD Polarity = positiva
						// Power-Down desactivado
						// Bomba de carga tri estado = desactivada
						// Counter reset = desactivado
						// Los 3 bits de control
	delay16b(ADFDEL);
	DPIN->PIO_SODR |= LE;
	
	// Configure REGISTER 1 
	DPIN->PIO_CODR |= LE;
	delay16b(ADFDEL);
	SPI_transfer(RESERVED << 7 | RESERVED << 6 | RESERVED << 5 | \
				RESERVED << 4  | ADF4350_PRESCALER_8_9 << 3 | \
				(ADF4350_PHASE_VALUE(1) & 0xE00) >> 9);	// Reservado
						// Prescaler 8/9
						// 3 bits mas significativos del valor de fase en 12 bit: 0b000. Resultado total: 0x001	(Es el que recomienda el datasheet)
	SPI_transfer((ADF4350_PHASE_VALUE(1) & 0x1FE) >> 1);	// 8 bits siguientes del valor de fase en 12 bit: 0x00.	Resultado total: 0x001	(Es el que recomienda el datasheet)
	SPI_transfer((ADF4350_PHASE_VALUE(1) & 0x001) << 7 | (ADF4350_MODULUS(4000) & 0xFE0) >> 5); // Bit menos significativo restante del valor de fase en 12 bit: 0b1. Resultado total: 0x001 (Es el que recomienda el datasheet).
											// Este ultimo bit se fuerza con la mascara | 128
											// Si se tiene un modulo en 12 bits (con valor max 4096, por lo tanto), se le cogen los 7 primeros bits y junto con el bit anteriormente mencionado se completa este byte
	SPI_transfer((ADF4350_MODULUS(4000) & 0x1F) << 3 | ADF4350_CTRL_REG1);    // Si se tiene un modulo en 12 bits (con valor max 4096, por lo tanto), se le cogen sus ultimos 5 bits y junto con un 0b001 como bits de control se completa el byte
										    // El 0b001 se fuerza con las 2 mascaras
	delay16b(ADFDEL);
	DPIN->PIO_SODR |= LE;
	
	// Configure REGISTER 0 
	DPIN->PIO_CODR |= LE;
	delay16b(ADFDEL);
	SPI_transfer(RESERVED << 7  | (ADF4350_INT_VALUE(INT) & 0xFE00) >> 9); // Del valor entero de 16 bit, se le cogen los primeros 7 bits y se le añade un 0b0 reservado a la izquierda
	SPI_transfer((ADF4350_INT_VALUE(INT) & 0x1FE) >> 1);  // Del valor entero de 16 bit, se le cogen los bits del 8 al 15
	SPI_transfer((ADF4350_INT_VALUE(INT) & 1) << 7 | (ADF4350_FRAC_VALUE(FRAC) & 0xFE0) >> 5); // Del valor entero de 16 bits, se le coge el ultimo. A eso se añaden los 7 primeros bits del valor de la fraccion de 12 bit
	SPI_transfer((ADF4350_FRAC_VALUE(FRAC) & 0x1F) << 3 | ADF4350_CTRL_REG0);  // Del valor de la fraccion de 16 bit se le cogen los 5 ultimos bits del valor de la fraccion de 12 bit y el 0b000 de los bits de control correspondientes a este registro
	delay16b(ADFDEL);
	DPIN->PIO_SODR |= LE;
}

// Control del atenuador
// No se usa el control de 0.5 dB
// Los pines de control que aqui se programan son simplemente los bits de la atenuacion en dB
void ATT_Def(uint8_t att)
{
#if defined(MALETA_5A) || defined(MALETA_5B) || \
	defined(ENRACKABLE_5A) || defined(ENRACKABLE_5B)
	
	if(att & 1)
		DPIN->PIO_CODR |= AT1;
	else
		DPIN->PIO_SODR |= AT1;
	
	if((att & 2) >> 1)
		DPIN2->PIO_CODR |= AT2;
	else
		DPIN2->PIO_SODR |= AT2;
	
	if((att & 4) >> 2)
		DPIN->PIO_CODR |= AT4;
	else
		DPIN->PIO_SODR |= AT4;
	
	if((att & 8) >> 3)
		DPIN->PIO_CODR |= AT8;
	else
		DPIN->PIO_SODR |= AT8;
	
	if((att & 16) >> 4)
		DPIN->PIO_CODR |= AT16;
	else
		DPIN->PIO_SODR |= AT16;
		
#else

	if(att & 1)
		DPIN->PIO_SODR |= AT1;
	else
		DPIN->PIO_CODR |= AT1;
	
	if((att & 2) >> 1)
		DPIN2->PIO_SODR |= AT2;
	else
		DPIN2->PIO_CODR |= AT2;
	
	if((att & 4) >> 2)
		DPIN->PIO_SODR |= AT4;
	else
		DPIN->PIO_CODR |= AT4;
	
	if((att & 8) >> 3)
		DPIN->PIO_SODR |= AT8;
	else
		DPIN->PIO_CODR |= AT8;
	
	if((att & 16) >> 4)
		DPIN->PIO_SODR |= AT16;
	else
		DPIN->PIO_CODR |= AT16;
#endif		
}

/*
*	DDS
*	===
*
*	Formulas del DDS:			(obtenidas de su datasheet)
*	=================
*	El reloj interno del AD9914 es SYNC_CLK
*	SYSCLK es la señal que entra por el par diferencial de pines REFCLK si asi se configura en los registros 
*		(o un PLL interno si se configura del otro modo posible)
*					****
*							En nuestro caso, cuando se alimenta la entrada con
*								4.4 GHz,	f_sysclk = 4.4 GHz
*								4.2 GHz,	f_sync_clk = 4.2 GHz
*					****
*	SYSCLK: DAC
*	SYSCLK / 24 = SYNC_CLK: nucleo DDS, DRG, PS[2:0] (Profile select), DRCTL y DRHOLD
*					**** 
*							En nuestro caso, cuando se alimenta la entrada con 
*								4.4 GHz,	f_sync_clk = 183.3 MHz
*								4.2 GHz,	f_sync_clk = 175 MHz
*					****
*	El DAC se comporta como un generador de corriente, pero las salidas son en voltaje
*
*	f_out = f_sysclk * FTW / 2^32 [1]		Nota: FTW = Frequency Tuning Word
*	Por lo tanto, para obtener el FTW a partir de la f_out deseada se debe escoger un FTW = round(2^32 * f_out / f_sysclk)
*	Ver pag. 20 del datasheet para mas ecuaciones acerca de la amplitud y la fase
*
*	En el DDS la frec. de Nyquist viene marcada por: f_sysclk / 2
*	Por lo tanto, de la formula [1] se deduce que el FTW es un entero de 32 bits cuyo margen
*	dentro de la frecuencia de Nyquist va de 0 a 2^31-1.
*	Programar el FTW entre 2^31-1 y 2^32 genera una imagen solapada a la frecuencia: f_out = (1 - FTW / 2^32) * f_sysclk
*
*	Tiempo necesario para calibracion: t_cal = 469.632 / fs	
*			(El maximo tiempo de calibracion es de 135 us a 3.5 GHz, pero depende de la f_sysclk y el primero aumenta cuando se disminuye la segunda)
*	El espectro completo del DAC está afectado por un sinc debido al mecanismo de sample & hold 
*		Nota: Probablemente este sinc tenga la distancia entre 0s definida por la frecuencia del DAC, que es f_sysclk


*	Notas sobre el uso que se esta haciendo fuera de especificaciones: 
			Pag. 4
			The AD9914 uses advanced DDS technology, coupled with
			an internal high speed, high performance DAC to form a digitally
			programmable, complete high frequency synthesizer capable of
			generating a frequency-agile analog output sinusoidal waveform
			at up to 1.4 GHz. <- ¡¡Atento a esto ultimo!!
			
			Pag. 6
			REF CLK INPUT	(REF CLK Multiplier Bypassed)
			Input Frequency Range Min. 500 MHz Max. 3500 MHz (Maximum f_out is 0.4 × f_sysclk) (0.4 * 3500 MHz = 1.4 GHz <- ¡¡Es coherente con lo que pone en la pag 4!!)
			Se le meten 4400 MHz cuando pone que el maximo es 3500 MHz y se pretenden sacar 2 GHz cuando ponen que el maximo es 1.4 GHz
			
			Pag. 22
			REF_CLK/REF_CLK Overview
			When the direct input path is selected, the REF_CLK/REF_CLK
			pins must be driven by an external signal source (single-ended or
			differential). Input frequencies up to 3.5 GHz are supported.
			¡¡ Insisten en el hecho que en este diseño se ha ignorado !!
									
*/


/*
*	Rutina de inicializacion general y en especial del generador de barrido del DDS
*	No se programan los limites de barrido aqui
*	
*	Pines en el circuito:
*	DRHOLD = GND
*	DRCTL = GND
*/

/*
*	 + Inc(t) = 24 P / f_sysclk
*
*	En nuestro caso, cuando se alimenta la entrada con (ver calculos de los datos mas abajo)
*								4.4 GHz		El barrido se produce a 
*												1.024 Hz / 5.45 ns = aprox 187.89 MHz / s
*												1 s / 187.89 MHz * 1000 MHz / 1 GHz = aprox 5.32 s / GHz 
*								4.2 GHz,	El barrido se produce a
*												0.978 Hz / 5.71 ns = aprox 171.278 MHz / s
*												1 s / 171.278 MHz * 1000 MHz / 1 GHz = aprox 5.84 s / GHz
*	Si se hiciera con el f_sysclk maximo de catalogo
*								3.5 GHz		El barrido se produce a
*												0.815 Hz / 6.86 ns = aprox 118.79 MHz / s
*												1 s / 118.79 MHz * 1000 MHz / 1 GHz = aprox 8.42 s / GHz
*
*	**** ¡¡Se tardan 5.84 s / GHz en hacer el barrido!! ****
*	**** ¡¡Se tardarían 8.42 s / GHz en hacer el barrido estando dentro de especificaciones!! ****
*/

void AD9914_sweepInit(float fclk, float fstp)
{
	// Reset IO communications
	DPIN->PIO_SODR |= SYNCIO;
	DPIN->PIO_CODR |= SYNCIO;
	
	// CRF4 DAC enable calibration
	SPI_transfer(AD9914_CFR4); // CFR4 address 
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | \
				AD9914_OPEN << 4 | AD9914_OPEN << 3 | AD9914_AUX_DIV_PWR_DWN_ENABLE << 2 | \
				AD9914_DAC_CAL_CLK_PWR_DWN_ENABLE << 1 | AD9914_DAC_CAL_ENABLE); // [31:24] -> Calibracion del DAC activada
	SPI_transfer(AD9914_DEFAULT_CFR4_1); // [23:16] -> Default value
	SPI_transfer(AD9914_DEFAULT_CFR4_2); // [15:8]  -> Default value
	SPI_transfer(AD9914_DEFAULT_CFR4_3); // [7:0]   -> Default value
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	delay32b(DACCAL);
	
	// CRF4 DAC disable calibration
	SPI_transfer(AD9914_CFR4); // Direccion CFR4 
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | \
				AD9914_OPEN << 4 | AD9914_OPEN << 3 | AD9914_AUX_DIV_PWR_DWN_ENABLE << 2 | \
				AD9914_DAC_CAL_CLK_PWR_DWN_ENABLE << 1 | AD9914_DAC_CAL_DISABLE); // [31:24] -> Calibracion del DAC desactivada
	SPI_transfer(AD9914_DEFAULT_CFR4_1); // [23:16] -> Default value
	SPI_transfer(AD9914_DEFAULT_CFR4_2); // [15:8]  -> Default value
	SPI_transfer(AD9914_DEFAULT_CFR4_3); // [7:0]   -> Default value
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	// CRF1 (Default values) 
	// (EXT_PWR_DOWN a masa, por lo tanto los bits de control de energia en este registro son funcionales)
	SPI_transfer(AD9914_CFR1); // Direccion CFR1
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | \
				AD9914_OPEN << 4 | AD9914_OPEN << 3 | AD9914_OPEN << 2 | \
				AD9914_OPEN << 1 | AD9914_VCO_CAL_NO_CAL); // [31:24] 
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | \
				AD9914_OPEN << 4 | AD9914_OPEN << 3 | AD9914_OPEN << 2 | \
				AD9914_PARALLEL_PORT_STR_ENABLE_IO_UPDATE << 1 | AD9914_ENABLE_SINE_OUTPUT); // [23:16] -> Salida senosoidad habilitada (no cosenoidal)
	SPI_transfer(AD9914_LOAD_LRR_NORMAL << 7 | AD9914_AUTO_CLR_DGTL_RMP_ACC_NORMAL << 6 | \
				AD9914_AUTO_CLR_PHS_ACC_NORMAL << 5 | AD9914_CLR_DGTL_RMP_ACC_NORMAL << 4 | \
				AD9914_CLR_PHS_ACC_NORMAL << 3 | AD9914_OPEN << 2 | \
				AD9914_EXTERNAL_OSK_MANUAL << 1 | AD9914_OSK_DISABLE); // [15:8]  -> No autoclear of DRG and no OSK 
	SPI_transfer(AD9914_DIGITAL_PWR_DWN_ACTIVE << 7 |AD9914_DAC_PWR_DWN_ACTIVE << 6 | \
				AD9914_REFCLK_INPUT_PWR_DWN_ACTIVE << 5 | AD9914_OPEN << 4  | \
				AD9914_EXT_PWR_DWN_CTRL_PWR_DWN << 3 | AD9914_OPEN << 2 | \
				AD9914_SDIO_INPUT_ONLY_2_WIRE << 1 | AD9914_MSB_FIRST); // [7:0]   -> Enable external powerdown control  - // EXT_PWR_DOWN apaga completamente todos los bloques y reinciarlo requeriria calibracion
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	// CRF2 dwelling configuration
	SPI_transfer(AD9914_CFR2); // Direccion CFR2
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | \
				AD9914_OPEN << 4 | AD9914_OPEN << 3 | AD9914_OPEN << 2 | \
				AD9914_OPEN << 1  | AD9914_OPEN); // [31:24] -> Open
	SPI_transfer(AD9914_PROFILE_MOD_DISABLE << 7 | AD9914_PARALLEL_DATA_PORT_DISABLE << 6 | \
				AD9914_DGTL_RMP_DEST(0) << 4 | AD9914_DGTL_RMP_ENABLE << 3 | \
				AD9914_DGTL_RMP_NO_DWELL_HIGH_ENABLE << 2 | \
				AD9914_DGTL_RMP_NO_DWELL_LOW_ENABLE << 1 | AD9914_PROG_MOD_DISABLE); // [23:16] -> active no-dwell low and no-dwell high (0x0E -> 0000 1110) - DRG activo, No dwell alto y bajo activos  
	SPI_transfer(AD9914_MATCH_LATENCY_ORDER_LIST << 7 | AD9914_FREC_JUMP_DISABLE << 6 | \
				AD9914_DRG_OVER_OUT_DISABLE << 5 | AD9914_OPEN << 4 | \
				AD9914_SYNC_CLK_DISABLE << 3 | AD9914_SYNC_CLK_NORMAL << 2 | \
				RESERVED << 1 | AD9914_OPEN); // [15:8]  -> DROVER disabled + SYNC_CLK disabled (DROVER esta no conectado)
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | AD9914_OPEN << 4 | \
				AD9914_OPEN << 3 | AD9914_OPEN << 2 | AD9914_OPEN << 1 | AD9914_OPEN); // [7:0]   -> Open
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	// CRF3 - PLL Configuration (default values)
	// No se usa el PLL
	SPI_transfer(AD9914_CFR3); // Direccion CFR3
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | AD9914_OPEN << 4 | \
				AD9914_OPEN << 3 | AD9914_OPEN << 2 | AD9914_OPEN << 1 | AD9914_OPEN); // [31:24]
	SPI_transfer(AD9914_OPEN << 7 | AD9914_PLL_INPUT_DIV_RST_DISABLE << 6 | \
				AD9914_PLL_INPUT_DIV_VAL(0) << 4 | AD9914_PLL_INPUT_DOUBLER_SEL_FEEDTHROUGH << 3 | \
				AD9914_PLL_DISABLE << 2 | AD9914_PLL_INPUT_DIV_2X << 1 | AD9914_PLL_EDGE_RISING); // [23:16]
	SPI_transfer(AD9914_FBCK_DIV(25)); // [15:8]
	SPI_transfer(AD9914_OPEN << 7 | AD9914_ICP_SEL_AUTOMATICALLY << 6 | AD9914_ICP(3) << 3 | \
				AD9914_LD_ENABLE << 2 | AD9914_MIN_LDW(0)); // [7:0]
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	// Digital ramp rate register (slope rate)
	SPI_transfer(AD9914_DIGITAL_RAMP_RATE_REGISTER);
	// N: Negative slope interval
	SPI_transfer((parametroDwelling & 0x0000FF00) >> 8); 
	SPI_transfer((parametroDwelling & 0x000000FF));  // Se usa el minimo negative slope rate no nulo posible (P = 1 de los posibles en el margen 0 <-> 2^16-1)
	// Por lo tanto usando la formula de la pag 25 del datasheet: + Inc(t) = 24 P / f_sysclk, con P = 1
	//					****
	//							En nuestro caso, cuando se alimenta la entrada con
	//								4.4 GHz		+ Inc(t) = 24 / 4.4e9 = 5.45 ns
	//								4.2 GHz		+ Inc(t) = 24 / 4.4e9 = 5.71 ns
	//					****
	//							Con valor maximo de catalogo
	//								3.5 GHz		+ Inc(t) = 6.86 ns
	// *** Todo esto implica que la rapidez de los barridos depende tambien de la señal que entra del ADF4350 ***
	// P: Positive slope interval
	SPI_transfer((parametroDwelling & 0x0000FF00) >> 8); 
	SPI_transfer((parametroDwelling & 0x000000FF)); // Se usa el minimo positive slope rate no nulo posible (valor 1 de los posibles en el margen 0 <-> 2^16-1)
	// En lo que respecta a los tiempos de un salto entre frecuencias es igual al caso positivo
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	/* 
	*	Tabla de equivalencias en nomenclatura entre este codigo y el datasheet
	*	=======================================================================
	*
	*	fstp = Frequency step
	*	fclk = f_sysclk
	*	FTWstep = M
	*
	*	Formula del datasheet
	*	=====================
	*	
	*	Frequency step = f_sysclk * M / 2^32		Pag. 25
	*
	*	Hay que tener en cuenta que incluso comenzando debajo de la frecuencia de Nyquist, con el barrido podria superarse este umbral
	*/
	
	uint32_t FTWstep = 0xFFFFFFFF * fstp / fclk; // fstp = fclk * FWstep / 2^32; FTWstep = 2^32 * fstp / fclk
											     // 0xFFFFFFFF no es exactamente lo mismo que el 2^32 que pone el datasheet 
											     // pero la diferencia en los calculos es despreciable
	/*
	*	En este codigo se llama a esta subrutina con parametros: 
	*		fclk = 4400 y fstp = 1 => FWTstep = 0.976. Truncado a un entero es 0 ¿?¿¿?¿?
	*		Si de verdad esta truncado a 0, no se generaria barrido: Freq. step = 0
	*		Si de algun modo el valor del FTWstep es 1, entonces Freq. step = 1.024 Hz
	*
	*	En este codigo Pedro llama a esta subrutina con parametros:
	*		fclk = 4200 y fstp = 1 => FWTstep = 1.023. Truncado a entero es 1
	*		Freq. step = 0.977889 Hz
	*
	*	Con valor maximo de catalogo:
	*		fclk = 3500 y fstp = 1 => FWTstep = 1.227. Truncado a entero es 1
	*		Freq. step = 0.814907 Hz
	*/
	
	// Rising Digital Ramp Step Size Register
	SPI_transfer(AD9914_RISING_DIGITAL_RAMP_STEP_SIZE_REGISTER);
	SPI_transfer((FTWstep & 0xFF000000) >> 24); // [31:24]
	SPI_transfer((FTWstep & 0x00FF0000) >> 16); // [23:16]
	SPI_transfer((FTWstep & 0x0000FF00) >> 8);  // [15:8]
	SPI_transfer((FTWstep & 0x000000FF));       // [7:0]
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;

	/* 
		Misma estrategia que el anterior registro
		Por lo tanto las rampas del DRG son simetricas en parte positiva y negativa
	*/
	// Falling Digital Ramp Step Size Register
	SPI_transfer(AD9914_FALLING_DIGITAL_RAMP_STEP_SIZE_REGISTER);
	SPI_transfer((FTWstep & 0xFF000000) >> 24); // [31:24] -> 1er byte del freq. step
	SPI_transfer((FTWstep & 0x00FF0000) >> 16); // [23:16] -> 2º byte del freq. step
	SPI_transfer((FTWstep & 0x0000FF00) >> 8);  // [15:8]  -> 3er byte del freq. step
	SPI_transfer((FTWstep & 0x000000FF));       // [7:0]   -> 4º byte del freq. step
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
}

/* 
	Esta funcion se hizo probablemente pensando en el caso de una programacion con una unica ventana
	en el main, ya que lo unico que hace es reprogramar las frecuencias de inicio y final
	en el DDS. Sin embargo no se usa. Queda como vestigio a revisar o eliminar directamente 
*/
/* void AD9914_1WSweep(float fclk, float fstart, float fstop, uint32_t att)
{
	uint32_t FTWstart = 0xFFFFFFFF * fstart / fclk;
	uint32_t FTWstop  = 0xFFFFFFFF * fstop / fclk;
	
	ATT_Def(att);
	
	SPI_transfer(0x04);
	SPI_transfer((FTWstart & 0xFF000000) >> 24);
	SPI_transfer((FTWstart & 0x00FF0000) >> 16);
	SPI_transfer((FTWstart & 0x0000FF00) >> 8);
	SPI_transfer((FTWstart & 0x000000FF));
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	SPI_transfer(0x05);
	SPI_transfer((FTWstop & 0xFF000000) >> 24);
	SPI_transfer((FTWstop & 0x00FF0000) >> 16);
	SPI_transfer((FTWstop & 0x0000FF00) >> 8);
	SPI_transfer((FTWstop & 0x000000FF));
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
} */

/*
*	Rutina de establecimiento de limites del generador de barrido del DDS
*   Se llama al atenuador
*/

void AD9914_Sweep(float fclk, float fstart, float fstop, uint8_t att)
{
	uint32_t FTWstart = 0xFFFFFFFF * fstart / fclk;
	uint32_t FTWstop = 0xFFFFFFFF * fstop / fclk;
	
#ifdef SILENCIO
	SPI_transfer(AD9914_CFR2); // Direccion CFR2
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | \
				AD9914_OPEN << 4 | AD9914_OPEN << 3 | AD9914_OPEN << 2 | \
				AD9914_OPEN << 1 | AD9914_OPEN); // [31:24] -> Open
	SPI_transfer(AD9914_PROFILE_MOD_DISABLE << 7 | AD9914_PARALLEL_DATA_PORT_DISABLE << 6 | \
				AD9914_DGTL_RMP_DEST(0) << 4 | AD9914_DGTL_RMP_DISABLE << 3 | \
				AD9914_DGTL_RMP_NO_DWELL_HIGH_DISABLE << 2 | \
				AD9914_DGTL_RMP_NO_DWELL_LOW_DISABLE << 1 | AD9914_PROG_MOD_DISABLE); // [23:16] -> Digital ramp disabled + No dwell high disabled + No dwell low disabled
	SPI_transfer(AD9914_MATCH_LATENCY_ORDER_LIST << 7 | AD9914_FREC_JUMP_DISABLE << 6 | \
				AD9914_DRG_OVER_OUT_DISABLE << 5 | AD9914_OPEN << 4 | \
				AD9914_SYNC_CLK_DISABLE << 3 | AD9914_SYNC_CLK_NORMAL << 2 | \
				RESERVED << 1 | AD9914_OPEN); // [15:8]  -> DROVER disabled + SYNC_CLK disabled (DROVER esta no conectado)
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | \
				AD9914_OPEN << 4 | AD9914_OPEN << 3 | AD9914_OPEN << 2 | \
				AD9914_OPEN << 1 | AD9914_OPEN); // [7:0]   -> Open
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
#endif

	// Digital Ramp Upper Limit
	SPI_transfer(AD9914_DIGITAL_RAMP_UPPER_LIMIT_REGISTER);
	SPI_transfer((FTWstop & 0xFF000000) >> 24); // [31:24]
	SPI_transfer((FTWstop & 0x00FF0000) >> 16); // [23:16]
	SPI_transfer((FTWstop & 0x0000FF00) >> 8);  // [15:8]
	SPI_transfer((FTWstop & 0x000000FF));       // [7:0]
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
		
	ATT_Def(att);
	
	// Digital Ramp Lower Limit
	SPI_transfer(AD9914_DIGITAL_RAMP_LOWER_LIMIT_REGISTER);
	SPI_transfer((FTWstart & 0xFF000000) >> 24); // [31:24]
	SPI_transfer((FTWstart & 0x00FF0000) >> 16); // [23:16]
	SPI_transfer((FTWstart & 0x0000FF00) >> 8);  // [15:8]
	SPI_transfer((FTWstart & 0x000000FF));       // [7:0]
	DPIN->PIO_SODR |= IOUPD; 
	DPIN->PIO_CODR |= IOUPD;
	
	// IN
	float bandwith = fstop - fstart;
	float fstp = ((bandwith / tiempoRampa) * parametroDwelling) * (24 / fclk); // DRG Timer intervalos = 24 / fclk
	uint32_t FTWstep = 0xFFFFFFFF * fstp / fclk;
	
	// Rising Digital Ramp Step Size Register
	//Esta al inicio de la ventana y va al final en pasos de FTWstep 
	SPI_transfer(AD9914_RISING_DIGITAL_RAMP_STEP_SIZE_REGISTER);
	SPI_transfer((FTWstep & 0xFF000000) >> 24); // [31:24]
	SPI_transfer((FTWstep & 0x00FF0000) >> 16); // [23:16]
	SPI_transfer((FTWstep & 0x0000FF00) >> 8);  // [15:8]
	SPI_transfer((FTWstep & 0x000000FF));       // [7:0]
	DPIN->PIO_SODR |= IOUPD;
	// Habria que esperar a que el DROVER estuviera a 1 y esperar 2 ciclos de reloj que es lo que dura (end pag. 27 datasheet AD9914) 
	// Habria que habilitar 0x01[13] = 1 
	DPIN->PIO_CODR |= IOUPD;
	
	if(sentidoBarrido) // Esta al final de la ventana y vuelve en dos pasos (FTWflyback) al inicio de la ventana
	{
		uint32_t FTWflyback = (FTWstart - FTWstop) / 2; // Hay que hacer el flyback en 2 pasos o no funciona
	
		// Falling Digital Ramp Step Size Register
		SPI_transfer(AD9914_FALLING_DIGITAL_RAMP_STEP_SIZE_REGISTER);
		SPI_transfer(((FTWflyback) & 0xFF000000) >> 24); // [31:24]
		SPI_transfer(((FTWflyback) & 0x00FF0000) >> 16); // [23:16]
		SPI_transfer(((FTWflyback) & 0x0000FF00) >> 8);  // [15:8]
		SPI_transfer(((FTWflyback) & 0x000000FF));       // [7:0]
		DPIN->PIO_SODR |= IOUPD;
		DPIN->PIO_CODR |= IOUPD;
		// OUT
	}
	else // Esta al final de la ventana y vuelve en pasos de FTWstep hacia el inicio de la ventana 
	{
		//Falling Digital Ramp Step Size Register
		SPI_transfer(AD9914_FALLING_DIGITAL_RAMP_STEP_SIZE_REGISTER);
		SPI_transfer(((FTWstep) & 0xFF000000) >> 24); // [31:24]
		SPI_transfer(((FTWstep) & 0x00FF0000) >> 16); // [23:16]
		SPI_transfer(((FTWstep) & 0x0000FF00) >> 8);  // [15:8]
		SPI_transfer(((FTWstep) & 0x000000FF));       // [7:0]
		DPIN->PIO_SODR |= IOUPD;
		DPIN->PIO_CODR |= IOUPD;	
	}

#ifdef SILENCIO	
	SPI_transfer(AD9914_CFR2); // Direccion CFR2
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | \
				AD9914_OPEN << 4 | AD9914_OPEN << 3 | AD9914_OPEN << 2 | \
				AD9914_OPEN << 1  | AD9914_OPEN); // [31:24] -> Open
	SPI_transfer(AD9914_PROFILE_MOD_DISABLE << 7 | AD9914_PARALLEL_DATA_PORT_DISABLE << 6 | \
				AD9914_DGTL_RMP_DEST(0) << 4 | AD9914_DGTL_RMP_ENABLE << 3 | \
				AD9914_DGTL_RMP_NO_DWELL_HIGH_ENABLE << 2 | \
				AD9914_DGTL_RMP_NO_DWELL_LOW_ENABLE << 1 | AD9914_PROG_MOD_DISABLE); // [23:16] -> Digital ramp enabled + No dwell high enabled + No dwell low enabled
	SPI_transfer(AD9914_MATCH_LATENCY_ORDER_LIST << 7 | AD9914_FREC_JUMP_DISABLE << 6 | \
				AD9914_DRG_OVER_OUT_DISABLE << 5 | AD9914_OPEN << 4 | \
				AD9914_SYNC_CLK_DISABLE << 3 | AD9914_SYNC_CLK_NORMAL << 2 | \
				RESERVED << 1 | AD9914_OPEN); // [15:8]  -> DROVER disabled + SYNC_CLK disabled (DROVER esta no conectado)
	SPI_transfer(AD9914_OPEN << 7 | AD9914_OPEN << 6 | AD9914_OPEN << 5 | AD9914_OPEN << 4 | \
				AD9914_OPEN << 3 | AD9914_OPEN << 2 | AD9914_OPEN << 1 | AD9914_OPEN); // [7:0]   -> Open
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
#endif
}


void conmutarVentanas(void)
{
#ifndef TEST_DDS
	static uint8_t ww = 0;
	if(finProg && !off && (W > 0)) // Expresion que equivale a si hay RF
	{
		ww++;
		ww = ww % W;
		if(activeWindows[ww])
		{
			NVIC_DisableIRQ(TWI0_IRQn);
			AD9914_Sweep(fclock, frequencies[2 * ww], frequencies[2 * ww + 1], att[ww]);
			NVIC_EnableIRQ(TWI0_IRQn);
		}
	}
#endif
}

int main(void)
{
	// CLK Init
	CLK_Init(OSC, FOSC, 15, 2);
	
	// Set PB4 as GPIO pin
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
	
	// pinMode(..., OUTPUT)
	DPIN->PIO_OER |= AT16 | AT8 | AT4 | AT1 | EN18 | EN33 | EN50 | CSCLK | SYNCIO | IOUPD;
	DPIN2->PIO_OER |= AT2;
	
	// digitalWrite(..., HIGH)	
	DPIN->PIO_SODR |= CSCLK;	// Setting the CS in high state
	
	//[AR]Las generadoras 4 y 5 necesitan un PLL extra para usarlo como "Portadora" y poder elevar la frecuencia de la señal,
	//esta instrucción define el pin PA9 como salida y lo habilita a 1 para el "ChipSelect" del PLL 
#if !defined(MALETA_1) && !defined(ENRACKABLE_1) &&	\
	!defined(MALETA_2) && !defined(ENRACKABLE_2) &&	\
	!defined(MALETA_3) && !defined(ENRACKABLE_3)
	DPIN->PIO_OER |= CSLO;
	DPIN->PIO_SODR |= CSLO;
#endif
	//[AR] A la salida de RF existe un Atenuador controlado digitalmente.
	//En el caso de ser la generadora 5A o 5B, se deshabilitan las atenuaciones
#if defined(MALETA_5A) || defined(MALETA_5B) || \
	defined(ENRACKABLE_5A) || defined(ENRACKABLE_5B)
	DPIN->PIO_CODR |= AT16 | AT8 | AT4 | AT1;	// Setting max attenuation [AR] CREO QUE ESTO ESTÁ MAL
	DPIN2->PIO_CODR |= AT2;
#else
	//En el caso de NO ser la generadora 5A o 5B, se habilitan las atenuaciones
	DPIN->PIO_SODR |= AT16 | AT8 | AT4 | AT1;	// Setting the DAT31R5APP+ at max attenuation
	DPIN2->PIO_SODR |= AT2;
#endif
	
	// digitalWrite(..., LOW)
	DPIN->PIO_CODR |= EN18 | EN33 | EN50 | SYNCIO | IOUPD;	// Turn off the DC regulators and setting AD9914 communication pins
	alimentacion_AD4350 = OFF;
	
	// I2C MODULE
	//[AR] Habilitamos el i2c en modo esclavo, le asignamos una dirección (IDL) y habilitamos las interrupciones de recepción.
	I2C_slave_Init(IDL);
	
	//while(true)
	//{
	//}
}

#ifdef TEST_DDS 
void testDDS()
{
	DPIN->PIO_SODR |= EN18 | EN33 | EN50;
	delay32b(GPIODEL);
	SPI_Init(0, 0, 1);	//different 1 en esclavo (6)
	delay32b(GPIODEL);
	ADF4350_synthetizer(FOSC, fclock, PCLOCK, CSCLK);
	delay32b(GPIODEL);
	
	ATT_Def(0);
	
	//RESET IO COMMUNICATIONS
	DPIN->PIO_SODR |= SYNCIO;
	DPIN->PIO_CODR |= SYNCIO;
	
	//CRF4 DAC CALIBRATION
	SPI_transfer(0x03);
	SPI_transfer(0x01); // Calibracion del DAC activa
	SPI_transfer(0x05); 
	SPI_transfer(0x21);
	SPI_transfer(0x20);
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	delay32b(DACCAL);
	
	SPI_transfer(0x03); // CFR3 Address
	SPI_transfer(0x00); // Calibracion del DAC desactivada
	SPI_transfer(0x05);
	SPI_transfer(0x21);
	SPI_transfer(0x20);
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;	
	
	//CRF1
	SPI_transfer(0x00);
	SPI_transfer(0x00);
	SPI_transfer(0x01); // Activacion salida senoidal
	SPI_transfer(0x00); // Sin autoclear del DRG ni OSK
	SPI_transfer(0x08); // Control de apagado a traves de patilla activo
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	//CRF2
	SPI_transfer(0x01);
	SPI_transfer(0x00);
	SPI_transfer(0x0E); // DRG activo, No dwell alto y bajo activos
	SPI_transfer(0x00);
	SPI_transfer(0x00);
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	
	//CRF3
	// Controles del PLL, todo por defecto. No se usa el PLL
	//SPI_transfer(0x02);
	//SPI_transfer(0x00);
	//SPI_transfer(0x00);
	//SPI_transfer(0x19);
	//SPI_transfer(0x1C);
	//DPIN->PIO_SODR |= IOUPD;
	//DPIN->PIO_CODR |= IOUPD;
	
	//Digital ramp slope rate
	SPI_transfer(0x08);
	SPI_transfer((parametroDwelling & 0x0000FF00) >> 8);
	SPI_transfer((parametroDwelling & 0x000000FF));
	SPI_transfer((parametroDwelling & 0x0000FF00) >> 8);
	SPI_transfer((parametroDwelling & 0x000000FF));
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	uint32_t FTWstart = 0xFFFFFFFF * 200.0 / fclock;
	uint32_t FTWstop = 0xFFFFFFFF * 300.0 / fclock;
	
	SPI_transfer(0x04);
	SPI_transfer((FTWstart & 0xFF000000) >> 24);
	SPI_transfer((FTWstart & 0x00FF0000) >> 16);
	SPI_transfer((FTWstart & 0x0000FF00) >> 8);
	SPI_transfer((FTWstart & 0x000000FF));
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
	
	SPI_transfer(0x05);
	SPI_transfer((FTWstop & 0xFF000000) >> 24);
	SPI_transfer((FTWstop & 0x00FF0000) >> 16);
	SPI_transfer((FTWstop & 0x0000FF00) >> 8);
	SPI_transfer((FTWstop & 0x000000FF));
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
		
	//IN
	float bandwith = 300 - 200;
	float fstp = bandwith / tiempoRampa * parametroDwelling * 24 / fclock; // DRG Timer intervalos = 24 / fclk
	uint32_t FTWstep = 0xFFFFFFFF * fstp / fclock;
	
	//Rising Digital Ramp Step Size Register
	SPI_transfer(0x06);
	SPI_transfer((FTWstep & 0xFF000000) >> 24);
	SPI_transfer((FTWstep & 0x00FF0000) >> 16);
	SPI_transfer((FTWstep & 0x0000FF00) >> 8);
	SPI_transfer((FTWstep & 0x000000FF));
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
		
	uint32_t FTWflyback = (FTWstart - FTWstop) / 2; // Hay que hacer el flyback en 2 pasos o no funciona
	//Falling Digital Ramp Step Size Register
	SPI_transfer(0x07);
	SPI_transfer(((FTWflyback) & 0xFF000000) >> 24);
	SPI_transfer(((FTWflyback) & 0x00FF0000) >> 16);
	SPI_transfer(((FTWflyback) & 0x0000FF00) >> 8);
	SPI_transfer(((FTWflyback) & 0x000000FF));
	DPIN->PIO_SODR |= IOUPD;
	DPIN->PIO_CODR |= IOUPD;
}
#endif

/**
*  \brief TWI0 Interrupt handler.
*  Esto es una barbaridad ilegible. A arreglar, refactorizar y comentar debidamente cuando tenga tiempo
*/
void TWI0_Handler(void)
{
	uint8_t dataByte = REG_TWI0_RHR; // (TWI0) Receive Holding Register 
	
	if(!lockI2C) // Primer byte de cualquier comando
	{
		command = dataByte & 7;
		switch(command)
		{
			case CONFIGURAR_VENTANAS:
				W = (dataByte >> 3) & 7; 
				lockI2C = true;
			break;
			
			case MISC:
				subcommand = dataByte >> 3;
				switch(subcommand)
				{
					case JAMMING_PARAMETERS_OP:
						lockI2C = true;
					break;
				}
			break;
						
			case APAGAR_PLACA:
#ifndef TEST_DDS			
				DPIN -> PIO_CODR |= EN18 | EN33 | EN50;
				alimentacion_AD4350 = OFF;
				lockI2C = false;
#else
				testDDS();
#endif
			break;
			
			case CONMUTAR_VENTANAS:
				// ¿Es realmente necesario comprobar si se ha finalizado la programacion?
				// De hecho finProg es solo falso hasta que se termina de enviar un comando 2
				// A partir de ese momento ya es verdadero hasta que se quita la alimentacion
				// de la placa
				if(!finProg) { break; }

				// Ventanas de la 1 a la 5 (primer byte)
				activeWindows[0] = (dataByte >> 3) & 1;
				activeWindows[1] = (dataByte >> 4) & 1;
				activeWindows[2] = (dataByte >> 5) & 1;
				activeWindows[3] = (dataByte >> 6) & 1;
				activeWindows[4] = (dataByte >> 7) & 1;
				lockI2C = true;
				
			break;
		}
	}
	else // Segundo byte y siguientes de alguno de los comandos compuestos de varios bytes
	{
		switch(command)
		{
			case CONFIGURAR_VENTANAS:
				bufferWindows[commandByte] = dataByte;
				commandByte++;
#ifdef PARAMETROS_INHIBICION
				if(commandByte >= WINDOW_MEM_SIZE * W + SIZE_PARAMS)
#else
				if(commandByte >= WINDOW_MEM_SIZE * W)
#endif
				{
					for(uint8_t ww = 0; ww < W; ww++)
					{
						*ptrf1 = bufferWindows[WINDOW_MEM_SIZE * ww];
						*(ptrf1 + 1) = bufferWindows[WINDOW_MEM_SIZE * ww + 1];
						*(ptrf1 + 2) = bufferWindows[WINDOW_MEM_SIZE * ww + 2];
						*(ptrf1 + 3) = bufferWindows[WINDOW_MEM_SIZE * ww + 3];
						*ptrf2 = bufferWindows[9 * ww + 4];
						*(ptrf2 + 1) = bufferWindows[WINDOW_MEM_SIZE * ww + 5];
						*(ptrf2 + 2) = bufferWindows[WINDOW_MEM_SIZE * ww + 6];
						*(ptrf2 + 3) = bufferWindows[WINDOW_MEM_SIZE * ww + 7];
							
						att[ww] = bufferWindows[WINDOW_MEM_SIZE * ww + 8];

#if defined(MALETA_4A) || defined(ENRACKABLE_4A)
						frequencies[2 * ww] = FLOCAL - f2;
						frequencies[2 * ww + 1] = FLOCAL - f1;
#else
						frequencies[2 * ww] = f1 - FLOCAL;
						frequencies[2 * ww + 1] = f2 - FLOCAL;
#endif
					}

#ifdef PARAMETROS_INHIBICION
					parametroDwelling = bufferWindows[WINDOW_MEM_SIZE * W];								   // Primer byte de parametroDwelling, primera posicion de bufferWindows
					parametroDwelling = (parametroDwelling << 8) | bufferWindows[WINDOW_MEM_SIZE * W + 1]; // Segundo byte de parametroDwelling, segunda posicion de bufferWindows
					tiempoRampa = bufferWindows[WINDOW_MEM_SIZE * W + 2];								   // Primer byte de tiempoRampa, tercera posicion de bufferWindows
					tiempoRampa = (tiempoRampa << 8) | bufferWindows[WINDOW_MEM_SIZE * W + 3];			   // Segundo byte de tiempoRampa, cuarta posicion de bufferWindows
					tiempoBarrido = bufferWindows[WINDOW_MEM_SIZE * W + 4];								   // Primer byte de tiempoBarrido, quinta posicion de bufferWindows
					tiempoBarrido = (tiempoBarrido << 8) | bufferWindows[WINDOW_MEM_SIZE * W + 5];		   // Segundo byte de tiempoBarrido, quinta posicion de bufferWindows
					tiempoBarrido = (tiempoBarrido << 8) | bufferWindows[WINDOW_MEM_SIZE * W + 6];		   // Tercer byte de tiempoBarrido, sexta posicion de bufferWindows
					tiempoBarrido = (tiempoBarrido << 8) | bufferWindows[WINDOW_MEM_SIZE * W + 7];		   // Cuarto byte de tiempoBarrido, septima posicion de bufferWindows
					sentidoBarrido = bufferWindows[WINDOW_MEM_SIZE * W + 8];							   // Primer byte de sentidoBarrido, octava posicion de bufferWindows
					fclock = bufferWindows[WINDOW_MEM_SIZE * W + 9];									   // Primer byte de fclock, novena posicion de bufferWindows
					fclock = (fclock << 8) | bufferWindows[WINDOW_MEM_SIZE * W + 10];					   // Segundo byte de fclock, decima posicion de bufferWindows
#endif				
					lockI2C = false;
					finProg = true;
					commandByte = 0;

					DPIN->PIO_SODR |= EN18 | EN33 | EN50;
					alimentacion_AD4350 = ON;
					delay32b(GPIODEL);
					SPI_Init(0, 0, 1);	//different 1 en esclavo (6)
					delay32b(GPIODEL);
					ADF4350_synthetizer(FOSC, fclock, PCLOCK, CSCLK);//[AR] Configuramos el PLL que se usa como fuente de reloj del DDS
					delay32b(GPIODEL);
						
#if !defined(MALETA_1) && !defined(ENRACKABLE_1) &&	\
	!defined(MALETA_2) && !defined(ENRACKABLE_2) &&	\
	!defined(MALETA_3) && !defined(ENRACKABLE_3)
					ADF4350_synthetizer(FOSC, FLOCAL, PCLOCK, CSLO);//[AR] Configuramos el PLL que se usa como portadora para las generadoras 4 y 5
					delay32b(GPIODEL);
#endif
						
					rtt_ventana(tiempoBarrido);
					AD9914_sweepInit(fclock, FSTEP);//[AR] Configuramos el DDS
				}
			
			break;
			
			case CONMUTAR_VENTANAS:
				if(!finProg) { break; }
			
			    // Ventanas de l a 6 a la 10 (segundo byte)
				activeWindows[5] = (dataByte >> 3) & 1;
				activeWindows[6] = (dataByte >> 4) & 1;
				activeWindows[7] = (dataByte >> 5) & 1;
				activeWindows[8] = (dataByte >> 6) & 1;
				activeWindows[9] = (dataByte >> 7) & 1;
				lockI2C = false;				
				off = true;
				for(uint8_t ww = 0; ww < W; ww++)
				{
					if(activeWindows[ww])
					{
						off = false;
					}
				}
				
				if(off)
				{
					SPI_transfer(0x01);
					SPI_transfer(0x00);
					SPI_transfer(0x00);
					SPI_transfer(0x00);
					SPI_transfer(0x00);
					DPIN->PIO_SODR |= IOUPD;
					DPIN->PIO_CODR |= IOUPD;
					
					//CRF1
					SPI_transfer(0x00);
					SPI_transfer(0x00);
					SPI_transfer(0x01);
					SPI_transfer(0x00);
					SPI_transfer(0xE8); //E8: Parada general Pedro. 88: Parada Digital solamente
					DPIN->PIO_SODR |= IOUPD;
					DPIN->PIO_CODR |= IOUPD;
				}
				else
				{
					// Esto se pone aqui para el caso de que se ajusten parametros de inhibicion con una
					// programacion de ventanas ya cargada pero con las ventanas desactivadas
					AD9914_sweepInit(fclock, FSTEP);
					
					SPI_transfer(0x03);
					SPI_transfer(0x01);
					SPI_transfer(0x05);
					SPI_transfer(0x21);
					SPI_transfer(0x20);
					DPIN->PIO_SODR |= IOUPD;
					DPIN->PIO_CODR |= IOUPD;
					
					SPI_transfer(0x03); //CFR3 Address
					SPI_transfer(0x00);
					SPI_transfer(0x05);
					SPI_transfer(0x21);
					SPI_transfer(0x20);
					DPIN->PIO_SODR |= IOUPD;
					DPIN->PIO_CODR |= IOUPD;
					
					//CRF1
					SPI_transfer(0x00);
					SPI_transfer(0x00);
					SPI_transfer(0x01);
					SPI_transfer(0x00);
					SPI_transfer(0x08); 
					DPIN->PIO_SODR |= IOUPD;
					DPIN->PIO_CODR |= IOUPD;
					
					SPI_transfer(0x01);
					SPI_transfer(0x00);
					SPI_transfer(0x0E);
					SPI_transfer(0x00);
					SPI_transfer(0x00);
					DPIN->PIO_SODR |= IOUPD;
					DPIN->PIO_CODR |= IOUPD;
				}
				delay32b(DDSDEL);
			break;
			
			case MISC:
				switch(subcommand)
				{
					case JAMMING_PARAMETERS_OP:
						bufferParams[commandByte] = dataByte;
						commandByte++;
						
						if(commandByte >= SIZE_PARAMS)
						{
							parametroDwelling = bufferParams[0];
							parametroDwelling = (parametroDwelling << 8) | bufferParams[1];
							tiempoRampa = bufferParams[2];
							tiempoRampa = (tiempoRampa << 8) | bufferParams[3];
							tiempoBarrido = bufferParams[4];
							tiempoBarrido = (tiempoBarrido << 8) | bufferParams[5];
							tiempoBarrido = (tiempoBarrido << 8) | bufferParams[6];
							tiempoBarrido = (tiempoBarrido << 8) | bufferParams[7];
							sentidoBarrido = bufferParams[8];
							fclock = bufferParams[9];
							fclock = (fclock << 8) | bufferParams[10];
							
							if(alimentacion_AD4350)
							{
								delay32b(GPIODEL);
								SPI_Init(0, 0, 1);	//different 1 en esclavo (6)
								delay32b(GPIODEL);
								ADF4350_synthetizer(FOSC, fclock, PCLOCK, CSCLK);
								delay32b(GPIODEL);
							}
							
							if(finProg && !off && (W > 0)) // Expresion que equivale a si hay RF
							{
								AD9914_sweepInit(fclock, FSTEP);
							}
							
							commandByte = 0;
							lockI2C = false;
						}
					break;
				}
			break;
		}
	}
}