# importing the sys module
import sys  
import socket
 
# appending the directory of mod.py
# in the sys.path list
sys.path.insert(1, '../Database') 
import Desempaquetamiento

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
            data = conn.recv(1024)
            # print("message length = "+str(len(data)))
            # print("without decoding = "+ str(data[80:96]))
            # print("decoded = " + str(data[80:96].decode()))
            # print("number = " + str(int((data[80:96].decode()), 2)))
            # print(str(int(len(data)-96)//8))

            #if len(data) >= 12*8 and int((data[80:96].decode()), 2) == int((len(data)-96)//8):
                 #print("im in")
                 #if (int(data[72:80],2) == 1):
                     #print("ID Protocol: "+str(int(data[72:80],2)))
                     #break
                 #elif(int(data[72:80],2) == 2):
                     #print("ID Protocol: " + str(int(data[72:80],2)))
                     #break
                 #elif (int(data[72:80],2) == 3):
                     #print("ID Protocol: " + str(int(data[72:80],2)))
                     #break
                 #elif (int(data[72:80],2) == 4):
                     #print("ID Protocol: " + str(int(data[72:80],2)))
                     #break
            #else:
                 #print("error data length")
                 #break
            if data == b'':
                break
        except ConnectionResetError:
            break
        print(f"Recibido {data.decode('latin1')}")
        conn.send(data)
        res=Desempaquetamiento.parseData(data)
        if res==None:
            break
    
    conn.close()
    print('Desconectado')
    break

