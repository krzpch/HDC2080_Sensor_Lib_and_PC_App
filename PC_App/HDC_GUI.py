##
# @author: Krzysztof Półchłopek
# @date: 22.03.2022
#

from email.policy import default
from symtable import Symbol
from PyQt5 import QtWidgets, QtGui, uic
from numpy import arange, power
import pyqtgraph as pg
import serial
import sys
import glob
import time

from UartThread import *
from HDC_Commands import HDC2080

## Class for gui display and funcions
#
#   This class supports gui display and configuration of functions which are execuded
#   when user performs specific actions in gui
class MainGUI(QtWidgets.QMainWindow):

    ## Class dictionaries storing offset data for temperature and humidity
    __temp_offset_dict  = {0: 0.16, 1: 0.32, 2: 0.64, 3: 1.28, 4: 2.58, 5: 5.16, 6: 10.32, 7: -20.32}
    __hum_offset_dict   = {0: 0.20, 1: 0.40, 2: 0.80, 3: 1.60, 4: 3.10, 5: 6.30, 6: 12.50, 7: -25.00}
    
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

        # attach functions to connection part
        self.portRefreshButton.clicked.connect(self.refresh_portSelectBox_onClick)
        self.connectButton.clicked.connect(self.connect_onClick)
        
        # attach functions to controlling sensor
        self.sendButton.clicked.connect(self.transmitData_onClick)
        self.clearButton.clicked.connect(self.clear_onClick)
        self.InitButton.clicked.connect(self.transmitInit_onClick)
        self.DeinitButton.clicked.connect(self.transmitDeinit_onClick)
        self.GetTempButton.clicked.connect(self.transmitGetTemp_onClick)
        self.GetHumButton.clicked.connect(self.transmitGetHum_onClick)
        self.ForceMeasurementsButton.clicked.connect(self.transmitForceMeas_onClick)
        self.ReadMeasurementsButton.clicked.connect(self.transmitGetMeasurements_onClick)

        # attach functions to calcualte offset
        self.tempOffset_lineEdit.textChanged.connect(self.calcTempOffset)
        self.humOffset_lineEdit.textChanged.connect(self.calcHumOffset)

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
        self.tempDataLine =  self.tempGraphWidget.plot(self.tempPlotX, self.tempPlotY, pen=self.tempPen)
        self.humDataLine =  self.humGraphWidget.plot(self.tempPlotX, self.tempPlotY, pen=self.humPen)

    ## ---------------------------------------------------------------------------------------------------- ##
    ## Functions for serial port support ##

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
        match input[0]:
            case "TEMP":
                self.show_recv_data("Temperature " + input[1][:-2])
                self.temp_lcdNumber.display(float(input[1][:-2]))
                self.plotTemp(len(self.tempPlotX) + 1,float(input[1][:-2]))
                
            case "HUM":
                self.show_recv_data("Humidity " + input[1][:-2])
                self.hum_lcdNumber.display(float(input[1][:-2]))
                self.plotHum(len(self.humPlotX) + 1,float(input[1][:-2]))
            
            case _:
               self.show_recv_data(data)

         
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

    ## Function for sending data to sensor
    def transmit(self, data):
        if self.portOpened:
            self.UARTPort.send(data)

    ## Function for sending and printing to GUI sent data
    def transmit_and_show(self, data):
        if self.portOpened:
            self.UARTPort.send(data)
            self.show_send_data(data[:-1])

    ## ---------------------------------------------------------------------------------------------------- ##
    ## Functions for sensor buttons handling ##

    ## Function for transmitting data through serial port
    def transmitData_onClick(self):
        if self.portOpened:
            data = self.send_lineEdit.text() + '\n'
            self.transmit_and_show(data)

    ## Function for clearing communication display
    def clear_onClick(self):
        self.globalResponse.clear()


    ## Function for sending "Initialize" message 
    def transmitInit_onClick(self):
        if self.portOpened:
            data = self.getConfig()
            self.transmit_and_show(self.hdc.init(data[0]))
            time.sleep(0.5)
            self.transmit_and_show(self.hdc.config_temp(data[1]))
            time.sleep(0.5)
            self.transmit_and_show(self.hdc.config_hum(data[2]))
            time.sleep(0.5)
            self.transmit_and_show(self.hdc.config_int(data[3]))
            
    ## Function for sending deinitialization message 
    def transmitDeinit_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.deinit())
            
    ## Function for sending force measurement message
    def transmitForceMeas_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.force_measurement())
            
    ## Function for sending get temperazture message 
    def transmitGetTemp_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.recv_temp())
            
    ## Function for sending get humidity message 
    def transmitGetHum_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.recv_hum())
            
    ## Function for sending get measurements message 
    def transmitGetMeasurements_onClick(self):
        if self.portOpened:
            self.transmit_and_show(self.hdc.recv_mesurements())

    ## ---------------------------------------------------------------------------------------------------- ##
    ## Functions for displaying and ploting meaurements##
    
    ## Function to update temperature display
    def updateTempLCD(self, temp):
        self.temp_lcdNumber.display(temp)

    ## Function to update humidity display
    def updateHumLCD(self, hum):
        self.hum_lcdNumber.display(hum)
    
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
    ## Helper functions ##
    
    ## Function for reading data form UI
    def getConfig(self):
        data =      [self.measurementMode_comboBox.currentIndex(),  self.autoMode_comboBox.currentIndex()]
        temp_data = [self.tempRes_comboBox.currentIndex(),          self.tempOffset_lineEdit.text()]
        hum_data =  [self.humRes_comboBox.currentIndex(),           self.humOffset_lineEdit.text()]
        int_data =  [self.interruptEnable_comboBox.currentIndex(),  self.interruptMode_comboBox.currentIndex(),
                     self.interruptSelect_comboBox.currentIndex(),  self.maxTempInt_lineEdit.text(),
                     self.minTempInt_lineEdit.text(),               self.maxHumInt_lineEdit.text(),
                     self.minHumInt_lineEdit.text()]
        return [data, temp_data, hum_data, int_data]
        
    ## Function for calculating temperature offset
    def calcTempOffset(self):
        val = int(self.tempOffset_lineEdit.text(),base=16)
        out = 0.00
        for i in range(7,-1,-1):
            if((val - power(2,i)) >= 0 ):
                val = val - power(2,i)
                out += self.__temp_offset_dict[i]

        self.tempOffsetFloat_lineEdit.setText(str(round(out,2)))

    
    ## Function for calculating humidity offset
    def calcHumOffset(self):
        val = int(self.humOffset_lineEdit.text(),base=16)
        out = 0.00
        for i in range(7,-1,-1):
            if((val - power(2,i)) >= 0 ):
                val = val - power(2,i)
                out += self.__hum_offset_dict[i]

        self.humOffsetFloat_lineEdit.setText(str(round(out,2)))
