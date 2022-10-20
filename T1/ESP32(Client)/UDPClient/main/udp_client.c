/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include <math.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "addr_from_stdin.h"
#include "packetingUDP.c"

#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#elif defined(CONFIG_EXAMPLE_IPV6)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#else
#define HOST_IP_ADDR ""
#endif
#define PACK_LEN 1024
#define PORT CONFIG_EXAMPLE_PORT

static const char *TAG6 = "example";
char *payload;







static void udp_client_task()//(void *pvParameters)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;
    /*char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;
*/
    unsigned short msg_total_length[6] = {12+6, 12+16, 12+20, 12+44, 12+24016};

    while (1) {

#if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_IPV6)
        struct sockaddr_in6 dest_addr = { 0 };
        inet6_aton(HOST_IP_ADDR, &dest_addr.sin6_addr);
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port = htons(PORT);
        dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_DGRAM, &ip_protocol, &addr_family, &dest_addr));
#endif

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG6, "Unable to create socket: errno %d", errno);
            break;
        }

        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        ESP_LOGI(TAG6, "Socket created, sending to %s:%d", HOST_IP_ADDR, PORT);


        while (1) {


            char protocol = '4';
            char transportlayer= '1'; // TCP = 0 ; UDP = 1;
            ESP_LOGE(TAG6, "Creando mensaje...\n");

            int largo_mensaje= msg_total_length[((int) protocol)-48];
            ESP_LOGI(TAG6,"======= Mensaje Length = %i\n", largo_mensaje);
            
            payload = mensaje(protocol,transportlayer);
            ESP_LOGE(TAG6, "Mensaje del protocolo %c creado :D...\n",protocol);


            for (int i = 0 ; i<largo_mensaje ; i+= PACK_LEN){
                 // Generamos el siguiente trozo
               
                int size = fmin(PACK_LEN, largo_mensaje - i);
                char *pack = malloc(size);
                memcpy(pack, &(payload[i]), size);
                //ESP_LOGE(TAG6, "== size =  %i ==", size);
                //ESP_LOGE(TAG6, "== i = =  %i ==", i);

                //Enviamos el trozo
                int err = sendto(sock, pack, size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                //ESP_LOGE(TAG6, "== enviado un paquete  ==");
                free(pack);
                if (err < 0)
                {
                    ESP_LOGE(TAG6, "== Error occurred during sending: errno %d ==", err);
                     break;
                }
            }
            int err = sendto(sock, "\0", 1, 0,(struct sockaddr *)&dest_addr, sizeof(dest_addr));

            free(payload);
           /* int err = sendto(sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err < 0) {
                ESP_LOGE(TAG6, "Error occurred during sending: errno %d", errno);
                break;
            }*/
            ESP_LOGI(TAG6, "Message sent");
            
            //struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
            //socklen_t socklen = sizeof(source_addr);
            //int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
           /* if (len < 0) {
                ESP_LOGE(TAG6, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG6, "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI(TAG6, "%s", rx_buffer);
                if (strncmp(rx_buffer, "OK: ", 4) == 0) {
                    ESP_LOGI(TAG6, "Received expected message, reconnecting");
                    break;
                }
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);*/
        }

        if (sock != -1) {
            ESP_LOGE(TAG6, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    / * Read "Establishing Wi-Fi or Ethernet Connection" section in
    //  examples/protocols/README.md for more information about this function.*/
     
    ESP_ERROR_CHECK(example_connect());

   // xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);
   udp_client_task();
}
