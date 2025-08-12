/*
  Fichero: ./components/diymon_bsp/WS1.9TS/bsp_wifi.c
  Fecha: 12/08/2025 - 04:35 pm
  Último cambio: Añadido log de depuración para mostrar la contraseña leída de NVS.
  Descripción: Gestor de conexión WiFi. Se añade un log de advertencia para imprimir la contraseña real utilizada durante la conexión, facilitando la depuración de caracteres especiales.
*/
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

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        switch(event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "EVENTO: WIFI_EVENT_STA_START - Intentando conectar...");
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "EVENTO: WIFI_EVENT_STA_CONNECTED - Conectado al AP, esperando IP.");
                break;
            case WIFI_EVENT_STA_DISCONNECTED: {
                wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
                ESP_LOGE(TAG, "EVENTO: WIFI_EVENT_STA_DISCONNECTED - Desconexión de la red.");
                ESP_LOGE(TAG, "Razón de la desconexión: %d", event->reason);
                if (event->reason == WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT) {
                    ESP_LOGE(TAG, "DIAGNÓSTICO: Error '4-Way Handshake Timeout'. ¡Verifica que la contraseña guardada sea correcta para la red!");
                }
                break;
            }
            default:
                ESP_LOGD(TAG, "Recibido evento WiFi no gestionado: %d", (int)event_id);
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "EVENTO: IP_EVENT_STA_GOT_IP - ¡IP Obtenida!: " IPSTR, IP2STR(&event->ip_info.ip));
        if (s_ip_acquired_sem) {
            xSemaphoreGive(s_ip_acquired_sem);
        }
    }
}


void bsp_wifi_init_stack(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}

esp_err_t bsp_wifi_scan(void) {
    ESP_LOGI(TAG, "Iniciando escaneo de redes WiFi (modo autónomo)...");
    
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_scan_config_t scan_config = {
        .ssid = 0, .bssid = 0, .channel = 0, .show_hidden = false
    };
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    if (ap_count > 0) {
        wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * ap_count);
        if (ap_list) {
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_list));
            ESP_LOGI(TAG, "------------------- REDES WIFI ENCONTRADAS -------------------");
            for (int i = 0; i < ap_count; i++) {
                ESP_LOGI(TAG, "SSID: %-32s | RSSI: %d", (char*)ap_list[i].ssid, ap_list[i].rssi);
            }
            ESP_LOGI(TAG, "------------------------------------------------------------");
            free(ap_list);
        }
    } else {
        ESP_LOGW(TAG, "No se encontraron redes WiFi.");
    }
    
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
    esp_netif_destroy(sta_netif);
    
    ESP_LOGI(TAG, "Escaneo autónomo finalizado, recursos liberados.");
    return ESP_OK;
}

void bsp_wifi_start_ap(void) {
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = PORTAL_AP_SSID,
            .ssid_len = strlen(PORTAL_AP_SSID),
            .channel = PORTAL_AP_CHANNEL,
            .password = PORTAL_AP_PASS,
            .max_connection = PORTAL_AP_MAX_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(PORTAL_AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Punto de Acceso WiFi iniciado. SSID: %s, Password: %s",
             PORTAL_AP_SSID, PORTAL_AP_PASS);
}

void bsp_wifi_init_sta_from_nvs(void) {
    bsp_wifi_scan();
    
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    s_ip_acquired_sem = xSemaphoreCreateBinary();

    char ssid[32] = {0}, pass[64] = {0};
    int32_t authmode = 0;
    size_t len_ssid = sizeof(ssid), len_pass = sizeof(pass);

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "No se pudo abrir NVS para leer credenciales WiFi.");
        return;
    }
    nvs_get_str(nvs_handle, "wifi_ssid", ssid, &len_ssid);
    nvs_get_str(nvs_handle, "wifi_pass", pass, &len_pass);
    nvs_get_i32(nvs_handle, "wifi_authmode", &authmode);
    nvs_close(nvs_handle);

    if (len_ssid > 1 && strcmp(ssid, "skipped") != 0) {
        // --- [NUEVO] Log de depuración para mostrar la contraseña ---
        ESP_LOGW(TAG, "DEPURACIÓN: Usando SSID:[%s] | Contraseña:[%s] | ModoAuth:[%d]", ssid, pass, (int)authmode);

        wifi_config_t wifi_config = {0};
        strcpy((char *)wifi_config.sta.ssid, ssid);
        strcpy((char *)wifi_config.sta.password, pass);
        wifi_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
        
        switch (authmode) {
            case 2: // Forzar WPA3
                ESP_LOGI(TAG, "Configurando conexión: WPA3-PSK (PMF Requerido).");
                wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA3_PSK;
                wifi_config.sta.pmf_cfg.capable = true;
                wifi_config.sta.pmf_cfg.required = true;
                break;
            case 1: // Forzar WPA2
                ESP_LOGI(TAG, "Configurando conexión: WPA2-PSK (PMF Desactivado).");
                wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
                wifi_config.sta.pmf_cfg.capable = false;
                wifi_config.sta.pmf_cfg.required = false;
                break;
            case 0: // Automático (por defecto)
            default:
                ESP_LOGI(TAG, "Configurando conexión: Modo automático -> WPA2-PSK (PMF Desactivado para compatibilidad).");
                wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
                wifi_config.sta.pmf_cfg.capable = false;
                wifi_config.sta.pmf_cfg.required = false;
                break;
        }
        
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        
        ESP_LOGI(TAG, "Protocolos WiFi limitados a 802.11b/g/n para máxima compatibilidad.");
        ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N));
        
        ESP_ERROR_CHECK(esp_wifi_start());
        ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

        ESP_LOGI(TAG, "Iniciando conexión a la red guardada: %s", ssid);
    } else {
        ESP_LOGW(TAG, "No hay credenciales WiFi válidas para conectar.");
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
