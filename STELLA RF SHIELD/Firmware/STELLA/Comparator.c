#include <msp430g2553.h>
#include "main.h"
#include "Comparator.h"
#include "ADC.h"
#include "DigiPot.h"

const unsigned int VMargin = 900; //  mV
const unsigned int VShortBoundary = 200; // mV
volatile unsigned int cnt = 0;
volatile unsigned int i_left;
volatile int VRef_test;

//variables for timer
volatile unsigned int count = 0;
volatile unsigned int count1 = 0;
volatile unsigned int timer_tar_count = 0;
volatile unsigned int loop_count;
volatile unsigned int func_iden = 0;
unsigned int interrupt_variable = 0;
unsigned int vref_testing = 0;
unsigned int dbs_array[30] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//functions for timer and DBS freq
void setupTimer_freq() {
    TA1CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TA1CCR0= 65535;
    TA1CTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
}

void stopTimer_freq() {
    TA1CTL = MC_0 + TACLR;
}


unsigned int dbs_freq(Stimulation_Channel channel, uint16_t Freq_compare){
    P1IE |= BIT0;                // listen to an interrupt for P1.0
    count = 0;
    count1 = 0;
    loop_count = 1;
    timer_tar_count = 0;
    stopTimer_freq(); //stop the timer if it is running for RTC
    int ii= 0; //used for delay loop

    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT |=  _SHDN_OP1; // enable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT |=  _SHDN_OP2; // enable OP right side

    PORT_EN_VREF_FET_OUT |= EN_VREF_FET; // Turn VREF_MOSFET on

    while(!count) {
        if(loop_count == 1){
            Set_DIGIPOT(VREF, 10);
            loop_count++;
        }
        for(ii = 0;ii<Freq_compare;ii++){
            __delay_cycles(32);
            if(count == 1 || ii >= (Freq_compare-4)){
                break;
            }
        }
    }
    while(!count1){
        if(timer_tar_count > 60000){
            break;
        }

    }

    P1IE &= ~BIT0;   // stop listening to interrupt 1.0
    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET; // Turn VREF_MOSFET off

    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT &= ~_SHDN_OP1; // disable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT &= ~_SHDN_OP2; // disable OP right side

    return timer_tar_count;
}
// function for pulse width calculation
unsigned int dbs_pw(Stimulation_Channel channel, uint16_t Freq_compare){
    P1IE |= BIT0;                // listen to an interrupt for P1.0
    count = 0;
    count1 = 0;
    loop_count = 1;
    stopTimer_freq(); //stop the timer if it is running for RTC
    int ii= 0; //used for delay loop

    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT |=  _SHDN_OP1; // enable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT |=  _SHDN_OP2; // enable OP right side

    PORT_EN_VREF_FET_OUT |= EN_VREF_FET; // Turn VREF_MOSFET on

    while(!count) {
        if(loop_count == 1){
            Set_DIGIPOT(VREF, (VRef_test+1)); //Set_DIGIPOT(VREF, (VRef_test+9));
            loop_count++;
        }
        for(ii = 0;ii<Freq_compare;ii++){
            __delay_cycles(32);
            if(count == 1 || ii >= (Freq_compare-1)){
                break;
            }
        }
    }
    Set_DIGIPOT(VREF, 5);
    __delay_cycles(1000); // waiting for the signal within one period  (0.8ms)
    while(!count1){
        if(timer_tar_count > 60000){
            break;
        }

    }

    P1IE &= ~BIT0;   // stop listening to interrupt 1.0
    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET; // Turn VREF_MOSFET off

    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT &= ~_SHDN_OP1; // disable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT &= ~_SHDN_OP2; // disable OP right side

    return timer_tar_count;
}

void _compare_helper(Stimulation_Channel channel, uint16_t Freq_compare){
    //unsigned int interrupt_variable = 0;
    func_iden = 1;
    interrupt_variable = 0; // for initializing
    int ii;
    vref_testing = VRef_test;
    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT |=  _SHDN_OP1; // enable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT |=  _SHDN_OP2; // enable OP right side
    PORT_EN_VREF_FET_OUT |= EN_VREF_FET; // Turn VREF_MOSFET on
    Set_DIGIPOT(VREF, vref_testing);
    P1IE |= BIT0;                // listen to an interrupt for P1.0
    while(1){
        vref_testing++;
        interrupt_variable = 0;
        Set_DIGIPOT(VREF, vref_testing);
        for(ii = 0;ii<Freq_compare;ii++){
            __delay_cycles(32);
        }
        if(interrupt_variable ==0){
            break;
        }
    }
    P1IE &= ~BIT0;   // stop listening to interrupt 1.0
    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET; // Turn VREF_MOSFET off
    VRef_test = vref_testing-1;

    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT &= ~_SHDN_OP1; // disable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT &= ~_SHDN_OP2; // disable OP right side
}

void sample_dbs(Stimulation_Channel channel, unsigned int Wiper_new, uint16_t Freq_compare){

    count = 0;
    count1 = 0;
    loop_count = 1;
    unsigned int index = 0;
    volatile int vwipernew = Wiper_new-1;
    int ii= 0; //used for delay loop
    int reference_break = 0;
    stopTimer_freq();
    P1IE |= BIT0;                // listen to an interrupt for P1.0

    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT |=  _SHDN_OP1; // enable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT |=  _SHDN_OP2; // enable OP right side

    PORT_EN_VREF_FET_OUT |= EN_VREF_FET; // Turn VREF_MOSFET on


    for(vwipernew= Wiper_new; vwipernew > 12; vwipernew = vwipernew-10){
        count = 0; //just for testing
        count1 = 0; //just for testing
        loop_count = 1; //just for testing

        while(!count){
            if(loop_count == 1){
                Set_DIGIPOT(VREF, vwipernew); //Set_DIGIPOT(VREF, (VRef_test+9));
                loop_count++;
            }
            for(ii = 0;ii<Freq_compare;ii++){
                __delay_cycles(32);
                if(count == 1 || ii >= (Freq_compare-1)){
                    if(ii >= (Freq_compare-1)){
                        reference_break = 1;
                    }
                    break;

                }
            }
        }
        Set_DIGIPOT(VREF, (vwipernew-10));
        __delay_cycles(1000); // waiting for spi communication
        while(!count1){
            if(timer_tar_count > 60000 || reference_break == 1 || count1 == 1){
                reference_break = 0;
                break;
            }
        }
        dbs_array[index]= timer_tar_count;
        index++;
        loop_count = 1;
    }

    P1IE &= ~BIT0;   // stop listening to interrupt 1.0
    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET; // Turn VREF_MOSFET off

    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT &= ~_SHDN_OP1; // disable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT &= ~_SHDN_OP2; // disable OP right side

}

int _compare(Stimulation_Channel channel, int *VBoostArray, uint16_t Freq_compare, uint8_t pack_rec) {

    unsigned int VRefWiper = 255;
    unsigned int VRef = 0;
    unsigned int VMinDrive = 0;
    int i = 0;
    int ii= 0;

    //variable for checking which function is calling interrupt
    func_iden = 1;

    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT |=  _SHDN_OP1; // enable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT |=  _SHDN_OP2; // enable OP right side

    PORT_EN_VREF_FET_OUT |= EN_VREF_FET; // Turn VREF_MOSFET on

    Set_DIGIPOT(VREF, VRefWiper);

    P1IE |= BIT0;                // listen to an interrupt for P1.0
    cnt = 0;

    while(!cnt) {
              Set_DIGIPOT(VREF, --VRefWiper);
              if(VRefWiper == 0) break;
              for(ii = 0;ii<Freq_compare;ii++){
                  __delay_cycles(32);
              }
          }

    P1IE &= ~BIT0;   // stop listening to interrupt 1.0 (Removed on 23.02.21 for testing vwiper increment)
    VRef = CheckVRef(VBoostArray[15]); // voltage at the very end of the stimulation pulse (=maximum)
    VRef_test = VRefWiper;
    //P1IE &= ~BIT0;   // stop listening to interrupt 1.0 (added on 23.02.21 for testing vwiper increment)
    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET; // Turn VREF_MOSFET off

    if(channel == LATERAL_RIGHT)   PORT_SHDN_OP1_OUT &= ~_SHDN_OP1; // disable OP left side
    else if(channel == LATERAL_LEFT)   PORT_SHDN_OP2_OUT &= ~_SHDN_OP2; // disable OP right side

    _compare_helper(channel, Freq_compare); //(added on 24.02.21 for testing vwiper increment)
    if(pack_rec == 1 && VRef_test > 20 && VRef_test < 180){
        pack_rec = 0;
        //sample_dbs(channel, VRefWiper, Freq_compare);
        pack_rec = 0;
    }

    VMinDrive = VRef + VMargin;
    __delay_cycles(100);

    if(VRef < VShortBoundary)  return -1;

    while(VMinDrive > VBoostArray[i] && i < 16) i++;

    return i;
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {

    if(func_iden == 1){
        cnt++;
        interrupt_variable++;
    }
    //for timer and frequency

    if(loop_count == 2){
        count++;
        setupTimer_freq();
        loop_count = 0;

    }
    else if(loop_count == 0){
        count1++;
        timer_tar_count = TA1R;
        stopTimer_freq();
        loop_count = 1; // for dbs sampling testing, remove if problem

    }

    P1IFG &= ~BIT0;
}
