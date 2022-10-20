
import socket
import sys
sys.path.insert(1, './Database')
import DatabaseWork

sys.path.insert(1,'./TCPServer')
sys.path.insert(1,'./UDPServer')
from TCPRaspServer import run_server_tcp
from UDPRaspServer import run_server_udp



# "192.168.5.177"  # Standard loopback interface address (localhost)
HOST = "192.168.4.1"# "localhost"
PORT = 10003  # Port to listen on (non-privileged ports are > 1023)

s = socket.socket(socket.AF_INET, #internet
                  socket.SOCK_STREAM) #TCP
s.bind((HOST, PORT))
s.listen(50)


print(f"Listening on {HOST}:{PORT}")


while True:
    conn, addr = s.accept()

    #Parte de configuración
    initial_data = conn.recv(1024)
    if initial_data == b'\0':
        (protocol,transportlayer)= DatabaseWork.getConfig()
        print(f"protocolo de config {protocol}")
        print(f"transportlayer de config {transportlayer}")
        config=(protocol+transportlayer).encode()
        
        conn.send(config)
        print("Configuración enviada :)")
        conn.close()
        if transportlayer==0:
            #Ejecutar server TCP
            print("Ejecutar server TCP")
            run_server_tcp()
           
            
        else:
            #Ejecutar server UDP
            print("Ejecutar server UDP")
            run_server_udp()
    else:
        print("No me llego configuración inical :(")
        conn.close()
        print('Conexión cerrada')
    