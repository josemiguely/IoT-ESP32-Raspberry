# Importamos lo que necesitamos
from interface2 import Ui_Dialog
from PyQt5 import QtCore,QtGui,QtWidgets
from PyQt5.QtCore import QTimer,Qt
import datetime
from PlotData import getFromData1,getFromData2
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
from string import punctuation

class GUIController:
    def __init__(self,parent):
        self.ui = Ui_Dialog()
        self.parent = parent
        self.timer1 = 0
        self.timer2 = 0
        self.timer3 = 0
        
        print()
    def setSignals(self):
       
        self.ui.disc_time_field.textChanged.connect(self.disableButton)
        self.ui.port_tcp_field.textChanged.connect(self.disableButton)
        self.ui.port_udp_field.textChanged.connect(self.disableButton)
        self.ui.host_ip_addr_field.textChanged.connect(self.disableButton)
        self.ui.ssid_field.textChanged.connect(self.disableButton)
        self.ui.pass_field.textChanged.connect(self.disableButton)
        self.ui.esp32_select.activated.connect(self.disableButton)
        self.ui.config_btn.clicked.connect(self.popupBakan)
        self.ui.plot_1_start_btn.clicked.connect(self.start_timer1)
        self.ui.plot_2_start_btn.clicked.connect(self.start_timer2)
        self.ui.plot_3_start_btn.clicked.connect(self.start_timer3)
        self.ui.plot_1_stop_btn.clicked.connect(self.stop_data1)
        self.ui.plot_2_stop_btn.clicked.connect(self.stop_data2)
        self.ui.plot_3_stop_btn.clicked.connect(self.stop_data3)
        self.ui.status_select.activated.connect(self.activated)
        self.ui.config_mode_select.activated.connect(self.consistencia)



    def disableButton(self):
        #print(((self.ui.disc_time_field.toPlainText().translate(str.maketrans('', '',punctuation))).isspace()) )
        if (not(self.ui.disc_time_field.toPlainText().translate(str.maketrans('', '',punctuation)).isspace())and len(self.ui.disc_time_field.toPlainText().translate(str.maketrans('', '',punctuation)))> 0
        and not(self.ui.port_tcp_field.toPlainText().translate(str.maketrans('', '',punctuation)).isspace()) and len(self.ui.port_tcp_field.toPlainText().translate(str.maketrans('', '',punctuation))) > 0
        and not(self.ui.port_udp_field.toPlainText().translate(str.maketrans('', '',punctuation)).isspace()) and len(self.ui.port_udp_field.toPlainText().translate(str.maketrans('', '',punctuation))) > 0
        and not(self.ui.host_ip_addr_field.toPlainText().translate(str.maketrans('', '',punctuation)).isspace()) and len(self.ui.host_ip_addr_field.toPlainText().translate(str.maketrans('', '',punctuation))) > 0
        and not(self.ui.ssid_field.toPlainText().translate(str.maketrans('', '',punctuation)).isspace()) and len(self.ui.ssid_field.toPlainText().translate(str.maketrans('', '',punctuation))) > 0
        and not(self.ui.pass_field.toPlainText().translate(str.maketrans('', '',punctuation)).isspace()) and len(self.ui.pass_field.toPlainText().translate(str.maketrans('', '',punctuation))) > 0
        and self.ui.esp32_select.currentIndex() != -1 and len(self.ui.esp32_select.currentText()) > 0 ):

            self.ui.config_btn.setEnabled(True)
        else:
            self.ui.config_btn.setEnabled(False)
    def display_time(self):
    
        current_time = datetime.datetime.now().strftime('%Y.%m.%d - %H:%M:%S')
        self.ui.label.setText(current_time)

        print('current_time:', current_time)

    def activated(self, index):
        #print("Activated index:", index)
        if(index == 3 or index == 4):
            #print("Debería eliminarlo")
            self.ui.config_mode_select.removeItem(4)
        else:
            if not self.ui.id_protocol_select.itemText(4):
                self.ui.id_protocol_select.addItem('5')
    
    def consistencia(self):
        #print("Activated index:", index)
        print("config mode selected index = " , self.ui.config_mode_select.currentIndex())
       
        print(self.ui.config_mode_select.findText("Configuracion por Bluetooth"))
        if(self.ui.config_mode_select.currentIndex() == self.ui.config_mode_select.findText("Configuracion por Bluetooth")):
            print("Config Mode = Configuracion por Bluetooth")
            print("Eliminar",self.ui.status_select.findText("Actualización via TCP"))
            self.ui.status_select.removeItem(self.ui.status_select.findText("Actualización via TCP"))
            if self.ui.status_select.findText("Actualización via BLE") == -1:
                self.ui.status_select.addItem("Actualización via BLE")
                
        elif(self.ui.config_mode_select.currentIndex() == self.ui.config_mode_select.findText("Configuracion via TCP en BD")):
            print("Config Mode = Configuracion via TCP en BD")
            print("Eliminar",self.ui.status_select.findText("Actualización via BLE"))
            self.ui.status_select.removeItem(self.ui.status_select.findText("Actualización via BLE"))
            if self.ui.status_select.findText("Actualización via TCP") == -1:
                self.ui.status_select.addItem("Actualización via TCP")    
        # else:
        #     if not self.ui.status_select.findText("Actualización via TCP"):
        #         self.ui.status_select.addItem("Actualización via TCP")    
        #     if not self.ui.status_select.findText("Actualización via BLE"):  
        #         self.ui.status_select.addItem("Actualización via BLE")
                 
                
                    
    def display_data(self,grafico,select_var,device_name_select):
        grafico.clear()
        #text = self.ui.plot_1_select_var.currentText()
        text = select_var.currentText()
        dispositivo = device_name_select.currentText()
        if (text == "Racc_x" or text == "Racc_y" or text == "Racc_z"):
            data = getFromData2(f"{text,dispositivo}") 
        else:
            data = getFromData1(f"{text,dispositivo}") 
       
        tiempo = []
        for i in range(1,len(data)+1):
            tiempo.append(i)
        # self.graphWidget = pg.PlotWidget()
        # self.setCentralWidget(self.graphWidget)
        #self.ui.plot1.setBackground('w')
        grafico.plot(tiempo, data)

    def llamar_a_display1(self):
        cont.display_data(self.ui.plot_1,self.ui.plot_1_select_var,self.ui.plot_1_device_name_select)

    def llamar_a_display2(self):
        cont.display_data(self.ui.plot_2,self.ui.plot_2_select_var,self.ui.plot_2_device_name_select)

    def llamar_a_display3(self):
        cont.display_data(self.ui.plot_3,self.ui.plot_3_select_var,self.ui.plot_3_device_name_select)    


    def start_timer1(self):
        self.timer1 = QTimer()
        self.timer1.timeout.connect(cont.llamar_a_display1)  # execute `display_time`
        self.timer1.setInterval(1000)  # 1000ms = 1s
        self.timer1.start() 
    def start_timer2(self):
        self.timer2 = QTimer()
        self.timer2.timeout.connect(cont.llamar_a_display2)  # execute `display_time`
        self.timer2.setInterval(1000)  # 1000ms = 1s
        self.timer2.start() 
    def start_timer3(self):
        self.timer3 = QTimer()
        self.timer3.timeout.connect(cont.llamar_a_display3)  # execute `display_time`
        self.timer3.setInterval(1000)  # 1000ms = 1s
        self.timer3.start()         

    def stop_data1(self):
        self.ui.plot_1.clear()
        self.timer1.stop()
    def stop_data2(self):
        self.ui.plot_2.clear()
        self.timer2.stop()    
    def stop_data3(self):
        self.ui.plot_3.clear()
        self.timer3.stop()

        
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
    window = QtWidgets.QMainWindow()
    label = QtWidgets.QLabel(window, text='???', alignment=Qt.AlignCenter)
    window.setCentralWidget(label)
    Dialog.show()


        # timer which repate function `display_time` every 1000ms (1s)
    # timer = QTimer()
    # timer.timeout.connect(cont.display_time)  # execute `display_time`
    # timer.setInterval(1000)  # 1000ms = 1s
    # timer.start()
    cont.setSignals()
    cont.disableButton()
    cont.consistencia()
    sys.exit(app.exec_())