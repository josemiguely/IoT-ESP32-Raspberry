# Importamos lo que necesitamos
from interface import Ui_Dialog
from PyQt5 import QtCore,QtGui,QtWidgets

class GUIController:
    def __init__(self,parent):
        self.ui = Ui_Dialog()
        self.parent = parent
        print()
    def setSignals(self):
        self.ui.search_esp32.clicked.connect(self.popupBakan)

    def popupBakan(self):
        popup = QtWidgets.QMessageBox(parent = self.parent)
        popup.setWindowTitle("Titulo bakan 😼")
        popup.setText("Apretaste el boton bakan 😼")
        #popup.setIcon("😼")
        popup.exec_()
        return 


        


if __name__ == "__main__":
    import sys 
    app = QtWidgets.QApplication(sys.argv)
    Dialog = QtWidgets.QDialog()
    cont = GUIController(Dialog)   
    ui = cont.ui
    ui.setupUi(Dialog)
    Dialog.show()
    cont.setSignals()
    sys.exit(app.exec_())