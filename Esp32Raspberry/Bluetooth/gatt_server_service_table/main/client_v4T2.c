/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>  
#include <arpa/inet.h>
#include <math.h>
#include "packetingT2.c"
#include "wifi_connect.c"



#if defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
#include "addr_from_stdin.h"
#endif

#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
#define HOST_IP_ADDR ""
#endif

#define PORT CONFIG_EXAMPLE_PORT
#define PACK_LEN 1024
static const char *TAGCLIENT = "CLIENT";
char *payload;

unsigned short msg_total_length[6] = {12+6, 12+16, 12+20, 12+44, 12+48016};

void tcp(char,char);
void udp(char,char);



char * format_ip(int32_t ip, char* new_ip3){
    //printf("tamaño se s = %li",sizeof(char)*(int32_t)log10(ip)+1);
    char s[sizeof(char)*(int32_t)log10(ip)+1];
    int f = sprintf(s, "%li", ip);
    //printf("f=%i\n",f);
    //printf("s = %s\n",s);
    char * p = ".";
    char new_ip[f+3];
    int x = 3;
    strncpy(new_ip, s, x);
    new_ip[x] = '\0';
    strcat(new_ip, p);
    strcat(new_ip, s + x);
    //printf("%s\n", new_ip);
    
    
    char new_ip2[f+3];
    //strncpy()
    x += 3+1;
    strncpy(new_ip2, new_ip, x);
    new_ip2[x] = '\0';
    strcat(new_ip2, p);
   // printf("%s\n", new_ip);
    strcat(new_ip2, new_ip + x);
    //printf("%s\n", new_ip2);
   
    //char new_ip3[f+3];
    //strncpy()
    x += 2;
    strncpy(new_ip3, new_ip2, x);
    new_ip3[x] = '\0';
    strcat(new_ip3, p);
   // printf("%s\n", new_ip);
    strcat(new_ip3, new_ip2 + x);
    //printf("%s\n", new_ip3);
    new_ip3[f+3] = '\0';

    return new_ip3;

}
void tcp_udp_client(void) //Se deberia llamar tcp_udp_client()
{   




    
}



void tcp (char status,char transport_layer) {


   



    printf("TCP EJECUTADO\n");
    printf("STATUS QUE ENTRO %c \n",status);
    nvs_handle_t my_handle;
    esp_err_t err = nvs_flash_init();
    
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else {
    
    printf("Done\n");
    
    int32_t port_tcp;
    int8_t id_protocol;
    int32_t host_ip;
    int32_t wifi_len;
    int32_t pass_len;
    int32_t discontinous_time;

    err = nvs_get_i32(my_handle, "discontinous_time", &discontinous_time);
    switch (err) {
        case ESP_OK:
            printf("Done\n");
            printf("discontinous_time = %ld\n", discontinous_time);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("Discontinous time is not initialized yet!\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
    
    

    err = nvs_get_i32(my_handle, "port_tcp", &port_tcp);
    err = nvs_get_i8(my_handle, "id_protocol", &id_protocol);
    err = nvs_get_i32(my_handle, "host_ip", &host_ip);
    err = nvs_get_i32(my_handle, "wifi_len", &wifi_len);
    err = nvs_get_i32(my_handle, "pass_len", &pass_len);

    char wifi_name[wifi_len+1];
    char password[pass_len+1];
    size_t wl = (size_t) (wifi_len+1);
    size_t pl = (size_t) (pass_len+1);
    err = nvs_get_str(my_handle, "wifi_name", wifi_name,&wl);
    err = nvs_get_str(my_handle, "password", password,&pl);
    
    
    

    printf("recieve_port_tcp =  %li\n",port_tcp);
    printf("id_protocol =  %i\n",id_protocol);
    printf("recieve_host_ip =  %li\n",host_ip);
    printf("wifi_len =  %li\n",wifi_len);
    printf("pass_len =  %li\n",pass_len);
    printf("wifi =  %s\n",wifi_name);
    printf("password =  %s\n",password);

    char formated_ip[11]; // HAY QUE PASARLE UN STRING DEFINIDO DE ANTES O TODO EXPLOTA AAAAA-AAA-AAA
    format_ip(host_ip,formated_ip);
    //for(int i =0;i<12;i++){
    printf("IP HOST = %s\n",formated_ip);
   // }

    
    
  
    wifi_connect(wifi_name,password);


        while (1) {

        
        //char host_ip_TCP[] = formated_ip;
        //int addr_family_TCP = 0;
        int ip_protocol_TCP = (int)id_protocol;

        struct sockaddr_in dest_addr_TCP;
        inet_pton(AF_INET, formated_ip, &dest_addr_TCP.sin_addr);
        dest_addr_TCP.sin_family = AF_INET;
        dest_addr_TCP.sin_port = htons(port_tcp);
        //addr_family_TCP = AF_INET;
        //ESP_ERROR_CHECK(get_addr_from_stdin(port_tcp, SOCK_STREAM, &ip_protocol_TCP, &addr_family_TCP, &dest_addr_TCP));

        //Codigo TCP
        //ESP_LOGI(TAG, "=====Creando socket TCP en %s:%d=====", host_ip_TCP, (PORT+1));
        int sock =  socket(AF_INET, SOCK_STREAM, ip_protocol_TCP);
        if (sock < 0) {
            ESP_LOGE(TAGCLIENT, "Unable to create socket: errno %d", errno);
            break;
        }
        //ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip_TCP, (PORT+1));

        int err = connect(sock, (struct sockaddr *)&dest_addr_TCP, sizeof(dest_addr_TCP));
        if (err != 0) {
            //ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAGCLIENT, "Successfully connected");
        //uint64_t timer = 60;

        while (1) {


            ESP_LOGE(TAGCLIENT, "Creando mensaje...\n");
            //ESP_LOGE(TAG, "PROTOCOLO QUE LE MANDAMOS A MENSAJE %c...\n",protocol);
            // payload = mensaje(protocol,transport_layer);
            
            
            char protocol = (char) (id_protocol+48);
            //char protocol = '2';//(char)(id_protocol + 48);
            printf("protocol = %c\n",protocol);
            payload = mensaje(protocol,'1');
            ESP_LOGE(TAGCLIENT, "Mensaje del protocolo %c creado :D...\n",protocol);
           
            int largo_mensaje= msg_total_length[((int) protocol)-48];
            //ESP_LOGI(TAG,"======= Mensaje Length = %i\n", largo_mensaje);
             
            //printf("Sending!\n");
            char rx_buffer[128];

            printf("VAMOS A ENVIAR ALGO WII\n");
            // break;

            for (int i = 0; i < largo_mensaje; i += PACK_LEN)
            {

                // Generamos el siguiente trozo
                int size = fmin(PACK_LEN, largo_mensaje - i);
                char *pack = malloc(size);
                memcpy(pack, &(payload[i]), size);
                
                //Enviamos el trozo
                int err = send(sock, pack, size, 0);
                //ESP_LOGI(TAG, "== enviado un paquete  ==");
                free(pack);
                if (err < 0)
                {
                    ESP_LOGE(TAGCLIENT, "== Error occurred during sending: errno %d ==", err);
                }

                // wait for confirmation
                int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
                //ESP_LOGE(TAG, "== len que llega %i ==", len);
                // Error occurred during receiving
                if (len < 0)
                {
                    //En caso de error abortamos
                    ESP_LOGE(TAGCLIENT, "== recv failed: errno %d ==", err);
                    break;
                }
                else
                {
                    rx_buffer[len] = 0;
                    char OK_r = rx_buffer[0];
                     //ESP_LOGE(TAG, "== OK que llega %c ==", OK_r);
                    if (!OK_r)
                    {
                        ESP_LOGE(TAGCLIENT, "== Server error in fragmented send. ==");
                        free(payload);
                        break;
                    }
                }
            }
            //el último mensaje es solo un \0 para avisarle al server que terminamos
            // int err = send(sock, "\0", 1, 0);
            send(sock, "\0", 1, 0);


            free(payload);

            printf("AHORA VEREMOS SI SE HACE DEEPSLEEP O NO\n");
            //int discontinous_time = 1*60;
            if (discontinous_time > 0 || status=='2')  {
            printf("Iniciando deep sleep de 60*%li segundos iniciado\n",discontinous_time);
            //esp_sleep_enable_timer_wakeup(discontinous_time*60*1000000);
            esp_sleep_enable_timer_wakeup(1*60*1000000);
            // ESP_LOGI(TAGCLIENT, "Deep sleep de 60 segundos iniciado");
            
            shutdown(sock, 0);
            close(sock);
            esp_deep_sleep_start();
            //ESP_LOGI(TAG, "Sale del deep sleep");
            }


             break;
        }

        if (sock != -1) {
            ESP_LOGE(TAGCLIENT, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
            printf("-----ESPEREMOS ALGUNOS SEGUNDOS PARA ANTES----");
            vTaskDelay(30000 / portTICK_PERIOD_MS);
        }

       }



       

    
        }
    
   
    }



void udp (char protocol,char transport_layer){

    printf("UDP EJECUTADO\n");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_flash_init();
    
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else {
    
    printf("Done\n");
    
    int32_t port_udp;
    int8_t id_protocol;
    int32_t host_ip;
    int32_t wifi_len;
    int32_t pass_len;
    int32_t discontinous_time;

    err = nvs_get_i32(my_handle, "discontinous_time", &discontinous_time);
    switch (err) {
        case ESP_OK:
            printf("Done\n");
            printf("discontinous_time = %ld\n", discontinous_time);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("Discontinous time is not initialized yet!\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
    }

    

    err = nvs_get_i32(my_handle, "port_udp", &port_udp);
    err = nvs_get_i8(my_handle, "id_protocol", &id_protocol);
    err = nvs_get_i32(my_handle, "host_ip", &host_ip);
    err = nvs_get_i32(my_handle, "wifi_len", &wifi_len);
    err = nvs_get_i32(my_handle, "pass_len", &pass_len);

    char wifi_name[wifi_len+1];
    char password[pass_len+1];
    size_t wl = (size_t) (wifi_len+1);
    size_t pl = (size_t) (pass_len+1);
    err = nvs_get_str(my_handle, "wifi_name", wifi_name,&wl);
    err = nvs_get_str(my_handle, "password", password,&pl);
    
    
    

    printf("recieve_port_udp =  %li\n",port_udp);
    printf("id_protocol =  %i\n",id_protocol);
    printf("recieve_host_ip =  %li\n",host_ip);
    printf("wifi_len =  %li\n",wifi_len);
    printf("pass_len =  %li\n",pass_len);
    printf("wifi =  %s\n",wifi_name);
    printf("password =  %s\n",password);

    char formated_ip[11]; // HAY QUE PASARLE UN STRING DEFINIDO DE ANTES O TODO EXPLOTA AAAAA-AAA-AAA
    format_ip(host_ip,formated_ip);
    //for(int i =0;i<12;i++){
    printf("IP HOST = %s\n",formated_ip);
    

    wifi_connect(wifi_name,password);

    while (1) {

        //ESP_LOGI(TAG, "=====Esperemos 10 segundos antes de crear el cliente UDP...=====");
        //vTaskDelay(10000 / portTICK_PERIOD_MS);
        //ESP_LOGI(TAG, "=====Creando socket UDP en...=====");

        // char rx_buffer_UDP[128];
        //char host_ip_UDP[] = HOST_IP_ADDR;
        int addr_family_UDP = 0;
        int ip_protocol_UDP = (int) id_protocol;
        char ip_protocol_string[50];
        sprintf(ip_protocol_string, "%i", ip_protocol_UDP);
        
        
        struct sockaddr_in dest_addr_UDP;
        // dest_addr_UDP.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
        dest_addr_UDP.sin_addr.s_addr = inet_addr(formated_ip);
        dest_addr_UDP.sin_family = AF_INET;
        dest_addr_UDP.sin_port = htons(port_udp);
        addr_family_UDP = AF_INET;
        
        //ESP_LOGE(TAG, "===== Creando socket ====");
        int sock_UDP = socket(addr_family_UDP, SOCK_DGRAM, ip_protocol_UDP);
        //ESP_LOGE(TAG, "===== socket creado ====");
        if (sock_UDP < 0) {
            //ESP_LOGE(TAGCLIENT, "Unable to create socket UDP: errno %d", errno);
            printf("Unable to create socket UDP: errno %d\n", errno);
            continue;
        }
  
        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt (sock_UDP, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        //ESP_LOGI(TAGCLIENT, "Socket UDP created, sending to %s:%d", ip_protocol_string, port_udp);
        printf("Socket UDP created, sending to %s:%ld\n", formated_ip, port_udp);


        while (1) {
            
            //ESP_LOGE(TAG, "Creando mensaje UDP...\n");
            //char protocol = '2';//(char)(id_protocol + 48);
             char protocol = (char) (id_protocol+48);
            int largo_mensaje= msg_total_length[((int) protocol)-48];
            //ESP_LOGI(TAG,"======= Mensaje Length UDP = %i\n", largo_mensaje);
            
            // char protocol = '0';//(char)(id_protocol + 48);
            printf("protocol = %c\n",protocol);
            payload = mensaje(protocol,'0');
            //ESP_LOGE(TAG, "Mensaje del protocolo UDP %c creado :D...\n",protocol);


            for (int i = 0 ; i<largo_mensaje ; i+= PACK_LEN){
                 // Generamos el siguiente trozo
               
                int size = fmin(PACK_LEN, largo_mensaje - i);
                char *pack = malloc(size);
                memcpy(pack, &(payload[i]), size);

                //Enviamos el trozo
                int err = sendto(sock_UDP, pack, size, 0, (struct sockaddr *)&dest_addr_UDP, sizeof(dest_addr_UDP));
          
                free(pack);
                //printf("ENVIADO UN PAQUETE de tamano size = %i \n",size);
                if (err < 0)
                {
                    //ESP_LOGE(TAGCLIENT, "== Error occurred during sending: errno %d ==", err);
                     printf(TAGCLIENT, "== Error occurred during sending: errno %d ==\n", err);
                    vTaskDelay(10000 / portTICK_PERIOD_MS);
                    break;
                }
            }
            int err2 = sendto(sock_UDP, "\0", 1, 0,(struct sockaddr *)&dest_addr_UDP, sizeof(dest_addr_UDP));
            if (err2 < 0){
                //ESP_LOGE(TAGCLIENT, "== Algo paso al enviar el fin de paquete 0: errno %d ==", err2);
                 printf("== Algo paso al enviar el fin de paquete 0: errno %d ==\n", err2);
                break;
            }
            free(payload);
      
            //ESP_LOGI(TAG, "Message UDP sent");
            vTaskDelay(20000 / portTICK_PERIOD_MS);
            break;
        }

               
            printf("----Volviendo al while de UDP OH SI\n-----");
        }
    }
}
