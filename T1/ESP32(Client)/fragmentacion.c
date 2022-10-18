int TCP_send_frag(int sock, char status, char protocolo)
{
    //Parte el mensaje (payload) en trozos de 1000 btyes y los manda por separado, esperando un OK con cada trozo
    printf("Sending!\n");
    char *payload = mensaje(protocolo, status);
    int payloadLen = messageLength(protocolo) - 1;
    char rx_buffer[128];

    for (int i = 0; i < payloadLen; i += PACK_LEN)
    {

        // Generamos el siguiente trozo
        int size = fmin(PACK_LEN, payloadLen - i);
        char *pack = malloc(size);
        memcpy(pack, &(payload[i]), size);

        //Enviamos el trozo
        int err = send(sock, pack, size, 0);
        free(pack);
        if (err < 0)
        {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        }

        // wait for confirmation
        int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        // Error occurred during receiving
        if (len < 0)
        {
            //En caso de error abortamos
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            break;
        }
        else
        {
            rx_buffer[len] = 0;
            char OK_r = rx_buffer[0];
            if (!OK_r)
            {
                ESP_LOGE(TAG, "Server error in fragmented send.");
                free(payload);
                return -1;
            }
        }
    }
    //el último mensaje es solo un \0 para avisarle al server que terminamos
    int err = send(sock, "\0", 1, 0);

    free(payload);

    return err;
}