import json
import sqlite3 as sql
import os
from datetime import datetime
# Documentación https://docs.python.org/3/library/sqlite3.html

## Corregir los data tuple cuando se termine Desempaquetamiento.py
# timestamp from now
def tsNow():
    d = datetime.now()
    #print(d)
    # timestamp in milliseconds
    ts = d.timestamp()  
    return ts

def dataSave(header, data):
    protocol=header["protocol"]
    ts = tsNow()
    path=os.getcwd()
    print(f" EL PATH PARA ABRIR ESTA EN {path}")
    try:
        sqliteConnection = sql.connect('../Database/DB.sqlite')
        cursor = sqliteConnection.cursor()
    except Exception:
        print("No se logro conectar a la base de datos")
        return 

    if  protocol == 0:
        print("Entró protocolo 0 en dataSave")
        # Tabla Datos
        try:

            sqlite_insert_with_param = """
            INSERT INTO Datos (
            IdDevice,
            MAC,
            Val,
            Batt,
            Timestamp,
            Temp,
            Press,
            Hum,
            Co,
            RMS,
            AmpX,
            FrecX,
            AmpY,
            FrecY,
            AmpZ,
            FrecZ,
            AccX,
            AccY,
            AccZ) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            ## Poner la configuracion segun el parser
            print("DATAAAAAA = ",end=" ")
            print(data)
            
            data_tuple = (header["ID_Dev"], header["MAC"],int(chr(data["Val"])), data["Batt"],ts, None, None, None, None, None, None, None, None, None, None, None, None, None, None)
            cursor.execute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            
            
        except sql.Error as error:
            print("Error al insertar en protocolo 0",error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()

    elif protocol == 1:
    # Tabla Datos
        try:
            print("Entró protocolo 1 en dataSave")
            sqlite_insert_with_param = """
            INSERT INTO Datos (
            IdDevice,
            MAC,
            Val,
            Batt,
            Timestamp,
            Temp,
            Press,
            Hum,
            Co,
            RMS,
            AmpX,
            FrecX,
            AmpY,
            FrecY,
            AmpZ,
            FrecZ,
            AccX,
            AccY,
            AccZ) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            ## Poner la configuracion segun el parser
            data_tuple = (header["ID_Dev"], header["MAC"],int(chr(data["Val"])), data["Batt"],ts, data["Temp"], data["Press"], data["Hum"], data["Co"], None, None, None, None, None, None, None, None, None, None)
            cursor.execute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            
        except sql.Error as error:
            print("Error al insertar en protocolo 1",error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()


    elif protocol == 2:
    # Tabla Datos
        try:
            print("Entró protocolo 2 en dataSave")
            
            sqlite_insert_with_param = """
            INSERT INTO Datos (
            IdDevice,
            MAC,
            Val,
            Batt,
            Timestamp,
            Temp,
            Press,
            Hum,
            Co,
            RMS,
            AmpX,
            FrecX,
            AmpY,
            FrecY,
            AmpZ,
            FrecZ,
            AccX,
            AccY,
            AccZ) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            ## Poner la configuracion segun el parser
            data_tuple = (header["ID_Dev"], header["MAC"],int(chr(data["Val"])), data["Batt"],ts, data["Temp"], data["Press"], data["Hum"], data["Co"], data["RMS"], None, None, None, None, None, None, None, None, None)
            print(data_tuple)
            cursor.execute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
        except sql.Error as error:
             print("Error al insertar en protocolo 2",error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()

    elif protocol == 3:
    # Tabla Datos
        try:
            print("Entró protocolo 3 en dataSave")
            
            sqlite_insert_with_param = """
            INSERT INTO Datos (
            IdDevice,
            MAC,
            Val,
            Batt,
            Timestamp,
            Temp,
            Press,
            Hum,
            Co,
            RMS,
            AmpX,
            FrecX,
            AmpY,
            FrecY,
            AmpZ,
            FrecZ,
            AccX,
            AccY,
            AccZ) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            data_tuple = (header["ID_Dev"], header["MAC"],int(chr(data["Val"])), data["Batt"],ts, data["Temp"], data["Press"], data["Hum"], data["Co"], data["RMS"], data["Ampx"],  data["Frecx"], data["Ampy"], data["Frecy"], data["Ampz"], data["Frecz"], None, None, None)
            cursor.execute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            #cursor.close()
        except sql.Error as error:
             print("Error al insertar en protocolo 3",error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()

    elif protocol == 4:
    # Tabla Datos
        try:
    
            print("Entró protocolo 4 en dataSave")

            sqlite_insert_with_param = """
            INSERT INTO Datos (
            IdDevice,
            MAC,
            Val,
            Batt,
            Timestamp,
            Temp,
            Press,
            Hum,
            Co,
            RMS,
            AmpX,
            FrecX,
            AmpY,
            FrecY,
            AmpZ,
            FrecZ,
            AccX,
            AccY,
            AccZ) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            ## Poner la configuracion segun el parser
            data_tuple = (header["ID_Dev"], header["MAC"],int(chr(data["Val"])), data["Batt"],ts, data["Temp"], data["Press"], data["Hum"], data["Co"], None, None, None, None, None, None, None, data["Accx"], data["Accy"], data["Accz"])
            cursor.execute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            
        except sql.Error as error:
             print("Error al insertar en protocolo 4",error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()


def saveLogs(header):
    # Tabla Logs (Para todos)
    try:
        sqliteConnection = sql.connect('../Database/DB.sqlite')
        cursor = sqliteConnection.cursor()
    except Exception:
        print("No se logro conectar a la base de datos")
        return 
    ts = tsNow()
    try:
        
         cursor = sqliteConnection.cursor()
        
         sqlite_insert_with_param = """
         INSERT INTO Logs (
            IdDevice,
            Transport_Layer,
            Protocol,
            Timestamp
            ) 
         VALUES(?, ?, ?, ?);"""
         
         data_tuple = (header["ID_Dev"],header["transport"],header["protocol"],ts)
         cursor.execute(sqlite_insert_with_param, data_tuple)
         sqliteConnection.commit()
         cursor.close()
    except sql.Error as error:
         print("Failed to insert variable into sqlite table", error)
    finally:
         if sqliteConnection:
             sqliteConnection.close()

         
def getFromTable():
    try:
        sqliteConnection = sql.connect('../Database/DB.sqlite')
        cursor = sqliteConnection.cursor()
    except Exception:
        print("No se logro conectar a la base de datos")
        return
    finally:
        cursor.excecute("SELECT * FROM Config")
        row = cursor.fetchall()

        # Checkear como se entregan los datos
        print(row)
        id_protocol = row[0]
        transport_layer = row[1]
        return id_protocol, transport_layer   #row[0] = 