/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Punto de entrada principal para el firmware de DIYMON.
 *                  Inicializa los servicios básicos, el gestor de hardware y la
 *                  interfaz de usuario. Ahora también gestiona el temporizador de evolución.
 *
 *        Version:  1.2 (Con corrección del temporizador)
 *        Created:  [Fecha de hoy]
 *       Revision:  none
 *       Compiler:  xtensa-esp32-elf-gcc
 *
 *         Author:  Raúl Camacho Luna (con revisión de IA)
 *   Organization:  DIYMON Project
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_lvgl_port.h"
#include "esp_timer.h"

#include "hardware_manager.h"
#include "ui.h"
#include "diymon_evolution.h"

static const char *TAG = "DIYMON_MAIN";

// [CORRECCIÓN] Se declara el handle como 'static' para que no se destruya al salir de app_main
static esp_timer_handle_t evolution_timer_handle;

static void evolution_timer_callback(void* arg) {
    const char* current_code = diymon_get_current_code();
    ESP_LOGI(TAG, "Revisando evolución. DIYMON actual: %s", current_code);

    const char* next_evolution = diymon_get_next_evolution_in_sequence(current_code);

    if (next_evolution != NULL) {
        ESP_LOGI(TAG, "¡EVOLUCIÓN! Nuevo código: %s", next_evolution);
        diymon_set_current_code(next_evolution);
    } else {
        ESP_LOGI(TAG, "El DIYMON ha alcanzado su forma final. Deteniendo temporizador de evolución.");
        // Ahora, al llamar a stop, el handle es válido porque es estático.
        esp_timer_stop(evolution_timer_handle);
    }
}


void app_main(void)
{
    /* --- 1. Inicialización del Sistema Base --- */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was corrupt, erasing and re-initializing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS system initialized successfully.");


    /* --- 2. Inicialización del Hardware --- */
    hardware_manager_init();
    ESP_LOGI(TAG, "Hardware Manager initialized successfully.");


    /* --- 3. Inicialización del Motor de Lógica del Juego --- */
    diymon_evolution_init();
    ESP_LOGI(TAG, "DIYMON Core Logic initialized successfully.");


    /* --- 4. Inicialización de la Interfaz de Usuario (UI) --- */
    ESP_LOGI(TAG, "Initializing DIYMON User Interface...");
    if (lvgl_port_lock(0)) {
        ui_init();
        lvgl_port_unlock();
    }
    

    /* --- 5. Creación e Inicio del Temporizador de Evolución --- */
    const esp_timer_create_args_t evolution_timer_args = {
            .callback = &evolution_timer_callback,
            .arg = NULL, // No necesitamos pasar el handle como argumento si es estático/global
            .name = "evolution-timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&evolution_timer_args, &evolution_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(evolution_timer_handle, 5 * 1000000));
    
    ESP_LOGI(TAG, "DIYMON Firmware is running! Evolution timer started. Welcome, creator!");
}