# importing the sys module
import sys  
import socket
#from DatabaseWork import *
#from Raspberry(Server).Database.DatabaseWork import saveLogs
 
# appending the directory of mod.py
# in the sys.path list
sys.path.insert(1, '../Database') 
import Desempaquetamiento
import DatabaseWork
def int_to_string_bytearray(a: int) -> bytearray:
    return f'{a:d}'.encode('utf-8').zfill(5)


def string_bytearray_to_int(b: bytearray) -> int:
    return int(b.decode('utf-8'))

# "192.168.5.177"  # Standard loopback interface address (localhost)
HOST = "192.168.4.1"#"localhost"
PORT = 10000  # Port to listen on (non-privileged ports are > 1023)

s = socket.socket(socket.AF_INET, #internet
                  socket.SOCK_STREAM) #TCP
s.bind((HOST, PORT))
s.listen(50)
print(f"Listening on {HOST}:{PORT}")
while True:
    conn, addr = s.accept()
    print(f'Conectado por alguien ({addr[0]}) desde el puerto {addr[1]}')
  
    while True:
        try:
            data=""
            print("Esperando recibir data...")
            for i in range(31):
                print(f" cantidad de recv {i}")
                if i==0:
                    data=conn.recv(1024)
                else:
                    data += conn.recv(1024)
                

            
            # data = conn.recv(19216)
            print("Llego data :D")
            if data == b'':
                print("Llego data vacía, termina la conexión")
                break
        except ConnectionResetError:
            break
        
        #conn.send(data)
        print("Comienzo de empaquetamiento de data...")
        (header,data) = Desempaquetamiento.parseData(data)
        print("info = ",header,data)
        res= DatabaseWork.dataSave(header,data)
        DatabaseWork.saveLogs(header)
        print("Se desempaqueto data")
        break

    conn.close()
    print('Conexión cerrada')
    

