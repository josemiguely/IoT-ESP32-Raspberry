
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
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((HOST, PORT))
s.listen(50)


print(f"Listening on {HOST}:{PORT} en Main Server")


while True:
    print("Probando aceptar conexion en Main server")
    conn, addr = s.accept()
    print(f"Conexion aceptada en {HOST}:{PORT} en Main Server")

    print(f"Esperando soliciud de configuracion en Main server...")
    #Parte de configuración
    initial_data = conn.recv(1024)
    print(f"Llego solicitud de confiugracion :D")
    if initial_data == b'\0':
        (protocol,transportlayer)= DatabaseWork.getConfig()
        print(f"protocolo de config MS {str(protocol)}")
        print(f"transportlayer de config MS {str(transportlayer)}")
        config=((str(protocol)+str(transportlayer)).encode())
        conn.send(config)
        print("Configuración enviada desade Main Server :)")
        conn.close()
        print("Cerrando socket de main")
        if transportlayer==0:
            #Ejecutar server TCP
            print("Ejecutar server TCP desde Main")
            run_server_tcp()
            print("Terminando de ejecutar run_server_tcp()")
           
            
        else:
            #Ejecutar server UDP
            print("Ejecutar server UDP desde Main")
            run_server_udp()
    else:
        print("No me llego soliticud configuración inical desde Main Server :(")
        conn.close()
        print('Conexión cerrada')
    