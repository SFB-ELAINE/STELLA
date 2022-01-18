#ifndef MAIN_H_
#define MAIN_H_

#define SLEEP_MODE = 0x00
#define DEEP_SLEEP_MODE = 0x01

#define BATTERY_VOLTAGE_THRESHOLD   2600    // 2.6V

#define IMPLANT_FREQUENCY           130     // Hz
#define IMPLANT_PULSE_WIDTH         60      // us
#define IMPLANT_PERIOD              8000       // us

#define SLEEP_IMPLANT_TA0CCR0             7691    // (1000000 / IMPLANT_FREQUENCY - 1)
#define SLEEP_IMPLANT_TA0CCR1             7631    // (1000000 / IMPLANT_FREQUENCY - 1 - IMPLANT_PULSE_WIDTH)

#define DEEPSLEEP_32kHz_IMPLANT_TA0CCR0             252    // (1000000 / IMPLANT_FREQUENCY - 1)
#define DEEPSLEEP_32kHz_IMPLANT_TA0CCR1             251    // (1000000 / IMPLANT_FREQUENCY - 1 - IMPLANT_PULSE_WIDTH)

#define DEEPSLEEP_16kHz_IMPLANT_TA0CCR0            126    // (1000000 / IMPLANT_FREQUENCY - 1)
#define DEEPSLEEP_16kHz_IMPLANT_TA0CCR1            126    // (1000000 / IMPLANT_FREQUENCY - 1 - IMPLANT_PULSE_WIDTH)

typedef enum {
    BATTERY_LEVEL_NORMAL,
    BATTERY_LEVEL_LOW
} Battery_Status;

typedef enum {
    LATERAL_LEFT,
    LATERAL_RIGHT,

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
    SLEEP,
    DEEP_SLEEP
} Power_Mode;


typedef enum {
    CLK_32kHz,
    CLK_16kHz
} Stimulation_CLK;


typedef enum {
    STIMULATION_OFF,
    STIMULATION_ON
} Stimulation_Status;



typedef enum {
    AD5144,
    AD5142,
    CC2500
} AD_Type;

typedef enum {
    GET,
    SET
} Function;

void delay_ms(unsigned int delay);
#endif
