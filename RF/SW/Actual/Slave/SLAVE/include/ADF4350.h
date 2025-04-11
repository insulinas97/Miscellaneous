/***************************************************************************/
/* Macros para comandos y registros del sintetizador de frecuencia ADF4350 */
/***************************************************************************/

#define RESERVED               0  // MACRO COMUN PARA EL ADF4350 Y EL AD9914
#define	ADF4350_RESERVED_HIGH              1 
// REGISTRO 5 
// [31:24]
// RESERVED
// [23:16]
#define	ADF4350_LD_PIN_MODE(x)             x   // Lock detection pin mode -> 00: low, 01: digital lock detect, 10:  low, 11: high
// [15:8]
// RESERVED 
// [7:0]
#define	ADF4350_CTRL_REG5                  0b101 // Direccion del registro de control 5

// REGISTRO 4 
// [31:24]
// RESERVED
// [23:16]
//select the value of the output divider
#define ADF4350_DIV_SEL(X)                 X

#define	ADF4350_FBCK_DIVIDED               0    // 0: Divided. The signal is taken from the output of the output dividers 
#define	ADF4350_FBCK_FUNDAMENTAL           1    // 1: Fundamental. The signal is taken from the VCO directly

// Set a divider for the band select logic clock input
#define	ADF4350_SEL_CLK_DIV_VALUE(x)       x    // Select clock divider value. 0x01: 1   0x02:  2  ..  0xFF: 255    
// [15:8]
#define	ADF4350_VCO_PWR_UP                 0     // 0: VCO POWERED UP 
#define	ADF4350_VCO_PWR_DWN                1     // 1: VCO POWERED DOWN

// MUTE TILL LOCK DETECT
#define	ADF4350_MTLD_DISABLE               0     // 0: mute disable
#define	ADF4350_MTLD_ENABLE                1     // 1: mute enable. the supply current to the RF output stage is shut down until the device achieves 
                                                 // lock as measured by the digital lock detect circuitry

#define	ADF4350_AUX_OUT_DIVIDED            0     // 0: DIVIDED OUTPUT
#define	ADF4350_AUX_OUT_FUNDAMENTAL        1     // 1: FUNDAMENTAL VCO FRECUENCY
#define	ADF4350_AUX_OUT_DISABLE            0     // 0: DISABLED auxiliary RF
#define	ADF4350_AUX_OUT_ENABLE             1     // 1: ENABLED auxiliary RF
// [7:0]
#define	ADF4350_AUX_OUT_PWR(x)             x     // Auxiliary RF output power level 00: -4,   01: -1,   10: +2    11: +5
#define	ADF4350_RF_OUT_DISABLE             0     // 0: DISABLE primary RF output 
#define	ADF4350_RF_OUT_ENABLE              1     // 1: ENABLE primary RF output
#define	ADF4350_OUT_PWR(x)                 x     // value of the primary RF output power level -> 00: -4,   01: -1,   10: +2    11: +5
#define	ADF4350_CTRL_REG4                  0b100 // Direccion del registro de control 4

// REGISTRO 3 
// [31:24]
// RESERVED
// [23:16]
#define	ADF4350_CSR_DISABLE                0     // 0: disables cycle slip reduction
#define	ADF4350_CSR_ENABLE                 1     // 1: enables cycle slip reduction
#define	ADF4350_CLK_DIV_MODE(x)            x     // CLOCK DIVIDER MODE -> 00: OFF,   01: FAST-LOCK ENABLE,   10: RESYNC ENABLE,   11: RESERVED
// [15:8]
// sets the timeout counter for activation of PHASE resync
#define	ADF4350_CLK_DIV_VALUE(x)           x     // 0x000: 0,   0x001: 1, ..  0xFFF: 4095
// [7:0]
#define	ADF4350_CTRL_REG3                  0b011 // Direccion del registro de control 3 

// REGISTRO 2 
// [31:24]
/*The noise modes allow the user to optimize a design either for improved 
spurious perfor-mance or for improved phase noise performance.*/
#define	ADF4350_LOW_NOISE_AND_SPUR_MODES(x)   x  // 00: LOW NOISE MODE   01: RESERVED,   10: RESERVED,   11: LOW SPURE MODE

#define	ADF4350_MUX_OUT(x)                 x     // 000: THREE-STATE OUTPUT,   001: DVDD,                 010:  DGND,                011: R DIVIDER OUTPUT  
                                                 // 100: N DIVIDER OUTPUT,     101: ANALOG LOCK DETECT,   110: DIGITAL LOCK DETECT   111: RESERVED
#define	ADF4350_REF_DOUBLER_DISABLE        0     // 0: feeds the REFIN signal directly to the 10–bit R counter, disabling the doubler
#define	ADF4350_REF_DOUBLER_ENABLE         1     // 1: multiplies the REFIN frequency by a factor of 2 before feeding into the 10-bit R counter
#define	ADF4350_RDIV2_DISABLE              0     // 0: DISABLE REFERENCE DIVIDE BY 2
#define	ADF4350_RDIV2_ENABLE               1     // 1: inserts a divide-by-2 toggle flip-flop between the R counter and PFD,
// [23:16]
/*The 10–bit R counter allows the input reference frequency (REFIN) 
to be divided down to produce the reference clock to the PFD.*/
#define	ADF4350_R_COUNTER(x)               x     // R DIVIDER: 0000000000: 1, ..  1111111111: 1023
// [15:8]
#define	ADF4350_DOUBLE_BUFF_DISABLE        0     // 0: DOUBLE BUFFER DISABLED
#define	ADF4350_DOUBLE_BUFF_ENABLE         1     // 1: DOUBLE BUFFER ENABLED
#define	ADF4350_CP_CURRENT_SET(x)          x     // CHARGE PUMP CURRENT SETTING -> mA que pasan por la R = 5.1k Ohm,  0x0: 0.31mA, .. 0x7: 2.5mA .. 0xF: 5mA
#define	ADF4350_LDF_FRACN			       0     // 0: enables fractional–N digital lock detect
#define	ADF4350_LDF_INTN			       1     // 1: enables integer–N digital lock detect
// [7:0]
#define	ADF4350_LDP_10NS                   0     // 0: 40 consecutive PFD cycles of 10 ns must occur before digital lock detect is set
#define	ADF4350_LDP_6NS                    1     // 1: 40 consecutive reference cycles of 6 ns must occur before digital lock detect is set
#define	ADF4350_PD_POLARIRY_NEGATIVE       0     // Sets the phase detector polarity, 0: Negative,  
#define	ADF4350_PD_POLARIRY_POSITIVE       1     // Sets the phase detector polarity, 1: Positive

// Programmable power-down mode
#define	ADF4350_PWR_DWN_DISABLE 	       0     // 0: returns the synthesizer to normal operation
#define	ADF4350_PWR_DWN_ENABLE  	       1     // 1: performs a power-down

#define	ADF4350_CP_THREE_STATE_DISABLE     0     // It must be set to 0 for normal operation 
#define	ADF4350_CP_THREE_STATE_ENABLE      1     // 1: puts the charge pump into three-state mode
#define	ADF4350_COUNTER_RST_DISABLE        0     // For normal operation, this bit must be set to 0
#define	ADF4350_COUNTER_RST_ENABLE         1     // 1: The RF synthesizer N counter and R counter are held in reset
#define	ADF4350_CTRL_REG2                  0b010 // Direccion del registro de control 2

// REGISTRO 1 
// [31:24]
/* Operating at CML levels, the prescaler takes the clock from the VCO output and divides it down for the counters. 
It is based on a synchronous 4/5 core. When set to 4/5, the maximum RF frequency allowed is 3 GHz. Therefore, when 
operating the ADF4350 above 3 GHz, this must be set to 8/9. The prescaler limits the INT value, where P is 4/5, NMIN 
is 23 and P is 8/9, NMIN is 75. */
#define	ADF4350_PRESCALER_4_5              0     // 0: 4/5       
#define	ADF4350_PRESCALER_8_9              1     // 1: 8/9

/* These bits control what is loaded as the phase word. The word must be less than the MOD value programmed in Register 1.
 The word programs the RF output phase from 0° to 360° with a resolution of 360°/MOD */
#define	ADF4350_PHASE_VALUE(x)             x     // 0x000: 0,   0xFFF: 4095
// [23:16]
// [15:8]
// [7:0]
/* This programmable register sets the fractional modulus. This is the ratio of the PFD frequency to the channel step 
resolution on the RF output. See the RF Synthesizer */
#define ADF4350_MODULUS(X)                 X     //0x000: 0,   0xFFF: 4095

#define	ADF4350_CTRL_REG1                  0b001 // Direccion del registro de control 1

// REGISTRO 0 
// Determines the integer device of the feedback division factor
#define ADF4350_INT_VALUE(x)               x     // Integger value

/* The 12 FRAC bits set the numerator of the fraction that is input to the sigma-delta modulator. 
This, along with INT, specifies the new frequency channel that the synthesizer locks to, 
as shown in the RF Synthesizer */
#define ADF4350_FRAC_VALUE(x)              x     // Fractional Value

#define	ADF4350_CTRL_REG0                  0b000 // Direccion del registro de control 0

#define BYTE_1(x)	(x & 0x000000FF)
#define BYTE_2(x)	((x & 0x0000FF00) >> 8)
#define BYTE_3(x)	((x & 0x00FF0000) >> 16)
#define BYTE_4(x)	((x & 0xFF000000) >> 24)