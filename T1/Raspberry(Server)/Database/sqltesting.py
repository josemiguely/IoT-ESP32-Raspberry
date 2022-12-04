import sqlite3 as sq
import sys
#from numpy import insert

        insert=f'''
        
        
        '''

        conn = sq.connect("DB.sqlite")
        cur = conn.cursor()
        cur.execute(insert)
        conn.commit()
        conn.close()
else:
    print("Faltan parametros, modo de uso: PROTOCOL TRANSPOR_LAYER")    