#include <msp430g2553.h>
#include "main.h"
#include "Comparator.h"
#include "ADC.h"
#include "DigiPot.h"

const unsigned int VMargin = 900; //  mV
const unsigned int VShortBoundary = 200; // mV
volatile unsigned int cnt = 0;
volatile int i_left; // keep the value of i_left for evaluating the proper boost voltage in bilateral stimulation


int _compare(Stimulation_Channel channel, int *VBoostArray) {

    unsigned int VRefWiper = 255;
    unsigned int VRef = 0;
    unsigned int VMinDrive = 0;
    int i = 0;

    if(channel == CHAN_RIGHT)   PORT_SHDN_OP1_OUT |=  _SHDN_OP1; // enable OP right side
    else if(channel == CHAN_LEFT)   PORT_SHDN_OP2_OUT |=  _SHDN_OP2; // enable OP left side

    PORT_EN_VREF_FET_OUT |= EN_VREF_FET; // Turn VREF_MOSFET on

    Set_DIGIPOT(VREF, VRefWiper);

    P1IE |= BIT0;                // listen to an interrupt for P1.0
    cnt = 0;

    while(!cnt) { // TODO: Polling procedure instead of interrupt because of signal distortion
              Set_DIGIPOT(VREF, --VRefWiper);
              if(VRefWiper == 0) break;
              __delay_cycles(8000); // waiting for the signal within one period  (8 ms corresponds to 130 Hz)
          }


    P1IE &= ~BIT0;   // stop listening to P1.0

    VRef = CheckVRef(VBoostArray[15]); // voltage at the very end of the stimulation pulse (= maximum of stimulation voltage pulse)
    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET; // Turn VREF_MOSFET off

    if(channel == CHAN_RIGHT)   PORT_SHDN_OP1_OUT &= ~_SHDN_OP1; // disable OP right side
    else if(channel == CHAN_LEFT)   PORT_SHDN_OP2_OUT &= ~_SHDN_OP2; // disable OP left side

    VMinDrive = VRef + VMargin;

    if(VRef < VShortBoundary)  return -1;

    while(VMinDrive > VBoostArray[i] && i < 16) i++;

    return i;
}

Impedance_Status ImpedanceCheck(int *VBoostArray, Stimulation_Channel channel, Stimulation_Type type) {


    int i;
    int i_right;


    if(type == UNILATERAL) {

        i = _compare(channel, VBoostArray);

        if (i >= 0 && i < 16) {
            Set_DIGIPOT(VBOOSTER, i * 16);
            return IMPEDANCE_NORMAL;
                }
        else if (i == -1)  {
             Set_DIGIPOT(VBOOSTER, 0);
            return IMPEDANCE_LOW;
                }
        else if (i == 16)  {
            Set_DIGIPOT(VBOOSTER, 240);
            return IMPEDANCE_HIGH;
                }
    }


    else if(type == BILATERAL) {

        if(channel == CHAN_LEFT)  {

            i_left = _compare(channel, VBoostArray);

            if (i_left >= 0 && i_left < 16) return IMPEDANCE_NORMAL;
            else if (i_left == -1) return IMPEDANCE_LOW;
            else if (i_left == 16) return IMPEDANCE_HIGH;
        }

        if(channel == CHAN_RIGHT)  {

            i_right = _compare(channel, VBoostArray);

            if(i_left == 16 || i_right == 16)       Set_DIGIPOT(VBOOSTER,240);
            else if(i_left == -1  && i_right == -1) Set_DIGIPOT(VBOOSTER,0);
            else if(i_left < 16  && i_right < 16) {
                if(i_left > i_right)  Set_DIGIPOT(VBOOSTER, 16 * i_left);
                else Set_DIGIPOT(VBOOSTER, 16 * i_right);
        }

            if (i_right >= 0 && i_right < 16) return IMPEDANCE_NORMAL;
            else if (i_right == -1) return IMPEDANCE_LOW;
            else if (i_right == 16) return IMPEDANCE_HIGH;

        }

    }

}



#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {

    cnt++;
    P1IFG &= ~BIT0;

}
