#include "sensorsT2.c"
#include <stdlib.h>
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include <string.h>
#include "esp_log.h"



static const char *TAG2 = "PACKETING";

//Entrega el largo de la data según el protocolo

unsigned short lengmsg[6] = {6, 16, 20, 44, 48016};
int loop;

unsigned short dataLength(char protocol){
    //ESP_LOGI(TAG2, " protocol EN DATALENGTH = %c\n", protocol);
    unsigned int uprotocol =((unsigned int) protocol)-48;
     //ESP_LOGI(TAG2, " uprotocol EN DATALENGTH = %u\n", uprotocol);
    return lengmsg[uprotocol]-1;
}

unsigned short messageLength(char protocol){
    return 1+12+dataLength(protocol);
}

//Genera el header de un mensaje, con la MAC, el protocolo, status, y el largo del mensaje.
char* header(char protocol, char transportLayer){
	//ESP_LOGI(TAG2, " protocol HEADER = %c\n", protocol);
    char* head = malloc(12);
    char* ID_device = malloc(2);

    //char * ID = "D1"; 
    //memcpy((void*) &(head[0]), (void*) ID, 2);
	
    uint8_t* MACaddrs = malloc(6); 
    
    

	esp_efuse_mac_get_default(MACaddrs);
     
	memcpy((void*) &(head[2]), (void*) MACaddrs, 6);
    
    for (int i = 0 ; i< 6; i++){
        printf("MACaddrs[i] = %i\n",MACaddrs[i]);
    }
    
    head[8]= transportLayer;
	head[9]= protocol;
    memcpy(ID_device,&MACaddrs[4],2);// x01xA9 


    memcpy((void*) &(head[0]), (void*) ID_device, 2);
	unsigned short dataLen = dataLength(protocol);
	memcpy((void*) &(head[10]), (void*) &dataLen, 2);
        
	free(MACaddrs);
    free(ID_device);
	return head;
}



// Arma un paquete para el protocolo de inicio, que busca solo respuesta
// char* dataprotocol00(){
//     char* msg = malloc(dataLength(0));
//     msg[0] = 1;
//     return msg;
// }

// Arma un paquete para el protocolo 0, 6 bytes
char* dataprotocol0(){
    //ESP_LOGI(TAG2, " dataLength(0) = %hu\n", dataLength('0')); 
    char* msg = malloc(dataLength('0'));
    char batt = batt_sensor();
    
	msg[0] = '1'; // Val
	msg[1] = batt; //Battery

    long t=0; //Timestamp
   
    memcpy( (void*) &(msg[2]), (void*) &t, 4);
    
    return msg;
}

// Arma un paquete para el protocolo 1, 16 bytes
char* dataprotocol1(){
    //ESP_LOGI(TAG2, " dataLength(1) = %hu\n", dataLength('1')); 
    char* msg = malloc(dataLength('1'));

    char batt = batt_sensor();
    msg[0] = '1'; // Val
    msg[1] = batt;
    
   long t=0; //Timestamp
   
    memcpy( (void*) &(msg[2]), (void*) &t, 4);
    
    char temp = thcp_temp_sensor();
    msg[6] = temp;

    float press = thcp_pres_sensor();    
    memcpy((void*) &(msg[7]), (void*) &press, 4);
    
    char hum = thcp_hum_sensor();
    msg[11] = hum;

    float co = thcp_CO_sensor();
    memcpy((void*) &(msg[12]), (void*) &co, 4);

    return msg;
}

// Arma un paquete para el protocolo 2, 20 bytes
char* dataprotocol2(){
    //ESP_LOGI(TAG2, " dataLength(2) = %hu\n", dataLength('2')); 
    char* msg = malloc(dataLength('2'));

    char batt = batt_sensor();
    msg[0] = '1'; // Val
    msg[1] = batt;
    
    long t=0; //Timestamp
   
    memcpy( (void*) &(msg[2]), (void*) &t, 4);
    
    char temp = thcp_temp_sensor();
    msg[6] = temp;

    float press = thcp_pres_sensor();
    memcpy((void*) &(msg[7]), (void*) &press, 4);
    
    char hum = thcp_hum_sensor();
    msg[11] = hum;

    float co = thcp_CO_sensor();
    memcpy((void*) &(msg[12]), (void*) &co, 4);

    float rms2 = rms();
    memcpy((void*) &(msg[16]), (void*) &rms2, 4);

    return msg;
}

// Arma un paquete para el protocolo 3, 44 bytes
char* dataprotocol3() {
       
    char* msg = malloc(dataLength('3'));

    char batt = batt_sensor();
    msg[0] = '1'; // Val
    msg[1] = batt;
    
    long t = 0;
    memcpy((void*) &(msg[2]), (void*) &t, 4);
    
    char temp = thcp_temp_sensor();
    msg[6] = temp;

    float press = thcp_pres_sensor();
    memcpy((void*) &(msg[7]), (void*) &press, 4);
    
    char hum = thcp_hum_sensor();
    msg[11] = hum;

    float co = thcp_CO_sensor();
    memcpy((void*) &(msg[12]), (void*) &co, 4);

    float rms2 = rms();
    memcpy((void*) &(msg[16]), (void*) &rms2, 4);

    float ampx = accelerometer_kpi_amp_x();
    memcpy((void*) &(msg[20]), (void*) &ampx, 4);

    float frecx = accelerometer_kpi_frec_x();
    memcpy((void*) &(msg[24]), (void*) &frecx, 4);

    float ampy = accelerometer_kpi_amp_y();
    memcpy((void*) &(msg[28]), (void*) &ampy, 4);

    float frecy = accelerometer_kpi_frec_y();
    memcpy((void*) &(msg[32]), (void*) &frecy, 4);

    float ampz = accelerometer_kpi_amp_z();
    memcpy((void*) &(msg[36]), (void*) &ampz, 4);

    float frecz = accelerometer_kpi_frec_z();
    memcpy((void*) &(msg[40]), (void*) &frecz, 4);
      
    return msg; 
    
}

// Arma un paquete para el protocolo 4, 24016 bytes
char * dataprotocol4(){
           
    char* msg = malloc(dataLength('4'));

    char batt = batt_sensor();
    msg[0] = '1'; // Val
    msg[1] = batt;
    
    long t = 0;
    memcpy((void*) &(msg[2]), (void*) &t, 4);
    
    char temp = thcp_temp_sensor();
    msg[6] = temp;

    float press = thcp_pres_sensor();
    memcpy((void*) &(msg[7]), (void*) &press, 4);
    
    char hum = thcp_hum_sensor();
    msg[11] = hum;

    float co = thcp_CO_sensor();
    memcpy((void*) &(msg[12]), (void*) &co, 4);

    float * accx = acceloremeter_sensor_x();
    memcpy((void*) &(msg[16]), (void*) accx, 8000);
    
    float * accy = acceloremeter_sensor_y();
    memcpy((void*) &(msg[8016]), (void*) accy, 8000);

    float * accz = acceloremeter_sensor_z();
    memcpy((void*) &(msg[16016]), (void*) accz, 8000);

    float * rgyrx = rgyr_x();
    memcpy((void*) &(msg[24016]), (void*) rgyrx, 8000);
    
    float * rgyry = rgyr_y();
    memcpy((void*) &(msg[32016]), (void*) rgyry, 8000);

    float * rgyrz = rgyr_z();
    memcpy((void*) &(msg[40016]), (void*) rgyrz, 8000);

    return msg;
}


char* mensaje (char protocol, char transportLayer){
     time_t t;
     int intprotocol = ((unsigned int) protocol)-48;
    //ESP_LOGI(TAG2, " protocol in mensaje = %c\n", protocol); 
   
    /* Intializes random number generator */
    srand((unsigned) time(&t));

    char* mnsj = malloc(messageLength(protocol));
	mnsj[messageLength(protocol)-1]= '\0';
	char* hdr = header(protocol, transportLayer);
    char* data;
	switch (intprotocol) {
		
        case 0:
            ESP_LOGI(TAG2,"======= dataprotocol0 ====");
			data = dataprotocol0();
            ESP_LOGI(TAG2,"======= se salio de dataprotocol0 ====");
            break;
		case 1:
           ESP_LOGI(TAG2,"======= dataprotocol1 ====");
			data = dataprotocol1();
            
			break;
		case 2:
           ESP_LOGI(TAG2,"======= dataprotocol2 ====");
			data = dataprotocol2();
            
			break;
        case 3:
           ESP_LOGI(TAG2,"======= dataprotocol3 ====");
			data = dataprotocol3();
            
			break;
        case 4:
           ESP_LOGI(TAG2,"======= dataprotocol4 ====");
			data = dataprotocol4();
            
			break;
		default:
            ESP_LOGI(TAG2,"======= dataprotocol0 default ====");
            data = dataprotocol0();
            break;
	}
	memcpy((void*) &(mnsj[0]), (void*) hdr, 12);
	memcpy((void*) &(mnsj[12]), (void*) data, dataLength(protocol));
    free(hdr);
    free(data);
    return mnsj;
}