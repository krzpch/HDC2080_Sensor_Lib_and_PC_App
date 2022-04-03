##
# @author: Krzysztof Półchłopek
# @date: 22.03.2022
#

from PyQt5 import QtWidgets, QtCore, uic
import serial
import sys
import glob

from UartThread import *
from HDC_Commands import HDC, HDC2080

## Class for gui display and funcions
#
#   This class supports gui display and configuration of functions which are execuded
#   when user performs specific actions in gui
class MainGUI(QtWidgets.QMainWindow):

    ## Class variable storing port status
    portOpened = False

    ## Class variable for accessing commands, stored configuration and data
    hdc = HDC2080()

    ## Class variable storing UART 
    UARTPort = None

    ## The consrtuctor
    def __init__(self):
        super(MainGUI, self).__init__()
        uic.loadUi('mainWindow.ui', self) # Load the .ui file
        self.show() # Show the GUI

        # update prot select box 
        for port in self.list_ports():
            self.portSelectBox.addItem(port, port)

        #attach functions to buttons
        self.portRefreshButton.clicked.connect(self.refresh_portSelectBox_onClick)
        self.connectButton.clicked.connect(self.connect_onClick)
        self.sendButton.clicked.connect(self.transmitData_onClick)
        self.TestButton.clicked.connect(self.transmitInit_onClick)

    ## Function to update temperature display
    def updateTempLCD(self, temp):
        self.temp_lcdNumber.display(temp)


    ## Function to update humidity display
    def updateHumLCD(self, hum):
        self.hum_lcdNumber.display(hum)


    ## Function for listing all avaliable ports 
    def list_ports(self):
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            ports = glob.glob('/dev/ttyS[0-90-9]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')
        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        return result
    
    ## Function for updating portSelectBox
    def refresh_portSelectBox_onClick(self):
        self.portSelectBox.clear()
        for port in self.list_ports():
            self.portSelectBox.addItem(port, port)

    ## Function for opening/closing the serial port
    def connect_onClick(self):
        if self.portOpened == False:
            self.UARTPort = UARTThread(com_nbr= str(self.portSelectBox.currentData()),baudrate= int(self.baudrateLineEdit.text()), op_mode= COMMAND_MODE)
            self.portOpened = self.UARTPort.is_opened()
            if self.portOpened == False:
                print("PORT ERROR")
                return
            else:
                self.UARTPort.data_rec.connect(self.show_recv_data)
                self.UARTPort.start()
                self.connectButton.setText("Disconnect")
        else:
            self.UARTPort.closePort()
            self.portOpened = False
            self.connectButton.setText("Connect")


    ## Function for displaying received data through serial port
    def show_recv_data(self, data):
        input = "uC -> PC:" + data[:-2]
        self.globalResponse.append(input)


    ## Function for displaying transmitted data through serial port
    def show_send_data(self, data):
        input = "PC -> uC:" + data
        self.globalResponse.append(input)

    ## Function for sending and printing to GUI sent data
    def transmit_and_show(self, data):
        if self.portOpened:
            self.UARTPort.send(data)
            self.show_send_data(data[:-2])

    ## Function for transmitting data through serial port
    def transmitData_onClick(self):
        if self.portOpened:
            data = self.send_lineEdit.text() + '\n'
            self.transmit_and_show(data)

    ## Function for sending init message 
    def transmitInit_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.init())

