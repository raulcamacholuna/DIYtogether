#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_lvgl_port.h"
#include "esp_timer.h"
#include "esp_system.h"

#include "hardware_manager.h"
#include "diymon_evolution.h"
#include "ui.h"
#include "screens.h"
#include "wifi_portal.h"

static const char *TAG = "DIYMON_MAIN";

static esp_timer_handle_t evolution_timer_handle;

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
        ESP_LOGI(TAG, "El DIYMON ha alcanzado su forma final. Deteniendo temporizador.");
        esp_timer_stop(evolution_timer_handle);
    }
}

void app_main(void)
{
    // --- PASO 1: Inicialización de NVS ---
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Sistema NVS inicializado.");

    // --- PASO 2: Comprobación de configuración WiFi ---
    bool needs_portal = !wifi_portal_credentials_saved();
    
    if (needs_portal) {
        // --- ANOTACIÓN: Si no hay WiFi, se inicializa el hardware y se lanza el portal. ---
        hardware_manager_init();
        ESP_LOGI(TAG, "Hardware y LVGL inicializados para el portal.");
        
        wifi_portal_result_t portal_result = wifi_portal_start();
        
        if (portal_result == PORTAL_CONFIG_SAVED) {
            ESP_LOGI(TAG, "Configuracion guardada. Reiniciando dispositivo...");
            vTaskDelay(pdMS_TO_TICKS(1000)); // Pequeña pausa para que se complete cualquier I/O.
            esp_restart();
        }
        // Si se omite (portal_result == PORTAL_CONFIG_SKIPPED), simplemente continuamos.
    }

    // --- PASO 3: Carga de la aplicación principal ---
    // Este código se ejecuta si las credenciales ya existían, o si el usuario omitió el portal.
    if (!needs_portal) {
        // Si saltamos el portal, el hardware aún no está inicializado.
        hardware_manager_init();
        ESP_LOGI(TAG, "Hardware y LVGL inicializados para la aplicacion principal.");
    }
    
    ESP_LOGI(TAG, "Cargando aplicacion principal...");

    diymon_evolution_init();
    ESP_LOGI(TAG, "Logica del DIYMON Core inicializada.");

    if (lvgl_port_lock(0)) {
        ui_init();
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Interfaz de Usuario principal inicializada.");

    // --- PASO 4: Iniciar la lógica de juego ---
    const esp_timer_create_args_t evolution_timer_args = {
            .callback = &evolution_timer_callback, .name = "evolution-timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&evolution_timer_args, &evolution_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(evolution_timer_handle, 5 * 1000000));
    
    ESP_LOGI(TAG, "¡Firmware DIYMON en marcha! ¡Bienvenido, creador!");
}