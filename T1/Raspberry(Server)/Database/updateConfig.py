import sqlite3 as sq
import sys
#from numpy import insert
if len(sys.argv) == 3:
    protocol = sys.argv[1]
    transport_layer = sys.argv[2]

    if protocol not in ['0', '1', '2' ,'3', '4']:
        print("Protocolo invalido, validos son: 0 1 2 3 4")
    elif transport_layer not in ['0' ,'1']:
        print("Tasnport layer invalido, validos son: 0:TCP 1:UDP")
    else:
        update=f'''
        UPDATE Config
        SET ID_protocol={protocol},
            Transport_layer={transport_layer}
        WHERE Id=1
        '''

        conn = sq.connect("DB.sqlite")
        cur = conn.cursor()
        cur.execute(update)
        conn.commit()
        print(f"Insertada nueva configuracion protocol = {protocol}, transport_layer = {transport_layer}")
        conn.close()
else:
    print("Faltan parametros, modo de uso: PROTOCOL TRANSPOR_LAYER")    