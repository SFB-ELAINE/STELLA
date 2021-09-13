#ifndef MAIN_H_
#define MAIN_H_

#define BATTERY_VOLTAGE_THRESHOLD 2400   // 2.4V

#define IMPLANT_FREQUENCY         130     // Hz
#define IMPLANT_PULSE_WIDTH       60      // us
#define IMPLANT_PERIOD            8000       // us

//#define IMPLANT_TA0CCR0         7691    // (1000000 / IMPLANT_FREQUENCY - 1)
//#define IMPLANT_TA0CCR1         7631    // (1000000 / IMPLANT_FREQUENCY - 1 - IMPLANT_PULSE_WIDTH)

#define FREQ_1MHz_IMPLANT_TA0CCR0      60      // (1000000 / IMPLANT_FREQUENCY - 1)

#define FREQ_32kHz_IMPLANT_TA0CCR0     251     // (1000000 / IMPLANT_FREQUENCY - 1)
#define FREQ_32kHz_IMPLANT_TA0CCR1     250     // (1000000 / IMPLANT_FREQUENCY - 1 - IMPLANT_PULSE_WIDTH)

//#define FREQ_32kHz_IMPLANT_TA0CCR0     12000 // (1000000 / IMPLANT_FREQUENCY - 1)
//#define FREQ_32kHz_IMPLANT_TA0CCR1     11999    // (1000000 / IMPLANT_FREQUENCY - 1 - IMPLANT_PULSE_WIDTH)





typedef enum {
    BATTERY_LEVEL_NORMAL,
    BATTERY_LEVEL_LOW
} Battery_Status;

typedef enum {
    CHAN_LEFT,
    CHAN_RIGHT,
} Stimulation_Channel;


typedef enum {
    UNILATERAL,
    BILATERAL
} Stimulation_Type;


typedef enum {
    IMPEDANCE_NORMAL,
    IMPEDANCE_LOW,
    IMPEDANCE_HIGH
} Impedance_Status;


typedef enum {
    CURRENT_LEFT,
    CURRENT_RIGHT,
    COUNTERPULSE_ATTENUATION_LEFT,
    COUNTERPULSE_ATTENUATION_RIGHT,
    VBOOSTER,
    VREF
} Digipot_Function;


typedef enum {
    RESOLUTION_LOW,
    RESOLUTION_HIGH
} Timer_Resolution;


typedef enum {
    STIMULATION_ON,
    STIMULATION_OFF
} Stimulation_Status;

typedef enum { // Built-In Self-Test
    BIST_ON,
    BIST_OFF
} BIST_Status;



typedef enum {
    AD5144,
    AD5142
} AD_Type;


void delay_ms(unsigned int delay);

#endif
