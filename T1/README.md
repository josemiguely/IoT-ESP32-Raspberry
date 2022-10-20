# Tarea1 IoT
Para este proyecto se debe generar una red WiFi a través de una Raspberry Pi3, luego génerar un server TCP y un server UDP y una base de datos donde almacenar los datos enviados por una ESP32.  
En un ESP32 se debe programar un cliente TCP y un cliente UDP, el cuál va a generar datos que serán enviados a la Raspberry.

# Instrucciones de uso
## ESP32


## Raspberry


---
## Estructura del Proyecto

```
Proyecto
|  
|--- Raspberry-|--- TCP Server   
|              |--- UDP Server  
|              |--- Database
|  
|--- ESP32-|---TCP Client
           |---UDP Client  
```

## Flujo de datos
1. El microcontrolador inicia a través de conexión TCP.
2. Se realiza una consulta a la base de datos sobre el valor de la variable *ID_protocol* y *Transport_Layer*, con las cuáles se iniciarán el protocolo de paquete y tipo de conección (TCP o UDP) que se deberá trabajar.
3. Con esto obtenido se realizará el envio de paquetes con lo indicado, con la siguiente especificación:  
    - **En caso de usar TCP:** Se enviará el paquete de datos y entrara en modo DeepSleep por 60 segundos, tras lo cual repetirá el proceso.
    - **En caso de usar UDP:** Se enviarán los datos sin parar hasta que el valor de *Transport_Layer* cambie, este deberá poder ser cambiado manualmente.
4. Los paquetes recibidos por la Raspberry deberán ser abiertos y guardados dentro de una base de datos de SQL (SQLite).

### Comando para cambiar de protocolo/transport layer

Para cambiar entre protocolo/transport layer se puede ejecutar el archivo updateConfig.py.
Ejemplo de uso: ```python updateConfig.py <protocolo> <transport_layer>```. La variable protocol va entre 0 y 4 y para transport_layer entre 0 y 1 , siendo 0 TCP y 1 UDP.


## Base de datos
La base de datos contiene las siguientes tablas:
- **Datos:** Esta tabla posee todos los datos recibidos en su respectivo timestamp y el identificador del dispositivo (Id_debice y MAC).
- **Logs:** Esta tabla posee la información de cada conexión que haya recibido el servidor, teniendo el ID del dispositivo, el tipo de transport layer, su protocolo y timestamp.
- **Configuración:** Esta tabla contendrá las variables *ID_protocol* y *Transport_Layer* con las cuales se configurara el envió de datos al inicial la conexión de la ESP32. Estos deben poder cambiarse de alguna forma.  
\* *En la base de datos se usa 0 para representar a TCP y 1 para representar a UDP*

## Generación de datos
Para generar los datos se implementarán las siguientes funciones que emularán el funcionamiento de los sensores, usando las siguientes indicaciones:
- **Accelometer_Sensor:** Un medidor de aceleración genera un vector de 2000 datos por eje. Estos serán floats generados por la siguiente formula ($n$ es un número cualquiera, deben ser distintos entre datos del mismo eje):
    - $Acc_x = 2 \cdot\sin(2\pi\cdot0.001\cdot n)$
    - $Acc_x = 3 \cdot\cos(2\pi\cdot0.001\cdot n)$
    - $Acc_x = 10\cdot\sin(2\pi\cdot0.001\cdot n)$
- **THPC_Sensor** (Temperatura-Humedad-Presión-$CO_2$): representa un sensor de cada uno de estos aspectos, genere su medición usando los siguientes valores:
    - *Temp* = Valor aleatorio entre 5.0 y 30.0
    - *Hum* = Valor aleatorio entre 30 a 80
    - *Pres* = Valor aleatorio entre 1000 y 1200
    - $CO_2$ = Valor aleatorio entre 30.0 y 200.0

- **Batt_sensor:** representando el nivel de bateria del aparato, deberá ser un valor entre 1 y 100

- **Acelerometer_kpi:** representa un sensor de vibraciones, midiendo en los tres ejes y sacando su promedio, para sus valores use:
    - $Amp_x$ = valor aleatorio entre 0.0059 y 0.12
    - $Freq_x$ = valor aleatorio entre 29.0 y 31.0
    - $Amp_y$ = valor aleatorio entre 0.0041 y 0.11
    - $Freq_y$ = valor aleatorio entre 59.0 y 61.0
    - $Amp_z$ = valor aleatorio entre 0.008 y 0.15
    - $Freq_z$ = valor aleatorio entre 89.0 y 91.0
    - $RMS = \sqrt{(Amp_x^2+Amp_y^2+Amp_z^2)}$

## Forma de los paquetes
### Header
| ID Device | MAC | Transport Layer | ID Porotocol | Leng Msg |
| -- | -- | -- | -- | -- |
| 2 bytes | 6 bytes | 1 byte | 1 byte | 2 bytes |

### Data
| ID Protocol | Leng Msg (bytes) | Data 1 | Data 2 | Data 3 | Data 4 | Data 5 | Data 6 | Data 7 |
| -- | -- | :--: | :--: | :--: | :--: | :--: | :--: | :--: |
| - | - | 1 byte | 1 byte | 4 bytes | 10 bytes | 4 bytes | 24 bytes | 24000 bytes |
| 0 | 6 | Val: 1 | Batt level | Timestamp | - | - | - | - | - |
| 1 | 16 | Val: 1 | Batt level | Timestamp | THPC Sensor | - | - | - |
| 2 | 20 | Val: 1 | Batt level | Timestamp | THPC Sensor | RMS | - | - |
| 3 | 44 | Val: 1 | Batt level | Timestamp | THPC Sensor | RMS | Accelometer kpi | - |
| 4 | 24016 | Val: 1 | Batt level | Timestamp | THPC Sensor | RMS | - | Accelometer Sensor |