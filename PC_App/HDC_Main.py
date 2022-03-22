##
# @author: Krzysztof Półchłopek
# @date: 22.03.2022
#

from PyQt5.QtWidgets import QApplication
import sys
from HDC_GUI import MainGUI

## Main program function
def main():
    app = QApplication(sys.argv)
    window = MainGUI()
    window.show()
    app.exec_()


if __name__ == '__main__':
    main()