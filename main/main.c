/*
  Fichero: ./main/main.c
  Fecha: 12/08/2025 - 12:30
  Último cambio: Movida la inicialización del stack de red a los modos de operación específicos.
  Descripción: Orquestador principal. La inicialización del stack de red se ha movido
               de `app_main` a las funciones `run_ftp_mode` y `run_wifi_portal_mode`.
               Esto soluciona el fallo de asignación de memoria en el modo de aplicación
               principal al no reservar recursos de red innecesarios, y mantiene la
               correcta obtención de IP en los modos que sí la requieren.
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
#include "ftp_server.h"
#include "screen_manager.h"
#include "service_screen.h"

static const char *TAG = "DIYMON_MAIN";

// --- Declaraciones de funciones ---
static void run_ftp_mode(void);
static void run_wifi_portal_mode(void);
static void run_main_application_mode(void);
static bool check_ftp_flag(void);
static void erase_ftp_flag(void);
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

    if (check_ftp_flag()) {
        erase_ftp_flag();
        run_ftp_mode();
    } else if (!wifi_portal_credentials_saved()) {
        run_wifi_portal_mode();
    } else {
        run_main_application_mode();
    }
}

static void run_ftp_mode(void) {
    ESP_LOGI(TAG, "Arrancando en modo FTP...");
    
    // Se inicializa el stack de red SOLO para este modo.
    bsp_wifi_init_stack();
    
    service_screen_show("/sdcard/config/FTP.bin");
    
    bsp_wifi_init_sta_from_nvs();
    bool ip_ok = bsp_wifi_wait_for_ip(15000);

    if (ip_ok) {
        char ip_addr_buffer[16] = "0.0.0.0";
        bsp_wifi_get_ip(ip_addr_buffer);
        ESP_LOGI(TAG, "Dispositivo conectado. IP: %s. Iniciando servidor FTP.", ip_addr_buffer);
        ftp_server_start(); // Bloqueante
    } else {
        ESP_LOGE(TAG, "No se pudo obtener IP. Reiniciando en 10 segundos...");
        vTaskDelay(pdMS_TO_TICKS(10000));
        esp_restart();
    }
}

static void run_wifi_portal_mode(void) {
    ESP_LOGI(TAG, "No hay credenciales. Arrancando en modo Portal WiFi...");
    
    // Se inicializa el stack de red SOLO para este modo.
    bsp_wifi_init_stack();

    service_screen_show("/sdcard/config/WIFI.bin");
    wifi_portal_start(); // Bloqueante, reinicia al finalizar
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

    const esp_timer_create_args_t evolution_timer_args = {
        .callback = &evolution_timer_callback, .name = "evolution-timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&evolution_timer_args, &evolution_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(evolution_timer_handle, 5 * 1000000));
    
    ESP_LOGI(TAG, "¡Firmware DIYMON en marcha!");
}

static bool check_ftp_flag(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) return false;
    char ftp_flag[2] = {0};
    size_t len = sizeof(ftp_flag);
    err = nvs_get_str(nvs_handle, "enable_ftp", ftp_flag, &len);
    nvs_close(nvs_handle);
    return (err == ESP_OK && strcmp(ftp_flag, "1") == 0);
}

static void erase_ftp_flag(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_erase_key(nvs_handle, "enable_ftp");
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
        ESP_LOGI(TAG, "Marca FTP borrada de NVS.");
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