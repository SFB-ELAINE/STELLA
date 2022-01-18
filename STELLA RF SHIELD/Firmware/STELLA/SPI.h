#ifndef SPI_H_
#define SPI_H_

#include "SPI.h"
#include <string.h>
#include "main.h"

#define PORT_SPI_SEL0   P1SEL
#define PORT_SPI_SEL1   P1SEL2
#define MISO BIT6
#define MOSI BIT7
#define CLK BIT5

#define CS_AD5144 BIT0  // responsible for VRef, Boost Voltage, Counter Pulse Attenuation Adjustment (Left & Right)
#define PORT_AD5144_OUT P3OUT
#define PORT_AD5144_DIR P3DIR
#define CS_OUT      P3OUT
#define CS_BIT      BIT3 //responsible for CC2500 selection

#define CS_AD5142 BIT1  // responsible for Current Adjustment (Left & Right)
#define PORT_AD5142_OUT P3OUT
#define PORT_AD5142_DIR P3DIR


void SPISetup(void);
void SPI_writeData(AD_Type device, unsigned char wiper_address, unsigned char value);

#endif
