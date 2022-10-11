create = '''CREATE TABLE Info (
    MessageId INTEGER PRIMARY KEY,
    MAC TEXT NOT NULL,
    Status INTEGER NOT NULL,
    Protocol INTEGER NOT NULL,
    Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    Data1 INTEGER,
    Data2 FLOAT,
    Data3 FLOAT
    
);'''

import sqlite3 as sql


conn = sql.connect("DB.sqlite")
cur = conn.cursor()
cur.execute(create)
conn.close()

# inicializa la BDD