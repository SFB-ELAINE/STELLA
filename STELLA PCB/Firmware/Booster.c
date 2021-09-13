#include <msp430g2553.h>
#include "main.h"
#include "DigiPot.h"
#include "SPI.h"
#include "Booster.h"


void RestartBooster()

// Booster needs a restart to accept the new resistance value for adjusting the boost voltage

{

        PORT_EN_BOOSTER_DIR |= EN_BOOSTER;
        PORT_EN_BOOSTER_OUT |= EN_BOOSTER; // Shutdown Booster
        __delay_cycles(100);
        PORT_EN_BOOSTER_OUT &= ~EN_BOOSTER; // Start Booster
}





