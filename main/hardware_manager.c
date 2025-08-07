/*
 * Archivo: hardware_manager.c
 * Versión: 3.2 (FINALÍSIMA - Sincronizado con la API exacta de esp_lvgl_port)
 */

#include "hardware_manager.h"
#include "esp_log.h"
#include "bsp_api.h" 
#include "esp_lvgl_port.h"

static const char *TAG = "HW_MANAGER";
#define DEFAULT_BRIGHTNESS 80

esp_err_t hardware_manager_init(void)
{
    // 1. INICIALIZAR TODO EL HARDWARE DE BAJO NIVEL
    ESP_LOGI(TAG, "Initializing Board Support Package hardware...");
    ESP_ERROR_CHECK(bsp_init());
    
    // 2. INICIALIZAR EL MOTOR DE LVGL
    ESP_LOGI(TAG, "Initializing LVGL core...");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    // 3. REGISTRAR EL DISPLAY EN LVGL
    ESP_LOGI(TAG, "Registering display with LVGL...");
    lvgl_port_display_cfg_t disp_cfg = {
        .panel_handle = bsp_get_display_handle(),
        .hres = bsp_get_display_hres(),
        .vres = bsp_get_display_vres(),
        .double_buffer = 1,
        .flags = {
            .buff_dma = true,
        }
    };
    lv_display_t *disp = lvgl_port_add_disp(&disp_cfg);
    
    // 4. REGISTRAR EL PANEL TÁCTIL EN LVGL
    ESP_LOGI(TAG, "Registering touch input with LVGL...");
    
    // [CORRECCIÓN FINAL] La estructura SÍ necesita el handle del táctil,
    // y la función 'lvgl_port_add_touch' SÓLO toma un argumento.
    lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,                           // El miembro se llama 'disp'
        .handle = bsp_get_touch_handle(),       // El miembro se llama 'handle'
    };
    
    // [CORRECCIÓN FINAL] La función ahora solo tiene un argumento.
    // No usamos ESP_ERROR_CHECK, pero verificamos que el puntero devuelto no sea NULL.
    if (lvgl_port_add_touch(&touch_cfg) == NULL) {
        ESP_LOGE(TAG, "Failed to register touch input device");
        return ESP_FAIL; // Devolvemos un error si falla
    }
    
    ESP_LOGI(TAG, "Setting display brightness...");
    bsp_display_set_brightness(DEFAULT_BRIGHTNESS);
    
    ESP_LOGI(TAG, "Hardware Manager initialized successfully! Project is ready.");
    return ESP_OK;
}