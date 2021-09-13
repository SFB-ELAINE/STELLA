#ifndef ADC_H_
#define ADC_H_


// Boost Voltage Pin
#define EN_VBOOST_FET BIT2
#define PORT_EN_VBOOST_FET_OUT P2OUT
#define PORT_EN_VBOOST_FET_DIR P2DIR

// Ref Voltage Pin
#define EN_VREF_FET BIT1
#define PORT_EN_VREF_FET_OUT P2OUT
#define PORT_EN_VREF_FET_DIR P2DIR

unsigned int CheckVBoost(void);
unsigned int CheckVRef(int VBoostMax);
unsigned int CheckVBattery(int VBoostMax);

#endif
