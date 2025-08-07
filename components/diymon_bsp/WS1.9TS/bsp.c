/*
 * Archivo: bsp.c
 * Descripción: Implementación del inicializador principal del BSP.
 */
#include "bsp_api.h"
#include "esp_err.h"

// Esta función llama a todos los inicializadores granulares en el orden correcto.
esp_err_t bsp_init(void) {
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_display_init());
    ESP_ERROR_CHECK(bsp_touch_init());
    ESP_ERROR_CHECK(bsp_sdcard_init());
    // ESP_ERROR_CHECK(bsp_imu_init()); // Descomenta cuando lo implementes
    return ESP_OK;
}