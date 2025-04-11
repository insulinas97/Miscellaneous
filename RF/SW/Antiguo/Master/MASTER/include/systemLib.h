/*
* PIN ENA  ------ pin PA20
* PIN END  ------ pin PA23

* PIN PD   ------ pin PB0/AD4
* PIN PR   ------ pin PB1/AD5
* PIN IDC  ------ pin PB2/AD6
* PIN TEMP ------ pin PB3/AD7
*/

#ifndef _SYSTEM_LIB
#define _SYSTEM_LIB

#define DPIN PIOA
#define APIN PIOB

#define END	PIO_PA20 // Pin Enable DC-DC
#define ENA	PIO_PA23 // Pin Enable Amplificador de potencia

#define ANALOG_REF 3300	// mV de excursión max en la entrada del ADC

#define BAUDRATE 19200
#define I2CCLOCK 400

#define ROE_MAX 200
//#define SKIP_TELEMETRIA	10
#define SKIP_TELEMETRIA	20
//#define BUFFER_SIZE 1024
#define BUFFER_SIZE 512
//#define UMBRAL_ALARMAS 10
#define UMBRAL_ALARMAS 50

// Retardo desde que se enciende la etapa de potencia hasta 
//	que se empieza a tomar la telemetria [ms]
// Esto es necesario para no hacer saltar las alarmas con los transitorios de arranque
#define RETARDO_TELEMETRIA 1000


// Parametros extra para control interno de inhibicion. Estos parametros se añaden al comando 2
// Funcionalidad obsoleta puesto que se ha implementado a traves de un subcomando
//#define PARAMETROS_INHIBICION

// Envios extra para el tester de telemetria
//#define TEST_TELEMETRIA
#endif