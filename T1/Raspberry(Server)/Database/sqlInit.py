createDatos = '''CREATE TABLE Datos (
    MessageId INTEGER PRIMARY KEY,
    IdDevice INTEGER NOT NULL,
    MAC TEXT NOT NULL,
    Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    Data1 INTEGER,
    Data2 FLOAT,
    Data3 FLOAT,
    Data4 FLOAT,
    Data5 FLOAT,
    Data6 FLOAT,
    Data7 FLOAT,
    Data8 FLOAT,
    Data9 FLOAT,
    Data10 FLOAT,
    Data11 FLOAT,
    Data12 FLOAT,
    Data13 FLOAT,
    Data14 FLOAT,
    Data15 FLOAT,
    Data16 FLOAT,
    Data17 FLOAT
);'''

createLogs = '''CREATE TABLE Logs (
    MessageId INTEGER PRIMARY KEY,
    IdDevice INTEGER NOT NULL,
    Transport_Layer INTEGER NOT NULL,
    Protocol INTEGER NOT NULL,
    Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
    
);'''

createConfig = '''CREATE TABLE Config (
    Id INTEGER PRIMARY KEY,
    ID_protocol INTEGER NOT NULL,
    Transport_Layer INTEGER NOT NULL
);'''

import sqlite3 as sql


conn = sql.connect("DB.sqlite")
cur = conn.cursor()
cur.execute(createDatos)
cur.execute(createLogs)
cur.execute(createConfig)
conn.close()

# inicializa la BDD