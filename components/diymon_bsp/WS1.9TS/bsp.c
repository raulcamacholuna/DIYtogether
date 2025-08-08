/*
 * Archivo: bsp.c
 * Versión: FINAL - Todo el hardware habilitado
 */
#include "bsp_api.h"
#include "esp_err.h"
#include "esp_log.h" 

esp_err_t bsp_init(void) {
    ESP_LOGI("bsp.c", "Inicializando TODO el hardware...");
    
    // [CORRECCIÓN] Habilitamos todo en el orden correcto
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_display_init());
    ESP_ERROR_CHECK(bsp_touch_init());
    ESP_ERROR_CHECK(bsp_sdcard_init()); // <-- ¡DESCOMENTADO!
    
    return ESP_OK;
}