/* Fichero: components/bsp/bsp.c */
/* Descripción: Diagnóstico de Causa Raíz: Error de compilación 'too few arguments to function bsp_display_set_brightness'. La firma de la función fue modificada para requerir un segundo argumento booleano ('save_to_nvs'), pero la llamada en 'bsp_init_service_mode' no se actualizó. Solución Definitiva: Se ha corregido la llamada a 'bsp_display_set_brightness(100, true)', proporcionando el argumento faltante. Se asume 'true' para el guardado, ya que el modo de servicio es una acción explícita que justifica establecer un brillo máximo persistente. Esto resuelve el error de compilación. */
/* Último cambio: 21/08/2025 - 19:26 */
#include "bsp_api.h"
#include "esp_err.h"
#include "esp_log.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp";

// Inicialización completa para la aplicación principal
esp_err_t bsp_init(void) {
    ESP_LOGI(TAG, "Inicializando TODO el hardware para la aplicación principal...");
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_spi_init());
    ESP_ERROR_CHECK(bsp_display_init());
    ESP_ERROR_CHECK(bsp_touch_init());
    ESP_ERROR_CHECK(bsp_imu_init());
    
    if (bsp_sdcard_init() != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al inicializar la tarjeta SD, pero se continúa el arranque. Las animaciones no funcionarán.");
    }
    
    ESP_ERROR_CHECK(bsp_battery_init());
    return ESP_OK;
}

// Inicialización para modos de servicio que necesitan mostrar una imagen
esp_err_t bsp_init_service_mode(void) {
    ESP_LOGI(TAG, "Inicializando hardware para modo de servicio con pantalla...");
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_spi_init());
    
    if (bsp_sdcard_init() != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al inicializar la tarjeta SD en modo servicio. El servidor de ficheros no funcionará.");
    }

    ESP_ERROR_CHECK(bsp_display_init());
    ESP_ERROR_CHECK(bsp_touch_init());
    bsp_display_set_brightness(100, true);
    return ESP_OK;
}

// Inicialización mínima para modos de servicio que no usan pantalla
esp_err_t bsp_init_minimal_headless(void) {
    ESP_LOGI(TAG, "Inicializando hardware MÍNIMO para modo headless (SPI + SD)...");
    ESP_ERROR_CHECK(bsp_spi_init());
    
    if (bsp_sdcard_init() != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al inicializar la tarjeta SD en modo headless.");
    }

    return ESP_OK;
}
