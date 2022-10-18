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
            # data=""
            print("Esperando recibir data...")
            # data=conn.recv(1024) #2**15
            ### INICIO CODIGO AUX
            doc = b""
            while True:
               
                try:
                    print("esperando")
                    conn.settimeout(10)
                    data = conn.recv(1024)
                    print("llego algo")
                    if data == b'\0':
                        
                        print("Llego toda la informacion")
                        break
                    else:
                        print("doc+=")
                        doc += data
                except TimeoutError:
                    conn.send(b'\0')
                    raise
                except Exception:
                    conn.send(b'\0')
                    raise
                print(f"Enviando 1")
                conn.send(b'\1')
                print(f"Termino de enviar 1")
            
            
            ##TERMINO CODIGO AUX
            print("Llego data :D")
            if data == b'':
                print("Llego data vacía, termina la conexión")
                break
        except ConnectionResetError:
            break
        
        #conn.send(data)
        print("Comienzo de empaquetamiento de data...")
        try:
            (header,data) = Desempaquetamiento.parseData(doc)
            print("info = ",header,data)
            arreglos = data[16::]
            accx = arreglos[:2016]
            accy = arreglos[2017:]
            DatabaseWork.dataSave(header,data)
            print("Enviando OK!")
            conn.send(b'\1')
            print("Se envio OK!")
        except Exception as e:
            print("Excepción")
            print(e)
            print(str(e))
            print("Fin de excepción")
            break 
        DatabaseWork.saveLogs(header)
        print("Se desempaqueto data")
        break

    conn.close()
    print('Conexión cerrada')
    

