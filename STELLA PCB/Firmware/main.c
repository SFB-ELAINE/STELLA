/*
 * P1.0 : COMP1 SIGNAL LEFT && COMP2 SIGNAL RIGHT gated by SHDN-Pins
 * P1.1 : VREF ADC
 * P1.2 : PWM
 * P1.3 : VBattery ADC
 * P1.4 : VDD ADC
 * P1.5 : UCB0CLK
 * P1.6 : UCB0SOMI
 * P1.7 : UCB0SIMO
 *
 * P2.0 : EN_BOOSTER
 * P2.1 : EN_VREF_FET
 * P2.2 : EN_VBOOST_FET
 * P2.3 : ---
 * P2.4 : HALL OUT
 * P2.5 : GDO0
 * P2.6 : XIN
 * P2.7 : XOUT
 *
 * P3.0 : _CS_AD5144 (CounterPulse Attenuation Left && Right, Boost Voltage, Reference Voltage)
 * P3.1 : _CS_AD5142 (Current Adjustment)
 * P3.2 : ---
 * P3.3 : _CS_CC2500
 * P3.4 : LED
 * P3.5 : GDO2
 * P3.6 : _SHDN_OPV_P1.0
 * P3.7 : _SHDN_OPV_P1.1
 *
 * */

#include <msp430g2553.h>
#include "main.h"
#include "SPI.h"
#include "DigiPot.h"
#include "Booster.h"
#include "ADC.h"
#include "Comparator.h"
#include "BlinkPattern.h"


volatile Stimulation_Status stimulationStatus = STIMULATION_OFF;
volatile int stimulation_cnt = 0;

volatile Battery_Status batteryLevel = BATTERY_LEVEL_NORMAL;

volatile Impedance_Status impedanceLeftStatus;
volatile Impedance_Status impedanceRightStatus;

volatile Stimulation_Type type = BILATERAL; // STIM_OUT2 = CHAN_LEFT, STIM_OUT1 = CHAN_RIGHT
volatile Stimulation_Channel channel = CHAN_LEFT; // only for UNILATERAL
volatile Timer_Resolution timer_resolution = RESOLUTION_LOW;

volatile BIST_Status bist_status = BIST_ON; // Built-In Self-Test

unsigned int VBoostMax;
unsigned int VBoostMin;

unsigned int low_battery_cnt;
const unsigned int battery_check_cnt = 10;


void EnableStimulation() {

    stimulationStatus = STIMULATION_ON;

    if(timer_resolution == RESOLUTION_LOW) {
    P1OUT &= ~BIT2;              // Enable the current source
    P1SEL |= BIT2;
    }

    else if(timer_resolution == RESOLUTION_HIGH) TA1CTL = TASSEL_1 + MC_1;                    // Enable Timer1

    stimulation_cnt = 0;

}

void DisableStimulation() {

    stimulationStatus = STIMULATION_OFF;

    if(timer_resolution == RESOLUTION_LOW) P1SEL &= ~BIT2;

    else if(timer_resolution == RESOLUTION_HIGH) { // disable timers

    TA0CTL &= ~(MC_1);
    TA1CTL &= ~(MC_1);

    }

    P1OUT |= BIT2;              // Disable the current source
    StimulationOffPattern();
    stimulation_cnt = 0;
    Set_DIGIPOT(VBOOSTER, 0);
    _BIS_SR(LPM4_bits + GIE);
}


void PWM(int freq, int pulse_width) {


    P2SEL |= (BIT6 | BIT7); // Set P2.6 and P2.6 SEL for XIN, XOUT


    if (timer_resolution == RESOLUTION_LOW) {

        P1OUT &= ~BIT2;              // Enable the current source
        P1SEL |= BIT2;

        TA0CCTL1 = OUTMOD_7; // CCR1 toggle/set

        TA0CCR0 = FREQ_32kHz_IMPLANT_TA0CCR0;
        TA0CCR1 = FREQ_32kHz_IMPLANT_TA0CCR1;
        TA0CTL = TASSEL_1 + MC_1;         // ACLK, upmode, 32 kHz}
    }

    else if (timer_resolution == RESOLUTION_HIGH) {


        TA1CCTL0 = CCIE;                             // CCR0 interrupt enabled
        TA1CCR0 = 251 ;                             // set ControlRegister0 to the desired PWM frequency
//        TA1CTL = TASSEL_1 + MC_1;                    // ACLK, upmode, 32 kHz

        TA0CCTL0 = CCIE;  // T0 responsible for pulse width
        TA0CCR0 = 60;

        __enable_interrupt();

    }
}

void HallSensorSetup() {

    // Set-up the HALL Sensor
    P2REN |= BIT4;               // Enable internal pull-up/down resistors
    P2OUT |= BIT4;              // Select pull-up mode for P2.4
    P2IES |= BIT4;              // P2.4 Hi/Lo edge
    P2IE  |= BIT4;
    P2IFG &= ~BIT4;              // P2.4 IFG cleared

}

void ComparatorSetup()  {

    P1REN |= BIT0;               // Enable internal pull-up/down resistors
    P1OUT &= ~BIT0;              // Select pull-down mode for P1.0
    P1IE &= ~BIT0;                // don't listen to an interrupt for P1.0
    P1IES &= ~BIT0;              // 1.0 Lo/Hi edge
    P1IFG &= ~BIT0;              // P1.0 IFG cleared

}

void EnableHallSensor() {

    P2OUT |= BIT4;  // Select pull-up mode for P2.4

}

void DisableHallSensor() {

    P2OUT &= ~BIT4;  // Select pull-down mode for P2.4

}

void LEDSetup() {

    P3DIR |= BIT4;              // Status LED P3.4
}


int main(void)  {

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    BCSCTL1 = CALBC1_1MHZ;      /* Set DCO to 1MHz */
    DCOCTL = CALDCO_1MHZ;

    BCSCTL3 |= XCAP_1;              //5pF cap- setting for 32768Hz crystal


    P1OUT &= 0x00;              // Shut down everything
    P1DIR &= 0x00;
    P2OUT &= 0x00;
    P2DIR &= 0x00;
    P3OUT &= 0x00;
    P3DIR &= 0x00;

    P1SEL &= 0x00;              // select P1 for I/O
    P2SEL &= 0x00;              // select P2 for I/O
    P3SEL &= 0x00;              // select P3 for I/O

    P1OUT |= BIT2;              // Disable the current source
    P1DIR |= BIT2;

    PORT_EN_VBOOST_FET_DIR |= EN_VBOOST_FET; // MOSFET for enable measuring the supply voltage (boost voltage)
    PORT_EN_VBOOST_FET_OUT &= ~EN_VBOOST_FET; // disable MOSFET for enable measuring the supply voltage (boost voltage)

    PORT_EN_VREF_FET_DIR |= EN_VREF_FET; // MOSFET for enable measuring the reference voltage adjusted by the AD5144 (100kOhm)
    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET; // disable MOSFET for enable measuring the reference voltage adjusted by the AD5144 (100kOhm)

    PORT_SHDN_OP1_DIR |= _SHDN_OP1;
    PORT_SHDN_OP2_DIR |= _SHDN_OP2;
    PORT_SHDN_OP1_OUT &= ~_SHDN_OP1; // disable OP left side
    PORT_SHDN_OP2_OUT &= ~_SHDN_OP2; // disable OP right side

    HallSensorSetup();
    LEDSetup();
    ComparatorSetup();
    SPISetup();

    int VBoostArray[16];
    unsigned int VBoostCnt;
    int VBoostStep = 0;

   for(VBoostCnt = 0; VBoostCnt < 16; VBoostCnt++) {

       Set_DIGIPOT(VBOOSTER, VBoostStep);
       __delay_cycles(300000);
       VBoostArray[VBoostCnt] = CheckVBoost();
       VBoostStep += 16;

    }

   // Booster Voltages:
   //  VBOOSTER = 0 --> 3.07 V
   //  VBOOSTER == 16 --> 3.07 V
   // VBOOSTER == 32 --> 3.07 V
   // VBOOSTER == 48 --> 3.07 V
   // VBOOSTER = 64 --> 3.17 V
   // VBOOSTER = 80 --> 3.17 V
   // VBOOSTER = 96 --> 3.17 V
   // VBOOSTER = 112 --> 3.27 V
   // VBOOSTER = 128 --> 3.27 V
   // VBOOSTER = 144 --> 3.27 V
   // VBOOSTER = 160 --> 3.38 V
   // VBOOSTER = 176 --> 3.38 V
   // VBOOSTER = 192 --> 3.48 V
   // VBOOSTER = 208 --> 3.48 V
   // VBOOSTER = 224 --> 3.58 V
   // VBOOSTER = 240 --> 3.68 V


    VBoostMax = VBoostArray[15];
    VBoostMin = VBoostArray[0];

    Set_DIGIPOT(VBOOSTER,0); // VBoostArray[15]
    __delay_cycles(300000);


    Set_DIGIPOT(COUNTERPULSE_ATTENUATION_LEFT, 0); // charge balancing resistance
    Set_DIGIPOT(COUNTERPULSE_ATTENUATION_RIGHT, 0); // charge balancing resistance
    Set_DIGIPOT(CURRENT_LEFT, 111); //  @3V: 0 == 44 uA, 111 == 50uA, 228 == 100 uA, 245 == 200 uA, 254 = 1.16 mA, 255 = 3.7 mA
    Set_DIGIPOT(CURRENT_RIGHT, 111); // @3V: 0 == 44 uA, 111 == 50uA, 228 == 100 uA, 245 == 200 uA, 254 = 1.16 mA, 255 = 3.6 mA
    Set_DIGIPOT(VREF, 0);


    EnableHallSensor();
    __bis_SR_register(LPM4_bits + GIE); // Stimulation off
    DisableHallSensor();

    PWM(IMPLANT_FREQUENCY, IMPLANT_PULSE_WIDTH);
    EnableStimulation();

    while(1) {

        //   Signal the result of the built-in self-test (BIST)


        // For BIST, set the boost voltage to a defined voltage level (VBoostMax)
        Set_DIGIPOT(VBOOSTER,240); // VBoostArray[15]
        __delay_cycles(300000);

        // Check Battery Voltage
        for(low_battery_cnt = 1; low_battery_cnt <= battery_check_cnt; low_battery_cnt++) { // Check if battery voltage falls below the threshold several times

           if(CheckVBattery(VBoostMax) < BATTERY_VOLTAGE_THRESHOLD) {

               if (low_battery_cnt == battery_check_cnt) {

                  batteryLevel = BATTERY_LEVEL_LOW;
                  LowBatteryPattern();
               }

           }

           else {

               batteryLevel = BATTERY_LEVEL_NORMAL;
               break;
           }

        }

           if(type == BILATERAL) {

               impedanceLeftStatus = ImpedanceCheck(VBoostArray, CHAN_LEFT, BILATERAL);
               impedanceRightStatus = ImpedanceCheck(VBoostArray, CHAN_RIGHT, BILATERAL);

               ImpedanceBlinkPattern(impedanceLeftStatus, CHAN_LEFT);
               __delay_cycles(1500000);
               ImpedanceBlinkPattern(impedanceRightStatus, CHAN_RIGHT);
           }

           else if(type == UNILATERAL && channel == CHAN_LEFT) {

               impedanceLeftStatus = ImpedanceCheck(VBoostArray, CHAN_LEFT, UNILATERAL);
               ImpedanceBlinkPattern(impedanceLeftStatus, CHAN_LEFT);
           }
           else if(type == UNILATERAL && channel == CHAN_RIGHT) {

               impedanceRightStatus = ImpedanceCheck(VBoostArray, CHAN_RIGHT, UNILATERAL);
               ImpedanceBlinkPattern(impedanceRightStatus, CHAN_RIGHT);
           }

           EnableHallSensor();
           bist_status = BIST_OFF;  //end of BIST
           stimulation_cnt = 0;

           // Stay in Fusion-Mode of LPM0/LPM3 until hall sensor is triggered
           if(timer_resolution == RESOLUTION_HIGH) while(bist_status == BIST_OFF);

           // Stay in LPM3 until hall sensor is triggered
           else if(timer_resolution == RESOLUTION_LOW)  _BIS_SR(LPM3_bits + GIE);

           //if Hall Sensor is triggered wait 2 seconds before LED starts to light up for 3 seconds
           DisableHallSensor();
           __delay_cycles(2000000);
           EnableHallSensor();
           ControlLight();

           // if hall sensor is triggered twice within the ControlLight(), turn stimulation off and go to LPM4
           if(stimulation_cnt > 1)  {

               DisableStimulation(); // stay in LPM4 until hall sensor is triggered
               EnableStimulation();  // Enable the stimulation and start BIST

           }

           DisableHallSensor();
           stimulation_cnt = 0;
           __delay_cycles(2000000); // the battery needs time to recover its voltage after being stressed by the LED

   }
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {


   if(stimulationStatus == STIMULATION_OFF)   __bic_SR_register_on_exit(LPM4_bits); // enable Stimulation

   else if(stimulationStatus == STIMULATION_ON) { // if Stimulation is on

       stimulation_cnt++;

       if(timer_resolution == RESOLUTION_LOW) __bic_SR_register_on_exit(LPM3_bits);
       if(timer_resolution == RESOLUTION_HIGH) __bic_SR_register_on_exit(LPM0_bits + LPM3_bits);

   }

   // if Hall Sensor is triggered again the ISR is exited without waiting for 2 seconds
   bist_status = BIST_ON; //start of BIST
   P2IFG &= ~BIT4;
   __delay_cycles(4000);
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR( void )
{
      // TA0CCR0
      TA0CTL &= ~(MC_1);
      P1OUT |= BIT2;

      if (bist_status == BIST_OFF) __bis_SR_register_on_exit(LPM3_bits + GIE); // modify SR in stack -> when you finish the interrupt routine go into LPM3

      else if(bist_status == BIST_ON) __bic_SR_register_on_exit(LPM0_bits);

}

// Timer1 A0 interrupt service routine
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR( void )
{
       // TA1CCR0
      TA0CTL = TASSEL_2 + MC_1;         // SMCLK, upmode, 1 MHz}
      P1OUT &= ~BIT2;

      if (bist_status == BIST_OFF) {
      __bic_SR_register_on_exit(LPM3_bits + GIE);
      __bis_SR_register_on_exit(LPM0_bits + GIE); // modify SR in stack -> when you finish the interrupt routine go into LPM0
      }

      else if(bist_status == BIST_ON)  __bis_SR_register_on_exit(LPM0_bits + GIE);

}
