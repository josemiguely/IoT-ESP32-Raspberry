import json
import sqlite3 as sql

# Documentación https://docs.python.org/3/library/sqlite3.html

def dataSave(header, data):
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute('''insert into Info (MAC, Status, Protocol, Data1) values (?, ?, ?, ?)''', (header["MAC"], header["status"], header["protocol"], json.dumps(data)))