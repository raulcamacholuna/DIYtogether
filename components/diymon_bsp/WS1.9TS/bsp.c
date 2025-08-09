/*
 * Fichero: ./components/diymon_bsp/WS1.9TS/bsp.c
 * Fecha: 10/08/2025 - 21:00
 * Último cambio: Añadido un mecanismo de reintento para la inicialización de la SD.
 * Descripción: Se ha modificado la función `bsp_init` para reintentar la
 *              inicialización de la tarjeta SD hasta 3 veces en caso de fallo.
 *              Esto aumenta la robustez del arranque y soluciona los errores de
 *              CRC esporádicos relacionados con el tiempo de estabilización del hardware.
 */
#include "bsp_api.h"
#include "esp_err.h"
#include "esp_log.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp";

esp_err_t bsp_init(void) {
    ESP_LOGI(TAG, "Inicializando TODO el hardware...");
    
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_display_init());
    ESP_ERROR_CHECK(bsp_touch_init());

    // --- SOLUCIÓN AL ERROR DE CRC: Reintentar la inicialización de la SD ---
    esp_err_t sd_err = ESP_FAIL;
    for (int i = 0; i < 3; i++) {
        sd_err = bsp_sdcard_init();
        if (sd_err == ESP_OK) {
            ESP_LOGI(TAG, "Tarjeta SD inicializada con éxito en el intento %d.", i + 1);
            break;
        }
        ESP_LOGW(TAG, "Fallo al inicializar la SD (intento %d/%d): %s. Reintentando en 250ms...", i + 1, 3, esp_err_to_name(sd_err));
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    // Si todos los reintentos fallan, el error check detendrá la ejecución.
    ESP_ERROR_CHECK(sd_err);
    
    return ESP_OK;
}