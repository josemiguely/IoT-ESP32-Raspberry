# Version con dos threads: uno lee de stdin hacia el socket y el otro al revés
import filecmp
import socket

import jsockets
import sys, threading


def alternate():
    while True:
        yield 0
        yield 1


contador = alternate()
acknowledged = False




def int_to_string_bytearray(a: int) -> bytearray:
    return f'{a:d}'.encode('utf-8').zfill(5)


def string_bytearray_to_int(b: bytearray) -> int:
    return int(b.decode('utf-8'))


# Se checkea que se use el número de argumentos correctos
if len(sys.argv) != 3:
    print('Use: ' + sys.argv[0] + ' Text_File_name Max_Size')
    sys.exit(1)
# Se conecta al servidor
s = jsockets.socket_udp_connect("anakena.dcc.uchile.cl", 1818)
if s is None:
    print('could not open socket')
    sys.exit(1)

s.settimeout(0.5)  # s.settimeout(s) settea un límite de tiempo para recibir los archivos de s segundos, despues de
# los s segundos arroja una excepción lo que rompe el while true de Rdr avanzando con lo demás del código.

testFile = open('TestFile.txt',
                'w').close()  # Esto crea el archivo TestFile en blanco cada vez que se ejecuta el script

# -- Se manda el saludo y se recibe la respuesta:
saludo = "Hola"
print("Cliente envía: " + saludo)
s.send(saludo.encode('utf-8'))
respuesta1 = s.recv(5000).decode('utf-8')
print("Servidor responde: " + str(respuesta1))

# -- Se manda la propuesta de tamaño máximo y se recibe la respuesta del servidor:
bytesize = int_to_string_bytearray(int(sys.argv[2]))
print("Cliente envía: " + str(bytesize))
s.send(bytesize)

maxSize = int((s.recv(1000).decode('utf-8')))
print("Servidor responde: " + str(maxSize))

file1 = open(sys.argv[1], 'r')
enviados = 0
original = str(sys.argv[1])
while True:
    testfile = open("TestFile.txt", 'a')
    bloques = file1.read(maxSize - 4)

    if bloques == '':
        break


    acknowledged = False

    while not acknowledged:
        try:
            identificador = "[" + str(next(contador)) + "]" + " "
            numero = identificador[1]
            print("Cliente envía: " + (identificador + bloques))
            s.send((identificador + bloques).encode('utf-8'))
            data = s.recv(maxSize).decode()
            identificador_obtenido = data[:4]
            print("Identificador = " + identificador_obtenido)
            data_sin_identificador = data[4:]
            if (identificador_obtenido[1] == numero):
                acknowledged = True
                testfile.write(data_sin_identificador)
                print("Se imprime: " + data_sin_identificador + "\n", end='')
                enviados += 1
                print("Paquetes recibidos = " + str(enviados) + "\n")
                print("Servidor responde: " + data + "\n", end='')


        except socket.timeout:
            print("Cliente RE-envía: " + (identificador + bloques))
            s.send((identificador + bloques).encode('utf-8'))
            data2 = s.recv(maxSize).decode()
            identificador_obtenido2 = data2[:4]
            print("Identificador2 = " + identificador_obtenido2)
            data_sin_identificador2 = data2[4:]
            if (identificador_obtenido2[1] == numero):
                acknowledged = True
                testfile.write(data_sin_identificador2)
                print("Se imprime: " + data_sin_identificador2 + "\n", end='')
                enviados += 1
                print("Paquetes recibidos = " + str(enviados) + "\n")
                print("Servidor responde: " + data2 + "\n", end='')


# Se comparan los archivos
if filecmp.cmp(original, "TestFile.txt"):
    print("Los archivos son iguales.")
else:
    print("Los archivos son distintos.")

# Creo thread que lee desde el socket hacia stdout:


for line in sys.stdin:

    splitted = [line[i:i + maxSize] for i in range(0, len(line), maxSize)]

    for bloque in splitted:
        acknowledged = False

        while not acknowledged:
            try:
                identificador = "[" + str(next(contador)) + "]" + " "
                numero = identificador[1]
                print("Cliente envía: " + (identificador + bloque))
                s.send((identificador + bloque).encode('utf-8'))
                data = s.recv(maxSize).decode()
                identificador_obtenido = data[:4]
                print("Identificador = " + identificador_obtenido)
                data_sin_identificador = data[4:]
                if (identificador_obtenido[1] == numero):
                    acknowledged = True
                    testfile.write(data_sin_identificador)
                    print("Se imprime: " + data_sin_identificador + "\n", end='')
                    enviados += 1
                    print("Paquetes recibidos = " + str(enviados) + "\n")
                    print("Servidor responde: " + data + "\n", end='')


            except socket.timeout:
                try:
                    print("Cliente RE-envía: " + (identificador + bloque))
                    s.send((identificador + bloque).encode('utf-8'))
                    data2 = s.recv(maxSize).decode()
                    identificador_obtenido2 = data2[:4]
                    print("Identificador2 = " + identificador_obtenido2)
                    data_sin_identificador2 = data2[4:]
                    if (identificador_obtenido2[1] == numero):
                        acknowledged = True
                        testfile.write(data_sin_identificador2)
                        print("Se imprime: " + data_sin_identificador2 + "\n", end='')
                        enviados += 1
                        print("Paquetes recibidos = " + str(enviados) + "\n")
                        print("Servidor responde: " + data2 + "\n", end='')
                except:
                    print("multiple timeouts: Server or socket might be down")
                    acknowledged = False

