/*
 * =====================================================================================
 *       Filename:  hardware_manager.c
 *    Description:  Implementación del gestor de hardware para DIYMON.
 *        Version:  2.0 (ARQUITECTURA FINAL Y CORRECTA)
 * =====================================================================================
 */

#include "hardware_manager.h"

// Includes generales del sistema y drivers
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// [LA CLAVE] La cabecera del puente que nos da las herramientas correctas
#include "lvgl_helpers.h"

// Includes de la BSP (Board Support Package) que ahora son abstractos
#include "bsp_api.h" // ¡IMPORTANTE! Usamos nuestra nueva capa de abstracción

// Includes de LVGL
#include "esp_lvgl_port.h"

// TAG para los logs de este fichero
static const char *TAG = "HW_MANAGER";

// ... El resto de tus defines (DISPLAY_ROTATION, DEFAULT_BRIGHTNESS, etc.) se quedan igual ...
#define DISPLAY_ROTATION             0
#define DISPLAY_BUFF_HEIGHT          50
#define DISPLAY_BUFF_DOUBLE_BUFFER   1
#define DEFAULT_BRIGHTNESS           50

#if DISPLAY_ROTATION == 90 || DISPLAY_ROTATION == 270
#define LCD_H_RES (320)
#define LCD_V_RES (172)
#else
#define LCD_H_RES (172)
#define LCD_V_RES (320)
#endif

/* --- Variables estáticas (ya no las necesitamos aquí, las gestiona el BSP) --- */
// static esp_lcd_panel_io_handle_t io_handle = NULL; // etc.

/* --- Declaraciones de funciones privadas --- */
static esp_err_t lvgl_driver_init(void);

/* --- Implementación de funciones --- */

esp_err_t hardware_manager_init(void)
{
    // [MODIFICADO] Ahora llamamos a las funciones de nuestra API de BSP
    ESP_LOGI(TAG, "Initializing Board Support Package...");
    bsp_i2c_init();
    bsp_spi_init();
    bsp_display_init();
    bsp_touch_init();
    bsp_sdcard_init();
    
    ESP_LOGI(TAG, "Initializing LVGL graphics library driver...");
    ESP_ERROR_CHECK(lvgl_driver_init());

    ESP_LOGI(TAG, "Initializing display brightness control...");
    bsp_display_set_brightness(DEFAULT_BRIGHTNESS);

    // --- [CORRECCIÓN FINAL] INICIALIZACIÓN DEL SISTEMA DE FICHEROS ---
    // Esta es la función correcta que nos da el puente "esp_lvgl_port"
    lvgl_fs_init();
    ESP_LOGI(TAG, "LVGL File System initialized and SD card registered.");

    return ESP_OK;
}

static esp_err_t lvgl_driver_init(void)
{
    // Esta función probablemente deba ser eliminada o simplificada,
    // ya que ahora la inicialización de la pantalla y el táctil
    // la hace el BSP. Por ahora la dejamos para no romper nada más.
    
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 1024 * 10,
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5
    };
    return lvgl_port_init(&lvgl_cfg);
}