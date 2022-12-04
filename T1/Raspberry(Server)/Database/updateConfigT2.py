import sqlite3 as sql
import sys
#from numpy import insert


def insertConfigT2(configuration):


    id_device=configuration["id_device"]
    status_conf=configuration["status_conf"]
    protocol_conf=configuration["protocol_conf"]
    acc_sampling=configuration["acc_sampling"]
    acc_sensibility=configuration["acc_sensibility"]
    gyro_sensibility=configuration["gyro_sensibility"]
    bme_sampling=configuration["bme_sampling"]
    discontinous_time=configuration["discontinous_time"]
    tcp_port=configuration["tcp_port"]
    udp_port=configuration["udp_port"]
    host_ip=configuration["host_ip"]
    ssid=configuration["ssid"]
    passw=configuration["pass"]


    if protocol_conf not in ['0', '1', '2' ,'3', '4']:
        print("Protocolo invalido, validos son: 0 1 2 3 4")
  
    else:
        insert=f'''
        INSERT into configuration
            (Id_device,
            Status_conf,
            Acc_sampling,
            Acc_sensibility,
            Gyro_sensibility,
            BME688_sampling,
            Discontinous_time,
            TCP_PORT,
            UDP_port,
            Host_ip_addr,
            Ssid,
            Pass)
            VALUES(?,?,?,?,?,?,?,?,?,?,?,?)
        '''

        # conn = sq.connect("DB.sqlite")
        conn = sql.connect('../Database/DBakan.sqlite')
        cur = conn.cursor()
        ## Poner la configuracion segun el parser
        data_tuple = (id_device,status_conf, protocol_conf, acc_sampling,acc_sensibility,gyro_sensibility,bme_sampling,discontinous_time,tcp_port,udp_port,host_ip,ssid,passw)
        cur.execute(insert)
        conn.commit()
        print(f"Insertada nueva configuracion de id_device={id_device}. Se ingreso protocol = {protocol_conf}, status = {status_conf} ...")
        conn.close()


def updateConfigT2(configuration):


    id_device=configuration["id_device"]
    status_conf=configuration["status_conf"]
    protocol_conf=configuration["protocol_conf"]
    acc_sampling=configuration["acc_sampling"]
    acc_sensibility=configuration["acc_sensibility"]
    gyro_sensibility=configuration["gyro_sensibility"]
    bme_sampling=configuration["bme_sampling"]
    discontinous_time=configuration["discontinous_time"]
    tcp_port=configuration["tcp_port"]
    udp_port=configuration["udp_port"]
    host_ip=configuration["host_ip"]
    ssid=configuration["ssid"]
    passw=configuration["pass"]


    if protocol_conf not in ['0', '1', '2' ,'3', '4']:
        print("Protocolo invalido, validos son: 0 1 2 3 4")
  
    else:
        update=f'''
        UPDATE configuration
        SET Status_conf={status_conf},
            Acc_sampling={acc_sampling}
            Acc_sensibility={acc_sensibility}
            Gyro_sensibility={gyro_sensibility}
            BME688_sampling={bme_sampling}
            Discontinous_time={discontinous_time}
            TCP_PORT={tcp_port}
            UDP_port={udp_port}
            Host_ip_addr={host_ip}
            Ssid={ssid}
            Pass={passw}
        WHERE Id_device={id_device}
        '''

        # conn = sq.connect("DB.sqlite")
        conn = sql.connect('../Database/DBakan.sqlite')
        cur = conn.cursor()
        cur.execute(update)
        conn.commit()
        print(f"se updateo configuracion de id_device={id_device}. Se ingreso protocol = {protocol_conf}, status = {status_conf} ...")
        conn.close()
    