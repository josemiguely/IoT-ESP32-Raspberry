import socket
import sys
sys.path.insert(1, '../Database') 
import Desempaquetamiento
import DatabaseWork

def run_server_udp():
        UDP_IP = "192.168.4.1"# "localhost" 
        UDP_PORT = 10003+2


        sUDP = socket.socket(socket.AF_INET, # Internet
                            socket.SOCK_DGRAM) # UDP
        sUDP.bind((UDP_IP, UDP_PORT))


        print(f"Listening for UDP packets in {UDP_IP}:{UDP_PORT}")

        while True:
            # payload, client_address = sUDP.recvfrom(1)
            # print("Echoing data back to " + str(client_address) + ": " + payload)
            # sent = sUDP.sendto(payload, client_address)
            
            doc = b""

            while True:
                data,client_address = sUDP.recvfrom(1024)
                
                if data == b'\0':
                    print("Llego toda la información en UDP")
                    break
                else:
                # if numid -1 == numid_anterior  
                    doc += data
                    # print(f"doc+= {doc}") #Sacar este print después de verificar que UDP está funcionando bien

            print("Llego data :D")
            if doc == '\0':
                print("Llego data vacía, termina la conexión")
                break

            print("Cominezo de empaquetamiento de data...")
            try:
                (header,data)=Desempaquetamiento.parseData(doc)
                if data==None:
                    print("Paquete malo, esperaremos mas información")
                    break
                else:
                    DatabaseWork.dataSave(header,data)
                    DatabaseWork.saveLogs(header)
                    
                    

            except Exception as e:
                    print("Oh no, ocurre una excepción en parseo/guardado UDP. Aquí va:")
                    print(e)
                    break 

            

# run_server_udp()