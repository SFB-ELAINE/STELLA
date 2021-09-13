/*
 * DigiPot.h
 *
 *  Created on: Dec 17, 2018
 *      Author: Franz
 */

#ifndef DIGIPOT_H_
#define DIGIPOT_H_

#include "main.h"

// includes Control (write to RDAC) and Address Bits
#define AD5144_WIPER1 0b00010000
#define AD5144_WIPER2 0b00010001
#define AD5144_WIPER3 0b00010010
#define AD5144_WIPER4 0b00010011

#define AD5142_WIPER1 0b00010000
#define AD5142_WIPER2 0b00010001

#define _SHDN_OP1 BIT6  // Shutdown OP1
#define PORT_SHDN_OP1_OUT P3OUT
#define PORT_SHDN_OP1_DIR P3DIR

#define _SHDN_OP2 BIT7  // Shutdown OP2
#define PORT_SHDN_OP2_OUT P3OUT
#define PORT_SHDN_OP2_DIR P3DIR

void Set_DIGIPOT(Digipot_Function function, unsigned char value);

#endif /* DIGIPOT_H_ */

