import sqlite3 as sql
from DatabaseWork2 import dataSave,getConfig
conn = sql.connect("DBakan.sqlite")
cur = conn.cursor()

data = {}
header = {"ID_Dev":'D1',"protocol":0,"status":21,}
''''
cur.execute(createDatos)
cur.execute(createLogs)
cur.execute(createConfig)
cur.execute(initConfig)
conn.commit()
conn.close()''