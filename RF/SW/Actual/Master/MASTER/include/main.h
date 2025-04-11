#ifndef _MAIN
#define _MAIN

#define FOSC 16

// ID de las generadoras
#if defined(MALETA_1) || defined(ENRACKABLE_1)
#define IDG 0x1
#endif
#if defined(MALETA_2) || defined(ENRACKABLE_2)
#define IDG 0x2
#endif
#if defined(MALETA_3) || defined(ENRACKABLE_3)
#define IDG	0x3
#endif
#if defined(MALETA_4A) || defined(ENRACKABLE_4A)
#define IDG	0x4A
#endif
#if defined(MALETA_4B) || defined(ENRACKABLE_4B)
#define IDG	0x4B
#endif
#if defined(MALETA_5A) || defined(ENRACKABLE_5A)
#define IDG	0x5A
#endif
#if defined(MALETA_5B) || defined(ENRACKABLE_5B)
#define IDG	0x5B
#endif

#define IDR1 1

// Comandos
enum comando {
	ENCENDER_ETAPA_POTENCIA = 0,
	APAGAR_ETAPA_POTENCIA = 1,
	CONFIGURAR_VENTANAS = 2,
	ENVIAR_TELEMETRIA = 3,
	CONMUTAR_VENTANAS = 4, // Comando para activar y desactivar ventanas configuadas con el comando 2
	TEST_ALARMA = 5,
	MISC = 6, // Comando con 32 posibles subcomandos. Inicialmente usado solo para identificar las placas
	APAGAR_PLACA = 7 // Se apaga la RF y se pone la placa en modo de bajo consumo
};

// Subcomandos del comando 6
enum subcomando {
	ID_OP = 0,
	JAMMING_PARAMETERS_OP = 1
};

#define MAX_VENTANAS_PLACA 10

#endif