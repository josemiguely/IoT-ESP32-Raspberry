import sqlite3 as sql
import os
from datetime import datetime

def getFromData1(data,dispositivo):
    try:
        sqliteConnection = sql.connect('./Raspberry(Server)/Database/DBakan.sqlite')
        #sqliteConnection = sql.connect('./DBakan.sqlite')
        cursor = sqliteConnection.cursor()
    except Exception as e:
        print("Error en getData1")
        print(e)
        return 

    try:    
            '''getlogid = f"""
            SELECT Id_device 
            FROM Log WHERE configuration_Id_device = {dispositivo};"""
            ## Poner la configuracion segun el parser
            cursor.execute(getlogid)
            result = list(cursor.fetchall())
            fun1 = lambda x: x != None
            fun2 = lambda x: x[0]
            r1 =  list(filter(fun1,list(map(fun2,result))))
            print(r1)
            str_sql = "OR Log_Id_device = ? "*(len(r1)-1)
            ques = " ? "+",? "*(len(r1)-1)
            v = str(tuple(map(str,r1)))'''
            sqlite_select = f"""
            SELECT {data} FROM Data_1;""" #WHERE Log_Id_device =? """+str_sql+" "+v+";"

            ## Poner la configuracion segun el parser
            #print(sqlite_select,*r1)
            cursor.execute(sqlite_select)
            result = list(cursor.fetchall())
           # print ("result",result)
            fun1 = lambda x: x != None
            fun2 = lambda x: x[0]
            r =  list(filter(fun1,list(map(fun2,result))))
            sqliteConnection.close()
            return r
     
    except sql.Error as error:
            print(f"Error al obtener {data}",error)
    finally:
            if sqliteConnection:
                sqliteConnection.close()




def getFromData2(data,dispositivo):
    try:
        sqliteConnection = sql.connect('./Raspberry(Server)/Database/DBakan.sqlite')#sql.connect('../Database/DBakan.sqlite')
        #sqliteConnection = sql.connect('./DBakan.sqlite')
        cursor = sqliteConnection.cursor()
    except Exception as e:
        print("Error en getData2")
        print(e)
        return 

    try:

            '''getlogid = f"""
            SELECT Id_device 
            FROM Log WHERE configuration_Id_device = {dispositivo};"""
            ## Poner la configuracion segun el parser
            cursor.execute(getlogid)
            result = list(cursor.fetchall())
            fun1 = lambda x: x != None
            fun2 = lambda x: x[0]
            print(result)
            r1 =  list(filter(fun1,list(map(fun2,result))))
            print(r1)
            str_sql = "OR Log_Id_device = ? "*(len(r1)-1)
            ques = " ?"+",? "*(len(r1)-1)'''
            sqlite_select = f"""
            SELECT {data} FROM Data_2"""# WHERE Log_Id_device =? """+str_sql+"VALUES("+ques+");"
            ## Poner la configuracion segun el parser
            cursor.execute(sqlite_select)
            result = list(cursor.fetchall())
            fil = lambda x: x != None
            val = lambda x: x[0]
            r = list(filter(fil,list(map(val,result))))
            clear = lambda x: list(map(float,x[1:-1].split(";")))
            #floatinator = lambda y: float(y)
            newlist = list(map(clear,r))
            sqliteConnection.close()
            return newlist
     
    except sql.Error as error:
            print(f"Error al obtener {data} ",error)
    finally:
            if sqliteConnection:
                sqliteConnection.close()


def getIDdevices():
    try:
        sqliteConnection = sql.connect('./Raspberry(Server)/Database/DBakan.sqlite')
        #sqliteConnection = sql.connect('./DBakan.sqlite')
        cursor = sqliteConnection.cursor()
    except Exception as e:
        print("Error en getIDdevices")
        print(e)
        return 

    try:

           

            sqlite_select = f"""
            SELECT Id_device FROM configuration;"""
            ## Poner la configuracion segun el parser
            cursor.execute(sqlite_select)
            result = list(cursor.fetchall())
            fun1 = lambda x: x != None
            fun2 = lambda x: x[0]
            r =  list(filter(fun1,list(map(fun2,result))))
            sqliteConnection.close()
            return r
     
    except sql.Error as error:
            print(f"Error al obtener los ids",error)
    finally:
            if sqliteConnection:
                sqliteConnection.close()

#print(getFromData1("Press"))
#print(getFromData2("Rgyr_x"))
#print(len(*getFromData2("Rgyr_x")))