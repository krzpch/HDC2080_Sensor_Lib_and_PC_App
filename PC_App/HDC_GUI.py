##
# @author: Krzysztof Półchłopek
# @date: 22.03.2022
#

from PyQt5 import QtWidgets, QtCore, uic
import serial
import sys
import glob

from HDC_UartThread import UARTThread

## Class for gui display and funcions
class MainGUI(QtWidgets.QMainWindow):

    def __init__(self):
        super(MainGUI, self).__init__()
        uic.loadUi('mainWindow.ui', self) # Load the .ui file
        self.show() # Show the GUI

        # update prot select box 
        for port in self.list_ports():
            self.portSelectBox.addItem(port, port)

        #attach functions to buttons
        self.portRefreshButton.clicked.connect(self.refresh_portSelectBox_onClick)


    ## Funcion to update temperature display
    def updateTempLCD(self, temp):
        self.temp_lcdNumber.display(temp)


    ## Funcion to update humidity display
    def updateHumLCD(self, hum):
        self.hum_lcdNumber.display(hum)


    ## Funtion for listing all avaliable ports 
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