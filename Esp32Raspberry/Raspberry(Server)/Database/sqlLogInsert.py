import sqlite3 as sq
import sys
#from numpy import insert

insert=f'''INSERT INTO Log (Status_report, Protocol_report, `Battery_Level`, Conf_peripheral, configuration_Id_device)
VALUES (21,0,27,1,3);'''

conn = sq.connect("DB.sqlite")
cur = conn.cursor()
cur.execute(insert)
conn.commit()
conn.close()
