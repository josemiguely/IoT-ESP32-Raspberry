
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_sleep.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "gatts_table_creat_demo.h"

#include "esp_gatt_common_api.h"
#include <string.h>
#include "gatts_table_creat_demo.c"
#include "client_v4T2.c"
// #include "bluetooth_status_server.c"
// #include "bluetooth_status_server.h"

#include "lwip/err.h"
#include "lwip/sockets.h"





void app_main(void) {

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());


    // Read NVS STATUS

    nvs_handle_t my_handle;
    esp_err_t err = nvs_flash_init();
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else {
        printf("Done\n");

    // Read
    printf("Reading status from NVS ... \n");
    int8_t status= -1; // value will default to 1-, if not set yet in NVS
    int8_t config_mode = 0;
    int8_t id_protocol = -1; // Si no existe protocolo entonces sera por default -1
    int32_t discontinous_time = 0;
    err = nvs_get_i8(my_handle, "status", &status);
    nvs_get_i8(my_handle, "config_mode", &config_mode);
    nvs_get_i8(my_handle, "id_protocol", &id_protocol);
    nvs_get_i32(my_handle, "discontinous_time", &discontinous_time);
    printf("Discontinous time en NVS es = %li\n",discontinous_time);
    //status = -1; //Comentar esta linea si no queremos que se vuelva a reconfigurar


    if (err==ESP_ERR_NVS_NOT_FOUND || status == -1) {
        printf("Aun no ha sido configurado el ESP, mandemos funcion de configuracion\n");
        printf("Llamando a configuracion blueooth...\n");
        nvs_close(my_handle);
        

        bluetooth_main('9',0,-1); //Se le pasa un protocolo que no existe y discontinous time = 0.
    
        printf("Bluetooth main ya fue ejecutado\n");
        
        //Configurar ESP32 dedse rasbperry
    }
    
    /*status = 21;
    id_protocol = 4;
    acc_sampling = 100;
    acc_sensibility= 8;
    gyro_sensibility= 200;
    BME688_sampling = 4;
    discontinous_time = 1;
    port_tcp = 3000;
    ort_udp = 3001;
    host_ip = 1902344;
    wifi_len = 9;
    pass_len = 22;
    wifi_name = "Wifibakan" ;
    password = "AardvarkBadgerHedgehog";*/

    
    else{


        //status=31;

        printf("Se leyo status = %i from NVS ... \n",status);
        // Close
        nvs_close(my_handle);
        
        printf("Ejecutando un status...\n");


        if (status == 0){
            printf("Configuracion por bluetooth \n");
            bluetooth_main('9',0,status); //Se le pasa un protocolo que no existe y discontinous time = 0.
        
        }
    

        else if (status == 20){
            printf("Configuracion via TCP en BD \n");
        }


        else if (status == 21){
            printf("Conexion TCP Continua \n");
            
            tcp('a','1');//Creo que el primer parmaetro es innecesario dado que adentro de tcp se recuepra el protocolo

        }


        else if (status == 22){
            printf("Conexion TCP Discontinua\n");
            tcp('2','1');//Creo que el primer parmaetro es innecesario dado que adentro de tcp se recuepra el protocolo
            
        }

        else if (status == 23){
            printf("Conexion UDP\n");
            udp('a','0');//Creo que el primer parmaetro es innecesario dado que adentro de udp se recuepra el protocolo
        }

        else if (status == 30){
            printf("BLE Continua\n");
            bluetooth_main(id_protocol,discontinous_time,status);
        }

        else if (status == 31){
            printf("BLE Discontinua\n");
            bluetooth_main(id_protocol,discontinous_time,status);
            printf("--------AFUERA BLE DISCONTINUA AFUERA------\n");
        }

        else {
            printf("Status no definido. Error!!!");
        }
    
    
    
    
    }

    



}














}