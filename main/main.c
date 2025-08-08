/*
 * Fichero: main.c
 * Descripción: Punto de entrada principal para el firmware de DIYMON.
 *              Inicializa los servicios básicos, el gestor de hardware, la
 *              lógica del juego y la interfaz de usuario.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_lvgl_port.h"
#include "esp_timer.h"

// Componentes principales de la aplicación.
#include "hardware_manager.h"
#include "diymon_evolution.h"
#include "ui.h"

// Se incluye 'screens.h' para que este fichero conozca la declaración de la
// función 'delete_screen_main', solucionando así el error de compilación.
#include "screens.h"

static const char *TAG = "DIYMON_MAIN";

// Manejador para el temporizador periódico de evolución.
static esp_timer_handle_t evolution_timer_handle;

/**
 * @brief Callback que se ejecuta periódicamente para gestionar la evolución del DIYMON.
 */
static void evolution_timer_callback(void* arg) {
    const char* current_code = diymon_get_current_code();
    ESP_LOGI(TAG, "Revisando evolución. DIYMON actual: %s", current_code);

    const char* next_evolution = diymon_get_next_evolution_in_sequence(current_code);

    if (next_evolution != NULL) {
        ESP_LOGI(TAG, "¡EVOLUCIÓN! Nuevo código: %s", next_evolution);
        
        // 1. Se actualiza el estado lógico del Diymon a su nueva forma.
        diymon_set_current_code(next_evolution);

        // 2. Se actualiza la interfaz de usuario para reflejar la evolución.
        //    Se destruye la pantalla actual y se vuelve a inicializar la UI por completo
        //    para forzar la carga de los nuevos recursos desde la SD.
        delete_screen_main();
        ui_init();

    } else {
        ESP_LOGI(TAG, "El DIYMON ha alcanzado su forma final. Deteniendo temporizador de evolución.");
        esp_timer_stop(evolution_timer_handle);
    }
}


void app_main(void)
{
    // --- 1. Inicialización del Sistema Base ---
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Partición NVS corrupta, borrando y reinicializando...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Sistema NVS inicializado correctamente.");


    // --- 2. Inicialización del Hardware (BSP) ---
    hardware_manager_init();
    ESP_LOGI(TAG, "Gestor de Hardware inicializado correctamente.");


    // --- 3. Inicialización del Motor de Lógica del Juego ---
    diymon_evolution_init();
    ESP_LOGI(TAG, "Lógica del DIYMON Core inicializada correctamente.");


    // --- 4. Inicialización de la Interfaz de Usuario (UI) ---
    ESP_LOGI(TAG, "Inicializando Interfaz de Usuario...");
    ui_init();
    

    // --- 5. Creación e Inicio del Temporizador de Evolución ---
    const esp_timer_create_args_t evolution_timer_args = {
            .callback = &evolution_timer_callback,
            .arg = NULL,
            .name = "evolution-timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&evolution_timer_args, &evolution_timer_handle));
    
    // El temporizador se inicia para que compruebe la evolución cada 5 segundos.
    ESP_ERROR_CHECK(esp_timer_start_periodic(evolution_timer_handle, 5 * 1000000)); 
    
    ESP_LOGI(TAG, "¡Firmware DIYMON en marcha! Temporizador de evolución iniciado. ¡Bienvenido, creador!");
}