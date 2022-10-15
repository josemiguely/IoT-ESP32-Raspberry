import json
import sqlite3 as sql

# Documentación https://docs.python.org/3/library/sqlite3.html

## Corregir los data tuple cuando se termine Desempaquetamiento.py

def dataSave(header, data):
    protocol=header["protocol"]

    try:
        sqliteConnection = sql.connect('DB.sqlite')
        cursor = sqliteConnection.cursor()
    except Exception:
        print("No se logro conectar a la base de datos")
        return 

    if  protocol == 0:
        print("Entró protocolo 0 en dataSave")
        # Tabla Datos
        try:

            sqlite_insert_with_param = """INSERT INTO Datos (MessageId,
    IdDevice,
    MAC,
    Timestamp,
    Data1,
    Data2,
    Data3,
    Data4,
    Data5,
    Data6,
    Data7,
    Data8,
    Data9,
    Data10,
    Data11,
    Data12,
    Data13,
    Data14,
    Data15,
    Data16,
    Data17) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            ## Poner la configuracion segun el parser
            print("DATAAAAAA = ",end=" ")
            print(data)
            data_tuple = (header["ID_Dev"], header["MAC"],data["Timestamp"],data["Val"], data["Batt"],data["Timestamp"], None, None, None, None, None, None, None, None, None, None, None, None, None, None) # -> Corregir
            cursor.excecute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            cursor.close
        except sql.Error as error:
            print("Failed to insert Python variable into sqlite table", error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()

    elif protocol == "1":
    # Tabla Datos
        try:

            sqlite_insert_with_param = """INSERT INTO Datos (MessageId, IdDevice, MAC, Timestamp, Data1, Data2, Data3, Data4, Data5, Data6, Data7) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            ## Poner la configuracion segun el parser
            data_tuple = (data[0], data[1], data[2], data[3]) # -> Corregir
            cursor.excecute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            cursor.close
        except sql.Error as error:
            print("Failed to insert Python variable into sqlite table", error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()


    # elif protocol == "2":
    # Tabla Datos
        try:
    
            sqlite_insert_with_param = """INSERT INTO Datos (MessageId, IdDevice, MAC, Timestamp, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            ## Poner la configuracion segun el parser
            data_tuple = (data[0], data[1], data[2], data[3]) # -> Corregir
            cursor.excecute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            cursor.close
        except sql.Error as error:
            print("Failed to insert Python variable into sqlite table", error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()

    elif protocol == "3":
    # Tabla Datos
        try:
            
            sqlite_insert_with_param = """INSERT INTO Datos (MessageId, IdDevice, MAC, Timestamp, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8, Data9, Data10, Data11, Data12, Data13, Data14) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            ## Poner la configuracion segun el parser
            data_tuple = (data[0], data[1], data[2], data[3]) # -> Corregir
            cursor.excecute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            cursor.close
        except sql.Error as error:
            print("Failed to insert Python variable into sqlite table", error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()

    elif protocol == "4":
    # Tabla Datos
        try:
    

            sqlite_insert_with_param = """INSERT INTO Datos (MessageId, IdDevice, MAC, Timestamp, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8, Data9, Data10, Data11, Data12, Data13, Data14) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
            ## Poner la configuracion segun el parser
            data_tuple = (data[0], data[1], data[2], data[3]) # -> Corregir
            cursor.excecute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            cursor.close
        except sql.Error as error:
            print("Failed to insert Python variable into sqlite table", error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()


    # Tabla Logs (Para todos)
    try:
        
        cursor = sqliteConnection.cursor()
        
        sqlite_insert_with_param = """INSERT INTO Datos (MessageId, IdDevice, MAC, Timestamp, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data15, Data16, Data17) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
        ## Rellenar con los nombres segun el parser
        data_tuple = ()
        cursor.execute(sqlite_insert_with_param, data_tuple)
        sqliteConnection.coomit()
        cursor.close()
    except sql.Error as error:
        print("Failed to insert variable into sqlite table", error)
    finally:
        if sqliteConnection:
            sqliteConnection.close()
