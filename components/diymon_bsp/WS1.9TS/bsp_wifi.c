/* Fecha: 16/08/2025 - 07:50  */
/* Fichero: components/diymon_bsp/WS1.9TS/bsp_wifi.c */
/* Último cambio: Hecha la inicialización del stack de red (netif, event loop) idempotente para evitar un crash al llamarla múltiples veces. */
/* Descripción: Gestor de conexión WiFi. El crash (ESP_ERR_INVALID_STATE) ocurría porque `esp_event_loop_create_default()` era llamado en el arranque principal y de nuevo al entrar en modo configuración. La función `bsp_wifi_init_stack` ahora utiliza una bandera estática para asegurar que el stack de red solo se inicialice una vez, resolviendo el error. */
#include "bsp_api.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <assert.h>

#define PORTAL_AP_SSID          "DIYTogether"
#define PORTAL_AP_PASS          "MakeItYours"
#define PORTAL_AP_CHANNEL       1
#define PORTAL_AP_MAX_CONN      4

static const char *TAG = "bsp_wifi";
static SemaphoreHandle_t s_ip_acquired_sem = NULL;
static bool g_network_stack_initialized = false; // Flag de idempotencia

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        switch(event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "Conectado al AP, esperando IP.");
                break;
            case WIFI_EVENT_STA_DISCONNECTED: {
                vTaskDelay(pdMS_TO_TICKS(1000));
                ESP_LOGI(TAG, "Reintentando conexión...");
                esp_wifi_connect();
                break;
            }
            default:
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "¡IP Obtenida!: " IPSTR, IP2STR(&event->ip_info.ip));
        if (s_ip_acquired_sem) {
            xSemaphoreGive(s_ip_acquired_sem);
        }
    }
}

void bsp_wifi_init_stack(void) {
    if (g_network_stack_initialized) {
        ESP_LOGD(TAG, "El stack de red ya ha sido inicializado.");
        return;
    }
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    g_network_stack_initialized = true;
    ESP_LOGI(TAG, "Stack de red (netif, event loop) inicializado por primera vez.");
}

void bsp_wifi_start_ap(void) {
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = PORTAL_AP_SSID, .ssid_len = strlen(PORTAL_AP_SSID),
            .channel = PORTAL_AP_CHANNEL, .password = PORTAL_AP_PASS,
            .max_connection = PORTAL_AP_MAX_CONN, .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(PORTAL_AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void bsp_wifi_init_sta_from_nvs(void) {
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    s_ip_acquired_sem = xSemaphoreCreateBinary();

    char ssid[32] = {0}, pass[64] = {0};
    size_t len_ssid = sizeof(ssid), len_pass = sizeof(pass);

    nvs_handle_t nvs_handle;
    if (nvs_open("storage", NVS_READONLY, &nvs_handle) == ESP_OK) {
        nvs_get_str(nvs_handle, "wifi_ssid", ssid, &len_ssid);
        nvs_get_str(nvs_handle, "wifi_pass", pass, &len_pass);
        nvs_close(nvs_handle);
    }

    if (len_ssid > 1) {
        wifi_config_t wifi_config = {0};
        strcpy((char *)wifi_config.sta.ssid, ssid);
        strcpy((char *)wifi_config.sta.password, pass);
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
        
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
    }
}

bool bsp_wifi_wait_for_ip(uint32_t timeout_ms) {
    if (!s_ip_acquired_sem) return false;
    return (xSemaphoreTake(s_ip_acquired_sem, pdMS_TO_TICKS(timeout_ms)) == pdTRUE);
}

void bsp_wifi_get_ip(char *ip) {
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if(netif) {
        esp_netif_ip_info_t ip_info;
        esp_netif_get_ip_info(netif, &ip_info);
        sprintf(ip, IPSTR, IP2STR(&ip_info.ip));
    }
}

void bsp_wifi_init_sta(const char *ssid, const char *pass) {
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    s_ip_acquired_sem = xSemaphoreCreateBinary();

    if (ssid && strlen(ssid) > 0) {
        wifi_config_t wifi_config = {0};
        strcpy((char *)wifi_config.sta.ssid, ssid);
        if (pass) { strcpy((char *)wifi_config.sta.password, pass); }
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
    }
}
