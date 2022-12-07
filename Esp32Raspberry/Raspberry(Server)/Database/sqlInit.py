createDatos = '''CREATE TABLE Datos (
    MessageId INTEGER PRIMARY KEY,
    IdDevice INTEGER NOT NULL,
    MAC TEXT NOT NULL,
    Datetime DATETIME DEFAULT CURRENT_TIMESTAMP,
    Val INTEGER,
    Batt FLOAT,
    Timestamp FLOAT,
    Temp FLOAT,
    Press FLOAT,
    Hum FLOAT,
    Co FLOAT,
    RMS FLOAT,
    AmpX FLOAT,
    FrecX FLOAT,
    AmpY FLOAT,
    FrecY FLOAT,
    AmpZ FLOAT,
    FrecZ FLOAT,
    AccX TEXT,
    AccY TEXT,
    AccZ TEXT
);'''

createLogs = '''CREATE TABLE Logs (
    MessageId INTEGER PRIMARY KEY,
    IdDevice INTEGER NOT NULL,
    Transport_Layer INTEGER NOT NULL,
    Protocol INTEGER NOT NULL,
    Datetime DATETIME DEFAULT CURRENT_TIMESTAMP,
    Timestamp FLOAT 
    
);'''

createConfig = '''CREATE TABLE Config (
    Id INTEGER  PRIMARY KEY,
    ID_protocol INTEGER NOT NULL,
    Transport_Layer INTEGER NOT NULL,
    unique(Id,ID_protocol,Transport_Layer)
);'''

initConfig='''
INSERT INTO Config (Id, ID_protocol, Transport_Layer)
VALUES (1,0,0);'''

#conn = sq.connect("DB.sqlite")
#cur = conn.cursor()

#print("Insertada nueva configuracion")
#conn.close()
import sqlite3 as sql



conn = sql.connect("DB.sqlite")
cur = conn.cursor()
cur.execute(createDatos)
cur.execute(createLogs)
cur.execute(createConfig)
cur.execute(initConfig)
conn.commit()
conn.close()

# inicializa la BDD