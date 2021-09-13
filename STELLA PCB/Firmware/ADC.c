#include <msp430g2553.h>
#include "main.h"
#include "ADC.h"
#include "DigiPot.h"


const float vboost_weight = 4.88; // 1000 * 2 * 2.5 / 1024 = 4.88

void _helper() {

    __delay_cycles(100);

    ADC10CTL0 |= (REFON + ADC10ON + ADC10IE); // Turn everything on
    __delay_cycles(35); // REF needs ~30 us to settle, so give it 35
    ADC10CTL0 |= (ENC + ADC10SC); // start conversion and result in the ISR when data ready

    while(ADC10CTL1 & BUSY);

//    return ADC10MEM;

//    ADC10CTL0 &= ~ENC;      //disable ADC
//    ADC10CTL0 &= ~(REFON + ADC10ON + ADC10IE); // and then shutdown completely

}

unsigned int _averageADC() {

    unsigned int i;
    unsigned int sum = 0;
    const unsigned int n = 32; // power of 2

    for(i=0; i < n; i++) {

        _helper();
        sum += ADC10MEM;
    }

   return (sum >> 5);

}

unsigned int CheckVBoost() {

    PORT_EN_VBOOST_FET_OUT |= EN_VBOOST_FET;

    __delay_cycles(100);

    ADC10CTL0 = REF2_5V +  ADC10SHT_3 + SREF_1  ;// Use Internal Vref+ as positive rail and Vss as negative; Use 2.5 V Vref, Sample and hold for 64 Clock cycles, Set low speed sampling to minimize reference buffer power
    ADC10AE0 |= BIT4; // Enable A4 input (P1.4)
    ADC10CTL1 = INCH_4 + ADC10DIV_3 ; // Channel4, use the master clock for the ADC

    _helper();

    PORT_EN_VBOOST_FET_OUT &= ~EN_VBOOST_FET;

    return vboost_weight * ADC10MEM; // voltage divider (1:2, 10kOhm & 10kOhm), REF2_5V, (2.0 * (2.5 / 1024.0 * ADC10MEM)) * 1000.0 = 4.88 * ADC10MEM
}

unsigned int CheckVRef(int VBoostMax)    {


    PORT_EN_VREF_FET_OUT |= EN_VREF_FET;

    __delay_cycles(100);

    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10SR; // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, Set low speed sampling to minimize reference buffer power
    ADC10AE0 |= BIT1; // Enable A1 input (P1.1)
    ADC10CTL1 = INCH_1 + ADC10DIV_3; // Channel1, use the master clock for the ADC

    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET;

    return VBoostMax / 1024.0 * _averageADC();
}

unsigned int CheckVBattery(int VBoostMax)    {

   ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10SR; // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, Set low speed sampling to minimize reference buffer power, ADC on, ADC interrupt enable
   ADC10AE0 |= BIT3; // Enable A3 input (P1.3)
   ADC10CTL1 = INCH_3 + ADC10DIV_3; // Channel3, use the master clock for the ADC

   return VBoostMax / 1024.0 * _averageADC();

}


#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {

    ADC10CTL0 &= ~ENC;      //disable ADC
    ADC10CTL0 &= ~(REFON + ADC10ON + ADC10IE); // and then shutdown completely

}
