import sqlite3 as sq

#from numpy import insert



insertar='''
UPSERT INTO Config (Id, ID_protocol, Transport_Layer)
VALUES (1,0,0);'''

conn = sq.connect("DB.sqlite")
cur = conn.cursor()
cur.execute(insertar)
conn.commit()
print("Insertada nueva configuracion")
conn.close()