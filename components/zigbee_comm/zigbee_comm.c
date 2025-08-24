/*
 * Fichero: components/zigbee_comm/zigbee_comm.c
 * Último cambio: 25/08/2025 - LA VERSIÓN FINAL. Basada en la documentación oficial v1.6.6.
 * Descripción: Este código se alinea con la API antigua que el entorno del usuario está utilizando.
 */
#include "zigbee_comm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ha/esp_zigbee_ha_standard.h"
#include "string.h"

static const char *TAG = "ZIGBEE_COMM";

#define DIYTOGETHER_ENDPOINT 10
#define DIYTOGETHER_CUSTOM_CLUSTER 0xFC00

static zigbee_role_t s_current_role;
static zigbee_data_receive_callback_t s_data_callback = NULL;

/**
 * @brief Handler para recibir datos del clúster. La firma es la correcta para el usrcb.
 */
static void diytogether_cluster_handler(esp_zb_zcl_custom_cluster_cmd_rcv_message_t *msg)
{
    if (s_data_callback && msg->cluster_id == DIYTOGETHER_CUSTOM_CLUSTER && msg->command_id == 0x01)
    {
        game_packet_t packet;
        uint8_t *payload = (uint8_t *)msg->data_p;
        packet.game_id = payload[0];
        packet.len = msg->data_size - 1;
        if (packet.len > sizeof(packet.payload)) {
            packet.len = sizeof(packet.payload);
        }
        memcpy(packet.payload, &payload[1], packet.len);
        s_data_callback(msg->src_addr.u.short_addr, &packet);
    }
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct)
{
    uint32_t *p_sg_p = signal_struct->p_app_signal;
    esp_err_t err_status = signal_struct->esp_err_status;
    esp_zb_app_signal_type_t sig_type = *p_sg_p;

    switch (sig_type)
    {
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
    default:
        ESP_LOGD(TAG, "ZDO signal: 0x%x, status: %s", sig_type, esp_err_to_name(err_status));
        break;
    }
}

esp_err_t zigbee_comm_init(zigbee_role_t role)
{
    s_current_role = role;

    // [CORRECCIÓN] Usar las macros de inicialización que SÍ tienes.
    esp_zb_cfg_t zb_nwk_cfg;
    if (role == ZIGBEE_ROLE_COORDINATOR) {
        zb_nwk_cfg = ESP_ZB_ZC_CONFIG();
    } else {
        zb_nwk_cfg = ESP_ZB_ZR_CONFIG();
    }
    esp_zb_init(&zb_nwk_cfg);

    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();

    // [CORRECCIÓN] Usar `esp_zb_cluster_list_add_cluster` para el clúster básico.
    esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(NULL);
    esp_zb_cluster_list_add_cluster(ep_list, ESP_ZB_ZCL_CLUSTER_ID_BASIC, basic_cluster, DIYTOGETHER_ENDPOINT, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0);

    // [CORRECCIÓN] Añadir el clúster personalizado y castear el handler al tipo correcto.
    esp_zb_attribute_list_t *custom_cluster_attr_list = esp_zb_zcl_attr_list_create(DIYTOGETHER_CUSTOM_CLUSTER);
    esp_zb_cluster_list_add_custom_cluster(ep_list, DIYTOGETHER_CUSTOM_CLUSTER, custom_cluster_attr_list, DIYTOGETHER_ENDPOINT, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, (esp_zb_zcl_custom_cluster_usrcb_t)diytogether_cluster_handler);
    
    esp_zb_device_register(ep_list);
    esp_zb_set_primary_network_channel_set(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);

    return ESP_OK;
}

void zigbee_comm_start(void)
{
    esp_zb_start(false);
}

esp_err_t zigbee_comm_send_unicast(uint16_t short_addr, const game_packet_t* packet)
{
    // [CORRECCIÓN] Usar la estructura y función de envío originales.
    esp_zb_zcl_custom_cluster_cmd_t cmd_req;
    
    cmd_req.dst_addr_u.addr_short = short_addr;
    cmd_req.dst_endpoint = DIYTOGETHER_ENDPOINT;
    cmd_req.src_endpoint = DIYTOGETHER_ENDPOINT;
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.cluster_id = DIYTOGETHER_CUSTOM_CLUSTER;
    cmd_req.command_id = 0x01;
    cmd_req.is_manufacturer_specific = false;

    // La librería se encarga de la memoria del payload en esta versión.
    uint8_t *buffer = (uint8_t*)malloc(packet->len + 1);
    if (!buffer) {
        return ESP_ERR_NO_MEM;
    }
    buffer[0] = packet->game_id;
    memcpy(&buffer[1], packet->payload, packet->len);
    
    cmd_req.data_p = buffer;
    cmd_req.data_size = packet->len + 1;

    return esp_zb_zcl_custom_cluster_send_cmd(&cmd_req);
}

void zigbee_comm_register_data_callback(zigbee_data_receive_callback_t callback)
{
    s_data_callback = callback;
}