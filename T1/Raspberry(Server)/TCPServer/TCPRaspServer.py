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

def run_server_tcp(void):

    # "192.168.5.177"  # Standard loopback interface address (localhost)
    HOST = "192.168.4.1"#"localhost"
    PORT = 10003  # Port to listen on (non-privileged ports are > 1023)

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
                print("Esperando recibir data...")
                doc = b""
                while True:
                
                    try:
                        conn.settimeout(15)
                        data = conn.recv(1024)
                        if data == b'\0':
                            
                            print("Llego toda la informacion")
                            break
                        else:
                            # print("doc+=")
                            doc += data
                    except TimeoutError:
                        conn.send(b'\0')
                        raise
                    except Exception:
                        conn.send(b'\0')
                        raise
                    
                    conn.send(b'\1')
                
                
                
                ##TERMINO CODIGO AUX
                print("Llego data :D")
                if doc == b'':
                    print("Llego data vacía, termina la conexión")
                    break
            except ConnectionResetError:
                break
            
            #conn.send(data)
            print("Comienzo de empaquetamiento de data...")
            try:
                (header,data) = Desempaquetamiento.parseData(doc)
                DatabaseWork.dataSave(header,data)
                
                conn.send(b'\1')
                
            except Exception as e:
                print("Oh no, ocurre una excepción. Aquí va:")
                print(e)
                print(str(e))
                break 
            DatabaseWork.saveLogs(header)

            break

        conn.close()
        print('Conexión cerrada')
        

