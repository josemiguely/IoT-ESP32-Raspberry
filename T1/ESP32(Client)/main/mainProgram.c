#include <stdlib.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_event.h"

#include "UDPClient/main/udp_client.c"
#include "TCPClient/main/tcp_client_v4.c"


// Excecute main program

extern void tcp_client(void);

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    tcp_client();
    printf("No se deberia printear esto\n");
    int ch = 0;             // Character to end program
    int code;               // exit value
    int transport_layer=0;  // 0 -> TCP  || 1 -> UDP
    int id_protocol;

    // Create tcp server to receive data


    
    scanf("%d %d", transport_layer, id_protocol);


    if (transport_layer) {              // Create UDP server
        /*while(1) {
            // Exit program pressing X
            ch = getch();
            if (ch = 'x' || 'X') {
                printf("Clossing Connection\n");
                code = EXIT_SUCCESS;
                break;
            }

            // Excecute server


        }    */
    
    
    
    
    } 
    else {     
                                 // Create TCP server

         tcp_client();                         
       /* while(1) {
            // Exit program pressing X
            ch = getch();
            if (ch = 'x' || 'X') {
                printf("Clossing Connection\n");
                code = EXIT_SUCCESS;
                break;
            }

            // Excecute server


        }*/
    }


}

/*int main() {
    printf("No se deberia printear esto\n");
    int ch = 0;             // Character to end program
    int code;               // exit value
    int transport_layer=0;  // 0 -> TCP  || 1 -> UDP
    int id_protocol;

    // Create tcp server to receive data


    
    scanf("%d %d", transport_layer, id_protocol);


    if (transport_layer) {              // Create UDP server
        while(1) {
            // Exit program pressing X
            ch = getch();
            if (ch = 'x' || 'X') {
                printf("Clossing Connection\n");
                code = EXIT_SUCCESS;
                break;
            }

            // Excecute server


        }    
    
    
    
    
    } 
    else {                              // Create TCP server
        while(1) {
            // Exit program pressing X
            ch = getch();
            if (ch = 'x' || 'X') {
                printf("Clossing Connection\n");
                code = EXIT_SUCCESS;
                break;
            }

            // Excecute server


        }
    }


    



    return 0;
}*/