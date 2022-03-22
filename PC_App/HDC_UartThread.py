##
# @author: Krzysztof Półchłopek
# @date: 22.03.2022
#

import serial
from PyQt5 import QtCore

## Class for comunication through UART
class UARTThread(QtCore.QThread):
    
    ## class variable for signal emiting
    data_rec = QtCore.pyqtSignal(object)
    
    ## The Constructor
    def __init__(self, com_nbr, baudrate):
        QtCore.QThread.__init__(self)
        self.ser = serial.Serial(com_nbr, baudrate, timeout = 1000, parity = serial.PARITY_NONE, rtscts  =0)
        if self.ser.is_open():
            self.started = True
        else:
            self.started = False

    ## Method for checking status of serial port
    def is_opened(self):
        return self.started

    ## Method for receiving data form COM port
    def run(self):
        while self.started:
            ret = str(self.ser.readline())
            self.data_rec.emit(ret)

    ## Method for sending data through COM port   
    def send(self, msg):
        if self.started == True:
            self.ser.write(str.encode(msg))
            self.ser.flush()

    ## Method for closing serial port and terminating thread
    def closePort(self):
        if self.started == True:
            self.started = False
            self.ser.close()
        self.terminate()
            