/*
 * Fichero: components/zigbee_comm/zigbee_comm.c
 * Último cambio: 24/08/2025 - Versión FINAL corregida para la API de Zigbee de transición (ESP-IDF ~v5.1).
 * Descripción: Esta es la implementación definitiva que coincide con la API del usuario.
 *              - Se usa inicialización manual de `esp_zb_cfg_t`.
 *              - Se usa el flujo correcto de ep_list -> cluster_list -> endpoint_config_t -> add_ep.
 *              - La recepción de datos se hace a través de un callback de clúster que SÍ existe en esta API.
 *              - El envío de datos se hace con la función y estructura correctas para esta versión.
 */
#include "zigbee_comm.h"
#include "esp_log.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ha/esp_zigbee_ha_standard.h"

static const char *TAG = "ZIGBEE_COMM";

#define DIYTOGETHER_ENDPOINT 10
#define DIYTOGETHER_CUSTOM_CLUSTER 0xFC00

static zigbee_role_t s_current_role;
static zigbee_data_receive_callback_t s_data_callback = NULL;

/* -------------------------------------------------------------------------- */
/*                        Manejo de Señales y Callbacks                       */
/* -------------------------------------------------------------------------- */

static void bdb_start_top_level_commissioning_cb(uint8_t mode_mask) {
    ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(mode_mask));
}

/**
 * @brief Callback que maneja los comandos que llegan a nuestro clúster personalizado.
 * Esta es la forma correcta de recibir datos en esta versión de la API.
 */
static esp_err_t diytogether_cluster_handler(esp_zb_zcl_custom_cluster_cmd_t *cmd)
{
    if (s_data_callback && cmd->command_id == 0x01) {
        game_packet_t packet;
        // El payload está dentro del puntero 'data'
        uint8_t *payload = (uint8_t *)cmd->data;
        uint16_t data_len = cmd->data_size;

        if (data_len < 1) return ESP_FAIL;

        packet.game_id = payload[0];
        packet.len = data_len - 1;
        if (packet.len > sizeof(packet.payload)) {
            packet.len = sizeof(packet.payload);
        }
        memcpy(packet.payload, &payload[1], packet.len);
        
        // La dirección de origen está en la estructura zcl_basic_cmd
        s_data_callback(cmd->zcl_basic_cmd.src_addr_u.addr_short, &packet);
    }
    return ESP_OK;
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct) {
    uint32_t *p_sg_p = signal_struct->p_app_signal;
    esp_err_t err_status = signal_struct->esp_err_status;
    esp_zb_app_signal_type_t sig_type = *p_sg_p;

    switch (sig_type) {
    case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Zigbee stack initialized");
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
        break;
    case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
        if (err_status == ESP_OK) {
            ESP_LOGI(TAG, "Device started up in %s role", (s_current_role == ZIGBEE_ROLE_COORDINATOR) ? "Coordinator" : "Router");
            if (s_current_role == ZIGBEE_ROLE_COORDINATOR) {
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_FORMATION);
            } else {
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
            }
        } else {
            ESP_LOGE(TAG, "Failed to initialize Zigbee stack (status: %s)", esp_err_to_name(err_status));
        }
        break;
    case ESP_ZB_BDB_SIGNAL_FORMATION:
        if (err_status == ESP_OK) {
            uint16_t short_addr = esp_zb_get_short_address();
            ESP_LOGI(TAG, "Successfully formed network. Address: 0x%04x", short_addr);
            esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
        } else {
            ESP_LOGE(TAG, "Network formation failed (status: %s)", esp_err_to_name(err_status));
        }
        break;
    case ESP_ZB_BDB_SIGNAL_STEERING:
        if (err_status == ESP_OK) {
            uint16_t short_addr = esp_zb_get_short_address();
            ESP_LOGI(TAG, "Successfully joined network. Address: 0x%04x", short_addr);
        } else {
            ESP_LOGW(TAG, "Network steering failed (status: %s). Retrying...", esp_err_to_name(err_status));
            esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb, ESP_ZB_BDB_MODE_NETWORK_STEERING, 1000);
        }
        break;
    default:
        ESP_LOGI(TAG, "ZDO signal: %s (0x%x), status: %s", esp_zb_zdo_signal_to_string(sig_type), sig_type, esp_err_to_name(err_status));
        break;
    }
}

/* -------------------------------------------------------------------------- */
/*                      Inicialización y API Pública                          */
/* -------------------------------------------------------------------------- */

esp_err_t zigbee_comm_init(zigbee_role_t role) {
    s_current_role = role;

    // [CORRECCIÓN] Inicialización manual de la configuración
    esp_zb_cfg_t zb_nwk_cfg;
    memset(&zb_nwk_cfg, 0, sizeof(esp_zb_cfg_t));
    zb_nwk_cfg.esp_zb_role = (role == ZIGBEE_ROLE_COORDINATOR) ? ESP_ZB_DEVICE_TYPE_COORDINATOR : ESP_ZB_DEVICE_TYPE_ROUTER;
    zb_nwk_cfg.install_code_policy = false;
    zb_nwk_cfg.nwk_cfg.zczr_cfg.max_children = 10;
    esp_zb_init(&zb_nwk_cfg);

    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    
    // [CORRECCIÓN] Se crea una estructura de configuración para el endpoint
    esp_zb_endpoint_config_t endpoint_config = {
        .endpoint = DIYTOGETHER_ENDPOINT,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID, // Puede ser un device ID genérico
        .app_device_version = 0
    };

    esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();
    esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(NULL);
    // [CORRECCIÓN] La función de añadir clúster solo toma 3 argumentos ahora
    esp_zb_cluster_list_add_basic_cluster(cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    esp_zb_attribute_list_t *custom_cluster_attr_list = esp_zb_zcl_attr_list_create(DIYTOGETHER_CUSTOM_CLUSTER);
    // [CORRECCIÓN] Se registra el callback de manejo de clúster aquí
    esp_zb_custom_cluster_add_custom_handler(custom_cluster_attr_list, diytogether_cluster_handler, NULL);
    esp_zb_cluster_list_add_custom_cluster(cluster_list, custom_cluster_attr_list, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    // [CORRECCIÓN] La función de añadir endpoint ahora toma la estructura de config
    esp_zb_ep_list_add_ep(ep_list, cluster_list, endpoint_config);

    esp_zb_device_register(ep_list);
    esp_zb_set_primary_network_channel_set(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);
    
    // [CORRECCIÓN] Se usa esp_zb_set_network_device_role
    if (role == ZIGBEE_ROLE_COORDINATOR) {
        esp_zb_set_network_device_role(ESP_ZB_DEVICE_TYPE_COORDINATOR);
    } else {
        esp_zb_set_network_device_role(ESP_ZB_DEVICE_TYPE_ROUTER);
    }

    return ESP_OK;
}

void zigbee_comm_start(void) {
    esp_zb_start(false);
}

esp_err_t zigbee_comm_send_unicast(uint16_t short_addr, const game_packet_t* packet) {
    // [CORRECCIÓN] Se usa la función de envío que el compilador sugirió: esp_zb_zcl_custom_cluster_cmd_req
    esp_zb_zcl_custom_cluster_cmd_req_t cmd_req = {
        .endpoint = DIYTOGETHER_ENDPOINT,
        .cluster_id = DIYTOGETHER_CUSTOM_CLUSTER,
        .command_id = 0x01,
        .dst_address = {
            .addr_short = short_addr,
            .endpoint = DIYTOGETHER_ENDPOINT,
            .mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT
        }
    };

    uint8_t buffer[64];
    buffer[0] = packet->game_id;
    memcpy(&buffer[1], packet->payload, packet->len);

    cmd_req.data = buffer;
    cmd_req.data_size = packet->len + 1;

    return esp_zb_zcl_custom_cluster_cmd_req(&cmd_req);
}

void zigbee_comm_register_data_callback(zigbee_data_receive_callback_t callback) {
    s_data_callback = callback;
}