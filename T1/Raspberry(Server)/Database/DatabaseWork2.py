import json
import sqlite3 as sql
import os
from datetime import datetime


# timestamp from now
def tsNow() -> float: 
    ''' return the timestamp from now in milliseconds''' 
    d = datetime.now()
    ts = d.timestamp()   # timestamp in milliseconds
    return ts


def dataSave(header, data):
    '''
    Save de data in the correspondant database table
    '''
    protocol=header["protocol"]
    ts = tsNow()
    path=os.getcwd()
    try:
        sqliteConnection = sql.connect('./Database/DBakan.sqlite')
        cursor = sqliteConnection.cursor()
    except Exception as e:
        print("Error en dataSave")
        print(e)
        return 

    if  protocol == 0:
        print("Entró protocolo 0 en dataSave")
        # Tabla Datos
        '''
        try:
            
            sqlite_insert_with_param = """
            INSERT INTO Data_1 (
            Id_device,
            VALUES(?);"""
            ## Poner la configuracion segun el parser
            
            
            data_tuple = header["ID_Dev"]
            cursor.execute(sqlite_insert_with_param, data_tuple)
            sqliteConnection.commit()
            
            
        except sql.Error as error:
            print("Error al insertar en protocolo 0",error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()'''

    elif protocol == 1:
    # Tabla Datos
        try:
            print("Entró protocolo 1 en dataSave")
            sqlite_insert_with_param = """
            INSERT INTO Data_1 (
            Temperature,
            Press,
            Hum,
            Co,
            Time_client) VALUES(?, ?, ?, ?,?);"""
            ## Poner la configuracion segun el parser
            data_tuple = (data["Temp"], data["Press"], data["Hum"],data["Co"],data["Timestamp"])
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
            INSERT INTO Data_1 (
            Temperature,
            Press,
            Hum,
            Co,
            RMS,
            Time_client) VALUES(?, ?, ?, ?,?,?);"""
            data_tuple = (data["Temp"], data["Press"], data["Hum"],data["Co"],data["RMS"],data["Timestamp"])
           
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
            INSERT INTO Data_1 (
            Temperature,
            Press,
            Hum,
            Co,
            RMS,
            Amp_x,
            Freq_x,
            Amp_y,
            Freq_y,
            Amp_z,
            Freq_z,
            Time_client) VALUES(?, ?, ?, ?,?,?,?,?,?,?,?,?);"""
            data_tuple = (data["Temp"], data["Press"], 
                        data["Hum"],data["Co"],data["RMS"],data["Ampx"],
                        data["Frecx"],data["Ampy"],data["Frecy"],data["Ampz"],
                        data["Frecz"],data["Timestamp"])
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

            sqlite_insert_with_param1 = """
            INSERT INTO Data_1 (
            Temperature,
            Press,
            Hum,
            Co,
            Time_client) VALUES(?, ?, ?, ?,?);"""
            ## Poner la configuracion segun el parser
            data_tuple1 =data_tuple = (data["Temp"], data["Press"],
                                    data["Hum"],data["Co"],data["RMS"],data["Timestamp"])
            cursor.execute(sqlite_insert_with_param1, data_tuple1)

            sqlite_insert_with_param2 = """
            INSERT INTO Data_2 (
            Racc_x,
            Racc_y,
            Racc_z,
            Rgyr_x,
            Rgyr_y,
            Rgyr_z,
            Time_client) VALUES(?, ?, ?, ?, ?, ?,?);"""
            ## Poner la configuracion segun el parser
            data_tuple2 = (data["Accx"], data["Accy"], data["Accz"],
                           data["gyrx"], data["gyry"], data["gyrz"])
            cursor.execute(sqlite_insert_with_param2, data_tuple2)

            sqliteConnection.commit()
            
        except sql.Error as error:
             print("Error al insertar en protocolo 4",error)
        finally:
            if sqliteConnection:
                sqliteConnection.close()

def saveLogs(header,data):
    # Tabla Logs (Para todos)
    try:
        sqliteConnection = sql.connect('./Database/DBakan.sqlite')
        cursor = sqliteConnection.cursor()
    except Exception as e:
        print("Error en saveLogs")
        print(e)
        return 
    ts = tsNow()
    try:
        
         cursor = sqliteConnection.cursor()
        
         sqlite_insert_with_param = """
         INSERT INTO Logs (
            Status_report,
            Protocol_report,
            Battery_Level,
            Conf_peripheral,
            configuration_Id_device
            ) 
         VALUES(?, ?, ?, ?, ?);"""
         
         data_tuple = (header["status"],header["protocol"],data["Batt"],None,header["ID_Dev"])
         cursor.execute(sqlite_insert_with_param, data_tuple)
         sqliteConnection.commit()
         cursor.close()
    except sql.Error as error:
         print("Failed to insert variable into sqlite table", error)
    finally:
         if sqliteConnection:
             sqliteConnection.close()

         
def getConfig():
    '''
    0 = Status_conf,
    1 = Protocol_conf,
    2 = Acc_sampling,
    3 = Acc_sensibility,
    4 = Gyro_sensibility,
    5 = BME688_sampling,
    6 = Discontinuos_time,
    7 = TCP_PORT,
    8 = UDP_port,
    9 = Host_ip_addr,
    10 = Ssid,
    11 = Pass
    '''
    try:
        conn=sql.connect('./Database/DBakan.sqlite')
        
        cur = conn.cursor()

        cur.execute("""
        SELECT 
            Status_conf,
            Protocol_conf,
            Acc_sampling,
            Acc_sensibility,
            Gyro_sensibility,
            BME688_sampling,
            Discontinuos_time,
            TCP_PORT,
            UDP_port,
            Host_ip_addr,
            Ssid,
            Pass
        FROM Config
        """)
        row = cur.fetchone()
        # Checkear como se entregan los datos
        print(row)
        #id_protocol = row[0]
        #transport_layer = row[1]
        return row   #row[0] = 

    except Exception as e:
        
        print("Algo salio mal en getConfig uwu")
        print(e)
        return