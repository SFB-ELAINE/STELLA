#include <msp430g2553.h>
#include "main.h"


#define ON_TIME 30000
#define OFF_TIME1 600000
#define OFF_TIME2 370000
#define OFF_TIME3 70000
#define OFF_TIME4 180000

#define ON_TIME1 250
#define OFF_TIME5  1000



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

void LED_FLASH4(int i) {

    while(i > 0 ) {
    P3OUT |= BIT4;
    __delay_cycles(ON_TIME);
    P3OUT &= ~BIT4;
    __delay_cycles(OFF_TIME4);
    i--; }

}

void LED_FLASH5(int i) {

    while(i > 0 ) {
    P3OUT |= BIT4;
    __delay_cycles(ON_TIME1);
    P3OUT &= ~BIT4;
    __delay_cycles(OFF_TIME5);
    i--; }

}


void ImpedanceBlinkPattern(Impedance_Status impedanceStatus, Stimulation_Channel channel) {


    if(impedanceStatus == IMPEDANCE_NORMAL) {
        // blink two times
        if(channel == CHAN_LEFT) LED_FLASH1(2);
        if(channel == CHAN_RIGHT) LED_FLASH2(2);
    }

    if(impedanceStatus == IMPEDANCE_LOW) {

        // blink three times
        if(channel == CHAN_LEFT) LED_FLASH1(3);
        if(channel == CHAN_RIGHT) LED_FLASH2(3);
    }

    if(impedanceStatus == IMPEDANCE_HIGH) {

        // blink four times
        if(channel == CHAN_LEFT) LED_FLASH1(4);
        if(channel == CHAN_RIGHT) LED_FLASH2(4);
    }
}



void LowBatteryPattern() {

    // blink five times and wait 1000 ms
     __delay_cycles(1000000);
//    LED_FLASH4(5);
//    __delay_cycles(1000000);
    LED_FLASH4(2);
    __delay_cycles(290000);
    LED_FLASH4(2);
    __delay_cycles(290000);
    LED_FLASH4(2);
    __delay_cycles(1000000);



}

void StimulationOffPattern() {

    // blink ten times very shortly
    LED_FLASH3(10);
}

void ControlLight() {


    LED_FLASH5(2400);


}

//void ControlLight() {
//    // LED lights for 3 seconds
//    P3OUT |= BIT4;
//    __delay_cycles(3000000);
//    P3OUT &= ~BIT4;
//
//
//}


