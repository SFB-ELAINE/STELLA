/*
 * cc2500_reg.h
 *  Munawar
 *  Created on: 21 Jan 2019
 *      Author: ej108
 */

#ifndef CC2500_REG_H_
#define CC2500_REG_H_

/* Implant ID
 * It should be unique for each implant and its size is 2 bytes.
 * IMPLANT_ID_Hi is High bits
 * IMPLANT_ID_Lo is Lower bits */
#define IMPLANT_ID_Hi    0x00
#define IMPLANT_ID_Lo    0x01


// Configuration Registers
#define REG_IOCFG2       0x00         // GDO2 output pin configuration
#define REG_IOCFG1       0x01         // GDO1 output pin configuration
#define REG_IOCFG0       0x02         // GDO0 output pin configuration
#define REG_FIFOTHR      0x03         // RX FIFO and TX FIFO thresholds
#define REG_SYNC1        0x04         // Sync word, high byte
#define REG_SYNC0        0x05         // Sync word, low byte
#define REG_PKTLEN       0x06         // Packet length
#define REG_PKTCTRL1     0x07         // Packet automation control
#define REG_PKTCTRL0     0x08         // Packet automation control
#define REG_ADDR         0x09         // Device address
#define REG_CHANNR       0x0A         // Channel number
#define REG_FSCTRL1      0x0B         // Frequency synthesizer control
#define REG_FSCTRL0      0x0C         // Frequency synthesizer control
#define REG_FREQ2        0x0D         // Frequency control word, high byte
#define REG_FREQ1        0x0E         // Frequency control word, middle byte
#define REG_FREQ0        0x0F         // Frequency control word, low byte
#define REG_MDMCFG4      0x10         // Modem configuration
#define REG_MDMCFG3      0x11         // Modem configuration
#define REG_MDMCFG2      0x12         // Modem configuration
#define REG_MDMCFG1      0x13         // Modem configuration
#define REG_MDMCFG0      0x14         // Modem configuration
#define REG_DEVIATN      0x15         // Modem deviation setting
#define REG_MCSM2        0x16         // Main Radio Cntrl State Machine config
#define REG_MCSM1        0x17         // Main Radio Cntrl State Machine config
#define REG_MCSM0        0x18         // Main Radio Cntrl State Machine config
#define REG_FOCCFG       0x19         // Frequency Offset Compensation config
#define REG_BSCFG        0x1A         // Bit Synchronization configuration
#define REG_AGCCTRL2     0x1B         // AGC control
#define REG_AGCCTRL1     0x1C         // AGC control
#define REG_AGCCTRL0     0x1D         // AGC control
#define REG_WOREVT1      0x1E         // High byte Event 0 timeout
#define REG_WOREVT0      0x1F         // Low byte Event 0 timeout
#define REG_WORCTRL      0x20         // Wake On Radio control
#define REG_FREND1       0x21         // Front end RX configuration
#define REG_FREND0       0x22         // Front end TX configuration
#define REG_FSCAL3       0x23         // Frequency synthesizer calibration
#define REG_FSCAL2       0x24         // Frequency synthesizer calibration
#define REG_FSCAL1       0x25         // Frequency synthesizer calibration
#define REG_FSCAL0       0x26         // Frequency synthesizer calibration
#define REG_RCCTRL1      0x27         // RC oscillator configuration
#define REG_RCCTRL0      0x28         // RC oscillator configuration
#define REG_FSTEST       0x29         // Frequency synthesizer cal control
#define REG_PTEST        0x2A         // Production test
#define REG_AGCTEST      0x2B         // AGC test
#define REG_TEST2        0x2C         // Various test settings
#define REG_TEST1        0x2D         // Various test settings
#define REG_TEST0        0x2E         // Various test settings

// Strobe commands
#define CC2500_RES        0x30        // Reset chip.
#define CC2500_FSTXON     0x31        // Enable or calibrate freq synthesizer
#define CC2500_XOFF       0x32        // Turn off crystal oscillator.
#define CC2500_SCAL       0x33        // Calibrate freq synthesizer & disable
#define CC2500_RX         0x34        // Enable RX.
#define CC2500_TX         0x35        // Enable TX.
#define CC2500_IDLE       0x36        // Exit RX or TX
#define CC2500_AFC        0x37        // AFC adjustment of freq synthesizer
#define CC2500_WOR        0x38        // Start automatic RX polling sequence
#define CC2500_PWD        0x39        // Enter pwr down mode when CSn goes hi
#define CC2500_FRX        0x3A        // Flush the RX FIFO buffer.
#define CC2500_FTX        0x3B        // Flush the TX FIFO buffer.
#define CC2500_WORRST     0x3C        // Reset real time clock.
#define CC2500_NOP        0x3D        // No operation.

// Status registers
#define REG_PARTNUM           0x30        // Part number
#define REG_VERSION           0x31        // Current version number
#define REG_FREQEST           0x32        // Frequency offset estimate
#define REG_LQI               0x33        // Demodulator estimate for link quality
#define REG_RSSI              0x34        // Received signal strength indication
#define REG_FREQESTMARCSTATE  0x35        // Control state machine state
#define REG_WORTIME1          0x36        // High byte of WOR timer
#define REG_WORTIME0          0x37        // Low byte of WOR timer
#define REG_PKTSTATUS         0x38        // Current GDOx status and packet status
#define REG_VCO_VC_DAC        0x39        // Current setting from PLL cal module
#define REG_TXBYTES           0x3A        // Underflow and # of bytes in TXFIFO
#define REG_RXBYTES           0x3B        // Overflow and # of bytes in RXFIFO
#define REG_NUM_RXBYTES       0x7F        // Mask "# of bytes" field in _RXBYTES

// Other memory locations
#define REG_PATABLE      0x3E
#define REG_TXFIFO       0x3F
#define REG_RXFIFO       0x3F

// Masks for appended status bytes
#define REG_LQI_RX       0x01        // Position of LQI byte
#define REG_CRC_OK       0x80        // Mask "CRC_OK" bit within LQI byte

// Definitions to support burst/single access:
#define REG_WRITE_BURST  0x40
#define REG_READ_SINGLE  0x80
#define REG_READ_BURST   0xC0


#endif /* CC2500_REG_H_ */
