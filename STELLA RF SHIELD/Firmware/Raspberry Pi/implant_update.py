#!/usr/bin/python3
import spidev
import RPi.GPIO as GPIO
from time import sleep
from datetime import datetime #for timestamp
import numpy as np
import math as math
import cc2500_reg
import cc2500_val
from temp_offset import temp_off
import configparser
import pandas as pd
import os.path
import sys # for automatic termination of program

reg=cc2500_reg.register    # initializing object of class register, it contains all cc2500 registers address  
val=cc2500_val.value       # initializing object of class value, it contains all cc2500 configuration values  
GPIO.setmode(GPIO.BCM)     # set GPIO mode to BCM. https://pinout.xyz/#
GPIO.setup(17, GPIO.IN,pull_up_down=GPIO.PUD_UP) #define GPIO-BCM-17 as GDO0 input pin with pull down resistor
GPIO.setup(22, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) # RX interupt
#path = '/home/pi/Desktop/'
path = '/home/pi/Desktop/Stella_output/Magnet_data/'
#ID1 = '17'

class cc2500():
    x = 0
    def __init__(self):           # Constructor function, initializes SPI communication.
        spi = spidev.SpiDev(0,0)  # define spi port and CS pin. BCM 9 (MISO), BCM 10 (MOSI), BCM 11 (SCLK), BCM 8 (CE0)
        spi2 = spidev.SpiDev(0,1)  # define spi port and CS pin. BCM 9 (MISO), BCM 10 (MOSI), BCM 11 (SCLK), BCM 7 (CE1)
        spi.mode = 0              # Select SPI mode 0. https://en.wikipedia.org/wiki/Serial_Peripheral_Interface#Clock_polarity_and_phase
        spi2.mode = 0
        spi.max_speed_hz = 31250  # SCLK speed
        spi2.max_speed_hz = 31250
        self.spi = spi            # initialize spi to use in other functions.
        self.spi2 = spi2
        
    def cc2500_reset(self):       # function to reset CC2500. sends the strobe to cc2500.
        self.spi.xfer([reg.CC2500_RES])
        self.spi2.xfer([reg.CC2500_RES])
    
    def write_reg(self,address,value):   # function to write values in cc2500 registers. 8bit address and 8bit data.
        self.spi.xfer([address,value])
        
    def write_reg2(self,address,value):   # function to write values in cc2500 registers. 8bit address and 8bit data.
        self.spi2.xfer([address,value])
        
    def read_reg(self,address):                # function to read cc2500 register value. 
        y=self.spi.xfer([address | 0x80,0x00]) # to read MSB of address byte must be '1'.
        return y[1]

    def read_reg2(self,address):                # function to read cc2500 register value. 
        y=self.spi2.xfer([address | 0x80,0x00]) # to read MSB of address byte must be '1'.
        return y[1]
    
    def read_strobe2(self,address):
        y=self.spi2.xfer([address | 0xC0,0x00]) # to read status register, 2 MSB of address byte must be '1'.
        return y[1]
    
    def send_strobe(self, strobe):           # function to send strobe 
        result=self.spi.xfer([strobe])
        return result

    def send_strobe2(self, strobe):           # function to send strobe 
        result=self.spi2.xfer([strobe])
        return result

    def cc2500_init(self):                    # function to initialize cc2500 by setting values to corresponding registers         
        self.write_reg(reg.REG_IOCFG0,val.VAL_IOCFG0)
        self.write_reg(reg.REG_IOCFG2,val.VAL_IOCFG2)
        self.write_reg(reg.REG_IOCFG1,val.VAL_IOCFG1)
        
        self.write_reg(reg.REG_FIFOTHR,val.VAL_FIFOTHR)
        self.write_reg(reg.REG_SYNC1,val.VAL_SYNC1)
        self.write_reg(reg.REG_SYNC0,val.VAL_SYNC0)
        self.write_reg(reg.REG_PKTLEN,val.VAL_PKTLEN)
        self.write_reg(reg.REG_PKTCTRL1,val.VAL_PKTCTRL1)
        self.write_reg(reg.REG_PKTCTRL0,val.VAL_PKTCTRL0)
        self.write_reg(reg.REG_ADDR,val.VAL_ADDR)
        self.write_reg(reg.REG_CHANNR,val.VAL_CHANNR)
        self.write_reg(reg.REG_FSCTRL1,val.VAL_FSCTRL1)
        self.write_reg(reg.REG_FSCTRL0,val.VAL_FSCTRL0)
        self.write_reg(reg.REG_FREQ2,val.VAL_FREQ2)
        self.write_reg(reg.REG_FREQ1,val.VAL_FREQ1)
        self.write_reg(reg.REG_FREQ0,val.VAL_FREQ0)
        self.write_reg(reg.REG_MDMCFG4,val.VAL_MDMCFG4)
        self.write_reg(reg.REG_MDMCFG3,val.VAL_MDMCFG3)
        self.write_reg(reg.REG_MDMCFG2,val.VAL_MDMCFG2)
        self.write_reg(reg.REG_MDMCFG1,val.VAL_MDMCFG1)
        self.write_reg(reg.REG_MDMCFG0,val.VAL_MDMCFG0)
        self.write_reg(reg.REG_DEVIATN,val.VAL_DEVIATN)
        self.write_reg(reg.REG_MCSM2,val.VAL_MCSM2)
        self.write_reg(reg.REG_MCSM1,val.VAL_MCSM1)
        self.write_reg(reg.REG_MCSM0,val.VAL_MCSM0)
        self.write_reg(reg.REG_FOCCFG,val.VAL_FOCCFG)
        
        self.write_reg(reg.REG_BSCFG,val.VAL_BSCFG)
        self.write_reg(reg.REG_AGCCTRL2,val.VAL_AGCCTRL2)
        self.write_reg(reg.REG_AGCCTRL1,val.VAL_AGCCTRL1)
        self.write_reg(reg.REG_AGCCTRL0,val.VAL_AGCCTRL0)
        self.write_reg(reg.REG_WOREVT1,val.VAL_WOREVT1)
        self.write_reg(reg.REG_WOREVT0,val.VAL_WOREVT0)
        self.write_reg(reg.REG_WORCTRL,val.VAL_WORCTRL)
        self.write_reg(reg.REG_FREND1,val.VAL_FREND1)
        self.write_reg(reg.REG_FREND0,val.VAL_FREND0)
        self.write_reg(reg.REG_FSCAL3,val.VAL_FSCAL3)
        self.write_reg(reg.REG_FSCAL2,val.VAL_FSCAL2)
        self.write_reg(reg.REG_FSCAL1,val.VAL_FSCAL1)
        self.write_reg(reg.REG_FSCAL0,val.VAL_FSCAL0)
        self.write_reg(reg.REG_RCCTRL1,val.VAL_RCCTRL1)
        self.write_reg(reg.REG_RCCTRL0,val.VAL_RCCTRL0)
        self.write_reg(reg.REG_FSTEST,val.VAL_FSTEST)
        self.write_reg(reg.REG_PTEST,val.VAL_PTEST)
        self.write_reg(reg.REG_AGCTEST,val.VAL_AGCTEST)
        self.write_reg(reg.REG_TEST2,val.VAL_TEST2)
        self.write_reg(reg.REG_TEST1,val.VAL_TEST1)
        self.write_reg(reg.REG_TEST0,val.VAL_TEST0)
        
        #self.write_reg(reg.REG_PATABLE,0xC6)
        self.write_reg(reg.REG_PATABLE,0xFF) #For Max transmission power
        
    def cc2500_init2(self):                    # function to initialize cc2500 by setting values to corresponding registers         
        self.write_reg2(reg.REG_IOCFG0,val.VAL_IOCFG0)
        self.write_reg2(reg.REG_IOCFG2,val.VAL_IOCFG2)
        self.write_reg2(reg.REG_IOCFG1,val.VAL_IOCFG1)
        
        self.write_reg2(reg.REG_FIFOTHR,val.VAL_FIFOTHR)
        self.write_reg2(reg.REG_SYNC1,val.VAL_SYNC1)
        self.write_reg2(reg.REG_SYNC0,val.VAL_SYNC0)
        self.write_reg2(reg.REG_PKTLEN,val.VAL_PKTLEN)
        self.write_reg2(reg.REG_PKTCTRL1,val.VAL_PKTCTRL1)
        self.write_reg2(reg.REG_PKTCTRL0,val.VAL_PKTCTRL0)
        self.write_reg2(reg.REG_ADDR,val.VAL_ADDR)
        self.write_reg2(reg.REG_CHANNR,0x00)
        self.write_reg2(reg.REG_FSCTRL1,val.VAL_FSCTRL1)
        self.write_reg2(reg.REG_FSCTRL0,val.VAL_FSCTRL0)
        self.write_reg2(reg.REG_FREQ2,val.VAL_FREQ2)
        self.write_reg2(reg.REG_FREQ1,val.VAL_FREQ1)
        self.write_reg2(reg.REG_FREQ0,val.VAL_FREQ0)
        self.write_reg2(reg.REG_MDMCFG4,val.VAL_MDMCFG4)
        self.write_reg2(reg.REG_MDMCFG3,val.VAL_MDMCFG3)
        self.write_reg2(reg.REG_MDMCFG2,val.VAL_MDMCFG2)
        self.write_reg2(reg.REG_MDMCFG1,val.VAL_MDMCFG1)
        self.write_reg2(reg.REG_MDMCFG0,val.VAL_MDMCFG0)
        self.write_reg2(reg.REG_DEVIATN,val.VAL_DEVIATN)
        self.write_reg2(reg.REG_MCSM2,val.VAL_MCSM2)
        self.write_reg2(reg.REG_MCSM1,val.VAL_MCSM1)
        self.write_reg2(reg.REG_MCSM0,val.VAL_MCSM0)
        self.write_reg2(reg.REG_FOCCFG,val.VAL_FOCCFG)
        
        self.write_reg2(reg.REG_BSCFG,val.VAL_BSCFG)
        self.write_reg2(reg.REG_AGCCTRL2,val.VAL_AGCCTRL2)
        self.write_reg2(reg.REG_AGCCTRL1,val.VAL_AGCCTRL1)
        self.write_reg2(reg.REG_AGCCTRL0,val.VAL_AGCCTRL0)
        self.write_reg2(reg.REG_WOREVT1,val.VAL_WOREVT1)
        self.write_reg2(reg.REG_WOREVT0,val.VAL_WOREVT0)
        self.write_reg2(reg.REG_WORCTRL,val.VAL_WORCTRL)
        self.write_reg2(reg.REG_FREND1,val.VAL_FREND1)
        self.write_reg2(reg.REG_FREND0,val.VAL_FREND0)
        self.write_reg2(reg.REG_FSCAL3,val.VAL_FSCAL3)
        self.write_reg2(reg.REG_FSCAL2,val.VAL_FSCAL2)
        self.write_reg2(reg.REG_FSCAL1,val.VAL_FSCAL1)
        self.write_reg2(reg.REG_FSCAL0,val.VAL_FSCAL0)
        self.write_reg2(reg.REG_RCCTRL1,val.VAL_RCCTRL1)
        self.write_reg2(reg.REG_RCCTRL0,val.VAL_RCCTRL0)
        self.write_reg2(reg.REG_FSTEST,val.VAL_FSTEST)
        self.write_reg2(reg.REG_PTEST,val.VAL_PTEST)
        self.write_reg2(reg.REG_AGCTEST,val.VAL_AGCTEST)
        self.write_reg2(reg.REG_TEST2,val.VAL_TEST2)
        self.write_reg2(reg.REG_TEST1,val.VAL_TEST1)
        self.write_reg2(reg.REG_TEST0,val.VAL_TEST0)
        
        #self.write_reg2(reg.REG_PATABLE,0xC6)
        self.write_reg2(reg.REG_PATABLE,0xFF) #For Max transmission power
        
    def read_config_regs(self):                  # function to read 3 configuration register. if values are same as set value then cc2500 is working fine
        print(hex(self.read_reg(reg.REG_IOCFG2)))
        print(hex(self.read_reg(reg.REG_IOCFG1)))
        print(self.read_reg(reg.REG_IOCFG0))
        
    def read_config_regs2(self):                  # function to read 3 configuration register. if values are same as set value then cc2500 is working fine
        print(hex(self.read_reg2(reg.REG_IOCFG2)))
        print(hex(self.read_reg2(reg.REG_IOCFG1)))
        print(self.read_reg2(reg.REG_IOCFG0))
        
    def log(self, *params):
        with open("/home/pi/Desktop/stats_receive.txt", "a") as log:
            for param in params:
                log.write(str(param)+' ')
            log.write('\n')
            
    def read_config_file(self):
        global Freq_read, PW_read, Stim_read
        config = configparser.ConfigParser()
        config.read('parameters.ini')
        Freq_read = int(config['parameter']['Frequency'])
        PW_read = int(config['parameter']['Pulse-Width'])
        Stim_read = int(config['parameter']['Stimulation'])
        
    def write_config(self, Freq_write, PW_write, Stim_write, ID_Rec):
        config = configparser.ConfigParser()
        config['parameter'] = {}
        Freq_write = str(Freq_write)
        PW_write = str(PW_write)
        Stim_write = str(Stim_write)
        config['parameter']['Frequency'] = Freq_write
        config['parameter']['Pulse-Width'] = PW_write
        config['parameter']['Stimulation'] = Stim_write
        id1 = ID_Rec
        filename = id1 + '.ini'
        with open (filename, 'w') as configfile:
            config.write(configfile)
            
    def compare_params(self, Freq_rec, PW_rec):
        global Freq_validity, PW_validity
        Freq_validity = '-'
        PW_validity = '-'
        if((Freq_rec != 0) and (PW_rec != 0)):
            if((Freq_rec <= (Freq_read + 20)) or (Freq_rec >= (Freq_read - 20))):
                Freq_validity = "Valid"
            else:
                Freq_validity = "Not Valid"
                
            if((PW_rec >= (PW_read/2))):
                PW_validity = "Valid"
            else:
                PW_validity = "Not Valid"
    
    def column(self, ID1):
        columns1=['Time', 'Observer', 'ID', 'Stimulation', 'Temperature (°C)', 'Impedance (Left Lateral)', 'Impedance (Right Lateral)', 'Max. DBS Voltage (Left Lateral)', 'Max. DBS Voltage (Right Lateral)', 'Boost Voltage (V)', 'Battery Voltage (V)', 'RSSI at Implant (dBm)', 'RSSI at Base-station (dBm)','LCSF','LCPW', 'DBS1', 'DBS2', 'DBS3', 'DBS4', 'DBS5', 'DBS6']
        #columns1=['Time', 'Temperature (°C)', 'RSSI at Implant (dBm)', 'RSSI at Base-station (dBm)']
        #df = pd.DataFrame([columns])
        filename = ID1 + '_magnet.csv'
        a = os.path.exists(path + filename)
        if(a == False):
            df = pd.DataFrame([columns1])
            #filename = ID1 + '.csv'
            df.to_csv(path + filename, mode='w', index=False, header=False)
        
    def newcolum(self, tstamp, Name, ID1, Stim, temp, imps_left, imps_right, vref_left, vref_right, BoostVal, BatVal, RSSI_dbm, RSSI_receive_dbm, LCSF, LCPW, dbs_0, dbs_1, dbs_2, dbs_3, dbs_4, dbs_5):
        #imps[data[2]], imps[data[3]],round(BoostVal,2), round(BatVal,2), 
        #df = pd.DataFrame([[tstamp, temp, imps[data[2]], imps[data[3]], round(BoostVal,2), round(BatVal,2), RSSI_dbm, RSSI_receive_dbm]])
        if(Stim == 0):
            Stim1 = 'Off'
        else:
            Stim1 = 'On'
        filename = ID1 + '_magnet.csv'
        df = pd.DataFrame([[tstamp, Name, ID1, Stim1, temp, imps_left, imps_right, vref_left, vref_right, BoostVal, BatVal, RSSI_dbm, RSSI_receive_dbm, LCSF, LCPW, dbs_0, dbs_1, dbs_2, dbs_3, dbs_4, dbs_5]])
        #df = pd.DataFrame([[tstamp, temp, RSSI_dbm, RSSI_receive_dbm]])
        df.to_csv(path + filename, mode='a', index=False, header=False)
        
    def dbs_sample_check(self, dbs):
        for i in range(0, len(dbs)):
            if(dbs[i] > 100):
                dbs[i] = 0
        return dbs

    def rx_interrupt(self, channel):
        self.x = 1
        address = 50
        PacketLength = 0
        # create time stamp
        now = (datetime.now())
        tstamp = "{0:%Y}-{0:%m}-{0:%d}_{0:%H}:{0:%M}:{0:%S}".format(now)
        #ID1 = ID_Rec
        #self.column()

        bat = 0x0000
        Vbo = 0x0000
        Vbat =0x0000
        imps = ["Normal", "Low", "High","--"]
        config = configparser.ConfigParser() #added for stimulation logging
        config.read('parameters.ini') #added for stimulation logging
        print('RX_Mode')
        print ("Time is: ",tstamp) #printing time
        self.log(tstamp) # logging time
        print('Getting Address')          
        data = []
        i_address = self.read_reg2(reg.REG_RXFIFO)
        print('Got Address',i_address)
        if(i_address == address):
            PacketLength = self.read_reg2(reg.REG_RXFIFO)
            print("-----------------------------------------")
            print("Address",hex(i_address))
            print(str(PacketLength) + " Packet received")
            for i in range(0, PacketLength):    
                data.append(self.read_reg2(reg.REG_RXFIFO))
            #Read, calculate and print receive RSSI
            RSSI_receive_raw = self.read_strobe2(reg.REG_RSSI) #new addition 
            if (RSSI_receive_raw >= 128):
                RSSI_receive_dbm = (((RSSI_receive_raw - 256)/2)-72)
            else:
                RSSI_receive_dbm = ((RSSI_receive_raw/2)-72)
            #print("Receive signal RSSI in dBm: ", RSSI_receive_dbm) #printed at end
            #print(data)
            #for i in range(0, len(data)):    
                #print((data[i]))
            print(data) #just printing data for debugging
        #print(data)
            print("-----------------------------------------")
            ID_Rec = str(data[11])
            ID1 = ID_Rec
            print("Implant ID:" , ID1)
            Stim = int(config['parameter']['Stimulation']) #added for stimulation logging
            if(Stim == 0):
                Stim1 = 'Off'
            else:
                Stim1 = 'On'
            print("Stimulation is:" , Stim1)
            temp = 0x0000 | data[0] << 8 | data[1]
            temp = format(temp * 0.0014663/0.00355-277.75, '.0f')
            #temp = round(temp,1)
            print("Temperature:", temp, u'\N{DEGREE SIGN}'+'C')
            self.log("Temperature: ", temp, u'\N{DEGREE SIGN}'+'C') #logging data
            BatVal = 0x03FF &(bat| data[4] << 8 | data[5])
            #print(0x03FF &(bat| data[4] << 8 | data[5]) )
            #print ("Battery Voltage MemReg:" , BatVal)
            print("Impedance (Left Lateral): ", imps[data[2]])
            print("Impedance (Right Lateral): ", imps[data[3]])
            self.log("Impedance (Left Lateral): ", imps[data[2]]) # logging Imp left
            self.log("Impedance (Right Lateral): ", imps[data[3]]) # logging Imp right
            vref_left = data[9]
            vref_left = ((3.65/256)*vref_left)
            vref_left = round(vref_left,2)
            if(vref_left == 0):
                print("Max. DBS Voltage (Left Lateral): ", "--")
                vref_left = '--'
                self.log("Max. DBS Voltage (Left Lateral): ", vref_left) # logging vref_left
            else:
                print("Max. DBS Voltage (Left Lateral): ", round(vref_left,2), "V")                    
            #print("Vref Left Lateral: ", round(vref_left,2), "V")
                self.log("Max. DBS Voltage (Left Lateral): ", vref_left, 'V') # logging vref_left
            vref_right = data[10]
            vref_right = ((3.65/256)*vref_right)
            vref_right = round(vref_right,2)
            if(vref_right == 0):
                print("Max. DBS Voltage (Right Lateral): ", "--")
                vref_right = '--'
                self.log("Max. DBS Voltage (Right Lateral): ", vref_right) # logging vref_right
            else:
                print("Max. DBS Voltage (Right Lateral): ", round(vref_right,2), "V")
                self.log("Max. DBS Voltage (Right Lateral): ", vref_right, 'V') # logging vref_right
            #print("Vref Right Lateral: ", round(vref_right,2), "V")
            #BoostVal = 0x0000 | data[6] << 8 | data[7] # old implementation
            BoostVal = 0x03FF &(Vbo| data[6] << 8 | data[7]) # new implementation
            #print("VBoost Value: ", BoostVal)
            BoostVal = (4.88 * BoostVal)/1000
            print("Boost Voltage: ", round(BoostVal,2),"V")
            self.log("Boost Voltage: ", round(BoostVal,2),"V") # logging VBoost Volatge
            BatVal = BoostVal * BatVal /1024
            print("Battery Voltage: ", round(BatVal,2),"V")
            self.log("Battery Voltage: ", round(BatVal,2),"V") # logging Battery Voltage
            RSSI_RAW = data[8]
            if (RSSI_RAW >= 128):
                RSSI_dbm = (((RSSI_RAW - 256)/2)-71)
            else:
                RSSI_dbm = ((RSSI_RAW/2)-71)
            
            #print("Raw RSSI Value: ", data[8])
            if (RSSI_dbm <= -95):
                print("RSSI at Implant: ", RSSI_dbm, "dBm")
                print("Warning: Move closer  to the implant")
                self.log("RSSI at Implant: ", RSSI_dbm, "dBm") # logging RSSI
            else:
                print("RSSI at Implant: ", RSSI_dbm, "dBm")
                self.log("RSSI at Implant: ", RSSI_dbm, "dBm") # logging RSSI
            #warning for weak receive signal
            if(RSSI_receive_dbm<= -95):
                #print("Receive signal RSSI in dBm: ", RSSI_receive_dbm)
                print("Warning: Move closer to the implant or increase transmit power")
            #else:
                #print("Receive signal RSSI in dBm: ", RSSI_receive_dbm)
            print("RSSI at Base-station: ", RSSI_receive_dbm, "dBm")
            self.log("RSSI at Base-station: ", RSSI_receive_dbm, "dBm") # logging RSSI
            ID_Rec = str(data[11])
            ID1 = ID_Rec
            self.column(ID_Rec)
            self.log("Implant ID: " , ID1)
            rec_hh = str(data[12])
            rec_mm = str(data[13])
            rec_time = (rec_hh + ":" + rec_mm)
            #print("Implant Time", rec_time)
            Freq_Left = 0x0000 | data[14] << 8 | data[15]
            Freq_Left1 = Freq_Left
            if(Freq_Left != 0):
                Freq_Left = ((1/Freq_Left)*1000000)
            Freq_Left = round(Freq_Left,0)
            Freq_Right = 0x0000 | data[16] << 8 | data[17]
            Freq_Right1 = Freq_Right
            if(Freq_Right != 0):
                Freq_Right = ((1/Freq_Right)*1000000)
            Freq_Right = round(Freq_Right,0)
            #print("Left Channel Stimulation Frequency is", Freq_Left)
            #print("Right Channel Stimulation Frequency is", Freq_Right)
            PW_Left = 0x0000 | data[18] << 8 | data[19]
            PW_Right = 0x0000 | data[20] << 8 | data[21]
            if(PW_Left != 0):
                PW_Left = Freq_Left1 - PW_Left
            #print("Left Channel measured pulse width is",PW_Left, "usec")
            if(PW_Right != 0):
                PW_Right = Freq_Right1 - PW_Right
            #print("Right Channel measured pulse width is",PW_Right, "usec")
            #Stim = int(config['parameter']['Stimulation']) #added for stimulation logging
            Name = str(config['parameter']['UserName'])
            #printing DBS sampled packets
            dbs = []
            dbs_1 = 0x0000 | data[22] << 8 | data[23]
            #dbs_1 = dbs_1 - Freq_Left1 #for ascending sampling
            dbs_1 = Freq_Left1 - dbs_1 #for decending sampling
            dbs_2 = 0x0000 | data[24] << 8 | data[25]
            #dbs_2 = dbs_2 - Freq_Left1 #for ascending sampling
            dbs_2 = Freq_Left1 - dbs_2 #for decending sampling
            dbs_3 = 0x0000 | data[26] << 8 | data[27]
            #dbs_3 = dbs_3 - Freq_Left1 #for ascending sampling
            dbs_3 = Freq_Left1 - dbs_3
            dbs_4 = 0x0000 | data[28] << 8 | data[29]
            #dbs_4 = dbs_4 - Freq_Left1 #for ascending sampling
            dbs_4 = Freq_Left1 - dbs_4
            dbs_5 = 0x0000 | data[30] << 8 | data[31]
            #dbs_5 = dbs_5 - Freq_Left1 #for ascending sampling
            dbs_5 = Freq_Left1 - dbs_5
            dbs_6 = 0x0000 | data[32] << 8 | data[33]
            #dbs_6 = dbs_6 - Freq_Left1 #for ascending sampling
            dbs_6 = Freq_Left1 - dbs_6
            dbs.append(dbs_1)
            dbs.append(dbs_2)
            dbs.append(dbs_3)
            dbs.append(dbs_4)
            dbs.append(dbs_5)
            dbs.append(dbs_6)
            print("DBS Sampled values", dbs)
            dbs = self.dbs_sample_check(dbs)
            #print("DBS Sample testing", data[22], data[23], data[24], data[25], data[26], data[27], data[28])
            print("DBS Sampled values", dbs)
            #print('DBS 1: ', dbs[0])
            self.read_config_file()
            self.write_config(Freq_read, PW_read, Stim_read, ID_Rec)
            self.compare_params(Freq_Left, PW_Left)
            print("Left Channel Stimulation Frequency is" , Freq_validity)
            print("Left Channel Pulse Width is",PW_validity)
            newclass = temp_off(temp,ID1) #temp offset
            temp = newclass.temp_offset1() #temp offset
            self.newcolum(tstamp, Name, ID_Rec, Stim, temp, imps[data[2]], imps[data[3]], vref_left, vref_right, round(BoostVal,2), round(BatVal,2), RSSI_dbm, RSSI_receive_dbm, Freq_validity, PW_validity, dbs[0], dbs[1], dbs[2], dbs[3], dbs[4], dbs[5])
            #exit()
            print(Freq_read)
            quit()
            #raise SystemExit(0)
        else:
            print("wrong address")
            self.log("wrong address") # logging wrong address
        self.log(" ") # adding extra line
        self.send_strobe2(reg.CC2500_IDLE)
        self.send_strobe2(reg.CC2500_FRX)
        self.send_strobe2(reg.CC2500_IDLE)
        self.send_strobe2(reg.CC2500_RX) 


# _______________________________________________
# |1 byte address|1 byte data length| data bytes|
# -----------------------------------------------
        
    def tx_main(self,address, data): # function to transmit data to receiver(cc2500)
        
        length = len(data)     # get data length
        packet = list(data)    # copy data to packet
        packet.insert(0,length) # insert length byte into packet      
        packet.insert(0,address)  # insert address byte into packet
       # print(packet)
        self.send_strobe(reg.CC2500_IDLE)  # go to IDLE state. check CC2500 state machine diagram
        self.send_strobe(reg.CC2500_FTX)   # flush transmitter buffer
        self.send_strobe(reg.CC2500_IDLE)  # go to IDLE state
        print("Transmitting...")
        for i in range (0,len(packet)):    # copy data byte by byte into cc2500 TX buffer. First byte address, 2nd byte data length, then data bytes
            self.write_reg(reg.REG_TXFIFO,packet[i]) # copying into cc2500 tx buffer
            
        self.send_strobe(reg.CC2500_TX) # start transmission
        sleep(0.001)
    


def main():
    c1 = 0
    c2 = 0
    typ = 0x00             # bilateral or unilateral
    lat = 0x00             # left lateral or right lateral
    att = 20               # counter attenuation pulse
    a=cc2500()             # create object of class cc2500
    test_var = 0 #only for testing while loop
    print("Starting..")
    a.cc2500_reset()       # reset cc2500
    a.cc2500_init()        # initialize cc2500
    a.cc2500_init2()        # initialize cc2500
    a.read_config_regs()   # check cc2500
    a.read_config_regs2()   # check cc2500
    a.send_strobe2(reg.CC2500_RX)
    GPIO.add_event_detect(22, GPIO.FALLING, callback=a.rx_interrupt)
    
    S_G = 1 # Always SET mode
    Mode = 1   # Always Deep Sleep Mode
    clk = 32768 # cycle calculation value for Deep Sleep Mode
    
    while(1):
        sleep(0.25)
        input("Press Enter to Continue:")
        ############# EXTRACTING PARAMETERS FROM conf.txt FILE #######
        config = configparser.ConfigParser()
        config.read('parameters.ini')        
        #ID = int(config['parameter']['Address'])
        ID = 50
        frequency = int(config['parameter']['Frequency'])
        DC = int(config['parameter']['Pulse-Width'])
        att = int(config['parameter']['Impedance'])
        cur = int(config['parameter']['Current'])
        typ = int(config['parameter']['Lateral'])
        lat = int(config['parameter']['Channel'])
        Stim = int(config['parameter']['Stimulation'])
        TxnPower = int(config['parameter']['TransmissionPower'])
        TxnCycle_min = int(config['parameter']['Cycle_Time_min'])
        TxnCycle_hr = int(config['parameter']['Cycle_Time_hr'])
        print(TxnPower)
        ##############################################################
        now = (datetime.now())
        tstamp_h = "{0:%H}".format(now)
        tstamp_m = "{0:%M}".format(now)
        tstamp_h1 = int(tstamp_h)
        tstamp_m1 = int(tstamp_m)
        ##############################################################
        TP = 1/frequency  # calculating time period
        TP = TP*clk  # converting it into micro seconds        
        TP = round(TP, 0) # rounding the value to make it interger
        TP0 = int(TP) & 0xFF    # storing byte 0 in TP0
        TP1 = int(TP)>>8 & 0xFF # storing byte 2 in TP1
        PW = DC*32768/1000000
        print(PW)
        PW= TP-PW
        print(PW)
        PW = (round(PW,0) + 1)# rounding the value to make it integer
        PW0 = int(PW) & 0xFF   # storing byte 0 in PW0
        PW1 = int(PW)>>8 & 0xFF # storing byte 1 in PW1
        #############################################################
        TxnCycle = ((TxnCycle_hr * 3600) + (TxnCycle_min * 60))
        TxnCycle = TxnCycle/2
        TxnCycle = int(round(TxnCycle, 0))
        TxnCycle0 = int(TxnCycle) & 0xFF
        TxnCycle1 = int(TxnCycle)>>8 & 0xFF
        print(TxnCycle)
        if (TxnCycle == 0):
            print("Error: Re-check Transmission cycle time. Enter valid time greater than 0 and less than 24 hours")
            quit()
        data = (TP0,TP1,PW0,PW1,Mode,Stim,S_G,typ,lat,att,cur,TxnPower,tstamp_h1,tstamp_m1,TxnCycle0,TxnCycle1) # creating tuple of data     
        num2 = 0
        a.x = 0
        for i in range(0,100000):
            if (a.x == 1):
                break
            else:
                print(num2)
                a.tx_main(ID,data) # transmitting data with device ID
            num2 = num2 + 1
        print("transmission is over")   
        print("---------------------")

main()



