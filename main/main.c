/*
  Fichero: ./main/main.c
  Fecha: 13/08/2025 - 12:06 
  Último cambio: Eliminada la tarea de telemetría.
  Descripción: Se elimina la tarea sensor_log_task de este fichero. La responsabilidad
               de leer y mostrar los datos de los sensores se ha trasladado completamente
               al nuevo componente ui_telemetry dentro de diymon_ui, centralizando toda
               la lógica de la interfaz.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs.h"
#include "esp_lvgl_port.h"
#include "esp_timer.h"

#include "bsp_api.h"
#include "hardware_manager.h"
#include "diymon_evolution.h"
#include "ui.h"
#include "screens.h"
#include "wifi_portal.h"
#include "web_server.h"
#include "screen_manager.h"
#include "service_screen.h"

static const char *TAG = "DIYMON_MAIN";

// --- Declaraciones de funciones ---
static void run_config_server_mode(void);
static void run_wifi_portal_mode(void);
static void run_main_application_mode(void);
static bool check_config_mode_flag(void);
static void erase_config_mode_flag(void);
static void evolution_timer_callback(void* arg);

static esp_timer_handle_t evolution_timer_handle;

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Sistema NVS inicializado.");

    if (check_config_mode_flag()) {
        erase_config_mode_flag();
        run_config_server_mode();
    } else if (!wifi_portal_credentials_saved()) {
        run_wifi_portal_mode();
    } else {
        run_main_application_mode();
    }
}

static void run_config_server_mode(void) {
    ESP_LOGI(TAG, "Arrancando en modo Servidor Web de Configuración (STA)...");
    
    bsp_init_service_mode();
    service_screen_show("/sdcard/config/FTP.bin");

    bsp_wifi_init_stack();
    bsp_wifi_init_sta_from_nvs();
    bool ip_ok = bsp_wifi_wait_for_ip(15000);

    if (ip_ok) {
        char ip_addr_buffer[16] = "0.0.0.0";
        bsp_wifi_get_ip(ip_addr_buffer);
        ESP_LOGI(TAG, "Dispositivo conectado. IP: %s. Iniciando servidor web.", ip_addr_buffer);
        web_server_start();
    } else {
        ESP_LOGE(TAG, "No se pudo obtener IP. Reiniciando en 10 segundos...");
        vTaskDelay(pdMS_TO_TICKS(10000));
        esp_restart();
    }
}

static void run_wifi_portal_mode(void) {
    ESP_LOGI(TAG, "No hay credenciales. Arrancando en modo Portal WiFi...");
    
    bsp_init_service_mode();
    service_screen_show("/sdcard/config/WIFI.bin");
    
    bsp_wifi_init_stack();
    wifi_portal_start();
}

static void run_main_application_mode(void) {
    ESP_LOGI(TAG, "Cargando aplicación principal...");
    hardware_manager_init();
    screen_manager_init();
    
    ESP_LOGI(TAG, "El driver WiFi permanece desactivado para ahorrar RAM.");
    
    diymon_evolution_init();

    if (lvgl_port_lock(0)) {
        ui_init();
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Interfaz de Usuario principal inicializada.");
    
    ESP_LOGI(TAG, "¡Firmware DIYMON en marcha!");
}

static bool check_config_mode_flag(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al abrir NVS para comprobar la bandera de config. Error: %s", esp_err_to_name(err));
        return false;
    }

    char config_flag[8] = {0};
    size_t len = sizeof(config_flag);
    err = nvs_get_str(nvs_handle, "config_mode", config_flag, &len);
    nvs_close(nvs_handle);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Encontrada la bandera 'config_mode' en NVS. Valor: '%s'", config_flag);
        return (strcmp(config_flag, "1") == 0);
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "La bandera 'config_mode' NO se encontró en NVS.");
        return false;
    } else {
        ESP_LOGE(TAG, "Error al leer la bandera 'config_mode' de NVS. Error: %s", esp_err_to_name(err));
        return false;
    }
}

static void erase_config_mode_flag(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        err = nvs_erase_key(nvs_handle, "config_mode");
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) al borrar la clave 'config_mode'.", esp_err_to_name(err));
        }

        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) al hacer commit del borrado de la bandera.", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "Marca de modo configuración borrada de NVS.");
        }
        nvs_close(nvs_handle);
    } else {
        ESP_LOGE(TAG, "Error (%s) al abrir NVS para borrar la bandera.", esp_err_to_name(err));
    }
}

static void evolution_timer_callback(void* arg) {
    const char* current_code = diymon_get_current_code();
    const char* next_evolution = diymon_get_next_evolution_in_sequence(current_code);
    if (next_evolution != NULL) {
        ESP_LOGI(TAG, "¡EVOLUCIÓN! Nuevo código: %s", next_evolution);
        diymon_set_current_code(next_evolution);
        if (lvgl_port_lock(0)) {
            delete_screen_main();
            ui_init();
            lvgl_port_unlock();
        }
    } else {
        ESP_LOGI(TAG, "El DIYMON ha alcanzado su forma final.");
        esp_timer_stop(evolution_timer_handle);
    }
}
