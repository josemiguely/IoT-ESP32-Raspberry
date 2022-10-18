#include <stdlib.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_event.h"
// Excecute main program

int main() {
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
}