import sqlite3 as sq

#from numpy import insert



update='''
UPDATE Config
SET ID_protocol=4,
    Transport_layer=1
WHERE Id=1
'''

conn = sq.connect("DB.sqlite")
cur = conn.cursor()
cur.execute(update)
conn.commit()
print("Insertada nueva configuracion")
conn.close()