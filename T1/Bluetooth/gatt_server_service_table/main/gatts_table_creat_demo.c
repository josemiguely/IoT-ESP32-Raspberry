/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases creating a GATT database using a predefined attribute table.
* It acts as a GATT server and can send adv data, be connected by client.
* Run the gatt_client demo, the client demo will automatically connect to the gatt_server_service_table demo.
* Client demo will enable GATT server's notify after connection. The two devices will then exchange
* data.
*
****************************************************************************/


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "gatts_table_creat_demo.h"
#include "esp_gatt_common_api.h"
#include <string.h>

#define GATTS_TABLE_TAG "GATO_BAKAN_ESP"

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x55
#define SAMPLE_DEVICE_NAME          "GATO_BAKAN"
#define SVC_INST_ID                 0

/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
*/
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

static uint8_t adv_config_done       = 0;

uint16_t heart_rate_handle_table[HRS_IDX_NB];

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

static prepare_type_env_t prepare_write_env;

#define CONFIG_SET_RAW_ADV_DATA
#ifdef CONFIG_SET_RAW_ADV_DATA
static uint8_t raw_adv_data[] = {
        /* flags */
        0x02, 0x01, 0x06,
        /* tx power*/
        0x02, 0x0a, 0xeb,
        /* service uuid */
        0x03, 0x03, 0xFF, 0x00,
        /* device name */
        0x0b, 0x09, 'G', 'A', 'T', 'O', '_', 'B', 'A', 'K', 'A', 'N'
};
static uint8_t raw_scan_rsp_data[] = {
        /* flags */
        0x02, 0x01, 0x06,
        /* tx power */
        0x02, 0x0a, 0xeb,
        /* service uuid */
        0x03, 0x03, 0xFF,0x00
};

#else
static uint8_t service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

/* The length of adv data must be less than 31 bytes */
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp        = false,
    .include_name        = true,
    .include_txpower     = true,
    .min_interval        = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval        = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance          = 0x00,
    .manufacturer_len    = 0,    //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //test_manufacturer,
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = sizeof(service_uuid),
    .p_service_uuid      = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp        = true,
    .include_name        = true,
    .include_txpower     = true,
    .min_interval        = 0x0006,
    .max_interval        = 0x0010,
    .appearance          = 0x00,
    .manufacturer_len    = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //&test_manufacturer[0],
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = sizeof(service_uuid),
    .p_service_uuid      = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
#endif /* CONFIG_SET_RAW_ADV_DATA */

static esp_ble_adv_params_t adv_params = {
    .adv_int_min         = 0x20,
    .adv_int_max         = 0x40,
    .adv_type            = ADV_TYPE_IND,
    .own_addr_type       = BLE_ADDR_TYPE_PUBLIC,
    .channel_map         = ADV_CHNL_ALL,
    .adv_filter_policy   = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
					esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

/* Service */
static const uint16_t GATTS_SERVICE_UUID_TEST      = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A       = 0xFF01;
static const uint16_t GATTS_CHAR_UUID_TEST_B       = 0xFF02;
static const uint16_t GATTS_CHAR_UUID_TEST_C       = 0xFF03;

static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read                =  ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write               = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_read_write_notify   = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t heart_measurement_ccc[2]      = {0x00, 0x00};
static const uint8_t char_valueA[4]                 = {0x11, 0x22, 0x33, 0x44};
static const uint8_t char_valueB[5]                 = {'C', 'A', 'R', 'B','\0'};
static const uint8_t char_valueC[5]                 = {'C', 'A', 'R', 'C','\0'};


//La primera característica es la posicion 4 de python.
//es la 4 en la lista de python
//es la 5 en la lista de python
//es la 6 en la lista de python


/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] =
{
    // Service Declaration
    [IDX_SVC]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_TEST), (uint8_t *)&GATTS_SERVICE_UUID_TEST}},

    /* Characteristic Declaration */
    [IDX_CHAR_A]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_A] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_A, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_valueA), (uint8_t *)char_valueA}},

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_A]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},

    /* Characteristic Declaration */
    [IDX_CHAR_B]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_B]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_B, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_valueB), (uint8_t *)char_valueB}},

    /* Characteristic Declaration */
    [IDX_CHAR_C]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_C]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_C, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_valueC), (uint8_t *)char_valueC}},

};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    #ifdef CONFIG_SET_RAW_ADV_DATA
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
            adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
    #else
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
    #endif
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            /* advertising start complete event to indicate advertising start successfully or failed */
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(GATTS_TABLE_TAG, "advertising start failed");
            }else{
                ESP_LOGI(GATTS_TABLE_TAG, "gato bkn 😼");
                ESP_LOGI(GATTS_TABLE_TAG, "advertising start successfully");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(GATTS_TABLE_TAG, "Advertising stop failed");
            }
            else {
                ESP_LOGI(GATTS_TABLE_TAG, "Stop adv successfully\n");
            }
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
            break;
        default:
            break;
    }
}

void example_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(GATTS_TABLE_TAG, "prepare write, handle = %d, value len = %d", param->write.handle, param->write.len);
    esp_gatt_status_t status = ESP_GATT_OK;
    if (prepare_write_env->prepare_buf == NULL) {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
        if (prepare_write_env->prepare_buf == NULL) {
            ESP_LOGE(GATTS_TABLE_TAG, "%s, Gatt_server prep no mem", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
    } else {
        if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_OFFSET;
        } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_ATTR_LEN;
        }
    }
    /*send response when param->write.need_rsp is true */
    if (param->write.need_rsp){
        esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
        if (gatt_rsp != NULL){
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
               ESP_LOGE(GATTS_TABLE_TAG, "Send response error");
            }
            free(gatt_rsp);
        }else{
            ESP_LOGE(GATTS_TABLE_TAG, "%s, malloc failed", __func__);
        }
    }
    if (status != ESP_GATT_OK){
        return;
    }
    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);
    prepare_write_env->prepare_len += param->write.len;

}

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC && prepare_write_env->prepare_buf){
        esp_log_buffer_hex(GATTS_TABLE_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }else{
        ESP_LOGI(GATTS_TABLE_TAG,"ESP_GATT_PREP_WRITE_CANCEL");
    }
    if (prepare_write_env->prepare_buf) {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:{
            esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(SAMPLE_DEVICE_NAME);
            if (set_dev_name_ret){
                ESP_LOGE(GATTS_TABLE_TAG, "set device name failed, error code = %x", set_dev_name_ret);
            }
    #ifdef CONFIG_SET_RAW_ADV_DATA
            esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
            if (raw_adv_ret){
                ESP_LOGE(GATTS_TABLE_TAG, "config raw adv data failed, error code = %x ", raw_adv_ret);
            }
            adv_config_done |= ADV_CONFIG_FLAG;
            esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));
            if (raw_scan_ret){
                ESP_LOGE(GATTS_TABLE_TAG, "config raw scan rsp data failed, error code = %x", raw_scan_ret);
            }
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
    #else
            //config adv data
            esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
            if (ret){
                ESP_LOGE(GATTS_TABLE_TAG, "config adv data failed, error code = %x", ret);
            }
            adv_config_done |= ADV_CONFIG_FLAG;
            //config scan response data
            ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
            if (ret){
                ESP_LOGE(GATTS_TABLE_TAG, "config scan response data failed, error code = %x", ret);
            }
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
    #endif
            esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
            if (create_attr_ret){
                ESP_LOGE(GATTS_TABLE_TAG, "create attr table failed, error code = %x", create_attr_ret);
            }
        }
       	    break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_READ_EVT");
       	    break;
        case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep){
                // the data length of gattc write  must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
                ESP_LOGI(GATTS_TABLE_TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :", param->write.handle, param->write.len);
                esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);
                

                char* configuration = (char *) malloc(sizeof(char)*param->write.len);
                
                // char* wifi_size= (char *) malloc(sizeof(int));
                // char* ps_size = (char *) malloc(sizeof(int));
                // char* trash_ptr;
                // char* wifi_aux = wifi_size;
                // char* ps_aux = ps_size;
                //int8_t status;
                //int8_t id_protocol;
                //int32_t wifi_size;
                //int32_t ps_size;


                //Imprimos el mensaje que llega y guardamos configuracion
                for (int i=0;i<param->write.len;i++) {
                   /* 
                    //  if(i >=34 && i<38){ // tamaño de wifi
                    //      ESP_LOGI(GATTS_TABLE_TAG,"wifi tamaño= %x de tipo %i",param->write.value[i],sizeof(param->write.value[i]));
                    //      configuration[i] = param->write.value[i];
                    //      *wifi_size = param->write.value[i]; // 0009 
                    //      ESP_LOGI(GATTS_TABLE_TAG,"wifi tiene %x",*wifi_size);
                    //      wifi_size++;                   //     |
                    //  }
                    //  else if (i >= 38 && i<42){ // tamaño contraseña
                    //      ESP_LOGI(GATTS_TABLE_TAG,"%i",param->write.value[i]);
                    //      configuration[i] = param->write.value[i];
                    //      *ps_size = param->write.value[i];
                    //      ps_size++;
                    //  }
                   
                    //  else{ // copiar lo demas 
                   */     
                    ESP_LOGI(GATTS_TABLE_TAG,"%x",param->write.value[i]);
                    configuration[i] = param->write.value[i];
                    printf("configuration de %i es = %i\n",i,configuration[i]);
                  //  }
                   
                }/*
                // char* conf_ptr = configuration;
                // printf("largo de configuration = %i\n",strlen(configuration));
                // printf(" configuration actial (*) = %i\n",*conf_ptr);
                // conf_ptr+=34;
                // strncpy(wifi_size,conf_ptr,4);
                // printf(" configuration actial (*) = %i\n",*conf_ptr);
                // conf_ptr+=4;
                // strncpy(ps_size,conf_ptr,4); 
                // printf(" configuration actial (*) = %i\n",*conf_ptr);

                
                wifi_size = wifi_aux;
                ps_size = ps_aux;

                for (int i = 0;i<4;i++){
                    ESP_LOGI(GATTS_TABLE_TAG,"%i",wifi_size[i]); 
                    ESP_LOGI(GATTS_TABLE_TAG,"%i",wifi_aux[i]); 
                }


                //int ws = strtol(wifi_size[3], NULL,16);
                //int ps = strtol(ps_size[3], NULL,16);
               
                
                //ESP_LOGI(GATTS_TABLE_TAG,"%s",ps_size);
                //int ws = atoi(wifi_sizeS);
               // int ps = atoi(ps_size);
                //int config_sizes[12] = {1,1,4,4,4,4,4,4,4,4,ws,ps};
                // recorremos configuration para guardar en la memoria no volatil
                //char* conf_ptr = configuration;
                //char* compactc_conf[12];
                //printf("ws= %i\n",ws);
                //printf("ps= %i\n",ps);
                // char* status = (char *)malloc(sizeof(char));
                // char* id_protocol = (char *)malloc(sizeof(char));
                // char* acc_sampling =(char *) malloc(sizeof(int));
                // char* acc_sensibility = (char *)malloc(sizeof(int));
                // char* gyro_sensibility = (char *)malloc(sizeof(int));
                // char* BME688_sampling = (char *)malloc(sizeof(int));
                // char* discontinous_time = (char *)malloc(sizeof(int));
                // char* port_tcp = (char *)malloc(sizeof(int));
                // char* port_udp = (char *)malloc(sizeof(int));
                // char* host_ip = (char *)malloc(sizeof(int));
                // char* wifi_name =(char *)malloc(sizeof(char)*ws);
                // char* password = (char *)malloc(sizeof(char)*ps);*/

                int8_t status;
                int8_t id_protocol;
                int32_t acc_sampling ;
                int32_t acc_sensibility;
                int32_t gyro_sensibility;
                int32_t BME688_sampling;
                int32_t discontinous_time;
                int32_t port_tcp;
                int32_t port_udp ;
                int32_t host_ip;
                int32_t wifi_len;
                int32_t pass_len;
                //char* wifi_name;
                //char* password;

                /**/
                status = configuration[0];
                id_protocol = configuration[1];
                printf("status = %i\n",status);
                printf("id_protocol = %i\n",(int)id_protocol);

                memcpy(&acc_sampling,&configuration[2],4);
                printf("acc_sampling = %i\n",(int)acc_sampling);
            
                memcpy(&acc_sensibility,&configuration[6],4);
                printf("acc_sensibility = %i\n",(int)acc_sensibility);

                memcpy(&gyro_sensibility,&configuration[10],4);
                printf("gyro_sensibility = %i\n",(int)gyro_sensibility);

                memcpy(&BME688_sampling,&configuration[14],4);
                printf("BME688_sampling = %i\n",(int)BME688_sampling);

                memcpy(&discontinous_time,&configuration[18],4);
                printf("discontinous_time = %i\n",(int)discontinous_time);

                memcpy(&port_tcp,&configuration[22],4);
                printf("port_tcp = %i\n",(int)port_tcp);

                memcpy(&port_udp,&configuration[26],4);
                printf("port_udp = %i\n",(int)port_udp);

                memcpy(&host_ip,&configuration[30],4);
                printf("host_ip = %i\n",(int)host_ip);

                memcpy(&wifi_len,&configuration[34],4);
                printf("wifi_len = %i\n",(int)wifi_len);

                memcpy(&pass_len,&configuration[38],4);
                printf("pass_len = %i\n",(int)pass_len);
                /* ME QUIERO MATAR */
                int pl =  (int)pass_len;
                int wl = (int)wifi_len ;
                printf("pl = %i\n",pl);
                printf("wl = %i\n",wl);
                int n1 = 42+wl;
                printf("copiar contraseña desde = %i\n",n1);
                char * wifi_name = malloc(wl); 
                char * password = malloc(pl);
                //memset(wifi_name,0,wl);
                //memset(password,0,pl); 
                //memcpy(&wifi_name,&configuration[42],wl);
                //strncpy(wifi_name,&configuration[42],wl);
                //strncpy(password,&configuration[n1],pl);
                /**/
                
                //wifi
                for (int i = 0;i< wl; i++){
                    printf("configuracion en %i hay %c\n",i,configuration[42+i]);
                     memcpy(&wifi_name[i],&configuration[42+i],1);
                   // printf("wifi_name en %i hay %c\n",wifi_name[i]);
                }
                printf("wifi_name = %s\n",wifi_name);
                for (int i = 0;i< wl; i++){
                    printf("wifi en %i hay %c\n",i,wifi_name[i]);
                   // printf("wifi_name en %i hay %c\n",wifi_name[i]);
                }
                //memcpy(&password,&configuration[n1],pl);

                for (int i = 0;i< pl; i++){
                    printf("configuracion en %i hay %c\n",i,configuration[n1+i]);
                    
                    //printf("pass en %i hay %c\n",password[i]);
                }
               
                for (int i = 0;i< pl; i++){
                    //printf("password en %i hay %c\n",i,password[i]);
                   // printf("wifi_name en %i hay %c\n",wifi_name[i]); 
                   memcpy(&password[i],&configuration[n1+i],1);
                }
                 printf("password = %s\n",password);
                /*
                for (int i = 0 ; i<12;i++){
                    //conf_ptr = "";
                    //strncpy()
                    // for (int j = 0; j<config_sizes[i];j++){
                       
                    //     ESP_LOGI(GATTS_TABLE_TAG,"%s",conf_ptr[i]);
                    //     conf_ptr[i] = *configuration;
                    //     strncpy()
                    //     configuration++;

                    // }
                    if (i == 0){
                        //status = conf_ptr;
                        strncpy(status,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"status=%x",status[3]);
                        configuration+=config_sizes[i];}
                    else if( i == 1){
                        //id_protocol = conf_ptr;
                        strncpy(id_protocol,configuration,config_sizes[i]);
                       ESP_LOGI(GATTS_TABLE_TAG,"id_protocol=%x",id_protocol[3]);
                        configuration+=config_sizes[i];}
                    else if( i == 2){
                        //acc_sampling = conf_ptr;
                        strncpy(acc_sampling,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"acc_sampling=%x",acc_sampling[3]);
                        configuration+=config_sizes[i];}
                    else if( i == 3){
                        //acc_sensibility = conf_ptr;
                        strncpy(acc_sensibility,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"acc_sensibility=%x",acc_sensibility[3]);
                        configuration+=config_sizes[i];}
                    else if (i == 4){
                        //gyro_sensibility = conf_ptr;
                        strncpy(gyro_sensibility,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"gyro_sensibilitys=%x",gyro_sensibility[3]);
                        configuration+=config_sizes[i];}
                    else if( i == 5){
                        //BME688_sampling = conf_ptr;
                        strncpy(BME688_sampling,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"BME688_sampling=%x",BME688_sampling[3]);
                        configuration+=config_sizes[i];}    
                    else if( i == 6){
                        //discontinous_time = conf_ptr;
                        strncpy(discontinous_time,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"discontinous_time=%x",discontinous_time[3]);
                        configuration+=config_sizes[i];}
                    else if( i == 7){
                        //port_tcp = conf_ptr;
                        strncpy(port_tcp,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"port_tcp=%x",port_tcp[3]);
                        configuration+=config_sizes[i];}
                    else if( i == 8){
                        //port_udp = conf_ptr;
                        strncpy(port_udp,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"port_udp=%x",port_udp[3]);
                        configuration+=config_sizes[i];}
                    else if( i == 9){
                        //host_ip = conf_ptr;
                        strncpy(host_ip,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"host ip=%x",host_ip[3]);
                        configuration+=config_sizes[i];}
                    else if( i == 10){
                        //wifi_name = conf_ptr;
                        strncpy(wifi_name,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"wifi=%s",wifi_name);
                        configuration+=config_sizes[i];}
                        
                    else if( i == 11){
                        //password = conf_ptr;
                        strncpy(password,configuration,config_sizes[i]);
                        ESP_LOGI(GATTS_TABLE_TAG,"ps = %s",password);
                        configuration+=config_sizes[i];}
                    else
                        ESP_LOGI(GATTS_TABLE_TAG,"ERROR FATAL");
                   // ESP_LOGI(GATTS_TABLE_TAG, "%s",)
                    
                }
                
                 */
                
                // char* wifi_name2;
                // char* password2; 

                char wifi_name2[wl+1]; //malloc(wl);
                char password2[pl+1] ;//= malloc(pl); // char pass[pl];
                strncpy(wifi_name2, wifi_name,wl);
                strncpy(password2, password,pl);
                wifi_name2[wl] = '\0';
                password2[pl] = '\0';

                printf("wifi_name2 %s\n",wifi_name2);
                printf("password2 %s\n",password2);

                //Escribimos en memoria en no volatil
                ESP_LOGI(GATTS_TABLE_TAG, "ESCRIBIENDO EN MEMORIA NO VOLATIL");

                /*status = 21;
                id_protocol = 4;
                acc_sampling = 100;
                acc_sensibility= 8;
                gyro_sensibility= 200;
                BME688_sampling = 4;
                discontinous_time = 1;
                port_tcp = 3000;
                port_udp = 3001;
                host_ip = 1902344;
                wifi_len = 9;
                pass_len = 22;
                wifi_name = "Wifibakan" ;
                password = "AardvarkBadgerHedgehog";*/
                // Opening non volatile memory
                esp_err_t err;
                printf("\n");
                printf("Opening Non-Volatile Storage (NVS) handle... ");
                nvs_handle_t my_handle;
                err = nvs_open("storage", NVS_READWRITE, &my_handle);
                if (err != ESP_OK) {
                    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
                } 
                
                else {
                
                printf("Done\n");


                nvs_set_i8(my_handle, "status", status);
                nvs_set_i8(my_handle, "id_protocol", id_protocol);
                nvs_set_i32(my_handle, "acc_sampling", acc_sampling);
                nvs_set_i32(my_handle, "acc_sensibility", acc_sensibility);
                nvs_set_i32(my_handle, "gyro_sensibiliy", gyro_sensibility);
                nvs_set_i32(my_handle, "BME688_sampling", BME688_sampling);
                nvs_set_i32(my_handle, "discontinous_time", discontinous_time);
                nvs_set_i32(my_handle, "port_tcp", port_tcp);
                nvs_set_i32(my_handle, "port_udp", port_udp);
                nvs_set_i32(my_handle, "host_ip", host_ip);
                //nvs_set_i32(my_handle, "wifi_len", wl);
                //nvs_set_i32(my_handle, "pass_len", pl);
                nvs_set_str(my_handle, "wifi_name", wifi_name2);
                nvs_set_str(my_handle, "password", password2);



                printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
                printf("Committing updates in NVS ... ");
                err = nvs_commit(my_handle);
                        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

                }
                //Testeo: Recuperemos en memoria no volatil para ver si se escribio
                ESP_LOGI(GATTS_TABLE_TAG, "LEYENDO EN MEMORIA NO VOLATIL");


                int8_t recieve_status = 0 ;
                char recieve_wifi[wl+1];
                char receive_pass[pl+1];
                int8_t receive_id_protocol;
                int32_t receive_acc_sampling ;
                int32_t receive_acc_sensibility;
                int32_t receive_gyro_sensibility;
                int32_t receive_BME688_sampling;
                int32_t receive_discontinous_time;
                int32_t receive_port_tcp;
                int32_t receive_port_udp ;
                int32_t receive_host_ip;

                // Read
                printf("Reading status from NVS ... ");
                err = nvs_get_i8(my_handle, "status", &recieve_status);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("Restart recieve status = %d\n", recieve_status);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }

                printf("Reading id protocol from NVS ...\n ");
                err = nvs_get_i8(my_handle, "id_protocol", &receive_id_protocol);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("Restart recieve id_protocol = %d\n", receive_id_protocol);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                
                printf("Reading acc_sampling from NVS ... \n");
                err = nvs_get_i32(my_handle, "acc_sampling", &receive_acc_sampling);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("Restart recieve acc_sampling = %ld\n", receive_acc_sampling);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                printf("Reading receive_acc_sensibility from NVS ... \n");
                err = nvs_get_i32(my_handle, "acc_sensibility", &receive_acc_sensibility);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("acc_sampling = %ld\n", receive_acc_sensibility);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                printf("Reading gyro_sensibility from NVS ... \n");
                err = nvs_get_i32(my_handle, "gyro_sensibility", &receive_gyro_sensibility);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("gyro_sensibility = %ld\n", receive_gyro_sensibility);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                printf("Reading BME688_sampling from NVS ... \n");
                err = nvs_get_i32(my_handle, "BME688_sampling", &receive_BME688_sampling);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("BME688_sampling = %ld\n", receive_BME688_sampling);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                printf("Reading discontinous_time from NVS ... \n");
                err = nvs_get_i32(my_handle, "discontinous_time", &receive_discontinous_time);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("discontinous_time = %ld\n", receive_discontinous_time);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                printf("Reading port_tcp from NVS ... \n");
                err = nvs_get_i32(my_handle, "port_tcp", &receive_port_tcp);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("port_tcp = %ld\n", receive_port_tcp);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                printf("Reading port_udp from NVS ... \n");
                err = nvs_get_i32(my_handle, "port_udp", &receive_port_udp);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("port_udp = %ld\n", receive_port_udp);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                printf("Reading host_ip from NVS ... \n");
                err = nvs_get_i32(my_handle, "host_ip", &receive_host_ip);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("host_ip = %ld\n", receive_host_ip);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }

                // Read 
                printf("Reading wifi strings from NVS ... \n");
                size_t size = (size_t) (wl+1);
                err = nvs_get_str(my_handle, "wifi_name", recieve_wifi,&size);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("Restart wifi = %s\n", recieve_wifi);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }

                printf("Reading password strings from NVS ... \n");
                size_t size2 = (size_t) (pl+1);
                err = nvs_get_str(my_handle, "password", receive_pass,&size2);
                switch (err) {
                    case ESP_OK:
                        printf("Done\n");
                        printf("Restart password = %s\n", receive_pass);
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default :
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }



                // Close
                nvs_close(my_handle);
                ESP_LOGI(GATTS_TABLE_TAG,"----WRITE EVENT ANTES DE UN IF EXTRANO------");
                /*
                free(status);
                free(id_protocol);
                free(acc_sampling);
                free(acc_sensibility);
                free(gyro_sensibility);
                free(BME688_sampling);
                free(discontinous_time);
                free(port_tcp);
                free(port_udp);
                free(host_ip);*/
                free(wifi_name);
                free(password);
                //free(wifi_name2);
                //free(password2);


                if (heart_rate_handle_table[IDX_CHAR_CFG_A] == param->write.handle && param->write.len == 2){
                    uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
                    
                    if (descr_value == 0x0001){
                        ESP_LOGI(GATTS_TABLE_TAG, "notify enable");
                        uint8_t notify_data[15];
                        for (int i = 0; i < sizeof(notify_data); ++i)
                        {
                            notify_data[i] = i % 0xff;
                        }
                        //the size of notify_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, heart_rate_handle_table[IDX_CHAR_VAL_A],
                                                sizeof(notify_data), notify_data, false);
                    }else if (descr_value == 0x0002){
                        ESP_LOGI(GATTS_TABLE_TAG, "indicate enable");
                        uint8_t indicate_data[15];
                        for (int i = 0; i < sizeof(indicate_data); ++i)
                        {
                            indicate_data[i] = i % 0xff;
                        }
                        //the size of indicate_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, heart_rate_handle_table[IDX_CHAR_VAL_A],
                                            sizeof(indicate_data), indicate_data, true);
                    }
                    else if (descr_value == 0x0000){
                        ESP_LOGI(GATTS_TABLE_TAG, "notify/indicate disable ");
                    }else{
                        ESP_LOGE(GATTS_TABLE_TAG, "unknown descr value");
                        esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);
                    }

                }
                /* send response when param->write.need_rsp is true*/
                if (param->write.need_rsp){
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
                }
            }else{
                /* handle prepare write */
                example_prepare_write_event_env(gatts_if, &prepare_write_env, param);
            }
      	    break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            // the length of gattc prepare write data must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
            example_exec_write_event_env(&prepare_write_env, param);
            break;
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
            break;
        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d", param->conf.status, param->conf.handle);
            break;
        case ESP_GATTS_START_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
            break;
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
            esp_log_buffer_hex(GATTS_TABLE_TAG, param->connect.remote_bda, 6);
            esp_ble_conn_update_params_t conn_params = {0};
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            /* For the iOS system, please refer to Apple official documents about the BLE connection parameters restrictions. */
            conn_params.latency = 0;
            conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
            conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
            conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
            //start sent the update connection parameters to the peer device.
            esp_ble_gap_update_conn_params(&conn_params);
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
            esp_ble_gap_start_advertising(&adv_params);
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:{
            if (param->add_attr_tab.status != ESP_GATT_OK){
                ESP_LOGE(GATTS_TABLE_TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
            }
            else if (param->add_attr_tab.num_handle != HRS_IDX_NB){
                ESP_LOGE(GATTS_TABLE_TAG, "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, HRS_IDX_NB);
            }
            else {
                ESP_LOGI(GATTS_TABLE_TAG, "create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
                memcpy(heart_rate_handle_table, param->add_attr_tab.handles, sizeof(heart_rate_handle_table));
                esp_ble_gatts_start_service(heart_rate_handle_table[IDX_SVC]);
            }
            break;
        }
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_DELETE_EVT:
        default:
            break;
    }
}


static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{

    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            heart_rate_profile_tab[PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGE(GATTS_TABLE_TAG, "reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
            if (gatts_if == ESP_GATT_IF_NONE || gatts_if == heart_rate_profile_tab[idx].gatts_if) {
                if (heart_rate_profile_tab[idx].gatts_cb) {
                    heart_rate_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

void app_main(void)
{
    esp_err_t ret;

    /* Initialize NVS. */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gatts register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gap register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(ESP_APP_ID);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(GATTS_TABLE_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }
}
