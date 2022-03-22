##
# @author: Krzysztof Półchłopek
# @date: 22.03.2022
#

from PyQt5 import QtWidgets, QtCore, uic

from HDC_UartThread import UARTThread

## Class for gui display and funcions
class MainGUI(QtWidgets.QMainWindow):

    def __init__(self):
        super(MainGUI, self).__init__()
        uic.loadUi('mainWindow.ui', self) # Load the .ui file
        self.show() # Show the GUI
