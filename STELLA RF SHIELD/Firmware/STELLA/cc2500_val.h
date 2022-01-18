/*
 * cc2500_val.h
 *  Munawar
 *  Created on: 21 Jan 2019
 *      Author: ej108
 */

#ifndef CC2500_VAL_H_
#define CC2500_VAL_H_


#define VAL_IOCFG2            0x29   // 41
#define VAL_IOCFG1            0x2E   // 46
#define VAL_IOCFG0            0x06   // 6
#define VAL_FIFOTHR           0x07
#define VAL_SYNC1             0xD3
#define VAL_SYNC0             0x91
#define VAL_PKTLEN            0xFF
#define VAL_PKTCTRL1          0x04
#define VAL_PKTCTRL0          0x05
#define VAL_ADDR              0x00
#define VAL_CHANNR            0x01   // 00
#define VAL_FSCTRL1           0x0A   //0x08
#define VAL_FSCTRL0           0x00
#define VAL_FREQ2             0x5D
#define VAL_FREQ1             0x96
#define VAL_FREQ0             0x27
#define VAL_MDMCFG4           0x2D   //0x8D   //0x86
#define VAL_MDMCFG3           0x3B   //0x83
#define VAL_MDMCFG2           0x73   //0x73   //0x03
#define VAL_MDMCFG1           0x23   //0x22
#define VAL_MDMCFG0           0x3B   //0xF8
#define VAL_DEVIATN           0x01   //0x44
#define VAL_MCSM2             0x07
#define VAL_MCSM1             0x30
#define VAL_MCSM0             0x18
#define VAL_FOCCFG            0x1D   //0x16
#define VAL_BSCFG             0x1C   //0x6C
#define VAL_AGCCTRL2          0xC7   //0x03
#define VAL_AGCCTRL1          0x00   //0x40
#define VAL_AGCCTRL0          0xB0   //0x91
#define VAL_WOREVT1           0x87
#define VAL_WOREVT0           0x6B
#define VAL_WORCTRL           0xF8
#define VAL_FREND1            0xB6   //0x56
#define VAL_FREND0            0x10
#define VAL_FSCAL3            0xEA   //0xA9
#define VAL_FSCAL2            0x0A
#define VAL_FSCAL1            0x00
#define VAL_FSCAL0            0x11
#define VAL_RCCTRL1           0x41
#define VAL_RCCTRL0           0x00
#define VAL_FSTEST            0x59
#define VAL_PTEST             0x7F
#define VAL_AGCTEST           0x3F
#define VAL_TEST2             0x88
#define VAL_TEST1             0x31
#define VAL_TEST0             0x0B

#define VAL_PARTNUM           0x80
#define VAL_VERSION           0x03
#define VAL_FREQEST           0x00
#define VAL_LQI               0x00
#define VAL_RSSI              0x00
#define VAL_MARCSTATE         0x00
#define VAL_WORTIME1          0x00
#define VAL_WORTIME0          0x00
#define VAL_PKTSTATUS         0x00
#define VAL_VCO_VC_DAC        0x00
#define VAL_TXBYTES           0x00
#define VAL_RXBYTES           0x00
#define VAL_RCCTRL1_STATUS    0x00
#define VAL_RCCTRL0_STATUS    0x00


#endif /* CC2500_VAL_H_ */
