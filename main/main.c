/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Punto de entrada principal para el firmware de DIYMON.
 *                  Inicializa los servicios básicos, el gestor de hardware y la
 *                  interfaz de usuario.
 *
 *        Version:  1.0
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
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_lvgl_port.h"

#include "hardware_manager.h" // Módulo para gestionar todo el hardware de DIYMON.
#include "ui.h"               // Módulo principal de la interfaz de usuario (LVGL).

// TAG para los mensajes de log de este fichero. Ayuda a identificar de dónde vienen los mensajes.
static const char *TAG = "DIYMON_MAIN";

void app_main(void)
{
    /* --- 1. Inicialización del Sistema Base --- */
    // Se inicializa el Non-Volatile Storage (NVS). Es una memoria flash donde se pueden
    // guardar datos de forma persistente entre reinicios (p. ej., el estado del DIYMON,
    // la configuración de WiFi, etc.).
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was corrupt, erasing and re-initializing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS system initialized successfully.");


    /* --- 2. Inicialización del Hardware --- */
    // Se llama a una única función que se encarga de inicializar todos los periféricos:
    // pantalla, panel táctil, sensores, WiFi, etc. Esto mantiene el main.c limpio.
    // Si en el futuro se cambia un componente, solo se modifica el hardware_manager.
    hardware_manager_init();
    ESP_LOGI(TAG, "Hardware Manager initialized successfully.");


    /* --- 3. Inicialización de la Interfaz de Usuario (UI) --- */
    // Una vez todo el hardware está listo, se inicializa la interfaz gráfica.
    ESP_LOGI(TAG, "Initializing DIYMON User Interface...");
    
    // Se usa un "mutex" (lvgl_port_lock) para asegurar que no se intente dibujar
    // nada en la pantalla hasta que LVGL esté completamente listo. Es una medida de seguridad.
    if (lvgl_port_lock(0)) {
        ui_init(); // Esta función (de ui.c) crea la pantalla inicial y los elementos visuales.
        lvgl_port_unlock(); // Se libera el mutex.
    }
    
    ESP_LOGI(TAG, "DIYMON Firmware is running! Welcome, creator!");
}