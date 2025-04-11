/************************************************************************/
/* Macros para comandos y registros del DDS AD9914                      */
/************************************************************************/

#define RESERVED               0  // MACRO COMUN PARA EL ADF4350 Y EL AD9914
#define AD9914_OPEN            0

// CFR1 REGISTER
#define	AD9914_CFR1            0x00 // DIRECCION REGISTRO CFR1
// [31:24]
#define	AD9914_VCO_CAL_NO_CAL             0     // Must first be reset to Logic 0 before another calibration can be issued
#define AD9914_VCO_CAL_AUTO_CAL           1     // 1 initializes the auto internal PLL calibration
// [23:16]
#define	AD9914_PARALLEL_PORT_STR_ENABLE_CONT_UPDATE   1     // 1 the parallel port continuously samples data using SYNC_CLK
#define	AD9914_PARALLEL_PORT_STR_ENABLE_IO_UPDATE     0     // 0 the 32 bit parallel port needs an input/output update;
#define	AD9914_ENABLE_SINE_OUTPUT                     1     // 1 sine output of the DDS is selected (default).
#define	AD9914_ENABLE_COSINE_OUTPUT                   0     // 0 cosine output of the DDS is selected;
// [15:8]
#define	AD9914_LOAD_LRR_NORMAL                 0     // 0 normal operation of the digital ramp timer.
#define	AD9914_LOAD_LRR_INTERRUPT              1     // 1 interrupts the digital ramp timer operation to load a new LRR value any time I/O_UPDATE is
// asserted or a PS[2:0] change occurs.
#define	AD9914_AUTO_CLR_DGTL_RMP_ACC_NORMAL    0     // 0 Normal operation of the DRG accumulator.
#define	AD9914_AUTO_CLR_DGTL_RMP_ACC_RESET     1     // 1 reset the digital ramp accumulator
#define	AD9914_AUTO_CLR_PHS_ACC_NORMAL         0     // 0 Normal operation of the DDS phase accumulator.
#define	AD9914_AUTO_CLR_PHS_ACC_RESET          1     // 1 resets the DDS phase accumulator at I/O_UPDATE times
#define	AD9914_CLR_DGTL_RMP_ACC_NORMAL         0     // 0 Normal operation of the DRG accumulator.
#define	AD9914_CLR_DGTL_RMP_ACC_RESET          1     // 1 static reset of the DRG accumulator.
#define	AD9914_CLR_PHS_ACC_NORMAL              0     // 0 Normal operation of the DDS phase accumulator.
#define	AD9914_CLR_PHS_ACC_RESET               1     // 1 reset the DDS phase accumulator
#define	AD9914_EXTERNAL_OSK_MANUAL             0     // 0 manual OSK enabled;
#define	AD9914_EXTERNAL_OSK_AUTOMATIC          1     // 1 automatic OSK enabled
#define	AD9914_OSK_DISABLE                     0     // Output shift keying enable bit, 0: disabled
#define	AD9914_OSK_ENABLE                      1     // Output shift keying enable bit, 1: enabled
// [7:0]
#define	AD9914_DIGITAL_PWR_DWN_ACTIVE          0     // Clock signals to the digital core are, (0: active)
#define	AD9914_DIGITAL_PWR_DWN_DISABLE         1     // Clock signals to the digital core are, (1: disabled)
#define	AD9914_DAC_PWR_DWN_ACTIVE              0     // Clock signals to the digital core are, (0: active)
#define	AD9914_DAC_PWR_DWN_DISABLE             1     // Clock signals to the digital core are, (1: disabled)
#define	AD9914_REFCLK_INPUT_PWR_DWN_ACTIVE     0     // REFCLK input circuits and PLL are (0: active)
#define	AD9914_REFCLK_INPUT_PWR_DWN_DISABLE    1     // REFCLK input circuits and PLL are (1: disabled)
#define	AD9914_EXT_PWR_DWN_CTRL_PWR_DWN        1     // Assertion of the EXT_PWR_DWN affects (1: power-down)
#define	AD9914_EXT_PWR_DWN_CTRL_FAST_RECOVERY  0     // Assertion of the EXT_PWR_DWN affects (0: fast recovery power-down)
#define	AD9914_SDIO_INPUT_ONLY_2_WIRE          0     // 0 SDIO bidirectional - 2-wire
#define	AD9914_SDIO_INPUT_ONLY_3_WIRE          1     // 1 configures the SDIO input - 3-wire
#define	AD9914_LSB_FIRST                       1     // 0 MSB-first format; 1 LSB first
#define	AD9914_MSB_FIRST                       0     // 0 MSB-first format; 1 LSB first

// CFR2 REGISTER
#define	AD9914_CFR2                            0x01  // DIRECCION REGISTRO CFR2
// [31:24]
// OPEN
// [23:16]
#define	AD9914_PROFILE_MOD_DISABLE               0     // Profile mode functionality (0: disable)
#define	AD9914_PROFILE_MOD_ENABLE                1     // Profile mode functionality (1: enable)
#define	AD9914_PARALLEL_DATA_PORT_ENABLE         1     // Parallel data port modulation functionality (0 : disable)
#define	AD9914_PARALLEL_DATA_PORT_DISABLE        0     // Parallel data port modulation functionality (1: enable)
#define	AD9914_DGTL_RMP_DEST(x)                  x     // 00: FREQ; 01; PHASE; 1X: AMPLITUDE
#define	AD9914_DGTL_RMP_ENABLE                   1     // Digital ramp generator is (1 enabled)
#define	AD9914_DGTL_RMP_DISABLE                  0     // Digital ramp generator is (0 disabled)
#define	AD9914_DGTL_RMP_NO_DWELL_HIGH_ENABLE     1     // Digital ramp no dwell high (1 enabled)
#define	AD9914_DGTL_RMP_NO_DWELL_HIGH_DISABLE    0     // Digital ramp no dwell high (0 disabled)
#define	AD9914_DGTL_RMP_NO_DWELL_LOW_ENABLE      1     // Digital ramp no dwell low (1 enabled)
#define	AD9914_DGTL_RMP_NO_DWELL_LOW_DISABLE     0     // Digital ramp no dwell low (0 disabled)
#define	AD9914_PROG_MOD_DISABLE                  0     // Programmable modulus (0: disable)
#define	AD9914_PROG_MOD_ENABLE                   1     // Programmable modulus (1: enable)
// [15:8]
#define	AD9914_MATCH_LATENCY_ORDER_LIST       0     // Simultaneous application of amplitude, phase and frequency changes to the dds arrive at the output (0: in the order list)
#define	AD9914_MATCH_LATENCY_SIMULTANEOUSLY   1     // Simultaneous application of amplitude, phase and frequency changes to the dds arrive at the output (1: simulatenously)
#define	AD9914_FREC_JUMP_DISABLE              0     // Frequency jump mode (0: disable, 1: enable)
#define	AD9914_FREC_JUMP_ENABLE               1     // Frequency jump mode (1: enable). Must have the digital generator DRG enabled for this feature
#define	AD9914_DRG_OVER_OUT_DISABLE           0     // DROVER output (0: disable)
#define	AD9914_DRG_OVER_OUT_ENABLE            1     // DROVER output (1: enable)
#define	AD9914_SYNC_CLK_DISABLE               0     // 0: the SYNC_CLK pin is disabled and forced to a static Logic 0 state; the internal clock signal continues to operate and provide timing
#define	AD9914_SYNC_CLK_ENABLE                1     // 1: the internal SYNC_CLK signal appears at the SYNC_CLK pin
#define	AD9914_SYNC_CLK_NORMAL                0     // Pin polarity (0: normal)
#define AD9914_SYNC_CLK_INVERT                1     // Pin polarity (1: inverted)
// [7:0]
// OPEN

// CFR3 REGISTER
#define	AD9914_CFR3                           0x02  // DIRECCION REGISTRO CFR3
// [31:24]
// OPEN
// [23:16]
#define	AD9914_PLL_INPUT_DIV_RST_DISABLE  0     // 0: disables the PLL input divider reset function
#define	AD9914_PLL_INPUT_DIV_RST_RESET    1     // 1: resets the PLL input divider

// Divides the input REF CLK signal to the PLL by one of three values (2, 4, 8). Bit 17 must be set to Logic 1 to use the PLL input divider
#define	AD9914_PLL_INPUT_DIV_VAL(x)       x     // 00:disable divider, 01:divide by 2, 10:divide by 4, 11:divide by 8

// This bit is only meaningful when CFR3[18] = 1 and CFR3[17] = 0
#define	AD9914_PLL_INPUT_DOUBLER_SEL_2X            1 // 1: selects the 2× path
#define	AD9914_PLL_INPUT_DOUBLER_SEL_FEEDTHROUGH   0 // 0: selects the feedthrough path

#define	AD9914_PLL_ENABLE                 1     // enables the internal PLL for generating the system clock (the PLL must be calibrated via CFR1[24])
#define	AD9914_PLL_DISABLE                0     // disables the internal PLL

// This bit is only meaningful when CFR3[18] = 1.
#define	AD9914_PLL_INPUT_DIV              1     // 1: selects the PLL input divider path
#define	AD9914_PLL_INPUT_DIV_2X           0     // 0: Selects the feed through and 2× multiplier paths

// This bit is only meaningful when CFR3[18] = 1, CFR3[17] = 0 and CFR3[19] = 0. The PLL locks to the selected edge of REF_CLK.
#define	AD9914_PLL_EDGE_RISING            0     // 0: select rising edge
#define	AD9914_PLL_EDGE_FALLING           1     // 1: select falling edge
// [15:8]
/*The N divider value in Bits[15:8] is one part of the total PLL multiplication available. The second part is the fixed divide
by two element in the feedback path. Therefore, the total PLL multiplication value is 2N. The valid N divider range is
10× to 255×. The default N value for Bits[15:8] = 25, which sets the total default PLL multiplication to 50× or 2N.*/
#define	AD9914_FBCK_DIV(x)                x     // PLL divide modulus
// [7:0]
#define	AD9914_ICP_SEL_AUTOMATICALLY      0     // 0: the internal charge pump current is chosen automatically during the VCO calibration routine
#define	AD9914_ICP_SEL_MAUALLY            1     // 1 = the internal charge pump is set manually
#define	AD9914_ICP(x)                     x     // CHARGE PUMP CURRENT 000: 125uA; 001: 250uA; 010: 375uA; 011: 500uA; 100: 625uA; 101: 750uA; 110: 875uA & 111: 1000uA
#define	AD9914_LD_DISABLE                 0     // 0: disables PLL lock detection.
#define	AD9914_LD_ENABLE                  1     // 1: enables PLL lock detection.

/* Selects the number of REF CLK cycles that the phase error (at the PFD inputs) must remain within before a PLL lock
condition can be read back via Bit 24 in Register 0x00*/
#define	AD9914_MIN_LDW(x)                 x      // 00: 128 REF CLK cycles;  01: 256 REF CLK cycles;  10: 512 REF CLK cycles;  11; 1024 REF CLK cycles;

// CFR4 REGISTER
#define	AD9914_CFR4  0x03  // DIRECCION REGISTRO CFR4
// [31:24]
#define	AD9914_AUX_DIV_PWR_DWN_ENABLE      0     // 0: enables the SYNC OUT circuitry
#define	AD9914_AUX_DIV_PWR_DWN_DISABLE     1     // 1: disables the SYNC OUT circuitry
#define	AD9914_DAC_CAL_CLK_PWR_DWN_ENABLE  0     // 0 = enables the DAC CAL clock if Bit 26 in Register 0x03 is Logic 0
#define	AD9914_DAC_CAL_CLK_PWR_DWN_DISABLE 1     // 1: disables the DAC CAL clock

/* The DAC CAL calibration is required at power-up and any time the internal
system clock is changed*/
#define	AD9914_DAC_CAL_ENABLE              1     // Initiates DAC CAL calibration
#define	AD9914_DAC_CAL_DISABLE             0     // Stop DAC CAL calibration
// [23:16]
#define	AD9914_DEFAULT_CFR4_1              0x05  // Requires register default value settings CFR4
// [15:8]
#define	AD9914_DEFAULT_CFR4_2              0x21  // Requires register default value settings CFR4
// [7:0]
#define	AD9914_DEFAULT_CFR4_3              0x20  // Requires register default value settings CFR4

#define AD9914_DIGITAL_RAMP_LOWER_LIMIT_REGISTER        0x04  // Direccion del registro limite inferior de la rampa
#define AD9914_DIGITAL_RAMP_UPPER_LIMIT_REGISTER        0x05  // Direccion del registro limite superior de la rampa
#define AD9914_RISING_DIGITAL_RAMP_STEP_SIZE_REGISTER   0x06  // Direccion del registro del tamaño del paso de subida de la rampa
#define AD9914_FALLING_DIGITAL_RAMP_STEP_SIZE_REGISTER  0x07  // Direccion del registro del tamaño del paso de bajada de la rampa
#define AD9914_DIGITAL_RAMP_RATE_REGISTER               0x08  // Direccion del registro de velocidad de la rampa

#define AD9914_PARAMETRO_DWELLING_DRR(x)                x     // Parametro de dwelling del registro 8
#define AD9914_STEP_SIZE(x)                             x     // Frequency Tunning Word parameter (Step Size) del registro 6 y 7
#define AD9914_FREQUENCY_STOP(x)                        x     // Frequency stop del registro 5
#define AD9914_FREQUENCY_START(x)                       x     // Frequency start del registro 4

#define BYTE_1(x)	(x & 0x000000FF)
#define BYTE_2(x)	((x & 0x0000FF00) >> 8)
#define BYTE_3(x)	((x & 0x00FF0000) >> 16)
#define BYTE_4(x)	((x & 0xFF000000) >> 24)