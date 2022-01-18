#include <msp430g2553.h>
#include <stdint.h>
#include "main.h"
#include "ADC.h"

float vboost_weight = 4.88; // 1000 * 2 * 2.5 / 1024 = 4.88

uint16_t VBo,VBat;

void _helper() {

    __delay_cycles(100);

    ADC10CTL0 |= (REFON + ADC10ON + ADC10IE); // Turn everything on
    __delay_cycles(35); // REF needs ~30 us to settle, so give it 35
    ADC10CTL0 |= (ENC + ADC10SC); // start conversion and result in the ISR when data ready

    __bis_SR_register(LPM0_bits + GIE);

}


unsigned int CheckVBoost() {

    PORT_EN_VBOOST_FET_OUT |= EN_VBOOST_FET;

    __delay_cycles(100);

    ADC10CTL0 = REF2_5V +  ADC10SHT_3 + SREF_1  ;// Use Internal Vref+ as positive rail and Vss as negative; Use 2.5 V Vref, Sample and hold for 64 Clock cycles, Set low speed sampling to minimize reference buffer power
    ADC10AE0 |= BIT4; // Enable A4 input (P1.4)
    ADC10CTL1 = INCH_4 + ADC10DIV_3 ; // Channel4, use the master clock for the ADC

    _helper();

    PORT_EN_VBOOST_FET_OUT &= ~EN_VBOOST_FET;

    return  vboost_weight*ADC10MEM; // voltage divider (1:2, 10kOhm & 10kOhm), REF2_5V, (2.0 * (2.5 / 1024.0 * ADC10MEM)) * 1000.0 = 4.88 * ADC10MEM
}

uint16_t CheckVBoostNew() {

    PORT_EN_VBOOST_FET_OUT |= EN_VBOOST_FET;

    __delay_cycles(100);

    ADC10CTL0 = REF2_5V +  ADC10SHT_3 + SREF_1  ;// Use Internal Vref+ as positive rail and Vss as negative; Use 2.5 V Vref, Sample and hold for 64 Clock cycles, Set low speed sampling to minimize reference buffer power
    ADC10AE0 |= BIT4; // Enable A4 input (P1.4)
    ADC10CTL1 = INCH_4 + ADC10DIV_3 ; // Channel4, use the master clock for the ADC

    _helper();

    PORT_EN_VBOOST_FET_OUT &= ~EN_VBOOST_FET;

    return(ADC10MEM); // voltage divider (1:2, 10kOhm & 10kOhm), REF2_5V, (2.0 * (2.5 / 1024.0 * ADC10MEM)) * 1000.0 = 4.88 * ADC10MEM
}

unsigned int CheckVRef(int VBoostMax)    {


    //unsigned int VBoost_value = CheckVBoost();

    PORT_EN_VREF_FET_OUT |= EN_VREF_FET;

    __delay_cycles(100);

    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10SR; // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, Set low speed sampling to minimize reference buffer power
    ADC10AE0 |= BIT1; // Enable A1 input (P1.1)
    ADC10CTL1 = INCH_1 + ADC10DIV_3; // Channel1, use the master clock for the ADC

    _helper();

    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET;

    return VBoostMax / 1024.0 * ADC10MEM;
}

//unsigned int CheckVBattery()    {
uint16_t CheckVBattery()    {


   //unsigned int VBoost_value =  CheckVBoost();


   ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10SR; // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, Set low speed sampling to minimize reference buffer power, ADC on, ADC interrupt enable
   ADC10AE0 |= BIT3; // Enable A3 input (P1.3)
   ADC10CTL1 = INCH_3 + ADC10DIV_3; // Channel3, use the master clock for the ADC

   _helper();

   //return VBoost_value / 1024.0 * ADC10MEM;
   return ADC10MEM;
}


uint16_t temp_read() {
   ADC10CTL0 = ADC10SHT_3 + SREF_1  ;
   ADC10CTL1=INCH_10+ ADC10DIV_3;
   _helper();

   return(ADC10MEM);
}


#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {

    ADC10CTL0 &= ~ENC;      //disable ADC
    ADC10CTL0 &= ~(REFON + ADC10ON + ADC10IE); // and then shutdown completely

    __bic_SR_register_on_exit(LPM0_bits);
}
