/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\main\main.c
# Fecha: `$timestamp
# Último cambio: Implementado fallback a modo AP para el servidor de archivos.
# Descripción: Fichero principal de la aplicación DIYMON. Se ha modificado la lógica para que si el modo 'servidor de archivos' no logra conectar a una red WiFi guardada, inicie un Punto de Acceso y ofrezca el servicio desde allí, garantizando siempre el acceso.
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

#include "esp_err.h"
#include "esp_check.h"

static const char *TAG = "DIYMON_MAIN";

// --- Declaraciones de funciones ---
static void run_file_server_mode(void);
static void run_wifi_portal_mode(void);
static void run_main_application_mode(void);
static bool check_file_server_mode_flag(void);
static void erase_file_server_mode_flag(void);

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Sistema NVS inicializado.");

    if (check_file_server_mode_flag()) {
        erase_file_server_mode_flag();
        run_file_server_mode();
    } else if (!wifi_portal_credentials_saved()) {
        run_wifi_portal_mode();
    } else {
        run_main_application_mode();
    }
}

static void run_file_server_mode(void) {
    ESP_LOGI(TAG, "Arrancando en modo Servidor de Archivos...");
    
    bsp_init_service_mode();
    service_screen_show("/sdcard/config/FTP.bin");

    bsp_wifi_init_stack();
    bsp_wifi_init_sta_from_nvs();
    bool ip_ok = bsp_wifi_wait_for_ip(15000);

    if (ip_ok) {
        char ip_addr_buffer[16] = "0.0.0.0";
        bsp_wifi_get_ip(ip_addr_buffer);
        ESP_LOGI(TAG, "Dispositivo conectado a WiFi. IP: %s. Iniciando servidor web.", ip_addr_buffer);
    } else {
        ESP_LOGW(TAG, "No se pudo conectar a la red WiFi guardada. Iniciando en modo Punto de Acceso (AP).");
        bsp_wifi_start_ap();
        ESP_LOGI(TAG, "Punto de Acceso iniciado. Conéctate a 'DIYTogether' (pass: MakeItYours) y navega a http://192.168.4.1");
    }
    
    // El servidor web se inicia tanto si se conecta a una red como si crea la suya propia.
    ESP_LOGI(TAG, "Iniciando servidor web...");
    web_server_start(); // Esta función es bloqueante y no retorna.
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

static bool check_file_server_mode_flag(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al abrir NVS para comprobar la bandera. Error: %s", esp_err_to_name(err));
        return false;
    }

    char flag_val[8] = {0};
    size_t len = sizeof(flag_val);
    err = nvs_get_str(nvs_handle, "file_server", flag_val, &len);
    nvs_close(nvs_handle);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Encontrada la bandera 'file_server' en NVS. Valor: '%s'", flag_val);
        return (strcmp(flag_val, "1") == 0);
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGD(TAG, "La bandera 'file_server' NO se encontró en NVS.");
        return false;
    } else {
        ESP_LOGE(TAG, "Error al leer la bandera 'file_server' de NVS. Error: %s", esp_err_to_name(err));
        return false;
    }
}

static void erase_file_server_mode_flag(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        err = nvs_erase_key(nvs_handle, "file_server");
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) al borrar la clave 'file_server'.", esp_err_to_name(err));
        }

        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) al hacer commit del borrado de la bandera.", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "Marca de modo servidor de archivos borrada de NVS.");
        }
        nvs_close(nvs_handle);
    } else {
        ESP_LOGE(TAG, "Error (%s) al abrir NVS para borrar la bandera.", esp_err_to_name(err));
    }
}
