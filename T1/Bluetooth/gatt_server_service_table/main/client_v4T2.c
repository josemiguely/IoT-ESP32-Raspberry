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
static const char *TAG = "example";
char *payload;

unsigned short msg_total_length[6] = {12+6, 12+16, 12+20, 12+44, 12+24016};

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



//    printf("TCP_UDP_CLIENT\n");

//     char rx_buffer[128];
//     char host_ip[] = HOST_IP_ADDR;
//     int addr_family = 0;
//     int ip_protocol = 0;

    
    
    
// #if defined(CONFIG_EXAMPLE_IPV4)
//         struct sockaddr_in dest_addr;
//         inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
//         dest_addr.sin_family = AF_INET;
//         dest_addr.sin_port = htons(PORT);
//         addr_family = AF_INET;
//         ip_protocol = IPPROTO_IP;
// #elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
//         struct sockaddr_storage dest_addr = { 0 };
//         ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
// #endif


//         //Mandamos a pedir a rasbperry configuración inicial...
    
//         //ESP_LOGI(TAG, "=====Creando socket de main...====="); 
    
//         int sock_main =  socket(addr_family, SOCK_STREAM, ip_protocol);
//         if (sock_main < 0) {
//             ESP_LOGE(TAG, "Unable to create main socket: errno %d", errno);
//             // break;
//         }
//         //ESP_LOGI(TAG, "=====Main socket created, connecting to %s:%d=====", host_ip, PORT); //Corregir host_ip, PORT si no funciona

        
//         int err1 = connect(sock_main, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
//         if (err1 != 0) {
//             ESP_LOGE(TAG, "=====Main socket unable to connect: errno %d======", errno);
//             // break;
//         }
//         ESP_LOGI(TAG, "=====Main socket Successfully connected=====");
//         //uint64_t timer = 60;


//         char* solicitud="\0";
//         //Enviamos un 0 para pedirle la configuracion al Raspberry
//         ESP_LOGI(TAG, "=====Enviando mensaje de solicitud de configuracion a Raspberry=====");
//         int err2 = send(sock_main, solicitud, 1, 0);
//         ESP_LOGI(TAG, "=====Se envio mensaje de solicitud = %s, de configuracion a Raspberry=====",solicitud);

//         //Recuperamos configuracion de Raspberry
//         int len = recv(sock_main, rx_buffer, sizeof(rx_buffer) - 1, 0);
//         ESP_LOGI(TAG, "=====Recupero mensaje configuracion de Raspberry=====");

//         rx_buffer[len] = 0;
//         char protocol =  rx_buffer[0];
//         char transport_layer = rx_buffer[1];
//         //ESP_LOGI(TAG, "=====Se recibio como protocol %c=====",protocol);
//         //ESP_LOGI(TAG, "=====Se recibio como transport_layer %c=====",transport_layer);

//         //ESP_LOGI(TAG, "=====Cerrando socket de main...====="); 
//         shutdown(sock_main, 0);
//         close(sock_main);
//         //ESP_LOGI(TAG, "=====Socket de main cerrado...====="); 

//         if (transport_layer == '0'){ 
//         tcp(protocol,transport_layer);
//     }



//     else if (transport_layer == '1'){       
//         udp(protocol,transport_layer);
//     }
//     else
//     {
//         ESP_LOGI(TAG, "===== No recibio transport layer ====="); 
//     }
    
}



void tcp (char protocol,char transport_layer) {


    //     #if defined(CONFIG_EXAMPLE_IPV4)
    //     struct sockaddr_in dest_addr;
    //     inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
    //     dest_addr.sin_family = AF_INET;
    //     dest_addr.sin_port = htons(PORT);
    //     addr_family = AF_INET;
    //     ip_protocol = IPPROTO_IP;
    //     #elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
    //             struct sockaddr_storage dest_addr = { 0 };
    //             ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
    //     #endif



    printf("TCP EJECUTADO\n");
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
    }
    
    //     while (1) {

    //     char host_ip_TCP[] = HOST_IP_ADDR;
    //     int addr_family_TCP = 0;
    //     int ip_protocol_TCP = 0;

    //     #if defined(CONFIG_EXAMPLE_IPV4)
    //     struct sockaddr_in dest_addr_TCP;
    //     inet_pton(AF_INET, host_ip_TCP, &dest_addr_TCP.sin_addr);
    //     dest_addr_TCP.sin_family = AF_INET;
    //     dest_addr_TCP.sin_port = htons(PORT+1);
    //     addr_family_TCP = AF_INET;
    //     ip_protocol_TCP = IPPROTO_IP;
    //     #elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
    //             struct sockaddr_storage dest_addr_TCP = { 0 };
    //             ESP_ERROR_CHECK(get_addr_from_stdin(PORT+1, SOCK_STREAM, &ip_protocol_TCP, &addr_family_TCP, &dest_addr_TCP));
    //     #endif


    //     //Codigo TCP
    //     //ESP_LOGI(TAG, "=====Creando socket TCP en %s:%d=====", host_ip_TCP, (PORT+1));
    //     int sock =  socket(addr_family_TCP, SOCK_STREAM, ip_protocol_TCP);
    //     if (sock < 0) {
    //         ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    //         break;
    //     }
    //     //ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip_TCP, (PORT+1));

    //     int err = connect(sock, (struct sockaddr *)&dest_addr_TCP, sizeof(dest_addr_TCP));
    //     if (err != 0) {
    //         //ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
    //         break;
    //     }
    //     //ESP_LOGI(TAG, "Successfully connected");
    //     //uint64_t timer = 60;

    //     while (1) {


    //         ESP_LOGE(TAG, "Creando mensaje...\n");
    //         //ESP_LOGE(TAG, "PROTOCOLO QUE LE MANDAMOS A MENSAJE %c...\n",protocol);
    //         payload = mensaje(protocol,transport_layer);
    //         ESP_LOGE(TAG, "Mensaje del protocolo %c creado :D...\n",protocol);
           
    //         int largo_mensaje= msg_total_length[((int) protocol)-48];
    //         //ESP_LOGI(TAG,"======= Mensaje Length = %i\n", largo_mensaje);
             
    //         //printf("Sending!\n");
    //         char rx_buffer[128];

    //         for (int i = 0; i < largo_mensaje; i += PACK_LEN)
    //         {

    //             // Generamos el siguiente trozo
    //             int size = fmin(PACK_LEN, largo_mensaje - i);
    //             char *pack = malloc(size);
    //             memcpy(pack, &(payload[i]), size);
                
    //             //Enviamos el trozo
    //             int err = send(sock, pack, size, 0);
    //             //ESP_LOGI(TAG, "== enviado un paquete  ==");
    //             free(pack);
    //             if (err < 0)
    //             {
    //                 ESP_LOGE(TAG, "== Error occurred during sending: errno %d ==", err);
    //             }

    //             // wait for confirmation
    //             int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    //             //ESP_LOGE(TAG, "== len que llega %i ==", len);
    //             // Error occurred during receiving
    //             if (len < 0)
    //             {
    //                 //En caso de error abortamos
    //                 ESP_LOGE(TAG, "== recv failed: errno %d ==", err);
    //                 break;
    //             }
    //             else
    //             {
    //                 rx_buffer[len] = 0;
    //                 char OK_r = rx_buffer[0];
    //                  //ESP_LOGE(TAG, "== OK que llega %c ==", OK_r);
    //                 if (!OK_r)
    //                 {
    //                     ESP_LOGE(TAG, "== Server error in fragmented send. ==");
    //                     free(payload);
    //                     break;
    //                 }
    //             }
    //         }
    //         //el último mensaje es solo un \0 para avisarle al server que terminamos
    //         int err = send(sock, "\0", 1, 0);


    //         free(payload);
    
    //         esp_sleep_enable_timer_wakeup(60*1000000);
    //         ESP_LOGI(TAG, "Deep sleep de 60 segundos iniciado");
    //         shutdown(sock, 0);
    //         close(sock);
    //         esp_deep_sleep_start();
    //         //ESP_LOGI(TAG, "Sale del deep sleep");
    //     }

    //     if (sock != -1) {
    //         ESP_LOGE(TAG, "Shutting down socket and restarting...");
    //         shutdown(sock, 0);
    //         close(sock);
    //     }
    // }
}



void udp (char protocol,char transport_layer){

//     while (1) {

//         //ESP_LOGI(TAG, "=====Esperemos 10 segundos antes de crear el cliente UDP...=====");
//         //vTaskDelay(10000 / portTICK_PERIOD_MS);
//         //ESP_LOGI(TAG, "=====Creando socket UDP en...=====");

//         // char rx_buffer_UDP[128];
//         char host_ip_UDP[] = HOST_IP_ADDR;
//         int addr_family_UDP = 0;
//         int ip_protocol_UDP = 0;
//         #if defined(CONFIG_EXAMPLE_IPV4)
//                 struct sockaddr_in dest_addr_UDP;
//                 dest_addr_UDP.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
//                 dest_addr_UDP.sin_family = AF_INET;
//                 dest_addr_UDP.sin_port = htons((PORT+2));
//                 addr_family_UDP = AF_INET;
//                 ip_protocol_UDP = IPPROTO_IP;
//         #elif defined(CONFIG_EXAMPLE_IPV6)
//                 struct sockaddr_in6 dest_addr_UDP = { 0 };
//                 inet6_aton(HOST_IP_ADDR, &dest_addr_UDP.sin6_addr);
//                 dest_addr_UDP.sin6_family = AF_INET6;
//                 dest_addr_UDP.sin6_port = htons((PORT+2));
//                 dest_addr_UDP.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE);
//                 addr_family_UDP = AF_INET6;
//                 ip_protocol_UDP = IPPROTO_IPV6;
//         #elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
//                 struct sockaddr_storage dest_addr_UDP = { 0 };
//                 ESP_ERROR_CHECK(get_addr_from_stdin((PORT+2), SOCK_DGRAM, &ip_protocol_UDP, &addr_family_UDP, &dest_addr_UDP));
//         #endif
//         //ESP_LOGE(TAG, "===== Creando socket ====");
//         int sock_UDP = socket(addr_family_UDP, SOCK_DGRAM, ip_protocol_UDP);
//         //ESP_LOGE(TAG, "===== socket creado ====");
//         if (sock_UDP < 0) {
//             ESP_LOGE(TAG, "Unable to create socket UDP: errno %d", errno);
//             continue;
//         }
  
//         // Set timeout
//         struct timeval timeout;
//         timeout.tv_sec = 10;
//         timeout.tv_usec = 0;
//         setsockopt (sock_UDP, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

//         ESP_LOGI(TAG, "Socket UDP created, sending to %s:%d", HOST_IP_ADDR, (PORT+2));


//         while (1) {
            
//             //ESP_LOGE(TAG, "Creando mensaje UDP...\n");

//             int largo_mensaje= msg_total_length[((int) protocol)-48];
//             //ESP_LOGI(TAG,"======= Mensaje Length UDP = %i\n", largo_mensaje);
            
//             payload = mensaje(protocol,transport_layer);
//             //ESP_LOGE(TAG, "Mensaje del protocolo UDP %c creado :D...\n",protocol);


//             for (int i = 0 ; i<largo_mensaje ; i+= PACK_LEN){
//                  // Generamos el siguiente trozo
               
//                 int size = fmin(PACK_LEN, largo_mensaje - i);
//                 char *pack = malloc(size);
//                 memcpy(pack, &(payload[i]), size);

//                 //Enviamos el trozo
//                 int err = sendto(sock_UDP, pack, size, 0, (struct sockaddr *)&dest_addr_UDP, sizeof(dest_addr_UDP));
          
//                 free(pack);
//                 if (err < 0)
//                 {
//                     ESP_LOGE(TAG, "== Error occurred during sending: errno %d ==", err);
//                     vTaskDelay(10000 / portTICK_PERIOD_MS);
//                     break;
//                 }
//             }
//             int err2 = sendto(sock_UDP, "\0", 1, 0,(struct sockaddr *)&dest_addr_UDP, sizeof(dest_addr_UDP));
//             if (err2 < 0){
//                 ESP_LOGE(TAG, "== Algo paso al enviar el fin de paquete 0: errno %d ==", err2);
//                 break;
//             }
//             free(payload);
      
//             //ESP_LOGI(TAG, "Message UDP sent");
//             vTaskDelay(10000 / portTICK_PERIOD_MS);
//             break;
//         }

       
//    // }
    
//     //ESP_LOGE(TAG, "Shutting down socket UDP and restarting...");
//     //vTaskDelete(NULL);
//     esp_sleep_enable_timer_wakeup(5*1000000);
//     shutdown(sock_UDP, 0);
//     close(sock_UDP);
    
//     esp_deep_sleep_start();
//     }
        
}
