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
    print(headerD)
    dataD = dataDict(headerD["protocol"], data)
    if dataD is not None:
        dataSave(headerD, dataD)
    #print(header, data)
    return None if dataD is None else {**headerD, **dataD}
    

def protUnpack(protocol:int, data):
    print(f"bytes in data = {len(data)}")
    # protocol_unpack = ["<B", "<Bl", "<BlBfBf"]
    protocol_unpack = ["<BBBBBB", "<BBlBfBf", "<BBlBfBff","<BBlBfBff6f","<BBlBfBf6004f"]
    return unpack(protocol_unpack[protocol], data)

def headerDict(data):
    print(f"bytes in header = {len(data)}")
    size=calcsize("<12B")
    print(f"size of regex used= {size}")
    # "<2s6B2BH"
    # "<2s6BBB2B"
    # ID_Dev,ID_Dev1,M1, M2, M3, M4, M5, M6, TransportL,ID_protocol, leng_msg,leng_msg2 = unpack("<12B", data)
    ID_Dev,M1, M2, M3, M4, M5, M6, TransportL,ID_protocol, leng_msg = unpack("<2s6BBBH", data)
    MAC = ".".join([hex(x)[2:] for x in [M1, M2, M3, M4, M5, M6]])
    # ID_Dev=chr(ID_Dev)+chr(ID_Dev1)
    # ID_protocol2=int(chr(ID_protocol))
    # TransportL2=int(chr(TransportL))
    # leng_msg=int(chr(leng_msg)) #Byte menos significativo
    # leng_msg2=int(chr(leng_msg2))<<8 #Byte más singifiactivo
    # leng_msg_res=leng_msg+leng_msg2
    # return {"MAC":MAC, "protocol":ID_protocol, "status":status, "length":leng_msg}
    return {"ID_Dev":ID_Dev,"MAC":MAC, "protocol":ID_protocol, "transport":TransportL, "length":leng_msg}

def dataDict(protocol, data):
    print(f" Llego el protocolo = {protocol}")
    #protocol2 = protocol.encode('utf-8')
    #print(f" Llego el protocolo2 = {protocol2}")
    
    if protocol not in [0,1,2,3,4]:
        print("Error: protocol doesnt exist")
        return None
    
    def protFunc(protocol, keys):
        def p(data):
            unp = protUnpack(protocol, data)
            return {key:val for (key,val) in zip(keys, unp)}
        return p

    p0 = ["Val","Batt","Timestamp"]
    p1 = ["Batt_level", "Timestamp"]
    p2 = ["Batt_level", "Timestamp", "Temp", "Pres", "Hum", "Co"]
    p = [p0, p1, p2]

    try:
        data_dict = protFunc(protocol, p[protocol])(data)
        print(data_dict)
        return data_dict
    except Exception:
        print("Data unpacking Error:", traceback.format_exc())
        return None