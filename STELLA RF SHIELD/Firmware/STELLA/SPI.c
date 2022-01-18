#include <msp430g2553.h>
#include <string.h>
#include "main.h"
#include "SPI.h"
#include "DigiPot.h"

void SPISetup(void) {

    PORT_AD5144_DIR |= CS_AD5144;
    PORT_AD5144_OUT |= CS_AD5144; // deselect AD5144

    PORT_AD5142_DIR |= CS_AD5142;
    PORT_AD5142_OUT |= CS_AD5142; // deselect AD5142
    //added for cc2500
    P3DIR |= BIT3;
    P3OUT |= BIT3;              // set CS pin high for cc2500
    //for GDO0
    P2DIR &= ~BIT5;            // Set port 2 pin 5 as GDO0 input
    P2REN |= BIT5;            // P2.5 Connect to pull up/down resistor
    P2OUT &= ~BIT5;          // Set P2.5 to pull down resistor

    PORT_SPI_SEL0 = CLK | MISO | MOSI;
    PORT_SPI_SEL1 = CLK | MISO | MOSI;

    UCB0CTL1 |= 0x00;
    UCB0CTL0 |= 0x00;
    UCB0BR0 |= 0x00;
    UCB0BR1 |= 0x00;

    UCB0CTL1 = UCSWRST;
    UCB0CTL0 |= UCMSB + UCMST + UCSYNC;    // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2;                           // SMCLK
    UCB0BR0 |= 0x02;                                // Clock prescaler, /2
    UCB0BR1 = 0;                                    // High Byte for Clock /2
    UCB0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**

    __delay_cycles(10000);
}

void SPI_writeData(AD_Type device, unsigned char wiper_address, unsigned char value) {

    if (device == AD5144) {
        PORT_AD5144_OUT &= (~CS_AD5144);
    }
    else if (device == AD5142) {
        PORT_AD5142_OUT &= (~CS_AD5142);
    }
    else if (device == CC2500){
        CS_OUT &= (~CS_BIT);
    }

    while(!(IFG2 & UCB0TXIFG));                     // USCI_B0 TX buffer ready?
        UCB0TXBUF = wiper_address;

    while(!(IFG2 & UCB0TXIFG));                     // USCI_B0 TX buffer ready?
        UCB0TXBUF = value;

    //__delay_cycles(50); //testing Obaid
    while (UCB0STAT  & UCBUSY);                // wait until busy gets low and data is send //added for testing

    // Transfer data to register when CS returns to high
    if (device == AD5144) {
            PORT_AD5144_OUT |= CS_AD5144;
    }
    else if (device == AD5142) {
            PORT_AD5142_OUT |= CS_AD5142;
    }
    else if (device == CC2500){
        CS_OUT |= CS_BIT;
    }

}
