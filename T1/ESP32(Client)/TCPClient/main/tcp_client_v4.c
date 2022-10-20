/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>  
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"
#include <math.h>
#include "packeting.c"
          



// extern void ESP_LOGI(const char* a, char* data, int* len, int err);
// extern void ESP_LOGE(const char* a, char* data, int*len, int err);

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


// extern unsigned short dataLength(char protocol);
// extern unsigned short messageLength(char protocol);
// extern char* header(char protocol, char transportLayer);
// extern char* dataprotocol1();
// extern char* dataprotocol2();
// extern char* dataprotocol3();
// extern char* dataprotocol4();
// extern char* mensaje (char protocol, char transportLayer);
// extern 


void tcp_udp_client(void) //Se deberia llamar tcp_udp_client()
{   
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    unsigned short msg_total_length[6] = {12+6, 12+16, 12+20, 12+44, 12+24016};
    
    
#if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr;
        inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
#endif


        //Mandamos a pedir a rasbperry configuración inicial...
    
        ESP_LOGI(TAG, "=====Creando socket de main...====="); 
    
        int sock_main =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock_main < 0) {
            ESP_LOGE(TAG, "Unable to create main socket: errno %d", errno);
            // break;
        }
        ESP_LOGI(TAG, "=====Main socket created, connecting to %s:%d=====", host_ip, PORT); //Corregir host_ip, PORT si no funciona

        
        int err1 = connect(sock_main, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err1 != 0) {
            ESP_LOGE(TAG, "=====Main socket unable to connect: errno %d======", errno);
            // break;
        }
        ESP_LOGI(TAG, "=====Main socket Successfully connected=====");
        //uint64_t timer = 60;


        char* solicitud="\0";
        //Enviamos un 0 para pedirle la configuracion al Raspberry
        ESP_LOGI(TAG, "=====Enviando mensaje de solicitud de configuracion a Raspberry=====");
        int err2 = send(sock_main, solicitud, 1, 0);
        ESP_LOGI(TAG, "=====Se envio mensaje de solicitud = %s, de configuracion a Raspberry=====",solicitud);

        //Recuperamos configuracion de Raspberry
        int len = recv(sock_main, rx_buffer, sizeof(rx_buffer) - 1, 0);
        ESP_LOGI(TAG, "=====Recupero mensaje configuracion de Raspberry=====");

        rx_buffer[len] = 0;
        char protocol = rx_buffer[0];
        char transport_layer = rx_buffer[1];
        ESP_LOGI(TAG, "=====Se recibio como protocol %c=====",protocol);
        ESP_LOGI(TAG, "=====Se recibio como transport_layer %c=====",transport_layer);

        ESP_LOGI(TAG, "=====Cerrando socket de main...====="); 
        shutdown(sock_main, 0);
        close(sock_main);
        ESP_LOGI(TAG, "=====Socket de main cerrado...====="); 


        char host_ip_TCP[] = HOST_IP_ADDR;
        int addr_family_TCP = 0;
        int ip_protocol_TCP = 0;

        #if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr_TCP;
        inet_pton(AF_INET, host_ip_TCP, &dest_addr_TCP.sin_addr);
        dest_addr_TCP.sin_family = AF_INET;
        dest_addr_TCP.sin_port = htons(PORT+1);
        addr_family_TCP = AF_INET;
        ip_protocol_TCP = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr_TCP = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT+1, SOCK_STREAM, &ip_protocol_TCP, &addr_family_TCP, &dest_addr_TCP));
#endif

    while (1) {

        if (transport_layer == '0'){
        //Codigo TCP
        ESP_LOGI(TAG, "=====Creando socket TCP en %s:%d=====", host_ip_TCP, (PORT+1));
        int sock =  socket(addr_family_TCP, SOCK_STREAM, ip_protocol_TCP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, (PORT+1));

        int err = connect(sock, (struct sockaddr *)&dest_addr_TCP, sizeof(dest_addr_TCP));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");
        //uint64_t timer = 60;

        while (1) {

            /* separar los mensajes en caso de ser mayores a el buffer del server */
            /*generar el mensaje */
            char protocol = '4';
            char transportlayer='0'; // TCP = 0 ; UDP = 1;
            ESP_LOGE(TAG, "Creando mensaje...\n");
            payload = mensaje(protocol,transportlayer);
            ESP_LOGE(TAG, "Mensaje del protocolo %c creado :D...\n",protocol);
           
            //ESP_LOGI(TAG, "_____Mensaje = %s________\n", payload);
            int largo_mensaje= msg_total_length[((int) protocol)-48];
            ESP_LOGI(TAG,"======= Mensaje Length = %i\n", largo_mensaje);
             
            printf("Sending!\n");
            char rx_buffer[128];

            for (int i = 0; i < largo_mensaje; i += PACK_LEN)
            {

                // Generamos el siguiente trozo
                int size = fmin(PACK_LEN, largo_mensaje - i);
                char *pack = malloc(size);
                memcpy(pack, &(payload[i]), size);
                ESP_LOGE(TAG, "== size =  %i ==", size);
                 ESP_LOGE(TAG, "== i = =  %i ==", i);

                //Enviamos el trozo
                int err = send(sock, pack, size, 0);
                ESP_LOGE(TAG, "== enviado un paquete  ==");
                free(pack);
                if (err < 0)
                {
                    ESP_LOGE(TAG, "== Error occurred during sending: errno %d ==", err);
                }

                // wait for confirmation
                int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
                ESP_LOGE(TAG, "== len que llega %i ==", len);
                // Error occurred during receiving
                if (len < 0)
                {
                    //En caso de error abortamos
                    ESP_LOGE(TAG, "== recv failed: errno %d ==", err);
                    break;
                }
                else
                {
                    rx_buffer[len] = 0;
                    char OK_r = rx_buffer[0];
                     ESP_LOGE(TAG, "== OK que llega %c ==", OK_r);
                    if (!OK_r)
                    {
                        ESP_LOGE(TAG, "== Server error in fragmented send. ==");
                        free(payload);
                        break;
                    }
                }
            }
            //el último mensaje es solo un \0 para avisarle al server que terminamos
            int err = send(sock, "\0", 1, 0);


            free(payload);
    
        esp_sleep_enable_timer_wakeup(60*1000000);
        ESP_LOGI(TAG, "___delay de 60s___ ");
        //printf("___delay de 5s___ ");
       // ESP_LOGI(TAG, "durmiendo....");
        shutdown(sock, 0);
        close(sock);
        esp_deep_sleep_start();
        ESP_LOGI(TAG, "Sale del deep sleep");
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }



    else if (transport_layer == '1'){
         ESP_LOGI(TAG, "=====Creando socket UDP en...=====");

        



    }

}
}
