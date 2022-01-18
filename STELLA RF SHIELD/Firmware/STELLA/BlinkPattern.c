#include <msp430g2553.h>
#include "main.h"


#define ON_TIME 30000
#define OFF_TIME1 600000
#define OFF_TIME2 370000
#define OFF_TIME3 140000


void LED_FLASH1(int i) {

    // left channel (blinks slower than right channel)

    while(i > 0 ) {
    P3OUT |= BIT4;
    __delay_cycles(ON_TIME);
    P3OUT &= ~BIT4;
    __delay_cycles(OFF_TIME1);
    i--; }

}

void LED_FLASH2(int i) {

    // right channel (blinks faster than left channel)

    while(i > 0 ) {
    P3OUT |= BIT4;
    __delay_cycles(ON_TIME);
    P3OUT &= ~BIT4;
    __delay_cycles(OFF_TIME2);
    i--; }

}

void LED_FLASH3(int i) {

    while(i > 0 ) {
    P3OUT |= BIT4;
    __delay_cycles(ON_TIME);
    P3OUT &= ~BIT4;
    __delay_cycles(OFF_TIME3);
    i--; }

}


void ImpedanceBlinkPattern(Impedance_Status impedanceStatus, Stimulation_Channel channel) {


    if(impedanceStatus == IMPEDANCE_NORMAL) {
        // blink two times
        if(channel == LATERAL_LEFT) LED_FLASH1(2);
        if(channel == LATERAL_RIGHT) LED_FLASH2(2);
    }

    if(impedanceStatus == IMPEDANCE_LOW) {

        // blink three times
        if(channel == LATERAL_LEFT) LED_FLASH1(3);
        if(channel == LATERAL_RIGHT) LED_FLASH2(3);
    }

    if(impedanceStatus == IMPEDANCE_HIGH) {

        // blink four times
        if(channel == LATERAL_LEFT) LED_FLASH1(4);
        if(channel == LATERAL_RIGHT) LED_FLASH2(4);
    }
}




void LowBatteryPattern() {
    // S.O.S

    P3OUT |= BIT4;
    __delay_cycles(30000);
    P3OUT &= ~BIT4;
    __delay_cycles(300000);
    P3OUT |= BIT4;
    __delay_cycles(30000);
    P3OUT &= ~BIT4;
    __delay_cycles(300000);
    P3OUT |= BIT4;
    __delay_cycles(30000);
    P3OUT &= ~BIT4;
    __delay_cycles(500000);

    P3OUT |= BIT4;
    __delay_cycles(30000);
    P3OUT &= ~BIT4;
    __delay_cycles(500000);
    P3OUT |= BIT4;
    __delay_cycles(30000);
    P3OUT &= ~BIT4;
    __delay_cycles(500000);
    P3OUT |= BIT4;
    __delay_cycles(30000);
    P3OUT &= ~BIT4;
    __delay_cycles(500000);

    P3OUT |= BIT4;
    __delay_cycles(30000);
    P3OUT &= ~BIT4;
    __delay_cycles(300000);
    P3OUT |= BIT4;
    __delay_cycles(30000);
    P3OUT &= ~BIT4;
    __delay_cycles(300000);
    P3OUT |= BIT4;
    __delay_cycles(30000);
    P3OUT &= ~BIT4;
    __delay_cycles(500000);




}

void StimulationOffPattern() {

    // blink ten times very shortly
    LED_FLASH3(2); //LED_FLASH3(10);
}

void ContinuousTxnPattern() {
    // blink five times very shortly
    LED_FLASH3(4);
}

void StartErrorRecognitionPattern() {
    // LED lights for 0.5 seconds
    P3OUT |= BIT4;
    __delay_cycles(400000);
    P3OUT &= ~BIT4;
    __delay_cycles(1000000);
    /*P3OUT |= BIT4;
    __delay_cycles(40000);
    P3OUT &= ~BIT4;
    __delay_cycles(1000000);*/


}

void PacketNotReceive() {
    // LED lights for 1 seconds 3 times
/*    P3OUT |= BIT4;
    __delay_cycles(50000);
    P3OUT &= ~BIT4;
    __delay_cycles(1000000);
    P3OUT |= BIT4;
    __delay_cycles(50000);
    P3OUT &= ~BIT4;
    __delay_cycles(1000000);
    P3OUT |= BIT4;
    __delay_cycles(50000);
    P3OUT &= ~BIT4;
    __delay_cycles(1000000); */

    // blink 6 times totally
    __delay_cycles(500000);
    LED_FLASH3(2);
    __delay_cycles(500000);
    LED_FLASH3(2);
    __delay_cycles(500000);
    LED_FLASH3(2);
    __delay_cycles(80000);

}


