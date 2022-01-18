#include <msp430.h>
#include <stdint.h>
#include <cc2500_reg.h>
#include <cc2500_val.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "SPI.h"

// PIN2.4 is used as GDO0 input pin

#define SPI_A       BIT1 | BIT2 | BIT4      // MISO, MOSI, SCLK
#define SPI_B       BIT5 | BIT6 | BIT7      // SCLK, MISO, MOSI  // currently using this
#define CS_OUT      P3OUT
#define CS_BIT      BIT3
#define SPI_TX_I    UCB0TXIFG
#define TX_BUF      UCB0TXBUF
#define RX_BUF      UCB0RXBUF
#define SPI_STAT    UCB0STAT

uint8_t packet[11];


void spi_init(void){

    P3DIR |= CS_BIT;              // Set CS pin direction as output
    CS_OUT |= CS_BIT;             //Set port 3 pin 3 to CS
    P1SEL = SPI_B;                // Set UCB0 for spi configuration
    P1SEL2 = SPI_B;                // Set UCB0 for spi configuration

    P3DIR |= BIT0;  // CS for AD5144
    P3OUT |= BIT0;

    P3DIR |= BIT1;   // CS for AD5142
    P3OUT |= BIT1;



    P2DIR &= ~BIT5;            // Set port 2 pin 5 as GDO0 input
    P2REN |= BIT5;            // P2.5 Connect to pull up/down resistor
    P2OUT &= ~BIT5;          // Set P2.5 to pull down resistor

    UCB0CTL0 |= UCMSB + UCMST + UCSYNC + UCCKPH;  // Set MSB first, Master, Syncronize mode
    UCB0CTL1 |= UCSSEL_2;                         // Select SMCLK as SPI clock
    UCB0BR0 |= 0x02;                              // Divide main clock by 2
    UCB0BR1 = 0;

    UCB0CTL1 &= ~UCSWRST;


}


int cc2500_read_reg(uint8_t address){
    int x = 0;

    CS_OUT &= (~CS_BIT);                       // Set CS pin low

    while (!(IFG2 & SPI_TX_I));                // wait for TX buf to be ready
    TX_BUF = address|0x80;                     // send address with MSB '1'
    //while (SPI_STAT  & UCBUSY);
    while (!(IFG2 & SPI_TX_I));                // wait for TX buf to be ready
    TX_BUF = 0x00;                             // send dummy byte
    while (SPI_STAT  & UCBUSY);                // wait until busy gets low and data is send

    CS_OUT |= (CS_BIT);                        // Set CS pin high

 x = RX_BUF;                                   // copy data from rx but to x
 return x;                                     //return read data

}

int cc2500_SPIReadStatus(uint8_t addr)
{
  int status;

  CS_OUT &= (~CS_BIT);                      // Set CS pin to low
  while (!(IFG2 & SPI_TX_I));            // wait for TX buf to be ready
  TX_BUF = (addr | REG_READ_BURST);  // Send address
  while (!(IFG2 & SPI_TX_I));            // wait for TX buf to be ready
  TX_BUF = 0;                            // Dummy write so we can read data
  while (SPI_STAT  & UCBUSY);                // wait until busy gets low and data is send
  status = UCB0RXBUF;                       // Read data
  CS_OUT |= (CS_BIT);                        // Set CS pin high/ CS disable
  //status = UCB0RXBUF;                       // Read data


  return status;
}

void cc2500_reset(void){

        CS_OUT &= ~CS_BIT;                         // Set CS pin low

        while (!(IFG2 & SPI_TX_I));            // wait for TX buf to be ready
        TX_BUF = CC2500_RES;                 // Send reset strobe
        while (SPI_STAT & UCBUSY);              // wait until busy gets low and data is send
        while(P3IN & BIT3);                    // wait until reset is done

        CS_OUT |= CS_BIT;                          // Set CS pin high


}

void cc2500_strobe(uint8_t strobe){

    CS_OUT &= (~CS_BIT);                 // Set CS pin to low

    while (!(IFG2 & SPI_TX_I));          // wait for TX buf to be ready
    TX_BUF = strobe;                     // send strobe
    while (SPI_STAT  & UCBUSY);          // wait until busy gets low and strobe is send

    CS_OUT |= (CS_BIT);                 // Set CS pin high


}

void cc2500_init(void) {

    SPI_writeData(CC2500, REG_IOCFG2,   VAL_IOCFG2);     // GDO2 output pin config.
    SPI_writeData(CC2500, REG_IOCFG1,   VAL_IOCFG1);     // GDO1 output pin config.
    SPI_writeData(CC2500, REG_IOCFG0,   VAL_IOCFG0);     // GDO0 output pin config.
    SPI_writeData(CC2500, REG_FIFOTHR,  VAL_FIFOTHR);
    SPI_writeData(CC2500, REG_SYNC1,    VAL_SYNC1);
    SPI_writeData(CC2500, REG_SYNC0,    VAL_SYNC0);
    SPI_writeData(CC2500, REG_PKTLEN,   VAL_PKTLEN);
    SPI_writeData(CC2500, REG_PKTCTRL1, VAL_PKTCTRL1);
    SPI_writeData(CC2500, REG_PKTCTRL0, VAL_PKTCTRL0);
    SPI_writeData(CC2500, REG_ADDR,     VAL_ADDR);
    SPI_writeData(CC2500, REG_CHANNR,   VAL_CHANNR);
    SPI_writeData(CC2500, REG_FSCTRL1,  VAL_FSCTRL1);
    SPI_writeData(CC2500, REG_FSCTRL0,  VAL_FSCTRL0);
    SPI_writeData(CC2500, REG_FREQ2,    VAL_FREQ2);
    SPI_writeData(CC2500, REG_FREQ1,    VAL_FREQ1);
    SPI_writeData(CC2500, REG_FREQ0,    VAL_FREQ0);
    SPI_writeData(CC2500, REG_MDMCFG4,  VAL_MDMCFG4);
    SPI_writeData(CC2500, REG_MDMCFG3,  VAL_MDMCFG3);
    SPI_writeData(CC2500, REG_MDMCFG2,  VAL_MDMCFG2);
    SPI_writeData(CC2500, REG_MDMCFG1,  VAL_MDMCFG1);
    SPI_writeData(CC2500, REG_MDMCFG0,  VAL_MDMCFG0);
    SPI_writeData(CC2500, REG_DEVIATN,  VAL_DEVIATN);
    SPI_writeData(CC2500, REG_MCSM2,    VAL_MCSM2);
    SPI_writeData(CC2500, REG_MCSM1,    VAL_MCSM1);
    SPI_writeData(CC2500, REG_MCSM0,    VAL_MCSM0);
    SPI_writeData(CC2500, REG_FOCCFG,   VAL_FOCCFG);


    SPI_writeData(CC2500, REG_BSCFG,    VAL_BSCFG);
    SPI_writeData(CC2500, REG_AGCCTRL2, VAL_AGCCTRL2);
    SPI_writeData(CC2500, REG_AGCCTRL1, VAL_AGCCTRL1);
    SPI_writeData(CC2500, REG_AGCCTRL0, VAL_AGCCTRL0);
    SPI_writeData(CC2500, REG_WOREVT1,  VAL_WOREVT1);
    SPI_writeData(CC2500, REG_WOREVT0,  VAL_WOREVT0);
    SPI_writeData(CC2500, REG_WORCTRL,  VAL_WORCTRL);
    SPI_writeData(CC2500, REG_FREND1,   VAL_FREND1);
    SPI_writeData(CC2500, REG_FREND0,   VAL_FREND0);
    SPI_writeData(CC2500, REG_FSCAL3,   VAL_FSCAL3);
    SPI_writeData(CC2500, REG_FSCAL2,   VAL_FSCAL2);
    SPI_writeData(CC2500, REG_FSCAL1,   VAL_FSCAL1);
    SPI_writeData(CC2500, REG_FSCAL0,   VAL_FSCAL0);
    SPI_writeData(CC2500, REG_RCCTRL1,  VAL_RCCTRL1);
    SPI_writeData(CC2500, REG_RCCTRL0,  VAL_RCCTRL0);
    SPI_writeData(CC2500, REG_FSTEST,   VAL_FSTEST);
    SPI_writeData(CC2500, REG_PTEST,    VAL_PTEST);
    SPI_writeData(CC2500, REG_AGCTEST,  VAL_AGCTEST);
    SPI_writeData(CC2500, REG_TEST2,    VAL_TEST2);
    SPI_writeData(CC2500, REG_TEST1,    VAL_TEST1);
    SPI_writeData(CC2500, REG_TEST0,    VAL_TEST0);
    SPI_writeData(CC2500, REG_PATABLE, 0xBB);
}



void led_init_match(void){
    __delay_cycles(200000);
    //P3DIR |= BIT4;
    P3OUT |= BIT4;
    __delay_cycles(200000); //100000
    //400000
    P3OUT &= ~BIT4;
    __delay_cycles(500000);
    //P3DIR |= BIT4;
    P3OUT |= BIT4;
    __delay_cycles(200000); //100000
    //400000
    P3OUT &= ~BIT4;
}

void led_init_nmatch(void){
    __delay_cycles(100000);
    //P3DIR |= BIT4;
    P3OUT |= BIT4;
    __delay_cycles(500000);
    P3OUT &= ~BIT4;
}



uint8_t *cc2500_receive_wor() {
    int i, length;
    uint8_t data[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t address;
    uint8_t id = 50;

    address = cc2500_read_reg(REG_RXFIFO);     // get device address
    if (address == id){                        // compare address

    length=cc2500_read_reg(REG_RXFIFO);        // get length
    for(i=0; i<length; i++) {
        data[i]=cc2500_read_reg(REG_RXFIFO);    // write received data in data array

    }
    led_init_match();
    }

    else{
        led_init_nmatch();
    }
    cc2500_strobe(CC2500_IDLE);          // IDLE mode
    cc2500_strobe(CC2500_FRX);          // Flush RX register
    cc2500_strobe(CC2500_IDLE);          // IDLE mode
    //cc2500_strobe(CC2500_WOR);         // removed by Obaid, no need of WOR now
    //for(i=0; i<10; i++)
        //packet[i] = data[i];
    return data;
}

void cc2500_transmit_arr_new(uint8_t l, volatile uint8_t d[]){
    //function for transferring data after receiving data from base-station
    cc2500_strobe(CC2500_IDLE);
    cc2500_strobe(CC2500_FTX);
    cc2500_strobe(CC2500_IDLE);

    SPI_writeData(CC2500, REG_TXFIFO, 50);
    SPI_writeData(CC2500, REG_TXFIFO, l);
    SPI_writeData(CC2500, REG_TXFIFO, d[1]);
    SPI_writeData(CC2500, REG_TXFIFO, d[0]);
    SPI_writeData(CC2500, REG_TXFIFO, d[3]);
    SPI_writeData(CC2500, REG_TXFIFO, d[4]);

    SPI_writeData(CC2500, REG_TXFIFO, d[5]);
    SPI_writeData(CC2500, REG_TXFIFO, d[2]);
    SPI_writeData(CC2500, REG_TXFIFO, d[7]);
    SPI_writeData(CC2500, REG_TXFIFO, d[6]);
    SPI_writeData(CC2500, REG_TXFIFO, d[8]);
    SPI_writeData(CC2500, REG_TXFIFO, d[9]);
    SPI_writeData(CC2500, REG_TXFIFO, d[10]);
    SPI_writeData(CC2500, REG_TXFIFO, d[11]);
    SPI_writeData(CC2500, REG_TXFIFO, d[12]);
    SPI_writeData(CC2500, REG_TXFIFO, d[13]);
    SPI_writeData(CC2500, REG_TXFIFO, d[15]);
    SPI_writeData(CC2500, REG_TXFIFO, d[14]);
    SPI_writeData(CC2500, REG_TXFIFO, d[17]);
    SPI_writeData(CC2500, REG_TXFIFO, d[16]);
    SPI_writeData(CC2500, REG_TXFIFO, d[19]);
    SPI_writeData(CC2500, REG_TXFIFO, d[18]);
    SPI_writeData(CC2500, REG_TXFIFO, d[21]);
    SPI_writeData(CC2500, REG_TXFIFO, d[20]);
    SPI_writeData(CC2500, REG_TXFIFO, d[23]);
    SPI_writeData(CC2500, REG_TXFIFO, d[22]);
    SPI_writeData(CC2500, REG_TXFIFO, d[25]);
    SPI_writeData(CC2500, REG_TXFIFO, d[24]);
    SPI_writeData(CC2500, REG_TXFIFO, d[27]);
    SPI_writeData(CC2500, REG_TXFIFO, d[26]);
    SPI_writeData(CC2500, REG_TXFIFO, d[29]);
    SPI_writeData(CC2500, REG_TXFIFO, d[28]);
    SPI_writeData(CC2500, REG_TXFIFO, d[31]);
    SPI_writeData(CC2500, REG_TXFIFO, d[30]);
    SPI_writeData(CC2500, REG_TXFIFO, d[33]);
    SPI_writeData(CC2500, REG_TXFIFO, d[32]);
    //SPI_writeData(CC2500, REG_TXFIFO, d[3]);


    //cc2500_strobe(CC2500_IDLE);
    //__delay_cycles(1500000);
    cc2500_strobe(CC2500_TX);

    while((P2IN & BIT5) != BIT5);              // wait for GDO0 to be high
    while((P2IN & BIT5) == BIT5);              // wait for GDO0 to be low

    cc2500_strobe(CC2500_IDLE);
    //led_init_match();


}

void cc2500_transmit_arr(uint8_t l, volatile uint8_t d[]){
//static int x = 0;
    //int data = sdata;
    cc2500_strobe(CC2500_IDLE);
    cc2500_strobe(CC2500_FTX);
    cc2500_strobe(CC2500_IDLE);

    SPI_writeData(CC2500, REG_TXFIFO, 50);
    SPI_writeData(CC2500, REG_TXFIFO, l);
    SPI_writeData(CC2500, REG_TXFIFO, d[1]);
    SPI_writeData(CC2500, REG_TXFIFO, d[0]);
    SPI_writeData(CC2500, REG_TXFIFO, d[3]);
    SPI_writeData(CC2500, REG_TXFIFO, d[4]);

    SPI_writeData(CC2500, REG_TXFIFO, d[5]);
    SPI_writeData(CC2500, REG_TXFIFO, d[2]);
    SPI_writeData(CC2500, REG_TXFIFO, d[7]);
    SPI_writeData(CC2500, REG_TXFIFO, d[6]);
    SPI_writeData(CC2500, REG_TXFIFO, d[8]);
    SPI_writeData(CC2500, REG_TXFIFO, d[9]);
    SPI_writeData(CC2500, REG_TXFIFO, d[10]);
    SPI_writeData(CC2500, REG_TXFIFO, d[11]);
    SPI_writeData(CC2500, REG_TXFIFO, d[12]);
    //SPI_writeData(CC2500, REG_TXFIFO, d[13]);


    //cc2500_strobe(CC2500_IDLE);
    //__delay_cycles(1500000);
    cc2500_strobe(CC2500_TX);

    while((P2IN & BIT5) != BIT5);              // wait for GDO0 to be high
    while((P2IN & BIT5) == BIT5);              // wait for GDO0 to be low

    cc2500_strobe(CC2500_IDLE);
    //led_init_match();


}

void cc2500_wor_init(){

    // 15 s
//    SPI_writeData(CC2500, REG_MCSM2,    0x01);
//    SPI_writeData(CC2500, REG_MCSM0,    0x38);
//    SPI_writeData(CC2500, REG_WOREVT1,  0x3F);
//    SPI_writeData(CC2500, REG_WOREVT0,  0x7A);
//    SPI_writeData(CC2500, REG_WORCTRL,  0x09);

    // 3 s
    SPI_writeData(CC2500, REG_MCSM2,    0x03);
    SPI_writeData(CC2500, REG_MCSM0,    0x38);
    SPI_writeData(CC2500, REG_WOREVT1,  0x19);
    SPI_writeData(CC2500, REG_WOREVT0,  0x32);
    SPI_writeData(CC2500, REG_WORCTRL,  0x09);

    // 1 s / WORCTRL = 00001001

//    SPI_writeData(CC2500, REG_MCSM2,    0x06);
//    SPI_writeData(CC2500, REG_MCSM0,    0x38);
//    SPI_writeData(CC2500, REG_WOREVT1,  0x64);
//    SPI_writeData(CC2500, REG_WOREVT0,  0x3B);
//    SPI_writeData(CC2500, REG_WORCTRL,  0x08);

    // 1 s

//    SPI_writeData(CC2500, REG_MCSM2,    0x06);
//    SPI_writeData(CC2500, REG_MCSM0,    0x38);
//    SPI_writeData(CC2500, REG_WOREVT1,  0x87);
//    SPI_writeData(CC2500, REG_WOREVT0,  0x6A);
//    SPI_writeData(CC2500, REG_WORCTRL,  0x08);
}

/*void cc2500_wor_init(){
    SPI_writeData(CC2500, REG_MCSM2,    0x00); //8sec cycle
    SPI_writeData(CC2500, REG_MCSM0,    0x38);
    SPI_writeData(CC2500, REG_WOREVT1,  0x21);
    SPI_writeData(CC2500, REG_WOREVT0,  0xDB);
    SPI_writeData(CC2500, REG_WORCTRL,  0x09);
}*/

/*void cc2500_wor_init_old(){
    SPI_writeData(CC2500, REG_MCSM2,    0x00);
    SPI_writeData(CC2500, REG_MCSM0,    0x38);
    SPI_writeData(CC2500, REG_WOREVT1,  0xFF);
    SPI_writeData(CC2500, REG_WOREVT0,  0xFF);
    SPI_writeData(CC2500, REG_WORCTRL,  0x08);
}*/ //these are for 1.89s WOR cycle

void cc2500_nwor_init(){
    SPI_writeData(CC2500, REG_MCSM2,    VAL_MCSM2);
    SPI_writeData(CC2500, REG_MCSM0,    VAL_MCSM0);
    SPI_writeData(CC2500, REG_WOREVT1,  VAL_WOREVT1);
    SPI_writeData(CC2500, REG_WOREVT0,  VAL_WOREVT0);
    SPI_writeData(CC2500, REG_WORCTRL,  VAL_WORCTRL);
}

uint8_t cc2500_strobe1(uint8_t strobe){

    uint8_t statusByte;
    CS_OUT &= (~CS_BIT);                 // Set CS pin to low

    while (!(IFG2 & SPI_TX_I));          // wait for TX buf to be ready
    TX_BUF = strobe;                     // send strobe
    while (SPI_STAT  & UCBUSY);          // wait until busy gets low and strobe is send

    while(!(IFG2 & UCB0RXIFG));

    /* read the readio status byte returned by the command strobe */
    statusByte = RX_BUF;

    CS_OUT |= (CS_BIT);                 // Set CS pin high
    /* return the status byte */
    return(statusByte);


}

void pwd_mode_cc2500(void){

    cc2500_strobe(CC2500_IDLE);
    while(cc2500_strobe1(CC2500_NOP) & 0xF0);
    cc2500_strobe(CC2500_FRX);
    cc2500_strobe(CC2500_PWD);

}

void awake_cc2500(void){
    CS_OUT &= (~CS_BIT);                 // Set CS pin to low
    while(P1IN & BIT5);                  // wait for MISO to go high, indicating oscillator is stable
    CS_OUT |= (CS_BIT);                 // Set CS pin high

    SPI_writeData(CC2500, REG_TEST2,    VAL_TEST2);
    SPI_writeData(CC2500, REG_TEST1,    VAL_TEST1);
    SPI_writeData(CC2500, REG_TEST0,    VAL_TEST0);

    //enter idle mode before RX
    cc2500_strobe(CC2500_IDLE);
    while(cc2500_strobe1(CC2500_NOP) & 0xF0);

}
// Radio must be awake before RxOn is implemented
void Rx_on_cc2500(void){
    cc2500_strobe(CC2500_RX);
}

void Rx_idle_cc2500(void){
    cc2500_strobe(CC2500_IDLE);
    while(cc2500_strobe1(CC2500_NOP) & 0xF0);
    cc2500_strobe(CC2500_FRX);
}
