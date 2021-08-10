class register():
    
    def __init__(self):
        None
    
    REG_IOCFG2   =   0x00        # GDO2 output pin configuration
    REG_IOCFG1   =   0x01        # GDO1 output pin configuration
    REG_IOCFG0   =   0x02        # GDO0 output pin configuration
    REG_FIFOTHR  =   0x03        # RX FIFO and TX FIFO thresholds
    REG_SYNC1    =   0x04        # Sync word, high byte
    REG_SYNC0    =   0x05        # Sync word, low byte
    REG_PKTLEN   =   0x06        # Packet length
    REG_PKTCTRL1 =   0x07        # Packet automation control
    REG_PKTCTRL0 =   0x08        # Packet automation control
    REG_ADDR     =   0x09        # Device address
    REG_CHANNR   =   0x0A        # Channel number
    REG_FSCTRL1  =   0x0B        # Frequency synthesizer control
    REG_FSCTRL0  =   0x0C        # Frequency synthesizer control
    REG_FREQ2    =   0x0D        # Frequency control word, high byte
    REG_FREQ1    =   0x0E        # Frequency control word, middle byte
    REG_FREQ0    =   0x0F        # Frequency control word, low byte
    REG_MDMCFG4  =   0x10        # Modem configuration
    REG_MDMCFG3  =   0x11        # Modem configuration
    REG_MDMCFG2  =   0x12        # Modem configuration
    REG_MDMCFG1  =   0x13        # Modem configuration
    REG_MDMCFG0  =   0x14        # Modem configuration
    REG_DEVIATN  =   0x15        # Modem deviation setting
    REG_MCSM2    =   0x16        # Main Radio Cntrl State Machine config
    REG_MCSM1    =   0x17        # Main Radio Cntrl State Machine config
    REG_MCSM0    =   0x18        # Main Radio Cntrl State Machine config
    REG_FOCCFG   =   0x19        # Frequency Offset Compensation config
    REG_BSCFG    =   0x1A        # Bit Synchronization configuration
    REG_AGCCTRL2 =   0x1B        # AGC control
    REG_AGCCTRL1 =   0x1C        # AGC control
    REG_AGCCTRL0 =   0x1D        # AGC control
    REG_WOREVT1  =   0x1E        # High byte Event 0 timeout
    REG_WOREVT0  =   0x1F        # Low byte Event 0 timeout
    REG_WORCTRL  =   0x20        # Wake On Radio control
    REG_FREND1   =   0x21        # Front end RX configuration
    REG_FREND0   =   0x22        # Front end TX configuration
    REG_FSCAL3   =   0x23        # Frequency synthesizer calibration
    REG_FSCAL2   =   0x24        # Frequency synthesizer calibration
    REG_FSCAL1   =   0x25        # Frequency synthesizer calibration
    REG_FSCAL0   =   0x26        # Frequency synthesizer calibration
    REG_RCCTRL1  =   0x27        # RC oscillator configuration
    REG_RCCTRL0  =   0x28        # RC oscillator configuration
    REG_FSTEST   =   0x29        # Frequency synthesizer cal control
    REG_PTEST    =   0x2A        # Production test
    REG_AGCTEST  =   0x2B        # AGC test
    REG_TEST2    =   0x2C        # Various test settings
    REG_TEST1    =   0x2D        # Various test settings
    REG_TEST0    =   0x2E        # Various test settings
    
# Strobe commands
    CC2500_RES       = 0x30        # Reset chip.
    CC2500_FSTXON    = 0x31        # Enable or calibrate freq synthesizer
    CC2500_XOFF      = 0x32        # Turn off crystal oscillator.
    CC2500_SCAL      = 0x33        # Calibrate freq synthesizer & disable
    CC2500_RX        = 0x34        # Enable RX.
    CC2500_TX        = 0x35        # Enable TX.
    CC2500_IDLE      = 0x36        # Exit RX or TX
    CC2500_AFC       = 0x37        # AFC adjustment of freq synthesizer
    CC2500_WOR       = 0x38        # Start automatic RX polling sequence
    CC2500_PWD       = 0x39        # Enter pwr down mode when CSn goes hi
    CC2500_FRX       = 0x3A        # Flush the RX FIFO buffer.
    CC2500_FTX       = 0x3B        # Flush the TX FIFO buffer.
    CC2500_WORRST    = 0x3C        # Reset real time clock.
    CC2500_NOP       = 0x3D        # No operation.

# Status registers
    REG_PARTNUM     = 0x30        # Part number
    REG_VERSION     = 0x31        # Current version number
    REG_FREQEST     = 0x32        # Frequency offset estimate
    REG_LQI         = 0x33        # Demodulator estimate for link quality
    REG_RSSI        = 0x34        # Received signal strength indication
    REG_FREQESTMARCSTATE   = 0x35        # Control state machine state
    REG_WORTIME1    = 0x36        # High byte of WOR timer
    REG_WORTIME0    = 0x37        # Low byte of WOR timer
    REG_PKTSTATUS   = 0x38        # Current GDOx status and packet status
    REG_VCO_VC_DAC  = 0x39        # Current setting from PLL cal module
    REG_TXBYTES     = 0x3A        # Underflow and # of bytes in TXFIFO
    REG_RXBYTES     = 0x3B        # Overflow and # of bytes in RXFIFO
    REG_NUM_RXBYTES = 0x7F        # Mask "# of bytes" field in _RXBYTES
    
# Other memory locations
    REG_PATABLE    = 0x3E
    REG_TXFIFO     = 0x3F
    REG_RXFIFO     = 0x3F
    
# Masks for appended status bytes
    REG_LQI_RX   =   0x01        # Position of LQI byte
    REG_CRC_OK   =   0x80        # Mask "CRC_OK" bit within LQI byte
    
# Definitions to support burst/single access:
    REG_WRITE_BURST = 0x40
    REG_READ_SINGLE = 0x80
    REG_READ_BURST  = 0xC0