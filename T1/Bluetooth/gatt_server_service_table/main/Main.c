


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
    int32_t status= -1; // value will default to 1-, if not set yet in NVS
    err = nvs_get_i32(my_handle, "status", &status);
    
    if (err==ESP_ERR_NVS_NOT_FOUND || status= -1) {
        printf("Aun no ha sido configurado el ESP, mandemos funcion de configuracion");
    }

    
    printf("Se leyo status = %i from NVS ... \n",status);
    // Close
    nvs_close(my_handle);
    }

    if (status==0){
        printf("Configuracion por Bluetooth \n");
    }

    else if (status == 20){
        printf("Configuracion via TCP en BD \n");
    }


    else if (status == 21){
        printf("Conexion TCP Continua \n");
    }


    else if (status == 2){
        printf("Conexion TCP Discontinua\n");
    }

    else if (status == 23){
        printf("Conexion UDP\n");
    }

    else if (status == 30){
        printf("BLE Continua\n");
    }

    else if (status == 31){
        printf("BLE Discontinua\n");
    }

    else {
        printf("Status no definido. Error!!!");
    }


















}