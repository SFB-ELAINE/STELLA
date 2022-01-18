#include <msp430g2553.h>
#include "DigiPot.h"
#include "SPI.h"
#include "Booster.h"
#include "main.h"

void Set_DIGIPOT(Digipot_Function function, unsigned char value) {
    UCB0CTL0 &= ~(UCCKPH);

    if (function == VREF) SPI_writeData(AD5144, AD5144_WIPER4, value);

    else if (function == VBOOSTER) {
        SPI_writeData(AD5144, AD5144_WIPER3, value);
        // Restart the booster to apply the new settings
        RestartBooster();}

    else if (function == CURRENT_RIGHT) SPI_writeData(AD5142, AD5142_WIPER1, value);
    else if (function == CURRENT_LEFT) SPI_writeData(AD5142, AD5142_WIPER2, value);
    else if (function == COUNTERPULSE_ATTENUATION_RIGHT) SPI_writeData(AD5144, AD5144_WIPER1, value);
    else if (function == COUNTERPULSE_ATTENUATION_LEFT) SPI_writeData(AD5144, AD5144_WIPER2, value);

}



