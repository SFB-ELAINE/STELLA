/******************************************
 * STELLA RF  by Franz Plocksties / Obaid Ullah Shah
 ********************************************/

#include <msp430g2553.h>
#include <stdint.h>
#include <cc2500_reg.h>
#include <cc2500_val.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "SPI.h"
#include "DigiPot.h"
#include "Booster.h"
#include "ADC.h"
#include "Comparator.h"
#include "BlinkPattern.h"
#include "cc2500.h"
//#include "DBS_freq.h"


volatile Stimulation_Status stimulationStatus = STIMULATION_OFF;
volatile int stimulation_cnt = 0;

volatile Battery_Status batteryLevel = BATTERY_LEVEL_NORMAL;

volatile Impedance_Status impedanceLeftStatus;
volatile Impedance_Status impedanceRightStatus;

volatile uint8_t ImpStL = 0x00, ImpStR = 0x00;

volatile Stimulation_Type type = BILATERAL;
volatile Stimulation_Channel channel = LATERAL_RIGHT; // only for UNILATERAL

volatile Power_Mode power_mode = SLEEP;
volatile Stimulation_CLK stimulation_clk;

volatile Function func = SET;


volatile uint8_t t_pac[9] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // Transmission Packet
int value;
int VBoostArray[16];
volatile uint8_t *p; // Initialize Pointer for Receiving packet
uint8_t data_new[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x00}; //testing for p[8 debug]
int k;

volatile uint8_t *imp;  //impedance status

const unsigned int repetition = 3;

volatile uint8_t pack_rec = 0x00;
volatile uint8_t pack_search = 0x00;
volatile uint8_t timer_decision = 0x00; // for timer
volatile uint16_t timer_counter = 0x0000; // counts upto 65535 and then transmit
volatile uint16_t txn_counter = 5400; // transmission cycle counter
volatile uint16_t timer_wor_counter = 90; // transmission cycle counter
volatile uint8_t gdo0_change = 0x00;
// real time clock variable
volatile uint8_t  rtc_hh = 12;
volatile uint8_t  rtc_mm = 00;
volatile uint8_t  rtc_ss = 00;

volatile int i_left; // keep the value of i_left for evaluating the proper boost voltage in bilateral stimulation

volatile int VRef_test_left;
volatile int VRef_test_right;
extern volatile int VRef_test;
volatile int i_new_imp;
extern volatile unsigned int timer_tar_count; // returning TAR counter value for DBS freq calculation
volatile unsigned timer_tar_count_left;
volatile unsigned timer_tar_count_right;
volatile unsigned timer_tar_PW_left;
volatile unsigned timer_tar_PW_right;
volatile unsigned long delay = 0x0000;//volatile uint16_t delay = 0x0000; //variable for delay cycles in comparator functions
volatile uint16_t Freq_compare = 0x0000;
extern unsigned int dbs_array[20]; //returning sampled values of DBS signal (used in comparator.c)

void EnableStimulation() {

    stimulationStatus = STIMULATION_ON;
    P1OUT &= ~BIT2;              // Enable the current source
    P1SEL |= BIT2;
    //stimulation_cnt = 0;

}

void DisableStimulation() {

    stimulationStatus = STIMULATION_OFF;
    P1OUT |= BIT2;              // Disable the current source
    P1SEL &= ~BIT2;
    //StimulationOffPattern();
    //_BIS_SR(LPM4_bits + GIE);
}


void PWM(int freq, int pulse_width,  Power_Mode mode) {

    P1OUT &= ~BIT2;              // Enable the current source
    P1SEL |= BIT2;


    TA0CCTL1 = OUTMOD_7;        // CCR1 toggle/set

    if(mode == SLEEP) {
        //--------------------

        TA0CCR0 = freq;
        TA0CCR1 = pulse_width;
        //--------------------
        TA0CTL = TASSEL_2 + MC_1;   // SMCLK, up-down mode, 1MHz
    }

    else if(mode == DEEP_SLEEP) {

    P2SEL |= (BIT6 | BIT7); // Set P2.6 and P2.6 SEL for XIN, XOUT

        if(stimulation_clk == CLK_32kHz) {
            //----------------------------------------
            TA0CCR0 = freq;
            TA0CCR1 = pulse_width;
            //----------------------------------------
            TA0CTL = TASSEL_1 + MC_1;         // ACLK, upmode, 32 kHz}
        }

        else if(stimulation_clk == CLK_16kHz) {
            TA0CCR0 = DEEPSLEEP_16kHz_IMPLANT_TA0CCR0;
            TA0CCR1 = DEEPSLEEP_16kHz_IMPLANT_TA0CCR1;
            TA0CTL = TASSEL_1 + MC_1 + ID_1;         // ACLK, upmode, 16 kHz
}

}

}

void GDO0Setup() {

    // Set-up the HALL Sensor
    P2REN |= BIT5;               // Enable internal pull-up/down resistors
    P2OUT |= BIT5;              // Select pull-up mode for P2.5
    P2IES |= BIT5;              // P2.5 Hi/Lo edge
    P2IE  |= BIT5;
    P2IFG &= ~BIT5;              // P2.5 IFG cleared

}

void EnableGDO0() {

    P2OUT |= BIT5;  // Select pull-up mode for P2.5


}

void DisableGDO0() {

    P2OUT &= ~BIT5;  // Select pull-down mode for P2.5


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

uint16_t BatteryValue(){
    return CheckVBattery();
}

uint8_t ImpedanceStatus(int *VBoostArray, Stimulation_Channel channel, Stimulation_Type type, uint16_t Freq_compare, uint8_t pack_rec) {

    uint8_t status;


    //unsigned int VBoost = CheckVBoost();
    int i;
    int i_right;
    Set_DIGIPOT(VBOOSTER,240); //default value Set_DIGIPOT(VBOOSTER,240);
    __delay_cycles(40000); // for testing Obaid 09.11


    if(type == UNILATERAL) {

        i = _compare(channel, VBoostArray, Freq_compare, pack_rec);
        //dbs_freq(channel); // for freq of DBS signal
        __delay_cycles(100);

        if (i >= 0 && i < 16) {
            Set_DIGIPOT(VBOOSTER,i*16);
            i_new_imp = i*16;
            __delay_cycles(50000);  // for testing Obaid 10.11
            return status = 0x00;
                }
        else if (i == -1)  {
             Set_DIGIPOT(VBOOSTER,0);
             i_new_imp = 0;
             __delay_cycles(50000);  // for testing Obaid 10.11
            return status = 0x01;
                }
        else if (i == 16)  {
            Set_DIGIPOT(VBOOSTER,240);
            i_new_imp = 240;
            __delay_cycles(50000); // for testing Obaid 23.11
            return status = 0x02;
                }
    }


    else if(type == BILATERAL) {

        if(channel == LATERAL_LEFT)  {

            i_left = _compare(channel, VBoostArray, Freq_compare, pack_rec);
            //dbs_freq(channel); // for freq of DBS signal

            if (i_left >= 0 && i_left < 16) return status = 0x00;
            else if (i_left == -1) return status = 0x01;
            else if (i_left == 16) return status = 0x02;
        }

        if(channel == LATERAL_RIGHT)  {

            i_right = _compare(channel, VBoostArray, Freq_compare, pack_rec);
            //dbs_freq(channel); // for freq of DBS signal

            if(i_left == 16 || i_right == 16){
                Set_DIGIPOT(VBOOSTER,240);
                i_new_imp = 240;
            }
            else if(i_left == -1  && i_right == -1){
                Set_DIGIPOT(VBOOSTER,0);
                i_new_imp = 0;
            }
            else if(i_left < 16  && i_right < 16) {
                if(i_left > i_right)  {
                    Set_DIGIPOT(VBOOSTER,16*i_left);
                    i_new_imp = 16*i_left;
                }
                else {
                    Set_DIGIPOT(VBOOSTER, 16*i_right);
                    i_new_imp = 16*i_right;
                }
        }

            if (i_right >= 0 && i_right < 16) return status = 0x00;
            else if (i_right == -1) return status = 0x01;
            else if (i_right == 16) return status = 0x02;

        }

    }
return status;
}


uint8_t  *ImpChk(Stimulation_Type type, Stimulation_Channel channel, uint16_t Freq_compare, uint8_t pack_rec){
    uint8_t imps[2] = {0x03,0x03};
    if(type == BILATERAL) {

        imps[0] = ImpedanceStatus(VBoostArray, LATERAL_LEFT, BILATERAL, Freq_compare, pack_rec);
        VRef_test_left = VRef_test; //vref_return();
        if(imps[0]==0){
            //VRef_test_left = VRef_test; //vref_return();
            timer_tar_count_left = dbs_freq(channel, Freq_compare); // for freq of DBS signal
            timer_tar_PW_left = dbs_pw(channel, Freq_compare); // for PW of DBS signal
            //sample_dbs(channel, VRef_test, Freq_compare); //just for testing
        }
        //timer_tar_count_left = dbs_freq(channel); // for freq of DBS signal
        //timer_tar_PW_left = dbs_pw(channel); // for PW of DBS signal
        imps[1] = ImpedanceStatus(VBoostArray, LATERAL_RIGHT, BILATERAL, Freq_compare, pack_rec);
        VRef_test_right = VRef_test; //vref_return();
        if(imps[1]==0){
            timer_tar_count_right = dbs_freq(channel, Freq_compare); // for freq of DBS signal
            timer_tar_PW_right = dbs_pw(channel, Freq_compare); // for PW of DBS signal
        }
    }

    else if(type == UNILATERAL && channel == LATERAL_LEFT) {

        imps[0] = ImpedanceStatus(VBoostArray, LATERAL_LEFT, UNILATERAL, Freq_compare, pack_rec);
        VRef_test_left = VRef_test; //vref_return();
        if(imps[0]==0){
            if(pack_rec == 1){
                sample_dbs(channel, VRef_test, Freq_compare); //just for testing
                timer_tar_count_left = dbs_freq(channel, Freq_compare); // for freq of DBS signal
                timer_tar_PW_left = dbs_pw(channel, Freq_compare); // for PW of DBS signal
            }
        }
        VRef_test_right = 0x00;
        timer_tar_count_right = 0; // for freq of DBS signal
        timer_tar_PW_right = 0;
    }
    else if(type == UNILATERAL && channel == LATERAL_RIGHT) {

        imps[1] = ImpedanceStatus(VBoostArray, LATERAL_RIGHT, UNILATERAL, Freq_compare, pack_rec);
        VRef_test_right = VRef_test; //vref_return();
        VRef_test_left = 0x00;
        if(imps[1]==0){
            if(pack_rec == 1){
                sample_dbs(channel, VRef_test, Freq_compare); //just for testing
                timer_tar_count_right = dbs_freq(channel, Freq_compare); // for freq of DBS signal
                timer_tar_PW_right = dbs_pw(channel, Freq_compare); // for PW of DBS signal
            }
        }
        timer_tar_count_left = 0; // for freq of DBS signal
        timer_tar_PW_left = 0;

    }

    return imps;
}

void setupTimer() {
    P2SEL |= (BIT6 | BIT7); // Set P2.6 and P2.6 SEL for XIN, XOUT
    TA1CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TA1CCR0= 65535;
    TA1CTL = TASSEL_1 + MC_1;                  // ACLK, upmode
}

void count_rtc(){
    rtc_ss += 2;
    if(rtc_ss >= 60){
        rtc_ss = 0;
        if(++rtc_mm >= 60){
            rtc_mm = 0;
            if(++rtc_hh >= 24)
                rtc_hh = 0;
        }
    }
}

void ini_f_pw_variables(){
    timer_tar_count_left = 0;
    timer_tar_count_right = 0;
    timer_tar_PW_left = 0;
    timer_tar_PW_right = 0;
    dbs_array[0] = 0;
    dbs_array[1] = 0;
    dbs_array[2] = 0;
    dbs_array[3] = 0;
    dbs_array[4] = 0;
    dbs_array[5] = 0;
}

int main(void)  {
   //volatile Power_Mode p_mode = DEEP_SLEEP;
   volatile uint16_t FREQ=0x0000,PW=0x0000;
   volatile uint8_t Mode = 0x01;
   volatile uint8_t Stimulation = 0x00;
   volatile uint16_t tmp = 0x0000; //use to readout ADC value for temperature

   volatile uint16_t BatVal = 0x0000; // use to readout ADC value for battery
   volatile uint8_t RSSI_Value = 0x00; // use to readout RSSI register value for received packet
   volatile uint8_t Implant_ID = 0x01; // Implant id, it should be changed for every implant. Range 1-255

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

    if(power_mode == DEEP_SLEEP) stimulation_clk = CLK_32kHz;

    PORT_EN_VBOOST_FET_DIR |= EN_VBOOST_FET; // MOSFET for enable measuring the supply voltage (boost voltage)
    PORT_EN_VBOOST_FET_OUT &= ~EN_VBOOST_FET; // disable MOSFET for enable measuring the supply voltage (boost voltage)

    PORT_EN_VREF_FET_DIR |= EN_VREF_FET; // MOSFET for enable measuring the reference voltage adjusted by the AD5144 (100kOhm)
    PORT_EN_VREF_FET_OUT &= ~EN_VREF_FET; // disable MOSFET for enable measuring the reference voltage adjusted by the AD5144 (100kOhm)

    PORT_SHDN_OP1_DIR |= _SHDN_OP1;
    PORT_SHDN_OP2_DIR |= _SHDN_OP2;
    PORT_SHDN_OP1_OUT &= ~_SHDN_OP1; // disable OP left side
    PORT_SHDN_OP2_OUT &= ~_SHDN_OP2; // disable OP right side

    //------------------------

    HallSensorSetup(); // PWD (O)
    LEDSetup();
    ComparatorSetup();
    SPISetup();

    //------------------------
    //spi_init();
    UCB0CTL0 |= UCCKPH;
    cc2500_reset();
    cc2500_init();
    GDO0Setup();
    //cc2500_wor_init(); //16.04.21 comment out
    cc2500_nwor_init(); //Added 16.04.21
    pwd_mode_cc2500();
    DisableGDO0(); //very important to disable GDO0 for low current consumption
    //------------------------
    //    int VBoostArray[16];
        unsigned int VBoostCnt;
        int VBoostStep = 0; // initializa on 05112020 by Obaid

       for(VBoostCnt = 0; VBoostCnt < 16; VBoostCnt++) {

           Set_DIGIPOT(VBOOSTER, VBoostStep);
           __delay_cycles(300000);
           VBoostArray[VBoostCnt] = CheckVBoost(); // Obaid
           VBoostStep += 16;

        }


    Set_DIGIPOT(VBOOSTER, 0);
    __delay_cycles(300000);
    Set_DIGIPOT(COUNTERPULSE_ATTENUATION_LEFT, 0); // charge balancing resistance
    Set_DIGIPOT(COUNTERPULSE_ATTENUATION_RIGHT, 0); // charge balancing resistance
    Set_DIGIPOT(CURRENT_LEFT, 50);
    Set_DIGIPOT(CURRENT_RIGHT, 50);
    Set_DIGIPOT(VREF, 0);
    EnableHallSensor(); // PWD (O)
    DisableStimulation();
    StimulationOffPattern();
    setupTimer(); //Inclusion of second timer. Setup only
    CheckVBoostNew(); //just for testing of current reduction with 1.5v start
    BatteryValue(); //just for testing of current reduction with 1.5v start

   //------------------------
    while(1) {
        __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupt
        if(timer_counter >= txn_counter || timer_decision == 1){
            if(timer_counter >= txn_counter){
//                TA1CTL = MC_0; // testing only stop timer
                //DisableHallSensor(); //for testing on 01.03.2021 problem in swiping magnet after setting parameters
                timer_counter = 0;
                pack_rec = 0x00;
                //write transmit code
                volatile uint8_t t_pac[13] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // Transmission Packet
                ContinuousTxnPattern(); // just for testing, can be removed later
                GDO0Setup();
                DisableHallSensor(); //for testing on 11.03.2021 problem in swiping magnet after setting parameters
                UCB0CTL0 |= UCCKPH;
                UCB0CTL1 &= ~UCSWRST; //added for testing
                //cc2500_nwor_init(); //16.04.21 comment out
                __delay_cycles(10000); //just for testing 23.11
                DisableGDO0();  // Disable GDO interrupt while Transmitting data
                SPI_writeData(CC2500, REG_CHANNR,   0x00);
                pwd_mode_cc2500(); //PWD (O) 110321
                stimulationStatus = (Stimulation_Status)data_new[5];  // Stimulation ON or OFF
                if (stimulationStatus == STIMULATION_OFF){
                    DisableStimulation();
                    Set_DIGIPOT(VBOOSTER, 0); //ADDED 23.11
                    __delay_cycles(600000); // Added 23.11
                    tmp=temp_read();    // Reading temperature
                    t_pac[0] = 0x00 | tmp;  // breaking temperature value in to bytes for SPI transmission
                    t_pac[1] = 0x00 | tmp >> 8;
                    t_pac[3] = 0x03;          // Left Lateral Impedance
                    t_pac[4] = 0x03;          // Right Lateral Impedance
                    uint16_t VBoost_value = CheckVBoostNew();
                    BatVal = BatteryValue();
                    t_pac[2] = 0x00 | BatVal;  // breaking voltage value in to bytes for SPI transmission
                    t_pac[5] = 0x00 | BatVal >> 8;
                    t_pac[6] = 0x00 | VBoost_value;
                    t_pac[7] = 0x00 | VBoost_value >> 8;
                    UCB0CTL0 |= UCCKPH;
                    UCB0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
                    __delay_cycles(40000); // just for testing
                    awake_cc2500(); //110321
                    __delay_cycles(4000); // just for testing
                    //t_pac[8] = 0x00;  // RSSI Obaid
                    t_pac[8] = 0x00;
                    t_pac[9] = 0x00;
                    t_pac[10] = Implant_ID;
                    t_pac[11] = rtc_hh;
                    t_pac[12] = rtc_mm;
                    cc2500_transmit_arr(13,t_pac);
                    SPI_writeData(CC2500, REG_CHANNR,   VAL_CHANNR);
                    EnableGDO0(); // Enable GDO interrupt after transmission
                    //cc2500_wor_init(); //16.04.21 comment out
                }
                else if(stimulationStatus == STIMULATION_ON){
                    //EnableStimulation();
                    FREQ = data_new[1]<<8 | data_new[0];      // Getting Frequency Values
                    PW =  data_new[3]<<8| data_new[2];        //Getting PWM ON-time Value
                    Freq_compare = FREQ; // using for delay function. At moment in testing phase
                    Mode = data_new[4];                // Sleep or Deep Sleep mode
                    power_mode = (Power_Mode)Mode;
                    type = (Stimulation_Type)data_new[7];          // Bilateral or Uni-lateral //testing
                    channel = (Stimulation_Channel)data_new[8];       // Left or Right Channel //testing
                    Set_DIGIPOT(COUNTERPULSE_ATTENUATION_LEFT, data_new[9]); // charge balancing resistance
                    Set_DIGIPOT(COUNTERPULSE_ATTENUATION_RIGHT, data_new[9]); // charge balancing resistance
                    Set_DIGIPOT(CURRENT_LEFT, data_new[10]);
                    Set_DIGIPOT(CURRENT_RIGHT, data_new[10]);
                    //__delay_cycles(10000); // for testing 23.11
                    PWM(FREQ, PW,(Power_Mode)Mode);
                    EnableStimulation();
                    __delay_cycles(10000); // for testing 23.11
                    tmp=temp_read();    // Reading temperature
                    t_pac[0] = 0x00 | tmp;  // breaking temperature value in to bytes for SPI transmission
                    t_pac[1] = 0x00 | tmp >> 8;
                    pack_rec = 0x00;
                    imp=ImpChk(type,channel,Freq_compare, pack_rec);   // impedance status
                    t_pac[3] = imp[0];          // Left Lateral Impedance
                    t_pac[4] = imp[1];          // Right Lateral Impedance
                    rtc_ss++; // adding offset because timer was stopped for approximately 1.266 secs
                    rtc_ss++;
                    setupTimer(); // again setup the timer
                    __delay_cycles(900000);
                    uint16_t VBoost_value = CheckVBoostNew();
                    BatVal = BatteryValue();
                    t_pac[2] = 0x00 | BatVal;  // breaking voltage value in to bytes for SPI transmission
                    t_pac[5] = 0x00 | BatVal >> 8;
                    t_pac[6] = 0x00 | VBoost_value;
                    t_pac[7] = 0x00 | VBoost_value >> 8;
                    Set_DIGIPOT(VBOOSTER, 0); //ADDED 23.11
                    UCB0CTL0 |= UCCKPH; //110321
                    UCB0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
                    __delay_cycles(40000); // just for testing
                    awake_cc2500(); //110321
                    __delay_cycles(4000); // just for testing
                    //t_pac[8] = 0x00;  // RSSI Obaid
                    t_pac[8] = VRef_test_left;
                    t_pac[9] = VRef_test_right;
                    t_pac[10] = Implant_ID;
                    t_pac[11] = rtc_hh;
                    t_pac[12] = rtc_mm;
                    UCB0CTL0 |= UCCKPH;
                    UCB0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
                    cc2500_transmit_arr(13,t_pac);
                    SPI_writeData(CC2500, REG_CHANNR,   VAL_CHANNR);
                    EnableGDO0(); // Enable GDO interrupt after transmission
                    //cc2500_wor_init(); //16.04.21 comment out
                    Set_DIGIPOT(VBOOSTER, i_new_imp); //to adjust VBooster to proper value
                    __delay_cycles(300000);
                }
                pack_rec = 0;
                UCB0CTL0 |= UCCKPH;
                UCB0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
                pwd_mode_cc2500(); //PWD (O)
                DisableGDO0(); // PWD (O)
                EnableHallSensor(); // PWD (O)
                timer_decision = 0;
                ini_f_pw_variables(); // re-initialize f and pw variables
//                setupTimer(); //TA1CTL = MC_1;                  // start timer
            }
            else if(timer_decision == 0x01){ //(timer_decision == 0x01)

                //volatile uint8_t t_pac[22] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // Transmission Packet
                volatile uint8_t t_pac_new[34] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
                DisableHallSensor();

                Set_DIGIPOT(VREF, 0); //added 11.05.21 for minimum voltage
                __delay_cycles(500000); // for easy detection of wor on pattern after enabling it 23.11
                StartErrorRecognitionPattern();  // just for testing, can be removed later
                __delay_cycles(10000); // delay for voltage stabilization

                TA1CTL = MC_0; // testing only stop timer
                GDO0Setup();
                UCB0CTL0 |= UCCKPH;
                UCB0CTL1 &= ~UCSWRST; //added for testing
                cc2500_wor_init(); //added 16.04.21
                cc2500_strobe(CC2500_IDLE);
                cc2500_strobe(CC2500_FRX);
                cc2500_strobe(CC2500_IDLE);
                cc2500_strobe(CC2500_WOR);
                EnableGDO0(); // PWD (O)
                pack_search = 1; //used for selection of HS functionality in interrupt
                EnableHallSensor(); // PWD (O) dual
                timer_counter = 0; //reinitialize counter for use in WOR cycle count
                setupTimer(); //Inclusion of second timer. Setup only


                while((timer_counter <= timer_wor_counter && pack_rec == 1 && gdo0_change == 0)){
                    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupt
                    if(timer_counter >= timer_wor_counter-1){
                        pack_rec = 0x00;

                    }
                }

                if(pack_rec == 1){
                    TA1CTL = MC_0; // testing only stop timer
                    p=cc2500_receive_wor();
                    gdo0_change = 0x00;
                    if(p[14] != 0x00 || p[15] != 0x00){
                        RSSI_Value = cc2500_SPIReadStatus(REG_RSSI); // for testing purpose
                        for ( k = 0; k<= 15; k++) {
                            data_new[k] = p[k];
                        }
                    }

                    cc2500_nwor_init();
                    __delay_cycles(10000); //just for testing 23.11
                    DisableGDO0();  // Disable GDO interrupt while Transmitting data
                    SPI_writeData(CC2500, REG_PATABLE,  data_new[11]);
                    SPI_writeData(CC2500, REG_CHANNR,   0x00);
                    pwd_mode_cc2500(); //PWD (O) 110321
                    //always update time
                    rtc_hh = data_new[12];
                    rtc_mm = data_new[13];
                    txn_counter = data_new[15]<<8 | data_new[14]; // update the transmission cycle

                    stimulationStatus = (Stimulation_Status)data_new[5];  // Stimulation ON or OFF
                    if (stimulationStatus == STIMULATION_OFF){
                        DisableStimulation();
                        Set_DIGIPOT(VBOOSTER, 0); //ADDED 23.11
                        __delay_cycles(600000); // Added 23.11
                        tmp=temp_read();    // Reading temperature
                        t_pac_new[0] = 0x00 | tmp;  // breaking temperature value in to bytes for SPI transmission
                        t_pac_new[1] = 0x00 | tmp >> 8;
                        t_pac_new[3] = 0x03;          // Left Lateral Impedance
                        t_pac_new[4] = 0x03;          // Right Lateral Impedance
                        uint16_t VBoost_value = CheckVBoostNew();
                        BatVal = BatteryValue();
                        t_pac_new[2] = 0x00 | BatVal;  // breaking voltage value in to bytes for SPI transmission
                        t_pac_new[5] = 0x00 | BatVal >> 8;
                        t_pac_new[6] = 0x00 | VBoost_value;
                        t_pac_new[7] = 0x00 | VBoost_value >> 8;
                        UCB0CTL0 |= UCCKPH;
                        UCB0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
                        //awake_cc2500(); //110321
                        __delay_cycles(40000); // just for testing
                        awake_cc2500(); //110321
                        t_pac_new[8] = RSSI_Value;  // RSSI Obaid
                        t_pac_new[9] = 0x00;
                        t_pac_new[10] = 0x00;
                        t_pac_new[11] = Implant_ID;
                        t_pac_new[12] = rtc_hh;
                        t_pac_new[13] = rtc_mm;
                        t_pac_new[14] = 0x00;
                        t_pac_new[15] = 0x00;
                        t_pac_new[16] = 0x00;
                        t_pac_new[17] = 0x00;
                        t_pac_new[18] = 0x00;
                        t_pac_new[19] = 0x00;
                        t_pac_new[20] = 0x00;
                        t_pac_new[21] = 0x00;
                        t_pac_new[22] = dbs_array[0];
                        t_pac_new[23] = dbs_array[1];
                        t_pac_new[24] = dbs_array[2];
                        t_pac_new[25] = dbs_array[3];
                        t_pac_new[26] = dbs_array[4];
                        t_pac_new[27] = dbs_array[5];
                        t_pac_new[28] = dbs_array[6];
                        t_pac_new[29] = dbs_array[7];
                        t_pac_new[30] = dbs_array[8];
                        t_pac_new[31] = dbs_array[9];
                        t_pac_new[32] = dbs_array[10];
                        t_pac_new[33] = dbs_array[11];
                        cc2500_transmit_arr_new(34,t_pac_new);
                        //cc2500_transmit_arr(12,t_pac);
                        SPI_writeData(CC2500, REG_CHANNR,   VAL_CHANNR);

                        EnableGDO0(); // Enable GDO interrupt after transmission
                        //cc2500_wor_init(); //16.04.21 comment out
                    }
                    else if(stimulationStatus == STIMULATION_ON){
                        //EnableStimulation();
                        FREQ = data_new[1]<<8 | data_new[0];      // Getting Frequency Values
                        Freq_compare = FREQ; // using for delay function. At moment in testing phase
                        PW =  data_new[3]<<8| data_new[2];        //Getting PWM ON-time Value
                        Mode = data_new[4];                // Sleep or Deep Sleep mode
                        power_mode = (Power_Mode)Mode;
                        type = (Stimulation_Type)data_new[7];          // Bilateral or Uni-lateral //testing
                        channel = (Stimulation_Channel)data_new[8];       // Left or Right Channel //testing
                        Set_DIGIPOT(COUNTERPULSE_ATTENUATION_LEFT, data_new[9]); // charge balancing resistance
                        Set_DIGIPOT(COUNTERPULSE_ATTENUATION_RIGHT, data_new[9]); // charge balancing resistance
                        Set_DIGIPOT(CURRENT_LEFT, data_new[10]);
                        Set_DIGIPOT(CURRENT_RIGHT, data_new[10]);
                        //__delay_cycles(10000); // for testing 23.11
                        PWM(FREQ, PW,(Power_Mode)Mode);
                        EnableStimulation();
                        __delay_cycles(10000); // for testing 23.11

                        tmp=temp_read();    // Reading temperature
                        t_pac_new[0] = 0x00 | tmp;  // breaking temperature value in to bytes for SPI transmission
                        t_pac_new[1] = 0x00 | tmp >> 8;
                        imp=ImpChk(type,channel, Freq_compare, pack_rec);   // impedance status
                        t_pac_new[3] = imp[0];          // Left Lateral Impedance
                        t_pac_new[4] = imp[1];          // Right Lateral Impedance
                        setupTimer(); // again setup the timer
                        __delay_cycles(900000);
                        uint16_t VBoost_value = CheckVBoostNew();
                        BatVal = BatteryValue();
                        t_pac_new[2] = 0x00 | BatVal;  // breaking voltage value in to bytes for SPI transmission
                        t_pac_new[5] = 0x00 | BatVal >> 8;
                        t_pac_new[6] = 0x00 | VBoost_value;
                        t_pac_new[7] = 0x00 | VBoost_value >> 8;
                        Set_DIGIPOT(VBOOSTER, 0); //ADDED 23.11
                        t_pac_new[8] = RSSI_Value;  // RSSI Obaid
                        UCB0CTL0 |= UCCKPH; //110321
                        UCB0CTL1 &= ~UCSWRST; //110321
                        __delay_cycles(40000); // just for testing
                        awake_cc2500(); //110321
                        t_pac_new[9] = VRef_test_left;
                        t_pac_new[10] = VRef_test_right;
                        t_pac_new[11] = Implant_ID;
                        t_pac_new[12] = rtc_hh;
                        t_pac_new[13] = rtc_mm;
                        t_pac_new[14] = 0x00 | timer_tar_count_left;
                        t_pac_new[15] = 0x00 | timer_tar_count_left >> 8;
                        t_pac_new[16] = 0x00 | timer_tar_count_right;
                        t_pac_new[17] = 0x00 | timer_tar_count_right >> 8;
                        t_pac_new[18] = 0x00 | timer_tar_PW_left;
                        t_pac_new[19] = 0x00 | timer_tar_PW_left >> 8;
                        t_pac_new[20] = 0x00 | timer_tar_PW_right;
                        t_pac_new[21] = 0x00 | timer_tar_PW_right >> 8;
                        t_pac_new[22] = 0x00 | dbs_array[0];
                        t_pac_new[23] = 0x00 | dbs_array[0] >> 8;
                        t_pac_new[24] = 0x00 | dbs_array[1];
                        t_pac_new[25] = 0x00 | dbs_array[1] >> 8;
                        t_pac_new[26] = 0x00 | dbs_array[2];
                        t_pac_new[27] = 0x00 | dbs_array[2] >> 8;
                        t_pac_new[28] = 0x00 | dbs_array[3];
                        t_pac_new[29] = 0x00 | dbs_array[3] >> 8;
                        t_pac_new[30] = 0x00 | dbs_array[4];
                        t_pac_new[31] = 0x00 | dbs_array[4] >> 8;
                        t_pac_new[32] = 0x00 | dbs_array[5];
                        t_pac_new[33] = 0x00 | dbs_array[5] >> 8;

                        UCB0CTL0 |= UCCKPH;
                        UCB0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
                        cc2500_transmit_arr_new(34,t_pac_new);
                        SPI_writeData(CC2500, REG_CHANNR,   VAL_CHANNR);
                        EnableGDO0(); // Enable GDO interrupt after transmission
                        //cc2500_wor_init(); //16.04.21 comment out
                        Set_DIGIPOT(VBOOSTER, i_new_imp); //to adjust VBooster to proper value
                        __delay_cycles(300000);
                        dbs_array[0] = 0;
                        dbs_array[1] = 0;
                        dbs_array[2] = 0;
                        dbs_array[3] = 0;
                        dbs_array[4] = 0;
                        dbs_array[5] = 0;
                        dbs_array[6] = 0;
                        dbs_array[7] = 0;
                        dbs_array[8] = 0;
                        dbs_array[9] = 0;
                        dbs_array[10] = 0;
                    }
                    pack_rec = 0x00;
                    gdo0_change = 0x00;
                    UCB0CTL0 |= UCCKPH;
                    UCB0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
                    pwd_mode_cc2500(); //PWD (O)
                }
                else{
                    TA1CTL = MC_0; // stop timer 17.05.21
                    pwd_mode_cc2500(); //PWD (O)
                    PacketNotReceive();
                    Set_DIGIPOT(VBOOSTER, i_new_imp); //to adjust VBooster to proper value
                    //txn_counter = data_new[15]<<8 | data_new[14]; // update the transmission cycle, 25.05.21 Obaid
                    __delay_cycles(300000);
                    UCB0CTL0 |= UCCKPH;
                    UCB0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
                }
                pack_rec = 0x00;
                gdo0_change = 0x00;
                pack_search = 0;
                //----------------
                DisableGDO0(); // PWD (O)
                EnableHallSensor(); // PWD (O)
                timer_decision = 0;
                ini_f_pw_variables(); // re-initialize f and pw variables
                setupTimer(); //TA1CTL = MC_1;                  // start timer

            }
        }
    }
}


#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
    __bic_SR_register_on_exit(LPM3_bits);

       if(P2IFG&BIT4){
           if(pack_search == 0){
               //wake up radio
               awake_cc2500();
               pack_rec = 0x01;
               gdo0_change = 0x00;
           }
           else{

               pack_rec = 0x00;
               gdo0_change = 0x00;
           }
       }
       else{
           P3OUT^=BIT4;
           P3OUT^=BIT4;
           gdo0_change = 0x01;
       }

       P2IFG=0;
       timer_decision = 1;
//       TA1CTL = MC_0; // testing only stop timer
       __delay_cycles(4000);

}

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void){

    timer_counter += 1;
    count_rtc();

    __bic_SR_register_on_exit(LPM3_bits);
}
