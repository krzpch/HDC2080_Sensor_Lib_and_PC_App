##
# @author: Krzysztof Półchłopek
# @date: 22.03.2022
#

from symtable import Symbol
from PyQt5 import QtWidgets, QtGui, uic
from numpy import arange
import pyqtgraph as pg
import serial
import sys
import glob

from UartThread import *
from HDC_Commands import HDC2080

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

    ## Class variables for storing and ploting temperature and humidity
    tempPlotX = list()
    tempPlotY = list()
    humPlotX = list()
    humPlotY = list()

    ## The consrtuctor
    def __init__(self):
        super(MainGUI, self).__init__()
        uic.loadUi('mainWindow.ui', self) # Load the .ui file
        self.show() # Show the GUI

        # update port select box 
        for port in self.list_ports():
            self.portSelectBox.addItem(port, port)

        #attach functions to connection part
        self.portRefreshButton.clicked.connect(self.refresh_portSelectBox_onClick)
        self.connectButton.clicked.connect(self.connect_onClick)
        
        #
        self.sendButton.clicked.connect(self.transmitData_onClick)
        self.TestButton.clicked.connect(self.transmitInit_onClick)
        self.TestButton_2.clicked.connect(self.transmitGetTemp_onClick)
        self.TestButton_3.clicked.connect(self.transmitGetHum_onClick)
        self.TestButton_4.clicked.connect(self.transmitDeinit_onClick)
        self.ReadMeasurementsButton.clicked.connect(self.transmitGetMeasurements_onClick)
        
        # attach function to register tab
        self.ReadRegsButton.clicked.connect(self.readRegs_onClick)
        self.WriteRegsButton.clicked.connect(self.writeRegs_onClick)

        # init plots
        self.tempGraphWidget.setBackground('w')
        self.humGraphWidget.setBackground('w')
        self.tempGraphWidget.showGrid(x=True, y=True)
        self.humGraphWidget.showGrid(x=True, y=True)
        styles = {"color": "black", "font-size": "15px", "font-weight":"bold"}
        self.tempGraphWidget.setLabel("left", "Temperature [°C]", **styles)
        self.tempGraphWidget.setLabel("bottom", "Sample number", **styles)
        self.humGraphWidget.setLabel("left", "Humidity [% RH]", **styles)
        self.humGraphWidget.setLabel("bottom", "Sample number", **styles)
        self.tempPen = pg.mkPen(color=(255, 0, 0))
        self.humPen = pg.mkPen(color=(0, 0, 255))
        self.tempDataLine =  self.tempGraphWidget.plot(self.tempPlotX, self.tempPlotY, pen=self.tempPen, symbol='d')
        self.humDataLine =  self.humGraphWidget.plot(self.tempPlotX, self.tempPlotY, pen=self.humPen, symbol='d')
        

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
                self.UARTPort.data_rec.connect(self.decode_recv_data)
                self.UARTPort.start()
                self.connectButton.setText("Disconnect")
        else:
            self.UARTPort.closePort()
            self.portOpened = False
            self.connectButton.setText("Connect")

    ## Function for interprating input data
    def decode_recv_data(self, data):
        input = data.split(' ')
        print(input)
        
        match input[0]:
            case "TEMP":
                self.show_recv_data("Temperature " + input[1][:-2])
                self.temp_lcdNumber.display(float(input[1][:-2]))
                self.plotTemp(len(self.tempPlotX) + 1,float(input[1][:-2]))
            case "HUM":
                self.show_recv_data("Humidity " + input[1][:-2])
                self.hum_lcdNumber.display(float(input[1][:-2]))
                self.plotHum(len(self.humPlotX) + 1,float(input[1][:-2]))
                
            case "REGS":
                pass

    ## Function for displaying received data through serial port
    def show_recv_data(self, data):
        input = "uC -> PC: " + data
        self.globalResponse.append(input)
        self.globalResponse.moveCursor(QtGui.QTextCursor.MoveOperation.End)


    ## Function for displaying transmitted data through serial port
    def show_send_data(self, data):
        input = "PC -> uC: " + data
        self.globalResponse.append(input)
        self.globalResponse.moveCursor(QtGui.QTextCursor.MoveOperation.End)

    ## Function for sending and printing to GUI sent data
    def transmit_and_show(self, data):
        if self.portOpened:
            self.UARTPort.send(data)
            self.show_send_data(data[:-1])

    ## Function for transmitting data through serial port
    def transmitData_onClick(self):
        if self.portOpened:
            data = self.send_lineEdit.text() + '\n'
            self.transmit_and_show(data)

    ## ---------------------------------------------------------------------------------------------------- ##
    ## Functions for buttons handling ##

    ## Function for sending "Initialize" message 
    def transmitInit_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.init(self.tempOffset_lineEdit.text(),self.humOffset_lineEdit.text()))
            
    ## Function for sending "Initialize" message 
    def transmitDeinit_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.deinit())
            
    ## Function for sending "Get Temperature" message 
    def transmitGetTemp_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.recv_temp())
            
    ## Function for sending "Get Humidity" message 
    def transmitGetHum_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.recv_hum())
            
    ## Function for sending "Get Humidity" message 
    def transmitGetMeasurements_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.recv_mesurements())

    ## ---------------------------------------------------------------------------------------------------- ##
    ## Functions for ploting ##
    
    ## Function for ploting temperature
    def plotTemp(self, time, temperature):
        self.tempPlotX.append(time)
        self.tempPlotY.append(temperature)
        
        self.tempDataLine.setData(self.tempPlotX[0-self.tempPlotLen.value():], self.tempPlotY[0-self.tempPlotLen.value():])
        
    ## Function for ploting temperature
    def plotHum(self, time, humidity):
        self.humPlotX.append(time)
        self.humPlotY.append(humidity)
        
        self.humDataLine.setData(self.humPlotX[0-self.humPlotLen.value():], self.humPlotY[0-self.humPlotLen.value():])
    
    ## ---------------------------------------------------------------------------------------------------- ##
    ## Functions for Reading and writing to regosters ##
    
    ## Function for requesting register values
    def readRegs_onClick(self):
        self.register1_lineEdit.setText("0x11")
        pass
    
     ## Function for sening new register values
    def writeRegs_onClick(self):
        self.register1_lineEdit.setText("0x22")
        pass