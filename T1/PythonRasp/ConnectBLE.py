from findAddresses import findAddresses
import pygatt
import time
import pyqtgraph as pg
import logging
from interface import Ui_Dialog
from PyQt5 import QtCore,QtGui,QtWidgets
from struct import pack
#pyuic5 ejemplo_tarea2.ui -o ex.pypyt


class GUIController:
    def __init__(self, parent):
        self.ui = Ui_Dialog()
        self.parent = parent
        self.macs = []
        self.UUIDs = []
        self.servers = []
        self.plot = None
        self.grph = None
        self.macindx = None

        self.adapter = pygatt.GATTToolBackend() ##pygatt
        print()
        
    def actualizarMacs(self):
        # actualiza la lista de dispositivos con bluetooth disponibles
        adrs = findAddresses()
        print(f"adress = {adrs}")
        self.macs = adrs[1]
        self.UUIDs = adrs[2]
        self.ui.esp32_select.clear()
        self.ui.esp32_select.addItems(adrs[0])
        #print()

    def xd(self):
        p= self.getConfigParams()
        print(p)
        print(self.merge(p))
        print("si")
        self.configSetup()
        
    def pass_Mac_index(self):
        #self.ui.esp32_select.currentIndex()
        print(self.ui.esp32_select.currentText())
        self.macindx = self.ui.esp32_select.currentIndex()
        #self.macindx = indx
        self.conectarMac()

    def setSignals(self):
        self.ui.search_esp32.clicked.connect(self.actualizarMacs)
        self.ui.esp32_select.activated.connect(self.pass_Mac_index)
        self.ui.config_btn.clicked.connect(self.xd)
        
    def conectarMac(self):
        # se conecta mediante BLE a un dispostivo disponible
        #indx = self.ui.selec_7.currentIndex()
        #indx = 0
       
        indx = self.macindx
        print(f"Conectando a {self.macs[self.macindx]}")
        ##pygatt
        logging.basicConfig()
        logging.getLogger('pygatt').setLevel(logging.DEBUG)
        qty = 0
        while(qty<100):
            try:
                self.adapter.start()
                device = self.adapter.connect(self.macs[indx],timeout=2.0)
                #device = self.adapter.connect('4C:EB:D6:62:0B:B2',timeout=2.0)
                print('Se conecto!')
                # La siguiente linea es para ver todas las caracteristicas disponibles
                characteristics = device.discover_characteristics()
                for i in characteristics.keys():
                    print('Caracteristicas: '+str(i))#list(characteristics.keys())))
                time.sleep(1)
                qty = 100
            except pygatt.exceptions.NotConnectedError:
                qty += 1
                print(f"Se han fallado: {qty} intentos" )
                print("Not connected")
                time.sleep(1)
            finally:
                self.adapter.stop()
        print("Termino de test de conexión")
    def getConfigParams(self):
        
        acc_sampling = self.ui.acc_samp_field.toPlainText()
        acc_sensibility = self.ui.acc_sens_field.toPlainText()
        gyro_sensibility = self.ui.gyro_sens_field.toPlainText()
        BME688_sampling = self.ui.bme_field.toPlainText()
        discontinous_time = self.ui.disc_time_field.toPlainText()
        port_tcp = self.ui.port_tcp_field.toPlainText()
        port_udp = self.ui.port_udp_field.toPlainText()
        host_ip = self.ui.host_ip_addr_field.toPlainText()
        wifi_ssid = self.ui.ssid_field.toPlainText()
        wifi_password =self.ui.pass_field.toPlainText()
        config_mode =self.ui.config_mode_select.currentText()
        status =self.ui.status_select.currentIndex() # Arreglar porque depende del ip protocol

        id_protocol=self.ui.id_protocol_select.currentText()
        data = {"acc_sampling":acc_sampling,"acc_sensibility":acc_sensibility,
                "gyro_sensibility":gyro_sensibility,"BME688_sampling":BME688_sampling,
                "discontinous_time":discontinous_time,"port_tcp":port_tcp,"port_udp":port_udp,
                "host_ip":host_ip,"wifi_ssid":wifi_ssid,"wifi_password":wifi_password,
                "config_mode":config_mode,"status":status,"id_protocol":id_protocol}
        return data
    def merge(self,data):
        wifi=data["wifi_ssid"].encode("ASCII")
        passw=data["wifi_password"].encode("ASCII")
        wf = f"{len(wifi)}s"
        pw = f"{len(passw)}s"
        format_data = "<bbiiiiiiii"+wf+pw
        dl = [int(data["status"]),int(data["id_protocol"]),
                     int(data["acc_sampling"]),int(data["acc_sensibility"]),
                     int(data["gyro_sensibility"]),
                     int(data["BME688_sampling"]),int(data["discontinous_time"]),
                     int(data["port_tcp"]),int(data["port_udp"]),int(data["host_ip"]),
                     wifi,passw]
        
        result = pack(format_data,*dl)
        return result 
    def configSetup(self):
        # envía una configuración indicada por BLE al dispositivo conectado
        #ESPconf = self.getConfigParams()
        #pack = self.merge(ESPconf)
        #print("El largo del paquete es:" + str(len(pack)))
        qty=0
        while qty<100:
            try:
                self.adapter.start()
                #device = self.adapter.connect(self.macs[self.macindx], timeout=2.0)
                device = self.adapter.connect('4C:EB:D6:62:0B:B2', timeout=2.0)
                device.exchange_mtu(80)
                print('Se conecto!')
                characteristics = device.discover_characteristics().keys()
                # La siguiente linea es para escribir en la caracteristica de UUID list(characteristics)[4], puede hardcodear si
                # sabe la UUID de la caracteristica a escribir, este misma funcion para leer es tan solo char_read
                # Recomiendo leer acerca del sistema de Subscribe para recibir notificaciones del cambio u otros
                #device.char_write(list(characteristics)[4], pack)
                print("leyendo")
                value = device.char_read(list(characteristics)[4])
                print(value)
                print("Se escribio el paquete")
                qty = 100
            except pygatt.exceptions.NotConnectedError:
                qty += 1
                print(f"Se han fallado: {qty} intentos" )
                print("print bakan")
                print("Not connected")
                time.sleep(1)
            finally:
                self.adapter.stop()


if __name__ == "__main__":
    import sys 
    app = QtWidgets.QApplication(sys.argv)
    Dialog = QtWidgets.QDialog()
    cont = GUIController(Dialog)   
    ui = cont.ui
    ui.setupUi(Dialog)
    Dialog.show()
    cont.setSignals()
    #cont.actualizarMacs()
    #cont.conectarMac()
    cont.configSetup()

    sys.exit(app.exec_())