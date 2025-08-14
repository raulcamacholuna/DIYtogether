/*
Fichero: ./main/main.c
Fecha: $timestamp
Último cambio: Añadida la inicialización de LVGL para los modos de servicio y la llamada a `ui_config_screen_show` para mostrar el fondo de configuración estático, reemplazando la carga de imágenes .bin desde la SD.
Descripción: Orquestador principal de la aplicación. Decide en qué modo arrancar (Aplicación principal, Portal WiFi, Servidor de archivos) basándose en las credenciales guardadas en NVS y en los flags de modo de servicio. Ahora se encarga de preparar un entorno LVGL mínimo para mostrar la nueva pantalla de configuración.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_lvgl_port.h"

#include "bsp_api.h"
#include "hardware_manager.h"
#include "diymon_evolution.h"
#include "ui.h"
#include "wifi_portal.h"
#include "web_server.h"
#include "screen_manager.h"
#include "ui_config_screen.h" // <-- Añadido para la nueva pantalla
#include "ui_asset_loader.h" 

#include "esp_err.h"
#include "esp_check.h"

static const char *TAG = "DIYMON_MAIN";

// --- Declaraciones de funciones ---
static void run_file_server_mode(void);
static void run_wifi_portal_mode(void);
static void run_main_application_mode(void);
static bool check_file_server_mode_flag(void);
static void erase_file_server_mode_flag(void);
static void init_lvgl_for_service_screen(void); // <-- Nueva función de ayuda

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Sistema NVS inicializado.");

    ui_preinit();

    if (check_file_server_mode_flag()) {
        erase_file_server_mode_flag();
        run_file_server_mode();
    } else if (!wifi_portal_credentials_saved()) {
        run_wifi_portal_mode();
    } else {
        run_main_application_mode();
    }
}

static void init_lvgl_for_service_screen(void)
{
    ESP_LOGI(TAG, "Inicializando LVGL para pantalla de servicio...");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = bsp_get_panel_io_handle(),
        .panel_handle = bsp_get_display_handle(),
        .buffer_size = bsp_get_display_buffer_size(),
        .double_buffer = 1,
        .hres = bsp_get_display_hres(),
        .vres = bsp_get_display_vres(),
    };
    lvgl_port_add_disp(&disp_cfg);
}

static void run_file_server_mode(void) {
    ESP_LOGI(TAG, "Arrancando en modo Servidor de Archivos...");
    
    bsp_init_service_mode();
    init_lvgl_for_service_screen();
    
    if (lvgl_port_lock(0)) {
        ui_config_screen_show(); // Muestra la nueva pantalla estática
        lvgl_port_unlock();
    }

    bsp_wifi_init_stack();
    bsp_wifi_init_sta_from_nvs();
    bool ip_ok = bsp_wifi_wait_for_ip(15000);

    if (ip_ok) {
        char ip_addr_buffer[16] = "0.0.0.0";
        bsp_wifi_get_ip(ip_addr_buffer);
        ESP_LOGI(TAG, "Dispositivo conectado. IP: %s. Iniciando servidor web.", ip_addr_buffer);
    } else {
        ESP_LOGW(TAG, "No se pudo conectar. Iniciando en modo Punto de Acceso (AP).");
        bsp_wifi_start_ap();
        ESP_LOGI(TAG, "Punto de Acceso iniciado. Conéctate a 'DIYTogether' (pass: MakeItYours) y navega a http://192.168.4.1");
    }
    
    ESP_LOGI(TAG, "Iniciando servidor web...");
    web_server_start(); // Esta función es bloqueante
}

static void run_wifi_portal_mode(void) {
    ESP_LOGI(TAG, "No hay credenciales. Arrancando en modo Portal WiFi...");
    
    bsp_init_service_mode();
    init_lvgl_for_service_screen();
    
    if (lvgl_port_lock(0)) {
        ui_config_screen_show(); // Muestra la nueva pantalla estática
        lvgl_port_unlock();
    }

    bsp_wifi_init_stack();
    wifi_portal_start(); // Esta función es bloqueante y reinicia al final
}

static void run_main_application_mode(void) {
    ESP_LOGI(TAG, "Cargando aplicación principal...");
    hardware_manager_init();
    screen_manager_init();
    
    ESP_LOGI(TAG, "El driver WiFi permanece desactivado para ahorrar RAM.");
    
    diymon_evolution_init();
    
    ui_assets_init();

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
    if (err != ESP_OK) return false;

    char flag_val[8] = {0};
    size_t len = sizeof(flag_val);
    err = nvs_get_str(nvs_handle, "file_server", flag_val, &len);
    nvs_close(nvs_handle);
    return (err == ESP_OK && strcmp(flag_val, "1") == 0);
}

static void erase_file_server_mode_flag(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_erase_key(nvs_handle, "file_server");
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}
