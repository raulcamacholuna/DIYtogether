/*
  Fichero: ./components/wifi_portal/wifi_portal.c
  Fecha: 12/08/2025 - 11:00
  Último cambio: Eliminada la inicialización redundante del stack de red.
  Descripción: Portal de configuración WiFi. Se elimina la llamada a `bsp_wifi_init_stack()`
               para que el componente dependa de la inicialización global en `app_main`,
               evitando conflictos y asegurando un estado de red consistente.
*/
#include "wifi_portal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "bsp_api.h"
#include <stdlib.h>

static const char *TAG = "WIFI_PORTAL";
static EventGroupHandle_t s_portal_event_group;
const int CREDENTIALS_RECEIVED_BIT = BIT0;

static const char* HTML_FORM =
    "<!DOCTYPE html><html><head><title>Configuracion WiFi DIYMON</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>"
    "<body><h1>Configura el WiFi de tu DIYMON</h1>"
    "<h2>Conectate a tu red local</h2>"
    "<form method='post' action='/save'>"
    "<p><label>SSID (Nombre de tu red):</label><br><input type='text' name='ssid'></p>"
    "<p><label>Password:</label><br><input type='password' name='password'></p>"
    "<p><label>Seguridad (si falla, prueba 'Forzar WPA2'):</label><br>"
    "<select name='authmode'>"
    "  <option value='0' selected>Automatico (WPA2/WPA3)</option>"
    "  <option value='1'>Forzar WPA2 (Recomendado)</option>"
    "  <option value='2'>Forzar WPA3</option>"
    "</select></p>"
    "<p><button type='submit'>Guardar y Reiniciar</button></p>"
    "</form></body></html>";

static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_send(req, HTML_FORM, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t save_post_handler(httpd_req_t *req) {
    char buf[256];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';

    char ssid[32] = {0};
    char password[64] = {0};
    char authmode_str[4] = {0};
    httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(buf, "password", password, sizeof(password));
    httpd_query_key_value(buf, "authmode", authmode_str, sizeof(authmode_str));
    int32_t authmode = atoi(authmode_str);

    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "wifi_ssid", ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "wifi_pass", password));
    ESP_ERROR_CHECK(nvs_set_i32(nvs_handle, "wifi_authmode", authmode));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Credenciales guardadas en NVS (SSID: %s, Authmode: %ld).", ssid, authmode);

    httpd_resp_send(req, "<h1>Configuracion guardada!</h1><p>El dispositivo se reiniciara.</p>", HTTPD_RESP_USE_STRLEN);
    
    vTaskDelay(pdMS_TO_TICKS(1000));
    xEventGroupSetBits(s_portal_event_group, CREDENTIALS_RECEIVED_BIT);
    
    return ESP_OK;
}

static httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root = {.uri = "/", .method = HTTP_GET, .handler = root_get_handler};
        httpd_register_uri_handler(server, &root);
        httpd_uri_t save = {.uri = "/save", .method = HTTP_POST, .handler = save_post_handler};
        httpd_register_uri_handler(server, &save);
    }
    return server;
}

void wifi_portal_start(void) {
    s_portal_event_group = xEventGroupCreate();
    
    // Se elimina la inicialización del stack, ya que ahora se hace en main.c
    bsp_wifi_start_ap();
    
    httpd_handle_t server = start_webserver();
    if (server == NULL) {
        ESP_LOGE(TAG, "Error al iniciar servidor, reiniciando...");
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_restart();
    }
    
    ESP_LOGI(TAG, "Portal en marcha. Esperando credenciales via web...");
    xEventGroupWaitBits(s_portal_event_group, CREDENTIALS_RECEIVED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
    
    ESP_LOGI(TAG, "Credenciales recibidas. Deteniendo portal y reiniciando...");
    httpd_stop(server);
    esp_wifi_stop();
    esp_wifi_deinit();
    vEventGroupDelete(s_portal_event_group);
    
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
}

bool wifi_portal_credentials_saved(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) return false;
    size_t required_size = 0;
    err = nvs_get_str(nvs_handle, "wifi_ssid", NULL, &required_size);
    nvs_close(nvs_handle);
    if (err == ESP_OK && required_size > 1) {
        return true;
    }
    return false;
}

void wifi_portal_erase_credentials(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) return;
    nvs_erase_key(nvs_handle, "wifi_ssid");
    nvs_erase_key(nvs_handle, "wifi_pass");
    nvs_erase_key(nvs_handle, "wifi_authmode");
    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Credenciales WiFi completas borradas de NVS.");
}