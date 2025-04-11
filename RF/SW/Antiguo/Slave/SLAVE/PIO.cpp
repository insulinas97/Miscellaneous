#include "sam.h"

#define PA0	   0
#define PA1	   1 
#define PA2	   2
#define PA3	   3
#define PA4	   4
#define PA5	   5
#define PA6	   6
#define PA7	   7
#define PA8	   8 
#define PA9	   9
#define PA10  10
#define PA11  11
#define PA12  12
#define PA13  13
#define PA14  14
#define PA15  15
#define PA16  16
#define PA17  17
#define PA18  18
#define PA19  19
#define PA20  20
#define PA21  21
#define PA22  22
#define PA23  23
#define PA24  24
#define PA25  25
#define PA26  26
#define PA27  27
#define PA28  28
#define PA29  29
#define PA30  30
#define PA31  31

#define PB0   32
#define PB1   33
#define PB2   34
#define PB3   35
#define PB4   36
#define PB5   37
#define PB6   38
#define PB7   39
#define PB8   40
#define PB9   41
#define PB10  42
#define PB11  43
#define PB12  44
#define PB13  45
#define PB14  46
#define PB15  47
#define PB16  48
#define PB17  49
#define PB18  50
#define PB19  51
#define PB20  52
#define PB21  53
#define PB22  54
#define PB23  55
#define PB24  56
#define PB25  57
#define PB26  58
#define PB27  59
#define PB28  60
#define PB29  61
#define PB30  62
#define PB31  63
/*
#define PC0   64
#define PC1   65
#define PC2   66
#define PC3   67
#define PC4   68
#define PC5   69
#define PC6   70
#define PC7   71
#define PC8   72
#define PC9   73
#define PC10  74
#define PC11  75
#define PC12  76
#define PC13  77
#define PC14  78
#define PC15  79
#define PC16  80
#define PC17  81
#define PC18  82
#define PC19  83
#define PC20  84
#define PC21  85
#define PC22  86
#define PC23  87
#define PC24  88
#define PC25  89
#define PC26  90
#define PC27  91
#define PC28  92
#define PC29  93
#define PC30  94
#define PC31  95

#define PD0   96
#define PD1   97
#define PD2   98
#define PD3   99
#define PD4   100
#define PD5   101
#define PD6   102
#define PD7   103
#define PD8   104
#define PD9   105
#define PD10  106
#define PD11  107
#define PD12  108
#define PD13  109
#define PD14  110
#define PD15  111
#define PD16  112
#define PD17  113
#define PD18  114
#define PD19  115
#define PD20  116
#define PD21  117
#define PD22  118
#define PD23  119
#define PD24  120
#define PD25  121
#define PD26  122
#define PD27  123
#define PD28  124
#define PD29  125
#define PD30  126
#define PD31  127
*/
#define OUTPUT 0
#define INPUT 1

#define LOW 0
#define HIGH 1

void pinMode(uint8_t PIN, uint8_t mode)
{
	switch(PIN)
	{
		case PA0:
			REG_PIOA_PER |= PIO_PER_P0;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P0;
				break;
				
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P0;
				break;
			}
		break;
		
		case PA1:
			REG_PIOA_PER |= PIO_PER_P1;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P1;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P1;
				break;
			}
		break;
		
		case PA2:
			REG_PIOA_PER |= PIO_PER_P2;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P2;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P2;
				break;
			}
		break;
		
		case PA3:
			REG_PIOA_PER |= PIO_PER_P3;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P3;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P3;
				break;
			}
		break;
		
		case PA4:
			REG_PIOA_PER |= PIO_PER_P4;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P4;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P4;
				break;
			}
		break;
		
		case PA5:
			REG_PIOA_PER |= PIO_PER_P5;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P5;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P5;
				break;
			}
		break;
		
		case PA6:
			REG_PIOA_PER |= PIO_PER_P6;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P6;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P6;
				break;
			}
		break;
		
		case PA7:
			REG_PIOA_PER |= PIO_PER_P7;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P7;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P7;
				break;
				}
		break;
		
		case PA8:
			REG_PIOA_PER |= PIO_PER_P8;
	      	switch(mode)
	    	{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P8;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P8;
				break;
			}
		break;
		
		case PA9:
			REG_PIOA_PER |= PIO_PER_P9;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P9;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P9;
				break;
			}
		break;
		
		case PA10:
			REG_PIOA_PER |= PIO_PER_P10;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P10;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P10;
				break;
			}
		break;
		
		case PA11:
			REG_PIOA_PER |= PIO_PER_P11;
			switch(mode)
			{	
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P11;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P11;
				break;
			}
		break;
		
		case PA12:
			REG_PIOA_PER |= PIO_PER_P12;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P12;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P12;
				break;
			}
		break;
		
		case PA13:
			REG_PIOA_PER |= PIO_PER_P13;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P13;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P13;
				break;
			}
		break;
		
		case PA14:
			REG_PIOA_PER |= PIO_PER_P14;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P14;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P14;
				break;
			}
		break;
		
		case PA15:
			REG_PIOA_PER |= PIO_PER_P15;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P15;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P15;
				break;
			}
		break;
		
		case PA16:
			REG_PIOA_PER |= PIO_PER_P16;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P16;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P16;
				break;
			}
		break;
		
		case PA17:
			REG_PIOA_PER |= PIO_PER_P17;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P17;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P17;
				break;
			}
		break;
		
		case PA18:
			REG_PIOA_PER |= PIO_PER_P18;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P18;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P18;
				break;
			}
		break;
		
		case PA19:
			REG_PIOA_PER |= PIO_PER_P19;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P19;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P19;
				break;
			}
		break;
		
		case PA20:
			REG_PIOA_PER |= PIO_PER_P20;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P20;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P20;
				break;
			}
		break;
		
		case PA21:
			REG_PIOA_PER |= PIO_PER_P21;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P21;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P21;
				break;
				}
		break;
		
		case PA22:
			REG_PIOA_PER |= PIO_PER_P22;
	      	switch(mode)
	    	{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P22;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P22;
				break;
			}
		break;
		
		case PA23:
			REG_PIOA_PER |= PIO_PER_P23;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P23;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P23;
				break;
			}
		break;
		
		case PA24:
			REG_PIOA_PER |= PIO_PER_P24;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P24;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P24;
				break;
			}
		break;
		
		case PA25:
			REG_PIOA_PER |= PIO_PER_P25;
			switch(mode)
			{	
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P25;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P25;
				break;
			}
		break;
		
		case PA26:
			REG_PIOA_PER |= PIO_PER_P26;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P26;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P26;
				break;
			}
		break;
		
		case PA27:
			REG_PIOA_PER |= PIO_PER_P27;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P27;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P27;
				break;
			}
		break;
		
		case PA28:
			REG_PIOA_PER |= PIO_PER_P28;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P28;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P28;
				break;
			}
		break;
		
		case PA29:
			REG_PIOA_PER |= PIO_PER_P29;
			switch(mode)
			{	
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P29;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P29;
				break;
			}
		break;
		
		case PA30:
			REG_PIOA_PER |= PIO_PER_P30;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P30;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P30;
				break;
			}
		break;
		
		case PA31:
			REG_PIOA_PER |= PIO_PER_P31;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOA_OER |= PIO_PER_P31;
				break;
				case INPUT:
					REG_PIOA_ODR |= PIO_PER_P31;
				break;
			}
		break;
		
		case PB0:
			REG_PIOB_PER |= PIO_PER_P0;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P0;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P0;
				break;
			}
		break;
		
		case PB1:
			REG_PIOB_PER |= PIO_PER_P1;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P1;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P1;
				break;
			}
		break;
		
		case PB2:
			REG_PIOB_PER |= PIO_PER_P2;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P2;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P2;
				break;
			}
		break;
		
		case PB3:
			REG_PIOB_PER |= PIO_PER_P3;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P3;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P3;
				break;
			}
		break;
		
		case PB4:
			REG_PIOB_PER |= PIO_PER_P4;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P4;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P4;
				break;
			}
		break;
		
		case PB5:
			REG_PIOB_PER |= PIO_PER_P5;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P5;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P5;
				break;
			}
		break;
		
		case PB6:
			REG_PIOB_PER |= PIO_PER_P6;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P6;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P6;
				break;
			}
		break;
		
		case PB7:
			REG_PIOB_PER |= PIO_PER_P7;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P7;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P7;
				break;
			}
		break;
		
		case PB8:
			REG_PIOB_PER |= PIO_PER_P8;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P8;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P8;
				break;
			}
		break;
		
		case PB9:
			REG_PIOB_PER |= PIO_PER_P9;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P9;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P9;
				break;
			}
		break;
		
		case PB10:
			REG_PIOB_PER |= PIO_PER_P10;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P10;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P10;
				break;
			}
		break;
		
		case PB11:
			REG_PIOB_PER |= PIO_PER_P11;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P11;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P11;
				break;
			}
		break;
		
		case PB12:
			REG_PIOB_PER |= PIO_PER_P12;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P12;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P12;
				break;
			}
		break;
		
		case PB13:
			REG_PIOB_PER |= PIO_PER_P13;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P13;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P13;
				break;
			}
		break;
		
		case PB14:
			REG_PIOB_PER |= PIO_PER_P14;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P14;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P14;
				break;
			}
		break;
		
		case PB15:
			REG_PIOB_PER |= PIO_PER_P15;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P15;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P15;
				break;
			}
		break;
		
		case PB16:
			REG_PIOB_PER |= PIO_PER_P16;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P16;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P16;
				break;
			}
		break;
		
		case PB17:
			REG_PIOB_PER |= PIO_PER_P17;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P17;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P17;
				break;
			}
		break;
		
		case PB18:
			REG_PIOB_PER |= PIO_PER_P18;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P18;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P18;
				break;
			}
		break;
		
		case PB19:
			REG_PIOB_PER |= PIO_PER_P19;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P19;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P19;
				break;
			}
		break;
		
		case PB20:
			REG_PIOB_PER |= PIO_PER_P20;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P20;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P20;
				break;
			}
		break;
		
		case PB21:
			REG_PIOB_PER |= PIO_PER_P21;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P21;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P21;
				break;
			}
		break;
		
		case PB22:
			REG_PIOB_PER |= PIO_PER_P22;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P22;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P22;
				break;
			}
		break;
		
		case PB23:
			REG_PIOB_PER |= PIO_PER_P23;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P23;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P23;
				break;
			}
		break;
		
		case PB24:
			REG_PIOB_PER |= PIO_PER_P24;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P24;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P24;
				break;
			}
		break;
		
		case PB25:
			REG_PIOB_PER |= PIO_PER_P25;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P25;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P25;
				break;
			}
		break;
		
		case PB26:
			REG_PIOB_PER |= PIO_PER_P26;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P26;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P26;
				break;
			}
		break;
		
		case PB27:
			REG_PIOB_PER |= PIO_PER_P27;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P27;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P27;
				break;
			}
		break;
		
		case PB28:
			REG_PIOB_PER |= PIO_PER_P28;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P28;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P28;
				break;
			}
		break;
		
		case PB29:
			REG_PIOB_PER |= PIO_PER_P29;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P29;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P29;
				break;
			}
		break;
		
		case PB30:
			REG_PIOB_PER |= PIO_PER_P30;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P30;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P30;
				break;
			}
		break;
		
		case PB31:
			REG_PIOB_PER |= PIO_PER_P31;
			switch(mode)
			{
				case OUTPUT:
					REG_PIOB_OER |= PIO_PER_P31;
				break;
				case INPUT:
					REG_PIOB_ODR |= PIO_PER_P31;
				break;
			}
		break;
		
		/*
		case PC0:
		REG_PIOC_PER |= PIO_PER_P0;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P0;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P0;
				break;
			}
		break;
		
		case PC1:
		REG_PIOC_PER |= PIO_PER_P1;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P1;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P1;
				break;
			}
		break;
		
		case PC2:
		REG_PIOC_PER |= PIO_PER_P2;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P2;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P2;
				break;
			}
		break;
		
		case PC3:
		REG_PIOC_PER |= PIO_PER_P3;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P3;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P3;
				break;
			}
		break;
		
		case PC4:
		REG_PIOC_PER |= PIO_PER_P4;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P4;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P4;
				break;
			}
		break;
		
		case PC5:
		REG_PIOC_PER |= PIO_PER_P5;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P5;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P5;
				break;
			}
		break;
		
		case PC6:
		REG_PIOC_PER |= PIO_PER_P6;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P6;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P6;
				break;
			}
		break;
		
		case PC7:
		REG_PIOC_PER |= PIO_PER_P7;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P7;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P7;
				break;
			}
		break;
		
		case PC8:
		REG_PIOC_PER |= PIO_PER_P8;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P8;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P8;
				break;
			}
		break;
		
		case PC9:
		REG_PIOC_PER |= PIO_PER_P9;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P9;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P9;
				break;
			}
		break;
		
		case PC10:
		REG_PIOC_PER |= PIO_PER_P10;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P10;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P10;
				break;
			}
		break;
		
		case PC11:
		REG_PIOC_PER |= PIO_PER_P11;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P11;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P11;
				break;
			}
		break;
		
		case PC12:
		REG_PIOC_PER |= PIO_PER_P12;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P12;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P12;
				break;
			}
		break;
		
		case PC13:
		REG_PIOC_PER |= PIO_PER_P13;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P13;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P13;
				break;
			}
		break;
		
		case PC14:
		REG_PIOC_PER |= PIO_PER_P14;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P14;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P14;
				break;
			}
		break;
		
		case PC15:
		REG_PIOC_PER |= PIO_PER_P15;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P15;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P15;
				break;
			}
		break;
		
		case PC16:
		REG_PIOC_PER |= PIO_PER_P16;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P16;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P16;
				break;
			}
		break;
		
		case PC17:
		REG_PIOC_PER |= PIO_PER_P17;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P17;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P17;
				break;
			}
		break;
		
		case PC18:
		REG_PIOC_PER |= PIO_PER_P18;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P18;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P18;
				break;
			}
		break;
		
		case PC19:
		REG_PIOC_PER |= PIO_PER_P19;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P19;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P19;
				break;
			}
		break;
		
		case PC20:
		REG_PIOC_PER |= PIO_PER_P20;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P20;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P20;
				break;
			}
		break;
		
		case PC21:
		REG_PIOC_PER |= PIO_PER_P21;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P21;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P21;
				break;
			}
		break;
		
		case PC22:
		REG_PIOC_PER |= PIO_PER_P22;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P22;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P22;
				break;
			}
		break;
		
		case PC23:
		REG_PIOC_PER |= PIO_PER_P23;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P23;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P23;
				break;
			}
		break;
		
		case PC24:
		REG_PIOC_PER |= PIO_PER_P24;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P24;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P24;
				break;
			}
		break;
		
		case PC25:
		REG_PIOC_PER |= PIO_PER_P25;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P25;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P25;
				break;
			}
		break;
		
		case PC26:
		REG_PIOC_PER |= PIO_PER_P26;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P26;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P26;
				break;
			}
		break;
		
		case PC27:
		REG_PIOC_PER |= PIO_PER_P27;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P27;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P27;
				break;
			}
		break;
		
		case PC28:
		REG_PIOC_PER |= PIO_PER_P28;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P28;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P28;
				break;
			}
		break;
		
		case PC29:
		REG_PIOC_PER |= PIO_PER_P29;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P29;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P29;
				break;
			}
		break;
		
		case PC30:
		REG_PIOC_PER |= PIO_PER_P30;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P30;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P30;
				break;
			}
		break;
		
		case PC31:
		REG_PIOC_PER |= PIO_PER_P31;
			switch(mode)
			{
				case OUTPUT:
				REG_PIOC_OER |= PIO_PER_P31;
				break;
				case INPUT:
				REG_PIOC_ODR |= PIO_PER_P31;
				break;
			}
		break;
		*/
	}
}

void digitalWrite(uint8_t PIN, uint8_t level)
{
	switch(PIN)
	{
		case PA0:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P0;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P0;
			break;
		}
		break;
		
		case PA1:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P1;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P1;
			break;
		}
		break;
		
		case PA2:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P2;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P2;
			break;
		}
		break;
		
		case PA3:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P3;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P3;
			break;
		}
		break;
		
		case PA4:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P4;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P4;
			break;
		}
		break;
		
		case PA5:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P5;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P5;
			break;
		}
		break;
		
		case PA6:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P6;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P6;
			break;
		}
		break;
		
		case PA7:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P7;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P7;
			break;
		}
		break;
		
		case PA8:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P8;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P8;
			break;
		}
		break;
		
		case PA9:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P9;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P9;
			break;
		}
		break;
		
		case PA10:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P10;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P10;
			break;
		}
		break;
		
		case PA11:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P11;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P11;
			break;
		}
		break;
		
		case PA12:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P12;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P12;
			break;
		}
		break;
		
		case PA13:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P13;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P13;
			break;
		}
		break;
		
		case PA14:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P14;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P14;
			break;
		}
		break;
		
		case PA15:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P15;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P15;
			break;
		}
		break;
		
		case PA16:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P16;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P16;
			break;
		}
		break;
		
		case PA17:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P17;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P17;
			break;
		}
		break;
		
		case PA18:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P18;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P18;
			break;
		}
		break;
		
		case PA19:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P19;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P19;
			break;
		}
		break;
		
		case PA20:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P20;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P20;
			break;
		}
		break;
		
		case PA21:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P21;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P21;
			break;
		}
		break;
		
		case PA22:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P22;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P22;
			break;
		}
		break;
		
		case PA23:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P23;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P23;
			break;
		}
		break;
		
		case PA24:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P24;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P24;
			break;
		}
		break;
		
		case PA25:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P25;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P25;
			break;
		}
		break;
		
		case PA26:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P26;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P26;
			break;
		}
		break;
		
		case PA27:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P27;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P27;
			break;
		}
		break;
		
		case PA28:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P28;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P28;
			break;
		}
		break;
		
		case PA29:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P29;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P29;
			break;
		}
		break;
		
		case PA30:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P30;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P30;
			break;
		}
		break;
		
		case PA31:
		switch(level)
		{
			case LOW:
				REG_PIOA_CODR|=	PIO_PER_P31;
			break;
			case HIGH:
				REG_PIOA_SODR|=	PIO_PER_P31;
			break;
		}
		break;
		
		case PB0:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P0;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P0;
			break;
		}
		break;
		
		case PB1:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P1;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P1;
			break;
		}
		break;
		
		case PB2:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P2;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P2;
			break;
		}
		break;
		
		case PB3:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P3;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P3;
			break;
		}
		break;
		
		case PB4:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P4;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P4;
			break;
		}
		break;
		
		case PB5:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P5;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P5;
			break;
		}
		break;
		
		case PB6:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P6;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P6;
			break;
		}
		break;
		
		case PB7:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P7;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P7;
			break;
		}
		break;
		
		case PB8:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P8;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P8;
			break;
		}
		break;
		
		case PB9:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P9;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P9;
			break;
		}
		break;
		
		case PB10:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P10;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P10;
			break;
		}
		break;
		
		case PB11:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P11;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P11;
			break;
		}
		break;
		
		case PB12:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P12;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P12;
			break;
		}
		break;
		
		case PB13:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P13;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P13;
			break;
		}
		break;
		
		case PB14:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P14;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P14;
			break;
		}
		break;
		
		case PB15:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P15;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P15;
			break;
		}
		break;
		
		case PB16:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P16;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P16;
			break;
		}
		break;
		
		case PB17:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P17;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P17;
			break;
		}
		break;
		
		case PB18:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P18;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P18;
			break;
		}
		break;
		
		case PB19:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P19;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P19;
			break;
		}
		break;
		
		case PB20:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P20;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P20;
			break;
		}
		break;
		
		case PB21:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P21;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P21;
			break;
		}
		break;
		
		case PB22:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P22;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P22;
			break;
		}
		break;
		
		case PB23:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P23;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P23;
			break;
		}
		break;
		
		case PB24:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P24;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P24;
			break;
		}
		break;
		
		case PB25:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P25;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P25;
			break;
		}
		break;
		
		case PB26:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P26;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P26;
			break;
		}
		break;
		
		case PB27:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P27;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P27;
			break;
		}
		break;
		
		case PB28:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P28;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P28;
			break;
		}
		break;
		
		case PB29:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P29;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P29;
			break;
		}
		break;
		
		case PB30:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P30;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P30;
			break;
		}
		break;
		
		case PB31:
		switch(level)
		{
			case LOW:
				REG_PIOB_CODR|=	PIO_PER_P31;
			break;
			case HIGH:
				REG_PIOB_SODR|=	PIO_PER_P31;
			break;
		}
		break;
		/*
		case PC0:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P0;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P0;
			break;
		}
		break;
		
		case PC1:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P1;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P1;
			break;
		}
		break;
		
		case PC2:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P2;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P2;
			break;
		}
		break;
		
		case PC3:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P3;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P3;
			break;
		}
		break;
		
		case PC4:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P4;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P4;
			break;
		}
		break;
		
		case PC5:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P5;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P5;
			break;
		}
		break;
		
		case PC6:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P6;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P6;
			break;
		}
		break;
		
		case PC7:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P7;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P7;
			break;
		}
		break;
		
		case PC8:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P8;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P8;
			break;
		}
		break;
		
		case PC9:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P9;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P9;
			break;
		}
		break;
		
		case PC10:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P10;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P10;
			break;
		}
		break;
		
		case PC11:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P11;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P11;
			break;
		}
		break;
		
		case PC12:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P12;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P12;
			break;
		}
		break;
		
		case PC13:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P13;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P13;
			break;
		}
		break;
		
		case PC14:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P14;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P14;
			break;
		}
		break;
		
		case PC15:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P15;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P15;
			break;
		}
		break;
		
		case PC16:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P16;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P16;
			break;
		}
		break;
		
		case PC17:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P17;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P17;
			break;
		}
		break;
		
		case PC18:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P18;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P18;
			break;
		}
		break;
		
		case PC19:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P19;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P19;
			break;
		}
		break;
		
		case PC20:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P20;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P20;
			break;
		}
		break;
		
		case PC21:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P21;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P21;
			break;
		}
		break;
		
		case PC22:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P22;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P22;
			break;
		}
		break;
		
		case PC23:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P23;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P23;
			break;
		}
		break;
		
		case PC24:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P24;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P24;
			break;
		}
		break;
		
		case PC25:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P25;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P25;
			break;
		}
		break;
		
		case PC26:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P26;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P26;
			break;
		}
		break;
		
		case PC27:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P27;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P27;
			break;
		}
		break;
		
		case PC28:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P28;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P28;
			break;
		}
		break;
		
		case PC29:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P29;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P29;
			break;
		}
		break;
		
		case PC30:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P30;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P30;
			break;
		}
		break;
		
		case PC31:
		switch(level)
		{
			case LOW:
			REG_PIOC_CODR|=	PIO_PER_P31;
			break;
			case HIGH:
			REG_PIOC_SODR|=	PIO_PER_P31;
			break;
		}
		break;
		*/
	}
}
