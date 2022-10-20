from struct import unpack, pack, calcsize
import traceback
import sys
from DatabaseWork import * 

# Documentación struct unpack,pack :https://docs.python.org/3/library/struct.html#

## Corregir databaseWork.py con los nombres de dataDict
'''
Estas funciones se encargan de parsear y guardar los datos recibidos.
Usamos struct para pasar de un array de bytes a una lista de numeros/strings. (https://docs.python.org/3/library/struct.html)
(La ESP32 manda los bytes en formato little-endian, por lo que los format strings deben empezar con <)

-dataSave: Guarda los datos en la BDD
-response: genera un OK para mandar de vuelta cuando se recibe un mensaje, con posibilidad de pedir que se cambie el status/protocol
-protUnpack: desempaca un byte array con los datos de un mensaje (sin el header)
-headerDict: Transforma el byte array de header (los primeros 10 bytes de cada mensaje) en un diccionario con la info del header
-dataDict: Transforma el byta array de datos (los bytes luego de los primeros 10) en un diccionario con los datos edl mensaje

'''

    
def response(change:bool=False, status:int=255, protocol:int=255):
    OK = 1
    CHANGE = 1 if change else 0
    return pack("<BBBB", OK, CHANGE, status, protocol)

def parseData(packet):
    print(f"packet length = {len(packet)}")
    header = packet[:12]
    data = packet[12:]
    headerD = headerDict(header)
    # print(f" headerD[length] = {headerD['length']}")
    print(f" len(data) = {len(data)}")
    if (headerD["length"]!=len(data)-1):
        print("No llego correctamente la data, desechar paquete")
        return (None,None)

    else:
        print(headerD)
        dataD = dataDict(headerD["protocol"], data)
        #if dataD is not None:
        #    dataSave(headerD, dataD)
        #print(header, data)
        #return None if dataD is None else {**headerD, **dataD}
        return None if dataD is None else (headerD,dataD)

    # protocol_unpack = ["<BBl", "<BBlBfBf", "<BBlBfBff","<BBlBfBff6f","<BBlBfBf4800f"]
    # protocol_unpack = ["<B", "<Bl", "<BlBfBf"]
    

def protUnpack(protocol:int, data):
    print(f"protUnpack bytes in data = {len(data)}")

    protocol_unpack = ["<BBl", "<BBlBfBf", "<BBlBfBff","<BBlBfBff6f","<BBlBfBf2000f2000f2000f"]
   

   


    return unpack(protocol_unpack[protocol], data)

def headerDict(data):
    print(f"bytes in header = {len(data)}")
    size=calcsize("<2s6BBBH")
    print(f"size of regex used= {size}")
    # "<2s6B2BH"
    # "<2s6BBB2B"
    ID_Dev,M1, M2, M3, M4, M5, M6, TransportL,ID_protocol, leng_msg = unpack("<2s6BBBH", data)
    
   
    print("unpack realizado")
    ID_Dev = ID_Dev.decode('ascii')
    MAC = ".".join([hex(x)[2:] for x in [M1, M2, M3, M4, M5, M6]])
    ID_protocol2=int(chr(ID_protocol))
    TransportL2=int(chr(TransportL))
    return_dict={"ID_Dev":ID_Dev,"MAC":MAC, "protocol":ID_protocol2, "transport":TransportL2, "length":leng_msg}
    
    return return_dict

def dataDict(protocol, data):
    print(f" Llego el protocolo = {protocol}")
    
    
    if protocol not in [0,1,2,3,4]:
        print("Error: protocol doesnt exist")
        return None
    
    def protFunc(protocol, keys):
        def p(data):
            unp = protUnpack(protocol, data)
            # print(unp)
           
            if (protocol==4):
                
                unp=list(unp)
                lista_floatsACCx=unp[7:2007]
                lista_floatsACCy=unp[2007:4007]
                lista_floatsACCz=unp[4007:]
        
                stringfloatAccx="["+";".join(map(str,lista_floatsACCx))+"]"
                stringfloatAccy="["+";".join(map(str,lista_floatsACCy))+"]"
                stringfloatAccz="["+";".join(map(str,lista_floatsACCz))+"]"
                
                unp= unp[0:7]
                unp.append(stringfloatAccx)
                unp.append(stringfloatAccy)
                unp.append(stringfloatAccz)
               
            
            return {key:val for (key,val) in zip(keys, unp)}
            
        return p

    p0 = ["Val","Batt","Timestamp"]
    p1 = ["Val","Batt","Timestamp","Temp","Press","Hum","Co"]
    p2 = ["Val","Batt","Timestamp","Temp","Press","Hum","Co","RMS"]
    p3 = ["Val","Batt","Timestamp","Temp","Press","Hum","Co","RMS","Ampx","Frecx","Ampy","Frecy","Ampz","Frecz"]
    p4 = ["Val","Batt","Timestamp","Temp","Press","Hum","Co","Accx","Accy","Accz"]
    p = [p0, p1, p2,p3,p4]

    try:
        data_dict = protFunc(protocol, p[protocol])(data)
        # print(data_dict)
        return data_dict
    except Exception:
        print("Data unpacking Error:", traceback.format_exc())
        return None