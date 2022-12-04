import sqlite3 as sq
import sys
#from numpy import insert

insert=f'''INSERT INTO configuration (Id_device,Status_conf, Protocol_conf, Acc_sampling, Acc_sensibility, Gyro_sensibility, BME688_sampling, Discontinuos_time, TCP_PORT, UDP_port, Host_ip_addr, Ssid, Pass)
VALUES (2,21,9,1,2,3,4,1,3000,3001,190229,"Wifibakan","pass");'''

conn = sq.connect("DB.sqlite")
cur = conn.cursor()
cur.execute(insert)
conn.commit()
conn.close()