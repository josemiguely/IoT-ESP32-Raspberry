/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
// #include "packeting.c"
//@include
// #include "esp_log.h"     
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
// #include <stdio.h>
#include "esp_event.h"
// #include <math.h>
// #include "sdkconfig.h"
// #include <string.h>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <errno.h>
// #include <netdb.h>  
// #include <arpa/inet.h>
// #include "udp_client.c"



// static const char *TAG19 = "example";

extern void tcp_client(void);
// extern void udp_client_task(char protocol);
// #define PORT CONFIG_EXAMPLE_PORT
void app_main(void)

{

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    

    // #if defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
    // #include "addr_from_stdin.h"
    // #endif

    // #if defined(CONFIG_EXAMPLE_IPV4)
    // #define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
    // #elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
    // #define HOST_IP_ADDR ""
    // #endif


    // /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    //  * Read "Establishing Wi-Fi or Ethernet Connection" section in
    //  * examples/protocols/README.md for more information about this function.
    //  */
    ESP_ERROR_CHECK(example_connect());


    tcp_client(); // Cambiar nombre a tcp_udp_client()
    // tcp_udp_client();

    // char rx_buffer[128];
    // char host_ip[] = HOST_IP_ADDR;
    // int addr_family = 0;
    // int ip_protocol = 0;



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

   
    // #define PACK_LEN 1024
    

    // //Creamos socket...
    //  ESP_LOGI(TAG19, "Creando socket de main..."); 
    
    // int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
    // if (sock < 0) {
    //     ESP_LOGE(TAG19, "Unable to create socket: errno %d", errno);
    //     // break;
    // }
    // ESP_LOGI(TAG19, "Socket created, connecting to %s:%d", host_ip, PORT); //Corregir host_ip, PORT si no funciona

    // int err1 = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    // if (err1 != 0) {
    //     ESP_LOGE(TAG19, "Socket unable to connect: errno %d", errno);
    //     // break;
    // }
    // ESP_LOGI(TAG19, "Successfully connected");
    // //uint64_t timer = 60;

    // //Enviamos un 0 para pedirle la configuracion al Raspberry
    // int err2 = send(sock, 0, 1, 0);


    // //Recuperamos configuracion de Raspberry
    // int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    
    // rx_buffer[len] = 0;
    // char protocol = rx_buffer[0];
    // char transport_layer = rx_buffer[1];

    
    // shutdown(sock, 0);
    // close(sock);

    // //Si es TCP entonces ejecutar TCP_Client
    // if (!transport_layer){
        
    //     ESP_LOGI(TAG19, "Ejecutando tcp_client");
        // tcp_client(protocol);

    // }

    // //Ejecutar UDP_Client
    // else{

    // /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    // / * Read "Establishing Wi-Fi or Ethernet Connection" section in
    // //  examples/protocols/README.md for more information about this function.*/
     
    //     //ESP_ERROR_CHECK(example_connect());
    //    // xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);
        
    //     ESP_LOGI(TAG19, "Ejecutando udp_client");
    //     udp_client_task(protocol);
    //     // xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);
    // }
    
    

}
