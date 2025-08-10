/*
 * Fichero: ./components/diymon_bsp/WS1.9TS/bsp.c
 * Fecha: 12/08/2025 - 11:56
 * Último cambio: Corregido el orden de inicialización (SPI antes que Display).
 * Descripción: Se ha corregido el error `host_id not initialized` asegurando que `bsp_spi_init()` se llama antes que cualquier otro módulo que dependa del bus SPI, como el display o la tarjeta SD.
 */
#include "bsp_api.h"
#include "esp_err.h"
#include "esp_log.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp";

esp_err_t bsp_init(void) {
    ESP_LOGI(TAG, "Inicializando TODO el hardware...");
    
    // --- Orden de inicialización corregido ---
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_spi_init()); // 1. Inicializar el bus SPI
    ESP_ERROR_CHECK(bsp_display_init()); // 2. Ahora el display puede usar el bus SPI
    ESP_ERROR_CHECK(bsp_touch_init());

    esp_err_t sd_err = ESP_FAIL;
    for (int i = 0; i < 3; i++) {
        // 3. La SD también puede usar el bus SPI
        sd_err = bsp_sdcard_init();
        if (sd_err == ESP_OK) {
            ESP_LOGI(TAG, "Tarjeta SD inicializada con éxito en el intento %d.", i + 1);
            break;
        }
        ESP_LOGW(TAG, "Fallo al inicializar la SD (intento %d/%d): %s. Reintentando en 250ms...", i + 1, 3, esp_err_to_name(sd_err));
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    ESP_ERROR_CHECK(sd_err);
    
    return ESP_OK;
}