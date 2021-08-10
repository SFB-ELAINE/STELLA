#!/usr/bin/python3
import spidev
import RPi.GPIO as GPIO
from time import sleep
from datetime import datetime #for timestamp
import numpy as np
import math as math
import cc2500_reg
import cc2500_val
import temp_offset #temp offset file
from temp_offset import temp_off
import configparser
import pandas as pd
import os
import os.path
import sys # for automatic termination of program
from smbus import SMBus # library for i2c, temperature sensor reading

reg=cc2500_reg.register    # initializing object of class register, it contains all cc2500 registers address  
val=cc2500_val.value       # initializing object of class value, it contains all cc2500 configuration values
temp_compen = temp_offset.temp_off
GPIO.setmode(GPIO.BCM)     # set GPIO mode to BCM. https://pinout.xyz/#
GPIO.setup(17, GPIO.IN) #define GPIO-BCM-17 as GDO0 input pin with pull down resistor
#GPIO.setup(22, GPIO.IN,pull_up_down=GPIO.PUD_DOWN) # RX interupt
#path = '/home/pi/Desktop/'
path = '/home/pi/Desktop/Stella_output/Continuous_data_output/'
#ID1 = '17'

class cc2500():
    x = 0
    def __init__(self):           # Constructor function, initializes SPI communication.
        spi = spidev.SpiDev(0,0)  # define spi port and CS pin. BCM 9 (MISO), BCM 10 (MOSI), BCM 11 (SCLK), BCM 8 (CE0)
        #spi2 = spidev.SpiDev(0,1)  # define spi port and CS pin. BCM 9 (MISO), BCM 10 (MOSI), BCM 11 (SCLK), BCM 7 (CE1)
        spi.mode = 0              # Select SPI mode 0. https://en.wikipedia.org/wiki/Serial_Peripheral_Interface#Clock_polarity_and_phase
        #spi2.mode = 0
        spi.max_speed_hz = 31250  # SCLK speed
        #spi2.max_speed_hz = 31250
        self.spi = spi            # initialize spi to use in other functions.
        #self.spi2 = spi2
        
    def cc2500_reset(self):       # function to reset CC2500. sends the strobe to cc2500.
        self.spi.xfer([reg.CC2500_RES])
        #self.spi2.xfer([reg.CC2500_RES])
    
    def write_reg(self,address,value):   # function to write values in cc2500 registers. 8bit address and 8bit data.
        self.spi.xfer([address,value])
        
    #def write_reg2(self,address,value):   # function to write values in cc2500 registers. 8bit address and 8bit data.
        #self.spi2.xfer([address,value])
        
    def read_reg(self,address):                # function to read cc2500 register value. 
        y=self.spi.xfer([address | 0x80,0x00]) # to read MSB of address byte must be '1'.
        return y[1]

    #def read_reg2(self,address):                # function to read cc2500 register value. 
        #y=self.spi2.xfer([address | 0x80,0x00]) # to read MSB of address byte must be '1'.
        #return y[1]
    
    def read_strobe(self,address):
        y=self.spi.xfer([address | 0xC0,0x00]) # to read status register, 2 MSB of address byte must be '1'.
        return y[1]
    
    def send_strobe(self, strobe):           # function to send strobe 
        result=self.spi.xfer([strobe])
        return result

    #def send_strobe2(self, strobe):           # function to send strobe 
        #result=self.spi2.xfer([strobe])
        #return result

    def cc2500_init(self):                    # function to initialize cc2500 by setting values to corresponding registers         
        self.write_reg(reg.REG_IOCFG0,val.VAL_IOCFG0)
        self.write_reg(reg.REG_IOCFG2,val.VAL_IOCFG2)
        self.write_reg(reg.REG_IOCFG1,val.VAL_IOCFG1)
        
        self.write_reg(reg.REG_FIFOTHR,val.VAL_FIFOTHR)
        self.write_reg(reg.REG_SYNC1,val.VAL_SYNC1)
        self.write_reg(reg.REG_SYNC0,val.VAL_SYNC0)
        self.write_reg(reg.REG_PKTLEN,0x3D) #val.VAL_PKTLEN
        self.write_reg(reg.REG_PKTCTRL1,val.VAL_PKTCTRL1)
        self.write_reg(reg.REG_PKTCTRL0,val.VAL_PKTCTRL0)
        self.write_reg(reg.REG_ADDR,val.VAL_ADDR)
        self.write_reg(reg.REG_CHANNR,0x00) #val.VAL_CHANNR
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
        self.write_reg2(reg.REG_PKTLEN,0x3D)
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
        
    #def read_config_regs2(self):                  # function to read 3 configuration register. if values are same as set value then cc2500 is working fine
        #print(hex(self.read_reg2(reg.REG_IOCFG2)))
        #print(hex(self.read_reg2(reg.REG_IOCFG1)))
        #print(self.read_reg2(reg.REG_IOCFG0))
        

    def rx_interrupt(self, channel):
        self.x = 1 #self.x = self.x + 1 # previously x=1
        
        print('RX_Mode')
        
    def temp_reading(self):
        bus = SMBus(1)
        b = bus.read_i2c_block_data(0x48, 0, 2)
        #print(b)
        b_new = b[0]<<8 | b[1]
        #print(b_new)
        temperature = format(b_new * 0.0078125, '.3f')
        #print("The temperature is " , temperature)
        bus.close()
        return temperature
        
    def column(self, ID1):
        columns1=['Time', 'ID', 'Stimulation', 'Temperature (°C)','Sensor Temp (°C)', 'Impedance (Left Lateral)', 'Impedance (Right Lateral)', 'Max. DBS Voltage (Left Lateral)', 'Max. DBS Voltage (Right Lateral)', 'Boost Voltage (V)', 'Battery Voltage (V)', 'RSSI at Base-station (dBm)']
        #columns1=['Time', 'Temperature (°C)', 'RSSI at Implant (dBm)', 'RSSI at Base-station (dBm)']
        #df = pd.DataFrame([columns])
        filename = ID1 + '_continuous.csv'
        a = os.path.exists(path + filename)
        if(a == False):
            df = pd.DataFrame([columns1])
            #filename = ID1 + '.csv'
            df.to_csv(path + filename, mode='w', index=False, header=False)
            
    def newcolum(self, tstamp, ID1, Stim, temp,temperature, imps_left, imps_right, vref_left, vref_right, BoostVal, BatVal, RSSI_receive_dbm): 
        #df = pd.DataFrame([[tstamp, temp, imps[data[2]], imps[data[3]], round(BoostVal,2), round(BatVal,2), RSSI_dbm, RSSI_receive_dbm]])
        if(Stim == 0):
            Stim1 = 'Off'
        else:
            Stim1 = 'On'
        filename = ID1 + '_continuous.csv'
        #outfile = open(path + filename, 'a') #including for manual opening
        df = pd.DataFrame([[tstamp, ID1, Stim1, temp,temperature, imps_left, imps_right, vref_left, vref_right, BoostVal, BatVal, RSSI_receive_dbm]])
        #df = pd.DataFrame([[tstamp, temp, RSSI_dbm, RSSI_receive_dbm]])
        df.to_csv(path + filename, mode='a', index=False, header=False) #removed for manual closing
        #df.to_csv(outfile) #including for manual opening
        #outfile.close() #including for manual opening
        
    def flush_rx(self):
        sleep(0.01)
        self.send_strobe(reg.CC2500_IDLE)
        sleep(0.01)
        #flush RX FIFO
        self.send_strobe(reg.CC2500_FRX)
        sleep(0.01)
        self.send_strobe(reg.CC2500_IDLE)
        sleep(0.01)
        
    def check_gdo0(self):
        count_loop = 0
        #do while loop for checking the status of GDO0
        while True:
            State_GPIO = GPIO.input(17)
            if(State_GPIO == 0):
                sleep(0.01)
                count_loop += count_loop
                if(count_loop >= 10000):
                    self.flush_rx()
            else:
                break
            
    def temp_offset(self, temp, id1):
        print('inside function')
        id1 = int(id1)
        temp = int(temp)
        if(id1 == 5):
            if(temp == 30 or temp == 31):
                temp = temp - 1.67
                return temp
            elif(temp == 32 or temp == 33):
                temp = temp - 1.74
                return temp
            elif(temp == 34 or temp == 35):
                temp = temp - 1.83
                return temp
            elif(temp == 36 or temp == 37):
                temp = temp - 2.5
                return temp
            elif(temp == 38 or temp == 39):
                temp = temp - 3.33
                return temp
            elif(temp == 40 or temp == 41):
                temp = temp - 1.25
                return temp
            elif(temp == 42):
                temp = temp - 2.1
                return temp
            else:
                temp = temp - 1
                return temp
        elif(id1 == 6):
            if(temp == 30 or temp == 31):
                temp = temp + 4.74
                return temp
            elif(temp == 32 or temp == 33):
                temp = temp + 4.75
                return temp
            elif(temp == 34 or temp == 35):
                temp = temp + 5.11
                return temp
            elif(temp == 36 or temp == 37):
                temp = temp + 5
                return temp
            elif(temp == 38 or temp == 39):
                temp = temp + 4.83
                return temp
            elif(temp == 40 or temp == 41):
                temp = temp + 4.42
                return temp
            elif(temp == 42):
                temp = temp + 4.5
                return temp
            else:
                temp = temp + 4.5
                return temp
        elif(id1 == 7):
            if(temp == 30 or temp == 31):
                temp = temp - 7.08
                return temp
            elif(temp == 32 or temp == 33):
                temp = temp - 6.8
                return temp
            elif(temp == 34 or temp == 35):
                temp = temp - 8.36
                return temp
            elif(temp == 36 or temp == 37):
                temp = temp - 9.83
                return temp
            elif(temp == 38 or temp == 39):
                temp = temp - 10
                return temp
            elif(temp == 40 or temp == 41):
                temp = temp - 6.6
                return temp
            elif(temp == 42):
                temp = temp - 8.5
                return temp
            else:
                temp = temp - 7
                return temp
        elif(id1 == 8):
            if(temp == 30 or temp == 31):
                temp = temp - 4.88
                return temp
            elif(temp == 32 or temp == 33):
                temp = temp - 4.25
                return temp
            elif(temp == 34 or temp == 35):
                temp = temp - 5.31
                return temp
            elif(temp == 36 or temp == 37):
                temp = temp - 7.25
                return temp
            elif(temp == 38 or temp == 39):
                temp = temp - 6.75
                return temp
            elif(temp == 40 or temp == 41):
                temp = temp - 5.67
                return temp
            elif(temp == 42):
                temp = temp - 5.67
                return temp
            else:
                temp = temp - 5
                return temp
        elif (id1 == 10):
            if(temp == 30 or temp == 31):
                print(temp)
                temp = temp - 2.18
                print(temp)
                return temp
            elif(temp == 32 or temp == 33):
                temp = temp - 2.26
                return temp
            elif(temp == 34 or temp == 35):
                temp = temp - 2.79
                return temp
            elif(temp == 36 or temp == 37):
                temp = temp - 4.25
                return temp
            elif(temp == 38 or temp == 39):
                temp = temp - 4
                return temp
            elif(temp == 40 or temp == 41):
                temp = temp - 2.88
                return temp
            elif(temp == 42):
                temp = temp - 2.88
                return temp
            else:
                temp = temp - 2.5
                return temp
        else:
            return temp
            
    def rx_data_read(self):
        print("-----------------------------------------")
        #print("Data received, reading data:")
        address = 50
        PacketLength = 0
        #create time stamp
        now = (datetime.now())
        tstamp = "{0:%Y}-{0:%m}-{0:%d}_{0:%H}:{0:%M}:{0:%S}".format(now)
        bat = 0x0000
        Vbo = 0x0000
        Vbat =0x0000
        imps = ["Normal", "Low", "High","--"]
        config = configparser.ConfigParser() #added for stimulation logging
        #config.read('parameters.ini') #added for stimulation logging
        #print ("Time is: ",tstamp) #printing time
        #print('Getting Packet Address')
        data = []
        i_address = self.read_reg(reg.REG_RXFIFO)
        #print('Packet Address is:',i_address)
        #self.send_strobe(reg.CC2500_IDLE)
        
        try:
            if(i_address == address):
                print("Data received, reading data:")
                print ("Time is: ",tstamp) #printing time
                PacketLength = self.read_reg(reg.REG_RXFIFO)
                print("-----------------------------------------")
                print(str(PacketLength) + " Packet received")
                for i in range(0, PacketLength):
                    data.append(self.read_reg(reg.REG_RXFIFO))
                    #print("Appending data") #just for testing
                #Read, calculate and print receive RSSI
                RSSI_receive_raw = self.read_strobe(reg.REG_RSSI) #new addition
                #print("RSSI_receive_raw Value", RSSI_receive_raw) #just for testing
                if (RSSI_receive_raw >= 128):
                    RSSI_receive_dbm = (((RSSI_receive_raw - 256)/2)-71)
                else:
                    RSSI_receive_dbm = ((RSSI_receive_raw/2)-71)
                #for i in range(0, len(data)):    
                    #print((data[i]))
                #print(data)
                print("-----------------------------------------")
                ID_Rec = str(data[10])
                ID1 = int(ID_Rec)
                ID1 = str(ID_Rec)
                print("Implant ID:" , ID1)
                filename = ID1 + '.ini'
                filename = str(filename)
                #with open (filename, 'r') as configfile:
                    #config.read(configfile)
                #print(filename)
                config.read(filename)
                ID2 = int(ID_Rec)
                if(ID2 > 1 and ID2 < 13):
                    self.column(ID_Rec) #creating csv file if not already created
                Stim = int(config['parameter']['Stimulation']) #added for stimulation logging
                if(Stim == 0):
                    print("Stimulation: Off")
                else:
                    print("Stimulation: On")
                #print("Stimulation is:" , Stim)
                temp = 0x0000 | data[0] << 8 | data[1]
                temp = format(temp * 0.0014663/0.00355-277.75, '.0f')
                print("Temperature:", temp, u'\N{DEGREE SIGN}'+'C')
                BatVal = 0x03FF &(bat| data[4] << 8 | data[5])
                print("Impedance (Left Lateral): ", imps[data[2]])
                print("Impedance (Right Lateral): ", imps[data[3]])
            
                vref_left = data[8]
                vref_left = ((3.65/256)*vref_left)
                vref_left = round(vref_left,2)
                if(vref_left == 0):
                    print("Max. DBS Voltage (Left Lateral): ", "--")
                    vref_left = '--'
                else:
                    print("Max. DBS Voltage (Left Lateral): ", round(vref_left,2), "V")                    
                vref_right = data[9]
                vref_right = ((3.65/256)*vref_right)
                vref_right = round(vref_right,2)
                if(vref_right == 0):
                    print("Max. DBS Voltage (Right Lateral): ", "--")
                    vref_right = '--'
                else:
                    print("Max. DBS Voltage (Right Lateral): ", round(vref_right,2), "V")
                BoostVal = 0x03FF &(Vbo| data[6] << 8 | data[7]) # new implementation
                BoostVal = (4.88 * BoostVal)/1000
                print("Boost Voltage: ", round(BoostVal,2),"V")
                BatVal = BoostVal * BatVal /1024
                print("Battery Voltage: ", round(BatVal,2),"V")
                #warning for weak receive signal
                if(RSSI_receive_dbm<= -95):
                    #print("Receive signal RSSI in dBm: ", RSSI_receive_dbm)
                    print("Warning: Move closer to the implant or increase transmit power")
                print("RSSI at Base-station: ", RSSI_receive_dbm, "dBm")
                rec_hh = str(data[11])
                rec_mm = str(data[12])
                rec_time = (rec_hh + ":" + rec_mm)
                #print("Implant Time", rec_time)
                #Name = str(config['parameter']['UserName'])
                #print(Name) #only for testing
                temperature = self.temp_reading() #used for reading temp from external sensor
                #print("The temperature is " , temperature) #displays temperature of external sensor
                print("External Temperature:", temperature, u'\N{DEGREE SIGN}'+'C')
                newclass = temp_off(temp,ID1)
                temp = newclass.temp_offset1()
                #temp = self.temp_offset(temp, ID1)
                print("Temperature:", temp, u'\N{DEGREE SIGN}'+'C') #for checking diff
                if(ID2 > 1 and ID2 < 13):
                    self.newcolum(tstamp, ID_Rec, Stim, temp,temperature, imps[data[2]], imps[data[3]], vref_left, vref_right, round(BoostVal,2), round(BatVal,2), RSSI_receive_dbm)
                
            else:
                print("Wrong Packet Address")
        except:
            print("Exception")
        
    
    def rx_data_rf(self):
        #print("rx_data_rf_starts")
        #Enable RX mode
        self.send_strobe(reg.CC2500_RX)
        count_loop = 0
        
        #do while loop for checking the status of GDO0
        while True:
            State_GPIO = GPIO.input(17)
            if(State_GPIO == 0):
                sleep(0.001)
                count_loop += 1
                #print(count_loop)
                if(count_loop >= 1000):
                    self.flush_rx()
                    break
            elif(State_GPIO == 1):
                sleep(0.1)
                self.rx_data_read()
                #i_address = self.read_reg(reg.REG_RXFIFO)
                #print(State_GPIO) #for debugging only
                #print('Got Address',i_address)
                sleep(0.01)
                self.flush_rx()
                sleep(0.01)
                break
            
        


def main():
    c1 = 0
    c2 = 0
    typ = 0x00             # bilateral or unilateral
    lat = 0x00             # left lateral or right lateral
    att = 20               # counter attenuation pulse
    test_delay = 0
    #reset_counter = 0      # used for rebooting cc2500
    test_var = 0 #only for testing while loop
    a=cc2500()             # create object of class cc2500
    print("Starting..")
    a.cc2500_reset()       # reset cc2500
    a.cc2500_init()        # initialize cc2500
    #a.cc2500_init2()        # initialize cc2500
    a.read_config_regs()   # check cc2500
    #a.read_config_regs2()   # check cc2500
    #a.send_strobe2(reg.CC2500_RX)
    a.send_strobe(reg.CC2500_RX)
    #GPIO.add_event_detect(22, GPIO.FALLING, callback=a.rx_interrupt, bouncetime = 1000)
    #GPIO.add_event_detect(17, GPIO.FALLING, callback=a.rx_interrupt, bouncetime = 1000)
    
    S_G = 1 # Always SET mode
    Mode = 1   # Always Deep Sleep Mode
    clk = 32768 # cycle calculation value for Deep Sleep Mode
    
    try:    
        while(1):#(a.x == 0):
            sleep(0.1)
            a.rx_data_rf()
    except KeyboardInterrupt:
        GPIO.cleanup()
        
           
    #sleep(0.25)
          
        #print("---------------------")

main()




